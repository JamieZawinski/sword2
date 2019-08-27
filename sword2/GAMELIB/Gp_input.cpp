//********************************************
//	DirectInput 相关处理函数
//********************************************
#include <windows.h>
#include <stdio.h>
#include <dinput.h>
#include "gp_other.h"
#include "gp_init.h"
#include "gp_input.h"

//全局通用对象
CMouse Mouse;			//鼠标
CKey Key;				//键盘
CJoystick g_Joystick;	//手柄

extern HWND hWnd;
extern HINSTANCE hInst;
extern BOOL bActive;
#if USE_DX8
	LPDIRECTINPUT8	lpDI;	//DINPUT设备
#else
	LPDIRECTINPUT	lpDI;	//DINPUT设备
#endif

/////////////////////////////////////////////////////////////////
//INPUT
/////////////////////////////////////////////////////////////////

//初始化DIRECTINPUT
bool WINAPI InitInput()
{
    HRESULT hr;
#if USE_DX8
    hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,(void**)&lpDI, NULL );
#else
	hr = DirectInputCreate( hInst, DIRECTINPUT_VERSION, &lpDI, NULL );
#endif
    if ( FAILED(hr) )
	{
		MessageBox(hWnd,"Create DirectInput","Failed",MB_OK);
        return false;
	}
	return true;
}

//释放DIRECTINPUT
void WINAPI FreeInput()
{
	_RELEASE( lpDI );
}

/////////////////////////////////////////////////////////////////
//MOUSE
/////////////////////////////////////////////////////////////////

//初始化鼠标
HRESULT WINAPI CMouse::InitInput_Mouse( HWND hDlg )
{
    HRESULT hr;
	//创建设备
    hr = lpDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL );
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Create Mouse Device","Failed",MB_OK);
        return hr;
	}
	//数据格式
    hr = g_pMouse->SetDataFormat( &c_dfDIMouse );
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Set Mouse Data Format","Failed",MB_OK);
        return hr;
	}
	//协作级
    hr = g_pMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Set Mouse Cooperative Level","Failed",MB_OK);
        return hr;
	}
	//属性
	DIPROPDWORD property;
	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwObj=0;
	property.diph.dwHow=DIPH_DEVICE;
	property.dwData=16;

	hr = g_pMouse->SetProperty(DIPROP_BUFFERSIZE, &property.diph);
	if (FAILED(hr))
	{
		MessageBox(hWnd,"Set Mouse Property","Failed",MB_OK);
        return hr;
	}
	//获取
    hr = g_pMouse->Acquire(); 
    if FAILED(hr) 
    {
        return hr;
	}

    return true;
}

//获取设备
HRESULT WINAPI CMouse::SetAcquire( HWND hDlg )
{
    if (NULL == g_pMouse)
        return S_FALSE;

    if (bActive) 
    {
        g_pMouse->Acquire();
    } 
    else 
    {
        g_pMouse->Unacquire();
    }

    return S_OK;
}

//释放DIRECTINPUT设备MOUSE
void WINAPI CMouse::FreeInput_Mouse()
{
	if( g_pMouse )
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse=NULL;
	}
}

//获取缓冲区数据
unsigned char WINAPI CMouse::GetMouseData()
{
	if( g_pMouse )
	{
_Begin:
		DIDEVICEOBJECTDATA	data;
		DWORD				elements=1;		//一个元素
		HRESULT hr = g_pMouse->GetDeviceData(sizeof(data), &data, &elements, 0);
	    if FAILED(hr)
		{
			if(hr == DIERR_INPUTLOST)
			{
				hr = g_pMouse->Acquire();
			}
		}
		else
		{
			if(elements == 1)
			{
				switch(data.dwOfs)
				{
					case DIMOFS_X:			//忽略移动数据
					case DIMOFS_Y: 
						goto _Begin;
						break;
					case DIMOFS_BUTTON0: 
						if( data.dwData & 0x80 )		//高位为1：按下
							return LB_DOWN;
						return LB_UP;					//否则释放
						break;
					case DIMOFS_BUTTON1: 
						if( data.dwData & 0x80 ) 
							return RB_DOWN;
						return RB_UP;
						break;
				}
			}
			else 
				if(elements==0) 
					return 0;
		}
	}

	return 0;
}

//清空鼠标缓冲区
void WINAPI CMouse::ReleaseMouseBuffer()
{
	while(1)
	{
		if( GetMouseData()== 0 ) break;
	}
}

