//********************************************
//	DirectInput ��ش�����
//********************************************
#include <windows.h>
#include <stdio.h>
#include <dinput.h>
#include "gp_other.h"
#include "gp_init.h"
#include "gp_input.h"

//ȫ��ͨ�ö���
CMouse Mouse;			//���
CKey Key;				//����
CJoystick g_Joystick;	//�ֱ�

extern HWND hWnd;
extern HINSTANCE hInst;
extern BOOL bActive;
#if USE_DX8
	LPDIRECTINPUT8	lpDI;	//DINPUT�豸
#else
	LPDIRECTINPUT	lpDI;	//DINPUT�豸
#endif

/////////////////////////////////////////////////////////////////
//INPUT
/////////////////////////////////////////////////////////////////

//��ʼ��DIRECTINPUT
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

//�ͷ�DIRECTINPUT
void WINAPI FreeInput()
{
	_RELEASE( lpDI );
}

/////////////////////////////////////////////////////////////////
//MOUSE
/////////////////////////////////////////////////////////////////

//��ʼ�����
HRESULT WINAPI CMouse::InitInput_Mouse( HWND hDlg )
{
    HRESULT hr;
	//�����豸
    hr = lpDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL );
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Create Mouse Device","Failed",MB_OK);
        return hr;
	}
	//���ݸ�ʽ
    hr = g_pMouse->SetDataFormat( &c_dfDIMouse );
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Set Mouse Data Format","Failed",MB_OK);
        return hr;
	}
	//Э����
    hr = g_pMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if ( FAILED(hr) ) 
    {
		MessageBox(hWnd,"Set Mouse Cooperative Level","Failed",MB_OK);
        return hr;
	}
	//����
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
	//��ȡ
    hr = g_pMouse->Acquire(); 
    if FAILED(hr) 
    {
        return hr;
	}

    return true;
}

//��ȡ�豸
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

//�ͷ�DIRECTINPUT�豸MOUSE
void WINAPI CMouse::FreeInput_Mouse()
{
	if( g_pMouse )
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse=NULL;
	}
}

