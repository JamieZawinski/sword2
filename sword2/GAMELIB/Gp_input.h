//********************************************
//	DirectInput ��ش�����
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

//ȫ�ֶ���
extern CMouse Mouse;			//���
extern CKey Key;				//����
extern CJoystick g_Joystick;	//�ֱ�
extern unsigned mouse, key, joystick;	//��ꡢ���̺��ֱ��Ļ��������ݽ���

//DINPUT
bool WINAPI InitInput();	//��ʼ��
void WINAPI FreeInput();	//�ͷ�

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
//�����
///////////////////////////////////////////////////////
//mouse����
#define LB_DOWN 1
#define LB_UP	8
#define RB_DOWN 2
#define RB_UP	16
#define MB_DOWN 4
#define MB_UP	32

//������
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
	LPDIRECTINPUTDEVICE g_pMouse;			//����豸
#endif
	DWORD MouseX, MouseY;					//�������
	DWORD MouseButton[4];					//��ť״̬

public:
	HRESULT WINAPI InitInput_Mouse( HWND );	//��ʼ��
	HRESULT WINAPI SetAcquire( HWND );		//��ȡ�豸
	void WINAPI FreeInput_Mouse();			//�ͷ����

	unsigned char WINAPI GetMouseData();	//��ȡ����������
	void WINAPI ReleaseMouseBuffer();		//�����껺����

	void WINAPI GetMouseState();			//��ȡ��������
	bool WINAPI WaitMouse(char);			//�ȴ�ĳ�����£�������ѭ���У�
	char WINAPI DownButton();				//���ص�ǰ����ĳ�� 0=�� 1=�� 2=�� 4=���� -1=��
	bool WINAPI IsButton(char);				//ĳ���Ƿ��� 0=�� 1=�� 2=�� 4=����
};

///////////////////////////////////////////////////////
//������
///////////////////////////////////////////////////////
class CKey
{
#if USE_DX8
	LPDIRECTINPUTDEVICE8 lpDIKey;
#else
	LPDIRECTINPUTDEVICE lpDIKey;			//�����豸
#endif

public:
	CKey();
	virtual ~CKey();
	bool WINAPI InitInput_Key( HWND );		//��ʼ��
	HRESULT WINAPI SetAcquire( HWND );		//��ȡ�豸
	void WINAPI FreeInput_Key();			//�ͷ�

////////////////////////////////////////////
// ��ʱ����
private:
	#define KEYDOWN(name,key) (name[key]&0x80)
	unsigned char KeyState[256];			//��ʱ����

public:
	void WINAPI GetKeyState();				//��ȡ���̼�ʱ����
	bool WINAPI KeyDown(unsigned char);		//ĳ������
	bool WINAPI GetState(unsigned char);	//ĳ����ǰ�Ƿ���

////////////////////////////////////////////
// ��������
private:
	#define KEYBUFFERSIZE	64
	unsigned char KeyBuffer[KEYBUFFERSIZE];	//���̻�����
	int KeyBuffer_Start, KeyBuffer_End;		//���̻�����ָ��

public:
	void WINAPI GetKeyData();				//��ȡ���̻���������
	unsigned char WINAPI GetKey();			//�ӻ�����ȡһ������(0=�� �ͷŰ���>128)
	void WINAPI ReleaseKeyBuffer();			//��ռ��̻�����
};

///////////////////////////////////////////////////////
//��Ϸ�ֱ���
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

	unsigned char Buffer[BUFFERSIZE];	//������
	unsigned char State[MAXKEY];		//��ʱ����
	int Buffer_Start, Buffer_End;		//������ָ��

public:
	CJoystick();
	virtual ~CJoystick();

	HRESULT InitInput_Joystick( HWND );
	HRESULT SetAcquire( HWND );
	HRESULT FreeInput_Joystick();

	HRESULT GetJoystickState();			//��ȡ��ʱ����
	unsigned char GetState(unsigned char);		//ĳ����ǰ�Ƿ���
	void WINAPI GetJoystickData();		//��ȡ����������
	unsigned char WINAPI GetKey();		//�ӻ�����ȡһ������(0=�� �ͷŰ���>128)
	void WINAPI ReleaseBuffer();		//��ջ�����
};

BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

#endif