//**************************
//获取鼠标当前状态（经新浪网友wing_of_dream改进）
//没有太多改进，仅仅测试一下是否没有获取过鼠标设备
void WINAPI CMouse::GetMouseState()
{
	if (g_pMouse) 
	{
		DIMOUSESTATE dims; 
		HRESULT hr;

	_again:
		hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);//得到当前状态
		if( FAILED(hr) )
		{
			hr = g_pMouse->Acquire();			//重新获取
			if(!( hr== DI_OK))
			{
				for (int i=0;i<50;i++)
				{
					if ((hr = g_pMouse->Acquire()) == DI_OK)
					break;
				}
			}
			
			if (SUCCEEDED(hr)) 
			{
				goto _again;
			}
		}

		if( hr == DIERR_NOTACQUIRED )
		{
			return;
		}

		//获取鼠标按键信息
		if( SUCCEEDED( hr ))
		{
			if( dims.rgbButtons[0] & 0x80 )	//按下
				MouseButton[0]=1;
			else 
				MouseButton[0]=0;			//没有按下

			if( dims.rgbButtons[1] & 0x80 )
				MouseButton[1]=1;
			else 
				MouseButton[1]=0;
		}
	}
}

//等待鼠标输入，程序中断
bool WINAPI CMouse::WaitMouse(char act)
{
	char Button=GetMouseData();
	while( Button != act )
	{
		Button=GetMouseData();
	}
	return true;
}

//某键当前是否按下 0=左 1=右 2=中 4=左右
bool WINAPI CMouse::IsButton( char Button )
{
	GetMouseState();
	if( Button==4 )
	{
		if( MouseButton[0] && MouseButton[1] )	//按下
			while(1)
			{
				GetMouseState();
				if( GetAsyncKeyState( VK_RETURN ) ) break;
				if( !MouseButton[0] || !MouseButton[1] )	//松开
					return true;
			}
	}
	else
	if( MouseButton[Button] )	//按下
		while(1)
		{
			GetMouseState();
			if( GetAsyncKeyState( VK_RETURN ) ) break;
			if( !MouseButton[Button] )	//松开
				return true;
		}

	return false;
}

//返回当前按下某键 0=左 1=右 2=中 4=左右 -1=无
char WINAPI CMouse::DownButton()
{
	GetMouseState();
	
	if( MouseButton[0] && MouseButton[1] )	//左右
		return 4;
	if( MouseButton[0] )	//左
		return 0;
	if( MouseButton[1] )	//右
		return 1;

	return -1;
}

/////////////////////////////////////////////////////////////////
//KEYBOARD
/////////////////////////////////////////////////////////////////

CKey::CKey()
{
	memset( KeyState, 0, 256);
	memset( KeyBuffer, 0, KEYBUFFERSIZE );
	KeyBuffer_Start = KeyBuffer_End = 0;
}

CKey::~CKey()
{
}

//初始化DIRECTINPUT设备
bool WINAPI CKey::InitInput_Key(HWND hWnd)
{
	HRESULT hr;

	//返回一个DIRECTINPUT设备的指针
	hr=lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKey, NULL);
	if( FAILED(hr) ) 
	{
		FreeInput_Key();
		return false;
	}

	//设置数据格式
	hr=lpDIKey->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(hr) )
	{
		FreeInput_Key();
		return false;
	}

	//设置协作层
	hr=lpDIKey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if( FAILED(hr) ) 
	{
		FreeInput_Key();
		return false;
	}
	//设置属性
	DIPROPDWORD property;
	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwObj=0;
	property.diph.dwHow=DIPH_DEVICE;
	property.dwData=KEYBUFFERSIZE;

	hr = lpDIKey->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

    if FAILED(hr)
	{
		// 设置缓冲区失败
		MessageBox(hWnd, "error on set Key Buffer size", "error", MB_OK);
        return FALSE;
    }

	//得到设备通道
	hr=lpDIKey->Acquire();
	if( FAILED(hr) )
	{
		FreeInput_Key();
		return false;
	}

	return true;
}

//获取设备
HRESULT WINAPI CKey::SetAcquire( HWND hDlg )
{
    if ( NULL == lpDIKey )
        return S_FALSE;

    if (bActive) 
    {
        lpDIKey->Acquire();
    } 
    else 
    {
        lpDIKey->Unacquire();
    }

    return S_OK;
}

//释放DIRECTINPUT设备KEY
void WINAPI CKey::FreeInput_Key()
{
	if( lpDIKey )
	{
		lpDIKey->Unacquire();
		lpDIKey->Release();
		lpDIKey=NULL;
	}
}