//��ȡ����������
unsigned char WINAPI CMouse::GetMouseData()
{
	if( g_pMouse )
	{
_Begin:
		DIDEVICEOBJECTDATA	data;
		DWORD				elements=1;		//һ��Ԫ��
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
					case DIMOFS_X:			//�����ƶ�����
					case DIMOFS_Y: 
						goto _Begin;
						break;
					case DIMOFS_BUTTON0: 
						if( data.dwData & 0x80 )		//��λΪ1������
							return LB_DOWN;
						return LB_UP;					//�����ͷ�
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

//�����껺����
void WINAPI CMouse::ReleaseMouseBuffer()
{
	while(1)
	{
		if( GetMouseData()== 0 ) break;
	}
}

//**************************
//��ȡ��굱ǰ״̬������������wing_of_dream�Ľ���
//û��̫��Ľ�����������һ���Ƿ�û�л�ȡ������豸
void WINAPI CMouse::GetMouseState()
{
	if (g_pMouse) 
	{
		DIMOUSESTATE dims; 
		HRESULT hr;

	_again:
		hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);//�õ���ǰ״̬
		if( FAILED(hr) )
		{
			hr = g_pMouse->Acquire();			//���»�ȡ
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

		//��ȡ��갴����Ϣ
		if( SUCCEEDED( hr ))
		{
			if( dims.rgbButtons[0] & 0x80 )	//����
				MouseButton[0]=1;
			else 
				MouseButton[0]=0;			//û�а���

			if( dims.rgbButtons[1] & 0x80 )
				MouseButton[1]=1;
			else 
				MouseButton[1]=0;
		}
	}
}

//�ȴ�������룬�����ж�
bool WINAPI CMouse::WaitMouse(char act)
{
	char Button=GetMouseData();
	while( Button != act )
	{
		Button=GetMouseData();
	}
	return true;
}

//ĳ����ǰ�Ƿ��� 0=�� 1=�� 2=�� 4=����
bool WINAPI CMouse::IsButton( char Button )
{
	GetMouseState();
	if( Button==4 )
	{
		if( MouseButton[0] && MouseButton[1] )	//����
			while(1)
			{
				GetMouseState();
				if( GetAsyncKeyState( VK_RETURN ) ) break;
				if( !MouseButton[0] || !MouseButton[1] )	//�ɿ�
					return true;
			}
	}
	else
	if( MouseButton[Button] )	//����
		while(1)
		{
			GetMouseState();
			if( GetAsyncKeyState( VK_RETURN ) ) break;
			if( !MouseButton[Button] )	//�ɿ�
				return true;
		}

	return false;
}

//���ص�ǰ����ĳ�� 0=�� 1=�� 2=�� 4=���� -1=��
char WINAPI CMouse::DownButton()
{
	GetMouseState();
	
	if( MouseButton[0] && MouseButton[1] )	//����
		return 4;
	if( MouseButton[0] )	//��
		return 0;
	if( MouseButton[1] )	//��
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

//��ʼ��DIRECTINPUT�豸
bool WINAPI CKey::InitInput_Key(HWND hWnd)
{
	HRESULT hr;

	//����һ��DIRECTINPUT�豸��ָ��
	hr=lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKey, NULL);
	if( FAILED(hr) ) 
	{
		FreeInput_Key();
		return false;
	}

	//�������ݸ�ʽ
	hr=lpDIKey->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(hr) )
	{
		FreeInput_Key();
		return false;
	}

	//����Э����
	hr=lpDIKey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if( FAILED(hr) ) 
	{
		FreeInput_Key();
		return false;
	}
	//��������
	DIPROPDWORD property;
	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwObj=0;
	property.diph.dwHow=DIPH_DEVICE;
	property.dwData=KEYBUFFERSIZE;

	hr = lpDIKey->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

    if FAILED(hr)
	{
		// ���û�����ʧ��
		MessageBox(hWnd, "error on set Key Buffer size", "error", MB_OK);
        return FALSE;
    }

	//�õ��豸ͨ��
	hr=lpDIKey->Acquire();
	if( FAILED(hr) )
	{
		FreeInput_Key();
		return false;
	}

	return true;
}

//��ȡ�豸
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

//�ͷ�DIRECTINPUT�豸KEY
void WINAPI CKey::FreeInput_Key()
{
	if( lpDIKey )
	{
		lpDIKey->Unacquire();
		lpDIKey->Release();
		lpDIKey=NULL;
	}
}

//��ȡ���̼�ʱ����
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

//���һ������ǰ�Ƿ���
bool WINAPI CKey::KeyDown(unsigned char Key)
{
	GetKeyState();
	
	if( KEYDOWN(KeyState, Key))
	{
		return true;
	}
	return false;
}

//����һ�����Ƿ���
bool WINAPI CKey::GetState(unsigned char Key)
{
	if( KEYDOWN(KeyState, Key))
	{
		return true;
	}
	return false;
}

//***************************
//���ؼ��̻���������(������ʱ����)
void WINAPI CKey::GetKeyData()
{
	DIDEVICEOBJECTDATA rgdod[KEYBUFFERSIZE];
	DWORD dwItems = KEYBUFFERSIZE;

_again:
	HRESULT hr = lpDIKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
	if(hr != DI_OK)		
	{
		for(int i=0; i<256; i++)	//��0
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
			//��������
			if((rgdod[i].dwData & 0x80))	//����
			{
				KeyBuffer[KeyBuffer_End] = (BYTE)(rgdod[i].dwOfs);
				KeyBuffer_End ++;
				if( KeyBuffer_End >= KEYBUFFERSIZE ) //ѭ��
					KeyBuffer_End = 0;
			}
			else	//�ͷţ�128
			{
				KeyBuffer[KeyBuffer_End] = (BYTE)(rgdod[i].dwOfs+128);
				KeyBuffer_End ++;
				if( KeyBuffer_End >= KEYBUFFERSIZE ) KeyBuffer_End = 0;
			}

			//��ʱ����
			KeyState[rgdod[i].dwOfs] = (BYTE)(rgdod[i].dwData & 0x80);
		}
	}
}

