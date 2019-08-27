//********************************************
//	�����ʼ����ش�����
//  softboy ������2000��1��25��
//********************************************
#include <stdlib.h>
#include <crtdbg.h>
#include <time.h>
#include "..\resource.h"
#include "gp_init.h"
#include "gp_iniset.h"
#include "gp_draw.h"
#include "gp_input.h"
#include "gp_sound.h"
#include "gp_music.h"
#include "gp_other.h"

#define GameTitle "ʥ��Ӣ�۴�II"

extern WORD *GraphBuffer;			// ��ͼ������
extern WORD RMask, GMask, BMask;	//16λɫ��ɫ����
extern BYTE RMove, GMove;			//�ƶ������ұߵ�λ��
extern DWORD rgbMask;				//��չΪ32λ�������

HWND	hWnd;				//�����ھ��
HINSTANCE hInst;			//Ӧ�ó���ʵ��
BOOL	bActive = TRUE;		//Ӧ�ó����Ƿ��Ծ
BOOL    g_bDebug = FALSE;	//����ģʽ
int		RunGame=1;			//0=�˳�
int		WindowMode=1;		//0=ȫ�� other=����
BOOL	bSwitch = TRUE;		//�ܷ��л���ʾģʽ

//******************************************************************
//���������ڡ�
HWND InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    HWND				hwnd;	
    WNDCLASS			wc;		

	//��䴰����ṹ
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WinProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON_Heroland) );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU_Window);
	wc.lpszClassName = GameTitle;

	//ע�ᴰ����
    RegisterClass( &wc );
	
    if( WindowMode == 0 )
	{
		//����������-(ȫ��)
		hwnd = CreateWindowEx(
			WS_EX_TOPMOST,
			GameTitle,
			GameTitle,
			WS_POPUP,	
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
	        GetSystemMetrics(SM_CYSCREEN),
			NULL,
			NULL,
			hInstance,
			NULL);
	}
	else	//����������-(����)
	{
		RectWindow.left=0;
		RectWindow.top=0;
		RectWindow.right=ScreenWidth;
		RectWindow.bottom=ScreenHeight;
		AdjustWindowRect(&RectWindow, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, false);
		g_pointClient.x = 0 - RectWindow.left;
		g_pointClient.y = 0 - RectWindow.top;
		
		int w = RectWindow.right-RectWindow.left;
		int h = RectWindow.bottom-RectWindow.top;
		hwnd = CreateWindowEx(
			0,
			GameTitle,
			GameTitle,
			WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			w,
		    h,
			NULL,
			NULL,
			hInstance,
			NULL );

		if( WindowMode != 0 )	//����
		{
			int W=GetSystemMetrics(SM_CXSCREEN);
			int H=GetSystemMetrics(SM_CYSCREEN);
			int x1=(W-w)/2;
			int y1=(H-h)/2;
			SetWindowPos(hwnd, HWND_NOTOPMOST, x1, y1, w, h, SWP_SHOWWINDOW );
		}
	}

    if( !hwnd )		
		return FALSE;

	//��ʾ�����´���
    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

	return hwnd;
}

//*****************************************************
//�ͷ�ϵͳ��DirectDraw����
HRESULT FreeDDraw()
{
	if (lpDD != NULL)
    {	
		//�ָ�Э����
		lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		_RELEASE( lpDDSBack );
		_RELEASE(lpDDSPrimary );
	}
	return DD_OK;
}

//ģʽ�л�-------------------------------------------------------------------------
HRESULT ChangeCoopLevel(HWND hWnd )
{
    HRESULT hRet;

	//�ͷ�DDraw
    if (FAILED(hRet = FreeDDraw()))
	{
		ShowMessage("ReleaseAllObjects FAILED");
        return false;
	}

	if( WindowMode == 0 )
	{
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
	}

	//��ô�����ʾ����
    if ( WindowMode != 0 )
    {
		SetWindowLong(hWnd, GWL_STYLE,WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX);

		RectWindow.left=0;
		RectWindow.top=0;
		RectWindow.right=ScreenWidth;
		RectWindow.bottom=ScreenHeight;
		AdjustWindowRect(&RectWindow, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, false);
		g_pointClient.x = 0 - RectWindow.left;
		g_pointClient.y = 0 - RectWindow.top;
		
		int w = RectWindow.right-RectWindow.left;
		int h = RectWindow.bottom-RectWindow.top;

		int W=GetSystemMetrics(SM_CXSCREEN);
		int H=GetSystemMetrics(SM_CYSCREEN);
		int x1=(W-w)/2;
		int y1=(H-h)/2;
		SetWindowPos(hWnd, HWND_NOTOPMOST, x1, y1, w, h, SWP_SHOWWINDOW );
    }

	//��ʼ��DDraw
    hRet = InitDDraw();

    return hRet;
}