//获取键盘即时数据
void WINAPI CKey::GetKeyState()
{
	HRESULT hr;

	hr=lpDIKey->GetDeviceState(256, (LPVOID)&KeyState);
	if( FAILED(hr) )
	{
		lpDIKey->Acquire();
		hr=lpDIKey->GetDeviceState(256, (LPVOID)&KeyState);
	}
}

//检测一个键当前是否按下
bool WINAPI CKey::KeyDown(unsigned char Key)
{
	GetKeyState();
	
	if( KEYDOWN(KeyState, Key))
	{
		return true;
	}
	return false;
}

//返回一个键是否按下
bool WINAPI CKey::GetState(unsigned char Key)
{
	if( KEYDOWN(KeyState, Key))
	{
		return true;
	}
	return false;
}

//***************************
//返回键盘缓冲区数据(附带即时数据)
void WINAPI CKey::GetKeyData()
{
	DIDEVICEOBJECTDATA rgdod[KEYBUFFERSIZE];
	DWORD dwItems = KEYBUFFERSIZE;

_again:
	HRESULT hr = lpDIKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
	if(hr != DI_OK)		
	{
		for(int i=0; i<256; i++)	//清0
			KeyState[i] = 0;

		if( FAILED(hr) ) 
		{
			hr = lpDIKey->Acquire();
			if FAILED(hr)
			{
				return;
			}
			if SUCCEEDED(hr)
			{
				goto _again;
            }
        }
	}

	if( SUCCEEDED(hr) && dwItems>0 )
	{
		for(DWORD i=0; i<dwItems; i++)
		{
			//缓冲数据
			if((rgdod[i].dwData & 0x80))	//按下
			{
				KeyBuffer[KeyBuffer_End] = (BYTE)(rgdod[i].dwOfs);
				KeyBuffer_End ++;
				if( KeyBuffer_End >= KEYBUFFERSIZE ) //循环
					KeyBuffer_End = 0;
			}
			else	//释放＋128
			{
				KeyBuffer[KeyBuffer_End] = (BYTE)(rgdod[i].dwOfs+128);
				KeyBuffer_End ++;
				if( KeyBuffer_End >= KEYBUFFERSIZE ) KeyBuffer_End = 0;
			}

			//即时数据
			KeyState[rgdod[i].dwOfs] = (BYTE)(rgdod[i].dwData & 0x80);
		}
	}
}

//从缓冲区取一个值(0=无)
unsigned char WINAPI CKey::GetKey()
{
	//缓冲区为空
	if( KeyBuffer_Start == KeyBuffer_End )
		return 0;

	byte ret=KeyBuffer_Start;
	KeyBuffer_Start++;
	if( KeyBuffer_Start >= KEYBUFFERSIZE )
		KeyBuffer_Start = 0;

	return KeyBuffer[ret];
}