//�ӻ�����ȡһ��ֵ(0=��)
unsigned char WINAPI CKey::GetKey()
{
	//������Ϊ��
	if( KeyBuffer_Start == KeyBuffer_End )
		return 0;

	byte ret=KeyBuffer_Start;
	KeyBuffer_Start++;
	if( KeyBuffer_Start >= KEYBUFFERSIZE )
		KeyBuffer_Start = 0;

	return KeyBuffer[ret];
}

//��ռ��̻�����
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

//ý���ֱ��Ļص�����
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	HRESULT hr;

	// �õ�һ����ý�ٵ��ֱ��Ľӿ�
#if USE_DX8
	hr = CJoystick::g_pDI->CreateDevice( pdidInstance->guidInstance,&CJoystick::m_pJoystick, NULL );
#else
	hr = CJoystick::g_pDI->CreateDeviceEx( pdidInstance->guidInstance, IID_IDirectInputDevice2,
		(VOID**)&CJoystick::m_pJoystick, NULL );
#endif
	// ʧ���ˣ����ǾͲ���ʹ������ֱ�����ô����ý��
	if( FAILED(hr) ) 
		return DIENUM_CONTINUE;
	
	
	// �ɹ��ˣ���ô��ֹͣý�٣�����ֻʹ�õ�һ���ֱ�
	return DIENUM_STOP;
}

//ý���ֱ��ķ�����Ļص�����
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	HWND hDlg = (HWND)pContext;		//pContext�ŵ��Ǵ��ھ��
	
	DIPROPRANGE diprg; 
	diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow        = DIPH_BYOFFSET; 
	diprg.diph.dwObj        = pdidoi->dwOfs; // ý�ٷ�����
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000; 
	
	//���÷�����ķ�Χ
	if( FAILED( CJoystick::m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
		return DIENUM_STOP;			//ʧ���ˣ��Ͳ�������
		
	DIPROPDWORD property;

	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwHow = DIPH_BYOFFSET;
	property.diph.dwObj = pdidoi->dwOfs;
	property.dwData = 5000;
	HRESULT hr = CJoystick::m_pJoystick->SetProperty(DIPROP_DEADZONE,&property.diph);
	if(FAILED(hr))
	{
		// ��������ʧ��
		MessageBox(hDlg, "error on set Joystick deadzone size", "error", MB_OK);
        return FALSE;
	}
	// �����ǵ�Ӧ�ó�����ݷ�������֧�ֵķ�ʽ������ͬ����Ӧ
	// ��ϴ������Ĵ��ھ���������ܶ�����顭������
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
	
	return DIENUM_CONTINUE;				//����ý����һ������
}

//��ʼ��
HRESULT CJoystick::InitInput_Joystick( HWND hDlg )
{
	HRESULT hr;
	//����DirectInput
#if USE_DX8
	hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION,IID_IDirectInput8, 
							(LPVOID*)&g_pDI, NULL );
#else
	hr = DirectInputCreateEx( hInst, DIRECTINPUT_VERSION,IID_IDirectInput7, 
							(LPVOID*)&g_pDI, NULL );
#endif
	if( FAILED(hr) ) 
		return hr;
	
	// ý��һ���ֱ�
#if USE_DX8
#define JOY DI8DEVCLASS_GAMECTRL
#else
#define JOY  DIDEVTYPE_JOYSTICK
#endif
	hr = g_pDI->EnumDevices( JOY, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
	if( FAILED(hr) ) 
		return hr;
	
	// ȷ�ϵõ���һ���ֱ�����һ������ֻ��֤���Լ����óɹ�������֤�õ���һ�������õ��ֱ�
	if( NULL == m_pJoystick )
	{
		return E_FAIL;
	}
	//�������ݸ�ʽ
	hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick );
	if( FAILED(hr) ) 
		return hr;
	//����Э����
	hr = m_pJoystick->SetCooperativeLevel( hDlg, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );
	if( FAILED(hr) ) 
		return hr;
	
	m_diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hr = m_pJoystick->GetCapabilities(&m_diDevCaps);
	if ( FAILED(hr) ) 
		return hr;
	//ý�ٷ�����
	m_pJoystick->EnumObjects( EnumAxesCallback, (VOID*)hDlg, DIDFT_AXIS );
	//��������
	DIPROPDWORD property;
	property.diph.dwSize=sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	property.diph.dwObj=0;
	property.diph.dwHow=DIPH_DEVICE;
	property.dwData=32;

	hr = m_pJoystick->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

	if FAILED(hr)
	{
		// ���û�����ʧ��
		MessageBox(hDlg, "error on set Joystick Buffer size", "error", MB_OK);
        return FALSE;
    }

	//�õ��豸
	SetAcquire(hWnd);

	return true;
}

