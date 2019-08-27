//********************************************
//	程序初始化相关处理函数
//  softboy 创建于2000年1月25日
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

#define GameTitle "圣剑英雄传II"

extern WORD *GraphBuffer;			// 绘图缓冲区
extern WORD RMask, GMask, BMask;	//16位色颜色遮罩
extern BYTE RMove, GMove;			//移动到最右边的位数
extern DWORD rgbMask;				//扩展为32位后的遮罩

HWND	hWnd;				//主窗口句柄
HINSTANCE hInst;			//应用程序实例
BOOL	bActive = TRUE;		//应用程序是否活跃
BOOL    g_bDebug = FALSE;	//作弊模式
int		RunGame=1;			//0=退出
int		WindowMode=1;		//0=全屏 other=窗口
BOOL	bSwitch = TRUE;		//能否切换显示模式

//******************************************************************
//创建主窗口。
HWND InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    HWND				hwnd;	
    WNDCLASS			wc;		

	//填充窗口类结构
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

	//注册窗口类
    RegisterClass( &wc );
	
    if( WindowMode == 0 )
	{
		//创建主窗口-(全屏)
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
	else	//创建主窗口-(窗口)
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

		if( WindowMode != 0 )	//窗口
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

	//显示并更新窗口
    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

	return hwnd;
}

//*****************************************************
//释放系统的DirectDraw对象
HRESULT FreeDDraw()
{
	if (lpDD != NULL)
    {	
		//恢复协作级
		lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		_RELEASE( lpDDSBack );
		_RELEASE(lpDDSPrimary );
	}
	return DD_OK;
}

//模式切换-------------------------------------------------------------------------
HRESULT ChangeCoopLevel(HWND hWnd )
{
    HRESULT hRet;

	//释放DDraw
    if (FAILED(hRet = FreeDDraw()))
	{
		ShowMessage("ReleaseAllObjects FAILED");
        return false;
	}

	if( WindowMode == 0 )
	{
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
	}

	//获得窗口显示区域
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

	//初始化DDraw
    hRet = InitDDraw();

    return hRet;
}

//*******************************************************************
//创建一个GDI裁减器
BOOL CreateClipper()
{
	LPDIRECTDRAWCLIPPER lpClipper;
	HRESULT ddrval;
	//创建一个剪裁器对象
	ddrval = lpDD->CreateClipper(0, &lpClipper, NULL);
	if ( ddrval != DD_OK)
	{
		ShowMessage("CreateClipper FAILED");
	    return FALSE;
	}
	//设置窗口句柄
	ddrval = lpClipper->SetHWnd(0, hWnd);
	if ( ddrval != DD_OK)
	{
		ShowMessage("SetHWnd FAILED");
		return FALSE;
	}
	//关联主页面，注意这个函数会增加引用计数
	ddrval = lpDDSPrimary->SetClipper(lpClipper);
	if ( ddrval != DD_OK)
	{
		ShowMessage("SetClipper FAILED");
		return FALSE;
	}
	//理所当然要减少引用计数
	_RELEASE( lpClipper );
	return TRUE;
}