//清空键盘缓冲区
void WINAPI CKey::ReleaseKeyBuffer()
{
	while(1)
	{
		GetKeyData();
		if( GetKey()== 0 ) break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Joystick
//////////////////////////////////////////////////////////////////////////
#if USE_DX8
	LPDIRECTINPUT8			CJoystick::g_pDI=NULL;
	LPDIRECTINPUTDEVICE8	CJoystick::m_pJoystick=NULL;
#else
	LPDIRECTINPUT7			CJoystick::g_pDI=NULL;
	LPDIRECTINPUTDEVICE2	CJoystick::m_pJoystick=NULL;
#endif
DIDEVCAPS				CJoystick::m_diDevCaps;

CJoystick::CJoystick()
{
	memset( State, 0, MAXKEY );	
	memset( Buffer, 0, BUFFERSIZE );
	Buffer_Start = Buffer_End = 0;
}

CJoystick::~CJoystick()
{
	
}

//媒举手柄的回调函数
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	HRESULT hr;

	// 得到一个被媒举的手柄的接口
#if USE_DX8
	hr = CJoystick::g_pDI->CreateDevice( pdidInstance->guidInstance,&CJoystick::m_pJoystick, NULL );
#else
	hr = CJoystick::g_pDI->CreateDeviceEx( pdidInstance->guidInstance, IID_IDirectInputDevice2,
		(VOID**)&CJoystick::m_pJoystick, NULL );
#endif
	// 失败了，我们就不能使用这个手柄，那么接着媒举
	if( FAILED(hr) ) 
		return DIENUM_CONTINUE;
	
	
	// 成功了，那么就停止媒举，我们只使用第一个手柄
	return DIENUM_STOP;
}

//媒举手柄的方向轴的回调函数
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	HWND hDlg = (HWND)pContext;		//pContext放的是窗口句柄
	
	DIPROPRANGE diprg; 
	diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow        = DIPH_BYOFFSET; 
	diprg.diph.dwObj        = pdidoi->dwOfs; // 媒举方向轴
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000; 
	
	//设置方向轴的范围
	if( FAILED( CJoystick::m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
		return DIENUM_STOP;			//失败了，就不继续了
		
	DIPROPDWORD property;

	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwHow = DIPH_BYOFFSET;
	property.diph.dwObj = pdidoi->dwOfs;
	property.dwData = 5000;
	HRESULT hr = CJoystick::m_pJoystick->SetProperty(DIPROP_DEADZONE,&property.diph);
	if(FAILED(hr))
	{
		// 设置死区失败
		MessageBox(hDlg, "error on set Joystick deadzone size", "error", MB_OK);
        return FALSE;
	}
	// 让我们的应用程序根据方向轴所支持的方式做出不同的响应
	// 配合传进来的窗口句柄可以做很多的事情…………
	switch( pdidoi->dwOfs )
	{
	case DIJOFS_X:
		break;
	case DIJOFS_Y:
		break;
	case DIJOFS_Z:
		break;
	case DIJOFS_RX:
		break;
	case DIJOFS_RY:
		break;
	case DIJOFS_RZ:
		break;
	case DIJOFS_SLIDER(0):
		break;
	case DIJOFS_SLIDER(1):
		break;
	}
	
	return DIENUM_CONTINUE;				//继续媒举下一个方向
}

//初始化
HRESULT CJoystick::InitInput_Joystick( HWND hDlg )
{
	HRESULT hr;
	//创建DirectInput
#if USE_DX8
	hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION,IID_IDirectInput8, 
							(LPVOID*)&g_pDI, NULL );
#else
	hr = DirectInputCreateEx( hInst, DIRECTINPUT_VERSION,IID_IDirectInput7, 
							(LPVOID*)&g_pDI, NULL );
#endif
	if( FAILED(hr) ) 
		return hr;
	
	// 媒举一个手柄
#if USE_DX8
#define JOY DI8DEVCLASS_GAMECTRL
#else
#define JOY  DIDEVTYPE_JOYSTICK
#endif
	hr = g_pDI->EnumDevices( JOY, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
	if( FAILED(hr) ) 
		return hr;
	
	// 确认得到了一个手柄，上一个函数只保证它自己调用成功，不保证得到了一个可以用的手柄
	if( NULL == m_pJoystick )
	{
		return E_FAIL;
	}
	//设置数据格式
	hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick );
	if( FAILED(hr) ) 
		return hr;
	//设置协作级
	hr = m_pJoystick->SetCooperativeLevel( hDlg, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );
	if( FAILED(hr) ) 
		return hr;
	
	m_diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hr = m_pJoystick->GetCapabilities(&m_diDevCaps);
	if ( FAILED(hr) ) 
		return hr;
	//媒举方向轴
	m_pJoystick->EnumObjects( EnumAxesCallback, (VOID*)hDlg, DIDFT_AXIS );
	//设置属性
	DIPROPDWORD property;
	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwObj=0;
	property.diph.dwHow=DIPH_DEVICE;
	property.dwData=32;

	hr = m_pJoystick->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

	if FAILED(hr)
	{
		// 设置缓冲区失败
		MessageBox(hDlg, "error on set Joystick Buffer size", "error", MB_OK);
        return FALSE;
    }

	//得到设备
	SetAcquire(hWnd);

	return true;
}

//获取设备
HRESULT CJoystick::SetAcquire( HWND hWnd )
{
	if( m_pJoystick )
	{
		if( bActive )
			m_pJoystick->Acquire();
		else
			m_pJoystick->Unacquire();
	}
	
	return S_OK;
}

//得到手柄的当前状态
HRESULT CJoystick::GetJoystickState()
{
	HRESULT     hr;
	DIJOYSTATE  js;
	static DIJOYSTATE  oldjs;		//上一个状态

	if( m_pJoystick ) 
	{
		do
		{	//Poll
			hr = m_pJoystick->Poll();
			if( FAILED(hr) )
				return hr;
			//得到当前状态
			hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE), &js );

			if( hr == DIERR_INPUTLOST )
			{
				hr = m_pJoystick->Acquire();
				if( FAILED(hr) )  
					return hr;
			}
		}
		while( DIERR_INPUTLOST == hr );
		
		if( FAILED(hr) )
			return hr;
		//清0
		for(int i =0;i<MAXKEY;i++)
			State[i] =0;
		//我们只是关心这几个的值

		//先是上下方向的
		if(js.lY<0&&abs(oldjs.lY-js.lY)<10)
			State[DIJ_UP] = DIJ_UP+128;
		if(js.lY>0&&abs(oldjs.lY-js.lY)<10)
			State[DIJ_DOWN] = DIJ_DOWN+128;
		if(js.lY>0&&abs(oldjs.lY)<10)
			State[DIJ_DOWN]	= DIJ_DOWN;
		if(js.lY<0&&abs(oldjs.lY)<10)
			State[DIJ_UP]	= DIJ_UP;
		//然后是左右方向的
		if(js.lX<0&&abs(oldjs.lX-js.lX)<10)
			State[DIJ_LEFT] = DIJ_LEFT+128;
		if(js.lX>0&&abs(oldjs.lX-js.lX)<10)
			State[DIJ_RIGHT] = DIJ_RIGHT+128;
		if(js.lX>0&&abs(oldjs.lX)<10)
			State[DIJ_RIGHT] = DIJ_RIGHT;
		if(js.lX<0&&abs(oldjs.lX)<10)
			State[DIJ_LEFT]	= DIJ_LEFT;
		//最后是按纽
		State[DIJ_3] = (js.rgbButtons[0]&0x80?DIJ_3+48:0);	//按下或者是放开
		State[DIJ_4] = (js.rgbButtons[1]&0x80?DIJ_4+48:0);	
		State[DIJ_2] = (js.rgbButtons[2]&0x80?DIJ_2+48:0);
		State[DIJ_1] = (js.rgbButtons[3]&0x80?DIJ_1+48:0);
		memcpy(&oldjs,&js,sizeof(js));
	} 
	return S_OK;
}

