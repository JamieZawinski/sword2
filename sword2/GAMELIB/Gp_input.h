//********************************************
//	DirectInput 相关处理函数
//********************************************
#ifndef _GP_INPUT_
#define _GP_INPUT_

#include <dinput.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION         0x0300
#endif

#if DIRECTINPUT_VERSION<0x0800
#pragma comment(lib,"dinput.lib")
#else
#pragma comment(lib,"dinput8.lib")
#define USE_DX8  1
#endif

class CMouse;
class CKey;
class CJoystick;

//全局对象
extern CMouse Mouse;			//鼠标
extern CKey Key;				//键盘
extern CJoystick g_Joystick;	//手柄
extern unsigned mouse, key, joystick;	//鼠标、键盘和手柄的缓冲区数据接受

//DINPUT
bool WINAPI InitInput();	//初始化
void WINAPI FreeInput();	//释放

#define LEFT_UP		( key==DIK_LEFT-128 || joystick==DIJ_LEFT+48 )
#define LEFT_DOWN	( key==DIK_LEFT || g_Joystick.GetState( DIJ_LEFT )==DIJ_LEFT )
#define LEFT_ON		( Key.GetState( DIK_LEFT ) || g_Joystick.GetState( DIJ_LEFT )==DIJ_LEFT+128 )

#define RIGHT_UP	( key==DIK_RIGHT-128 || joystick==DIJ_RIGHT+48 )
#define RIGHT_DOWN	( key==DIK_RIGHT || g_Joystick.GetState( DIJ_RIGHT )==DIJ_RIGHT )
#define RIGHT_ON	( Key.GetState( DIK_RIGHT ) || g_Joystick.GetState( DIJ_RIGHT )==DIJ_RIGHT+128 )

#define UP_UP		( key==DIK_UP-128 || joystick==DIJ_UP+48 )
#define UP_DOWN		( key==DIK_UP || g_Joystick.GetState( DIJ_UP )==DIJ_UP )
#define UP_ON		( Key.GetState( DIK_UP ) || g_Joystick.GetState( DIJ_UP ) ==DIJ_UP+128)

#define DOWN_UP		( key==DIK_DOWN-128 || joystick==DIJ_DOWN+48 )
#define DOWN_DOWN	( key==DIK_DOWN || g_Joystick.GetState( DIJ_DOWN )==DIJ_DOWN )
#define DOWN_ON		( Key.GetState( DIK_DOWN ) || g_Joystick.GetState( DIJ_DOWN )==DIJ_DOWN+128 )

#define A_UP		( key==DIK_RETURN+128 || key==DIK_SPACE+128 || joystick==DIJ_2+48 )
#define A_DOWN		( key==DIK_RETURN || key==DIK_SPACE || joystick==DIJ_2 )
#define A_ON		( Key.GetState( DIK_RETURN ) || Key.GetState( DIK_SPACE ) || g_Joystick.GetState( DIJ_2 ) )

#define B_UP		( key==DIK_ESCAPE+128 || joystick==DIJ_1+48 )
#define B_DOWN		( key==DIK_ESCAPE || joystick==DIJ_1 )
#define B_ON		( Key.GetState( DIK_ESCAPE ) || g_Joystick.GetState( DIJ_1 ) )

///////////////////////////////////////////////////////
//鼠标类
///////////////////////////////////////////////////////
//mouse动作
#define LB_DOWN 1
#define LB_UP	8
#define RB_DOWN 2
#define RB_UP	16
#define MB_DOWN 4
#define MB_UP	32

//键定义
enum {
	LBUTTON=0,
	RBUTTON=1,
	MBUTTON=2,
	LRBUTTON=4
};

class CMouse
{
public:
#if USE_DX8
	LPDIRECTINPUTDEVICE8 g_pMouse;
#else
	LPDIRECTINPUTDEVICE g_pMouse;			//鼠标设备
#endif
	DWORD MouseX, MouseY;					//鼠标坐标
	DWORD MouseButton[4];					//按钮状态

public:
	HRESULT WINAPI InitInput_Mouse( HWND );	//初始化
	HRESULT WINAPI SetAcquire( HWND );		//获取设备
	void WINAPI FreeInput_Mouse();			//释放鼠标