//******************************************************************
//初始化DirectDraw环境
BOOL InitDDraw(void)
{
	DWORD			 dwFlags;
	DDSURFACEDESC	 ddsd;
	HRESULT          ddrval;

	if( WindowMode==0 )	//全屏
	{
		//设置协作级别为全屏模式
		dwFlags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT;	//DDSCL_ALLOWMODEX |
		lpDD->SetCooperativeLevel(hWnd, dwFlags);

		//设置显示模式
		lpDD->SetDisplayMode( ScreenWidth, ScreenHeight, ScreenColor);
    
		// 创建主页面
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
		//创建一个后台表面
		CreateBitmap(lpDDSBack, ScreenWidth, ScreenHeight);

 		// 测试像素格式是555还是565
		DDPIXELFORMAT DDPixelFormat;
		ZeroMemory(&DDPixelFormat, sizeof(DDPixelFormat));
		DDPixelFormat.dwSize = sizeof(DDPixelFormat);

		if (lpDDSPrimary->GetPixelFormat(&DDPixelFormat) != DD_OK)
			return 1;

		RMask=(WORD)DDPixelFormat.dwRBitMask;
		GMask=(WORD)DDPixelFormat.dwGBitMask;
		BMask=(WORD)DDPixelFormat.dwBBitMask;
		//扩展为32位 00000gggggg00000rrrrr000000bbbbb
		rgbMask=((DWORD)GMask<<16)|RMask|BMask;	
		//0x07e0 = 00000|11111|00000b   五位的绿色
		if (GMask == 0x07e0 ) 
			Is555 = 0;

		if( Is555 )
		{
			ColorKey16=0x00007c1f;		//00000|11111|00000|11111b    (255,0,255)
			RMove=10;					//R要向右10位
			GMove=5;					//G要向右5位
		}
		else
		{
			ColorKey16=0x0000f81f;		//0000|11111|000000|11111b     (255,0,255)
			RMove=11;					//R要向右11位
			GMove=6;					//注意这儿了，为什么是6呢？因为在进行灰度运算的时候，
		}								//一个6位和两个五位运算会超出5位，所以……呵呵，soft
										//在这儿一句注释都没有，把我可想苦了：)这下改明白了吧
											
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else	//窗口
	{
		//设置协作级别为窗口模式
		dwFlags = DDSCL_NORMAL;
		lpDD->SetCooperativeLevel(hWnd, dwFlags);

		// 创建主页面
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
		if( ddrval != DD_OK )
		{
			return false;
		}

		//生成剪裁器
		CreateClipper();

		//生成背景页面
		CreateBitmap(lpDDSBack, ScreenWidth, ScreenHeight);
		
		// 测试像素格式是555还是565
		DDPIXELFORMAT DDPixelFormat;
		ZeroMemory(&DDPixelFormat, sizeof(DDPixelFormat));
		DDPixelFormat.dwSize = sizeof(DDPixelFormat);
		if (lpDDSPrimary->GetPixelFormat(&DDPixelFormat) != DD_OK)
			return 1;
		RMask=(WORD)DDPixelFormat.dwRBitMask;
		GMask=(WORD)DDPixelFormat.dwGBitMask;
		BMask=(WORD)DDPixelFormat.dwBBitMask;
		//扩展为32位 00000gggggg00000rrrrr000000bbbbb
		//下面的部分，看上面的注释好了，我就不再写了
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
//应用程序入口
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow)
{
	MSG		msg;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// 检测内存泄漏

	//读入游戏启动配置文件-(读取窗口模式)
	CIniSet Ini("ini\\setup.ini");
	WindowMode=Ini.ReadInt("Setup", "WindowMode");
	
	int w=GetSystemMetrics(SM_CXSCREEN); 
	int h=GetSystemMetrics(SM_CYSCREEN);
	HDC hdc=GetDC(hWnd);
	int color=GetDeviceCaps(hdc, BITSPIXEL);

	ReleaseDC(hWnd,hdc);

	if( WindowMode != 0 && WindowMode != 1 )
	{
		if( IDYES==MessageBox(hWnd, "使用窗口显示模式吗？\n[Alt+Enter] 切换全屏或窗口显示."
						, "注意", MB_YESNO))
			WindowMode=1;
		else
			WindowMode=0;
		bSwitch=true;
	}

	//如果不是16位色，或者屏幕小于800*600则定为全屏模式
	if( color != 16 || w < 800 )
	{
		if( color != 16 )
			MessageBox(hWnd, "注意：当前Windows不是16位色，如果希望在窗口模式运行游戏\
						，请把桌面设置为16位色!","注意",MB_OK);
		else
			MessageBox(hWnd, "注意：当前Windows屏幕小于800*600，如果希望在窗口模式运行\
				游戏，请把桌面设置为800*600以上，否则只能在全屏模式下运行!","注意",MB_OK);

		bSwitch=false;
		WindowMode=0;
	}	

	//初始化主窗口
	hWnd=InitWindow( hInstance, nCmdShow ) ;
	if (hWnd == NULL )
		return FALSE;
	
	hInst=hInstance;

	//初始化DirectDraw环境，并实现DirectDraw功能
	DirectDrawCreate(NULL, &lpDD, NULL);	//创建DirectDraw对象
	if (!InitDDraw())
	{
		MessageBox(hWnd, "初始化DirectDraw过程中出错！请检查你是否正确的安装\
					了DirectX。", "Error", MB_OK );
		FreeDDraw();
		return FALSE;
	}

	//初始化DirectInput
	if( !InitInput() )
	{
		MessageBox(hWnd, "初始化DirectInput过程出错！请检查你是否\
			`			正确的安装了DirectX 5.0以上版本。", "Error", MB_OK );
		FreeInput();
		return FALSE;
	}

	//初始化DirectInput_Mouse
	if( !Mouse.InitInput_Mouse(hWnd) )
	{
		MessageBox(hWnd, "初始化DirectInput Mouse出错！请检查你是否正确的安装\
					了DirectX 5.0以上版本。", "Error", MB_OK );
		Mouse.FreeInput_Mouse();
		return FALSE;
	}

	//初始化DirectInput_Key
	if( !Key.InitInput_Key(hWnd) )
	{
		MessageBox(hWnd, "初始化DirectInput Keyboard出错！请检查你是否正确的安装\
						了DirectX 5.0以上版本。", "Error", MB_OK );
		Key.FreeInput_Key();
		return FALSE;
	}

	//初始化DirectInput_Joystick
	if( !g_Joystick.InitInput_Joystick(hWnd) )
	{
		g_Joystick.FreeInput_Joystick();
		return FALSE;
	}

	//初始化DirectInput_Sound
	if( Ini.ReadInt("Setup","游戏音效") && !CSound::InitDirectSound(hWnd) )
	{
		CSound::EnableSound(false);
		CSound::ReleaseDirectSound();
	}

	//初始化DirectMusic音乐
	if( Ini.ReadInt("Setup","背景音乐") )
	{
		CMusic::InitMusic();
	}

	//初始化随机指数
	srand( (unsigned)time( NULL ) ); 

	SetCursor(NULL);

	//读取数据等初始化操作
	if( !InitGame() )
	{
		MessageBox(hWnd, "初始化游戏数据过程出错！请检查你的文件", "Error", MB_OK );
		FreeDDraw();
		return FALSE;
	}

	while( RunGame )		//程序主循环
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
			MainLoop();		//进入游戏主循环
		}
		else WaitMessage();	//等待消息
	}

	ReleaseGame();					//释放数据
	CSound::ReleaseDirectSound();	//释放DirectSound对象
	g_Joystick.FreeInput_Joystick();
	Key.FreeInput_Key();
	Mouse.FreeInput_Mouse();
	FreeInput();
	FreeDDraw();					//释放所有DirectDraw对象

	//如果显示模式改变则记录到配置文件
	if( bSwitch && WindowMode != Ini.ReadInt("Setup", "WindowMode") )
	{
		Ini.WriteInt("Setup", "WindowMode", WindowMode);
		Ini.Save();
	}

	return 1;
}