//返回一个键当前是否按下
unsigned char CJoystick::GetState(unsigned char Key)
{
	return State[Key];
}

//***************************
//返回缓冲区数据
void WINAPI CJoystick::GetJoystickData()
{
	DIDEVICEOBJECTDATA rgdod[BUFFERSIZE];
	DWORD dwItems = BUFFERSIZE;

	if( m_pJoystick )
	{
	_again:
		// Poll 
		HRESULT hr = m_pJoystick->Poll();
		if( FAILED(hr) ) return;

		hr = m_pJoystick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
		if(hr != DI_OK)		
		{
			if( FAILED(hr) ) 
			{
				hr = m_pJoystick->Acquire();
				if FAILED(hr)
				{
					return;
				}
				if SUCCEEDED(hr)
				{
					goto _again;
				}
			}
		}

		if( SUCCEEDED(hr) && dwItems>0 )
		{
			DWORD i;
			for(i=0; i<dwItems; i++)
			{
				//缓冲数据，两个按纽的数据
				if(rgdod[i].dwOfs == DIJOFS_BUTTON0)
				{
					if((rgdod[i].dwData & 0x80) )	//按下 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_3);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//释放
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_3+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON1)
				{
					if((rgdod[i].dwData & 0x80) )	//按下 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_4);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//释放
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_4+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON2)
				{
					if((rgdod[i].dwData & 0x80) )	//按下 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_2);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//释放
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_2+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON3)
				{
					if((rgdod[i].dwData & 0x80) )	//按下 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_1);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//释放
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_1+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}

			}
		}
	}
}

//从缓冲区取一个值(0=无)
unsigned char WINAPI CJoystick::GetKey()
{
	if( m_pJoystick )
	{
		//缓冲区为空
		if( Buffer_Start == Buffer_End )
			return 0;

		byte ret=Buffer_Start;
		Buffer_Start++;
		if( Buffer_Start >= BUFFERSIZE )
			Buffer_Start = 0;

		return Buffer[ret];
	}
	return 0;
}

//清空缓冲区
void WINAPI CJoystick::ReleaseBuffer()
{
	if( m_pJoystick )
	{
		while(1)
		{
			GetJoystickData();
			if( GetKey()== 0 ) break;
		}
	}
}

//释放手柄
HRESULT CJoystick::FreeInput_Joystick()
{
	if( m_pJoystick !=  NULL ) 
	{
		m_pJoystick->Unacquire();
		m_pJoystick->Release();
		m_pJoystick = NULL;
	}

	if( lpDI ) 
	{
		lpDI->Release();
		lpDI = NULL;
	}
	
	return S_OK;
}