//*******************************************************************
//����һ��GDI�ü���
BOOL CreateClipper()
{
	LPDIRECTDRAWCLIPPER lpClipper;
	HRESULT ddrval;
	//����һ������������
	ddrval = lpDD->CreateClipper(0, &lpClipper, NULL);
	if ( ddrval != DD_OK)
	{
		ShowMessage("CreateClipper FAILED");
	    return FALSE;
	}
	//���ô��ھ��
	ddrval = lpClipper->SetHWnd(0, hWnd);
	if ( ddrval != DD_OK)
	{
		ShowMessage("SetHWnd FAILED");
		return FALSE;
	}
	//������ҳ�棬ע������������������ü���
	ddrval = lpDDSPrimary->SetClipper(lpClipper);
	if ( ddrval != DD_OK)
	{
		ShowMessage("SetClipper FAILED");
		return FALSE;
	}
	//������ȻҪ�������ü���
	_RELEASE( lpClipper );
	return TRUE;
}

//******************************************************************
//��ʼ��DirectDraw����
BOOL InitDDraw(void)
{
	DWORD			 dwFlags;
	DDSURFACEDESC	 ddsd;
	HRESULT          ddrval;

	if( WindowMode==0 )	//ȫ��
	{
		//����Э������Ϊȫ��ģʽ
		dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT;	//DDSCL_ALLOWMODEX |
		lpDD->SetCooperativeLevel(hWnd, dwFlags);

		//������ʾģʽ
		lpDD->SetDisplayMode( ScreenWidth, ScreenHeight, ScreenColor);
    
		// ������ҳ��
		ZeroMemory( &ddsd, sizeof( ddsd ) );
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
				  DDSCAPS_FLIP |
				  DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
		if( ddrval != DD_OK )
		{
			return false;
		}
		//����һ����̨����
		CreateBitmap(lpDDSBack, ScreenWidth, ScreenHeight);

 		// �������ظ�ʽ��555����565
		DDPIXELFORMAT DDPixelFormat;
		ZeroMemory(&DDPixelFormat, sizeof(DDPixelFormat));
		DDPixelFormat.dwSize = sizeof(DDPixelFormat);

		if (lpDDSPrimary->GetPixelFormat(&DDPixelFormat) != DD_OK)
			return 1;

		RMask=(WORD)DDPixelFormat.dwRBitMask;
		GMask=(WORD)DDPixelFormat.dwGBitMask;
		BMask=(WORD)DDPixelFormat.dwBBitMask;
		//��չΪ32λ 00000gggggg00000rrrrr000000bbbbb
		rgbMask=((DWORD)GMask<<16)|RMask|BMask;	
		//0x07e0 = 00000|11111|00000b   ��λ����ɫ
		if (GMask == 0x07e0 ) 
			Is555 = 0;

		if( Is555 )
		{
			ColorKey16=0x00007c1f;		//00000|11111|00000|11111b    (255,0,255)
			RMove=10;					//RҪ����10λ
			GMove=5;					//GҪ����5λ
		}
		else
		{
			ColorKey16=0x0000f81f;		//0000|11111|000000|11111b     (255,0,255)
			RMove=11;					//RҪ����11λ
			GMove=6;					//ע������ˣ�Ϊʲô��6�أ���Ϊ�ڽ��лҶ������ʱ��
		}								//һ��6λ��������λ����ᳬ��5λ�����ԡ����Ǻǣ�soft
										//�����һ��ע�Ͷ�û�У����ҿ�����ˣ�)���¸������˰�
											
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else	//����
	{
		//����Э������Ϊ����ģʽ
		dwFlags = DDSCL_NORMAL;
		lpDD->SetCooperativeLevel(hWnd, dwFlags);

		// ������ҳ��
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
		if( ddrval != DD_OK )
		{
			return false;
		}

		//���ɼ�����
		CreateClipper();

		//���ɱ���ҳ��
		CreateBitmap(lpDDSBack, ScreenWidth, ScreenHeight);
		
		// �������ظ�ʽ��555����565
		DDPIXELFORMAT DDPixelFormat;
		ZeroMemory(&DDPixelFormat, sizeof(DDPixelFormat));
		DDPixelFormat.dwSize = sizeof(DDPixelFormat);
		if (lpDDSPrimary->GetPixelFormat(&DDPixelFormat) != DD_OK)
			return 1;
		RMask=(WORD)DDPixelFormat.dwRBitMask;
		GMask=(WORD)DDPixelFormat.dwGBitMask;
		BMask=(WORD)DDPixelFormat.dwBBitMask;
		//��չΪ32λ 00000gggggg00000rrrrr000000bbbbb
		//����Ĳ��֣��������ע�ͺ��ˣ��ҾͲ���д��
		rgbMask=((DWORD)GMask<<16)|RMask|BMask;	
		if (GMask == 0x07e0 ) Is555 = 0;
		if( Is555 )
		{
			ColorKey16=0x00007c1f;
			RMove=10;
			GMove=5;
		}
		else
		{
			ColorKey16=0x0000f81f;
			RMove=11;
			GMove=6;
		}
	}

	return TRUE;
}

//*******************************************************************
//Ӧ�ó������
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow)
{
	MSG		msg;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ����ڴ�й©

	//������Ϸ���������ļ�-(��ȡ����ģʽ)
	CIniSet Ini("ini\\setup.ini");
	WindowMode=Ini.ReadInt("Setup", "WindowMode");
	
	int w=GetSystemMetrics(SM_CXSCREEN); 
	int h=GetSystemMetrics(SM_CYSCREEN);
	HDC hdc=GetDC(hWnd);
	int color=GetDeviceCaps(hdc, BITSPIXEL);

	ReleaseDC(hWnd,hdc);

	if( WindowMode != 0 && WindowMode != 1 )
	{
		if( IDYES==MessageBox(hWnd, "ʹ�ô�����ʾģʽ��\n[Alt+Enter] �л�ȫ���򴰿���ʾ."
						, "ע��", MB_YESNO))
			WindowMode=1;
		else
			WindowMode=0;
		bSwitch=true;
	}

	//�������16λɫ��������ĻС��800*600��Ϊȫ��ģʽ
	if( color != 16 || w < 800 )
	{
		if( color != 16 )
			MessageBox(hWnd, "ע�⣺��ǰWindows����16λɫ�����ϣ���ڴ���ģʽ������Ϸ\
						�������������Ϊ16λɫ!","ע��",MB_OK);
		else
			MessageBox(hWnd, "ע�⣺��ǰWindows��ĻС��800*600�����ϣ���ڴ���ģʽ����\
				��Ϸ�������������Ϊ800*600���ϣ�����ֻ����ȫ��ģʽ������!","ע��",MB_OK);

		bSwitch=false;
		WindowMode=0;
	}	

	//��ʼ��������
	hWnd=InitWindow( hInstance, nCmdShow ) ;
	if (hWnd == NULL )
		return FALSE;
	
	hInst=hInstance;

	//��ʼ��DirectDraw��������ʵ��DirectDraw����
	DirectDrawCreate(NULL, &lpDD, NULL);	//����DirectDraw����
	if (!InitDDraw())
	{
		MessageBox(hWnd, "��ʼ��DirectDraw�����г����������Ƿ���ȷ�İ�װ\
					��DirectX��", "Error", MB_OK );
		FreeDDraw();
		return FALSE;
	}

	//��ʼ��DirectInput
	if( !InitInput() )
	{
		MessageBox(hWnd, "��ʼ��DirectInput���̳����������Ƿ�\
			`			��ȷ�İ�װ��DirectX 5.0���ϰ汾��", "Error", MB_OK );
		FreeInput();
		return FALSE;
	}

	//��ʼ��DirectInput_Mouse
	if( !Mouse.InitInput_Mouse(hWnd) )
	{
		MessageBox(hWnd, "��ʼ��DirectInput Mouse�����������Ƿ���ȷ�İ�װ\
					��DirectX 5.0���ϰ汾��", "Error", MB_OK );
		Mouse.FreeInput_Mouse();
		return FALSE;
	}

	//��ʼ��DirectInput_Key
	if( !Key.InitInput_Key(hWnd) )
	{
		MessageBox(hWnd, "��ʼ��DirectInput Keyboard�����������Ƿ���ȷ�İ�װ\
						��DirectX 5.0���ϰ汾��", "Error", MB_OK );
		Key.FreeInput_Key();
		return FALSE;
	}

	//��ʼ��DirectInput_Joystick
	if( !g_Joystick.InitInput_Joystick(hWnd) )
	{
		g_Joystick.FreeInput_Joystick();
		return FALSE;
	}

	//��ʼ��DirectInput_Sound
	if( Ini.ReadInt("Setup","��Ϸ��Ч") && !CSound::InitDirectSound(hWnd) )
	{
		CSound::EnableSound(false);
		CSound::ReleaseDirectSound();
	}

	//��ʼ��DirectMusic����
	if( Ini.ReadInt("Setup","��������") )
	{
		CMusic::InitMusic();
	}

	//��ʼ�����ָ��
	srand( (unsigned)time( NULL ) ); 

	SetCursor(NULL);

	//��ȡ���ݵȳ�ʼ������
	if( !InitGame() )
	{
		MessageBox(hWnd, "��ʼ����Ϸ���ݹ��̳�����������ļ�", "Error", MB_OK );
		FreeDDraw();
		return FALSE;
	}

	while( RunGame )		//������ѭ��
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) 
				return msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			MainLoop();		//������Ϸ��ѭ��
		}
		else WaitMessage();	//�ȴ���Ϣ
	}

	ReleaseGame();					//�ͷ�����
	CSound::ReleaseDirectSound();	//�ͷ�DirectSound����
	g_Joystick.FreeInput_Joystick();
	Key.FreeInput_Key();
	Mouse.FreeInput_Mouse();
	FreeInput();
	FreeDDraw();					//�ͷ�����DirectDraw����

	//�����ʾģʽ�ı����¼�������ļ�
	if( bSwitch && WindowMode != Ini.ReadInt("Setup", "WindowMode") )
	{
		Ini.WriteInt("Setup", "WindowMode", WindowMode);
		Ini.Save();
	}

	return 1;
}