//��ȡ�豸
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

//�õ��ֱ��ĵ�ǰ״̬
HRESULT CJoystick::GetJoystickState()
{
	HRESULT     hr;
	DIJOYSTATE  js;
	static DIJOYSTATE  oldjs;		//��һ��״̬

	if( m_pJoystick ) 
	{
		do
		{	//Poll
			hr = m_pJoystick->Poll();
			if( FAILED(hr) )
				return hr;
			//�õ���ǰ״̬
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
		//��0
		for(int i =0;i<MAXKEY;i++)
			State[i] =0;
		//����ֻ�ǹ����⼸����ֵ

		//�������·����
		if(js.lY<0&&abs(oldjs.lY-js.lY)<10)
			State[DIJ_UP] = DIJ_UP+128;
		if(js.lY>0&&abs(oldjs.lY-js.lY)<10)
			State[DIJ_DOWN] = DIJ_DOWN+128;
		if(js.lY>0&&abs(oldjs.lY)<10)
			State[DIJ_DOWN]	= DIJ_DOWN;
		if(js.lY<0&&abs(oldjs.lY)<10)
			State[DIJ_UP]	= DIJ_UP;
		//Ȼ�������ҷ����
		if(js.lX<0&&abs(oldjs.lX-js.lX)<10)
			State[DIJ_LEFT] = DIJ_LEFT+128;
		if(js.lX>0&&abs(oldjs.lX-js.lX)<10)
			State[DIJ_RIGHT] = DIJ_RIGHT+128;
		if(js.lX>0&&abs(oldjs.lX)<10)
			State[DIJ_RIGHT] = DIJ_RIGHT;
		if(js.lX<0&&abs(oldjs.lX)<10)
			State[DIJ_LEFT]	= DIJ_LEFT;
		//����ǰ�Ŧ
		State[DIJ_3] = (js.rgbButtons[0]&0x80?DIJ_3+48:0);	//���»����Ƿſ�
		State[DIJ_4] = (js.rgbButtons[1]&0x80?DIJ_4+48:0);	
		State[DIJ_2] = (js.rgbButtons[2]&0x80?DIJ_2+48:0);
		State[DIJ_1] = (js.rgbButtons[3]&0x80?DIJ_1+48:0);
		memcpy(&oldjs,&js,sizeof(js));
	} 
	return S_OK;
}

//����һ������ǰ�Ƿ���
unsigned char CJoystick::GetState(unsigned char Key)
{
	return State[Key];
}

//***************************
//���ػ���������
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
				//�������ݣ�������Ŧ������
				if(rgdod[i].dwOfs == DIJOFS_BUTTON0)
				{
					if((rgdod[i].dwData & 0x80) )	//���� 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_3);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//�ͷ�
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_3+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON1)
				{
					if((rgdod[i].dwData & 0x80) )	//���� 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_4);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//�ͷ�
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_4+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON2)
				{
					if((rgdod[i].dwData & 0x80) )	//���� 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_2);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//�ͷ�
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_2+48);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
				}
				if(rgdod[i].dwOfs == DIJOFS_BUTTON3)
				{
					if((rgdod[i].dwData & 0x80) )	//���� 
					{
						Buffer[Buffer_End] = (BYTE)(DIJ_1);
						Buffer_End ++;
						if( Buffer_End >= BUFFERSIZE ) 
							Buffer_End = 0;
					}
					else	//�ͷ�
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

//�ӻ�����ȡһ��ֵ(0=��)
unsigned char WINAPI CJoystick::GetKey()
{
	if( m_pJoystick )
	{
		//������Ϊ��
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

//��ջ�����
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

//�ͷ��ֱ�
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