	unsigned char WINAPI GetMouseData();	//获取缓冲区数据
	void WINAPI ReleaseMouseBuffer();		//清空鼠标缓冲区

	void WINAPI GetMouseState();			//获取立即数据
	bool WINAPI WaitMouse(char);			//等待某健按下（进入死循环中）
	char WINAPI DownButton();				//返回当前按下某键 0=左 1=右 2=中 4=左右 -1=无
	bool WINAPI IsButton(char);				//某键是否按下 0=左 1=右 2=中 4=左右
};

///////////////////////////////////////////////////////
//键盘类
///////////////////////////////////////////////////////
class CKey
{
#if USE_DX8
	LPDIRECTINPUTDEVICE8 lpDIKey;
#else
	LPDIRECTINPUTDEVICE lpDIKey;			//键盘设备
#endif

public:
	CKey();
	virtual ~CKey();
	bool WINAPI InitInput_Key( HWND );		//初始化
	HRESULT WINAPI SetAcquire( HWND );		//获取设备
	void WINAPI FreeInput_Key();			//释放

////////////////////////////////////////////
// 即时数据
private:
	#define KEYDOWN(name,key) (name[key]&0x80)
	unsigned char KeyState[256];			//即时数据

public:
	void WINAPI GetKeyState();				//获取键盘即时数据
	bool WINAPI KeyDown(unsigned char);		//某键按下
	bool WINAPI GetState(unsigned char);	//某键当前是否按下

////////////////////////////////////////////
// 缓冲数据
private:
	#define KEYBUFFERSIZE	64
	unsigned char KeyBuffer[KEYBUFFERSIZE];	//键盘缓冲区
	int KeyBuffer_Start, KeyBuffer_End;		//键盘缓冲区指针

public:
	void WINAPI GetKeyData();				//获取键盘缓冲区数据
	unsigned char WINAPI GetKey();			//从缓冲区取一个数据(0=无 释放按键>128)
	void WINAPI ReleaseKeyBuffer();			//清空键盘缓冲区
};

///////////////////////////////////////////////////////
//游戏手柄类
///////////////////////////////////////////////////////
#define DIJ_1		10
#define DIJ_2		1
#define DIJ_3		2
#define DIJ_4		3
#define DIJ_R1		4
#define	DIJ_R2		5
#define DIJ_L1		6
#define DIJ_L2		7
#define DIJ_START	8
#define DIJ_SELECT	9
#define DIJ_UP		12
#define DIJ_LEFT	13
#define DIJ_DOWN	14
#define DIJ_RIGHT	15

class CJoystick
{
public:
	#define BUFFERSIZE 32
	#define MAXKEY 32
#if USE_DX8
	static LPDIRECTINPUT8       g_pDI;
	static LPDIRECTINPUTDEVICE8	m_pJoystick;
#else
	static LPDIRECTINPUT7       g_pDI;
	static LPDIRECTINPUTDEVICE2	m_pJoystick;
#endif
	static DIDEVCAPS			m_diDevCaps;

	unsigned char Buffer[BUFFERSIZE];	//缓冲区
	unsigned char State[MAXKEY];		//即时数据
	int Buffer_Start, Buffer_End;		//缓冲区指针

public:
	CJoystick();
	virtual ~CJoystick();

	HRESULT InitInput_Joystick( HWND );
	HRESULT SetAcquire( HWND );
	HRESULT FreeInput_Joystick();

	HRESULT GetJoystickState();			//获取即时数据
	unsigned char GetState(unsigned char);		//某键当前是否按下
	void WINAPI GetJoystickData();		//获取缓冲区数据
	unsigned char WINAPI GetKey();		//从缓冲区取一个数据(0=无 释放按键>128)
	void WINAPI ReleaseBuffer();		//清空缓冲区
};

BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

#endif