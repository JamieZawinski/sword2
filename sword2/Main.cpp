//********************************************
//	主模块 相关函数
//  创建于2000年4月日10日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "menu\menu_inc.h"
#include "mapedit\mapedit.h"
#include "mapedit\dialog.h"
#include "main.h"
#include "cursor.h"
#include "map.h"
#include "findpath.h"
#include "role.h"
#include "npc.h"
#include "talk.h"
#include "message.h"
#include "script.h"
#include "events.h"
#include "fight\fight.h"
#include "resource.h"

//////////////////////////////////
//通用变量
//////////////////////////////////

LPDIRECTDRAWSURFACE lpDDSSour=NULL, lpDDSTemp=NULL;	//临时页面
LPDIRECTDRAWSURFACE lpDDSBackGround;				// 背景页面
LPDIRECTDRAWSURFACE lpDDSTools;						// 编辑器专用小图标
LPDIRECTDRAWSURFACE *lpDDSMap;						// 地图地面页面
LPDIRECTDRAWSURFACE lpDDSMask;						// 光照图

enum ePLAYSTATE g_ePlayState=PS_MAIN;				//游戏当前状态
enum ePLAYSTATE g_ePlayState_Old=PS_MAIN;
enum eDISPLAYSTATE g_eDisplayState=DS_NORMAL;		//当前显示模式

POINT point;		//鼠标坐标
int cx,cy;			//鼠标格子

int StartX=0,StartY=0;				//主区起始坐标
int EndX=639,EndY=479;				//主区结束坐标
int MainWidth=640,MainHeight=480;	//主区大小
int ShowCellW=20, ShowCellH=15;		//显示区域格子数

int RoleNum;				//主角数量（1-4）

CIniSet GameIni;			//游戏配置文件
CScript Script;				//游戏脚本机
CMessage Message;			//游戏消息处理
CEvents Events;				//特殊事件
CTitle Title;				//封面
CMenu Menu;					//菜单
CMap map;					//当前地图
CGpFont font(true);			//字体对象
CGpFont DebugFont(true);	//调试字体对象
CRole role[4];				//角色
CTalk talk;					//对话
CMsgWindow GameMessage;		//游戏消息框
CSound Sound;				//音效
CMusic Music;				//背景音乐
CProess Proess;				//进度条
unsigned mouse, key, joystick;	//鼠标、键盘和手柄的缓冲区数据接受
bool bControl=true;			//是否可控制
CFight Fight;				//战斗

char CurrentPath[MAX_PATH];	//当前工作目录
int CurrentNpc=-1;			//当前鼠标指向的NPC编号(-1=NULL)
int NowTick=0, OldTick=0, DelayTick=40;
extern char strHelp[];		//帮助
extern char *CGetDlgMessage;	//提示文字
extern char *CGetStringDlgBuffer;	//字符串
extern HMENU pmmParent;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//游戏初始化
BOOL InitGame()
{
	char *strTemp=NULL;

	//创建后台临时表面
	CreateBitmap(lpDDSBackGround, ScreenWidth, ScreenHeight);
	CreateBitmap(lpDDSSour, ScreenWidth, ScreenHeight);
	CreateBitmap(lpDDSTemp, ScreenWidth, ScreenHeight);
	Events.GoldPointLogo("pic\\interface\\logo.bmp", RGB(255,255,255), 
							190, 170, 272, 112, 7);
	//获取当前路径
	GetCurrentDirectory(256, CurrentPath);

	//打开游戏主配置文件
	GameIni.Open("ini\\heroland.ini");

	//透明色
	int r=GameIni.ReadInt("ColorKey","Red");
	int g=GameIni.ReadInt("ColorKey","Green");
	int b=GameIni.ReadInt("ColorKey","Blue");
	ColorKey=RGB(r,g,b);

	//初始化字体
	font.InitFont(16, "宋体");

	//初始化字体
	strTemp = GameIni.ReadText("配置文件","字体");
	DebugFont.LoadFont(strTemp, "版本字体");
	_FREE(strTemp);

	// 进度条
	Proess.LoadIni("ini\\menu\\proess.ini", "进度条");

	//初始化编辑器专用图素图片
	strTemp=GameIni.ReadText("编辑器","ToolPic");
	CreateBitmap(lpDDSTools, 0,0, strTemp);
	DDSetColorKey(lpDDSTools, ColorKey);
	_FREE(strTemp);

	//初始化光线图片
	CreateBitmap(lpDDSMask, 0,0, "pic\\mask.bmp" );

	//初始化图素图片
	Max_GroundPic=GameIni.ReadInt("地图元素","Max_GroundPic");
	lpDDSMap=new LPDIRECTDRAWSURFACE[Max_GroundPic];
	for(int i=0; i<Max_GroundPic; i++)
	{
		strTemp = GameIni.ReadText("地图元素",i);
		CreateBitmap(lpDDSMap[i], 0,0, strTemp);
		DDSetColorKey(lpDDSMap[i], ColorKey);
		_FREE(strTemp);
	}

	//初始化光标
	strTemp = GameIni.ReadText("配置文件","光标");
	CCursor::Init(strTemp, "header");
	_FREE(strTemp);

	//初始化对话框
	strTemp = GameIni.ReadText("配置文件","对话框");
	talk.Init(strTemp, "普通对话");
	_FREE(strTemp);

	//初始化消息框
	strTemp = GameIni.ReadText("配置文件","消息框");
	GameMessage.LoadMsgWindowIni(strTemp, "游戏消息");
	_FREE(strTemp);

	//初始化菜单系统
	strTemp = GameIni.ReadText("配置文件","菜单系统");
	Menu.LoadMenuIni(strTemp, "菜单系统");
	_FREE(strTemp);

	strTemp = GameIni.ReadText("配置文件","物品列表");
	Menu.Goods.LoadGoodsIni(strTemp);
	_FREE(strTemp);

	strTemp = GameIni.ReadText("配置文件","魔法列表");
	Menu.Magic.LoadMagicIni(strTemp);
	_FREE(strTemp);

	//初始化地图
	map.InitMap(32,32);
	strTemp = GameIni.ReadText("配置文件", "动态图块");
	map.LoadAniCell(strTemp, "动态图块");
	_FREE(strTemp);

	talk.OpenFile("maps\\talk.txt");

	//初始化脚本解释机
	strTemp = GameIni.ReadText("配置文件","脚本指令");
	char *index = GameIni.ReadText("配置文件","脚本变量");
	Script.Init(strTemp, index);
	_FREE(strTemp);
	_FREE(index);

	//初始化战斗
	Fight.Init();

	//初始化敌人列表
	CFight::LoadEnemyList("ini\\fight\\enemy.ini", "敌人列表");

	//为新游戏初试数据
	NewGame();

	//读入模式
	g_ePlayState = g_ePlayState_Old = (enum ePLAYSTATE)GameIni.ReadInt("Start","PlayState");
	g_eDisplayState = (enum eDISPLAYSTATE)GameIni.ReadInt("Start","DisplayState");
	SetTimer(hWnd, 1, 5000, NULL);	//时钟

	//debug方式
	g_bDebug = GameIni.ReadInt("Start","Debug");

	//封面
	Title.LoadTitleIni( "ini\\menu\\title.ini", "封面");
	Title.DoMenu(lpDDSBack);

	return true;
}

//新游戏初始化数据
void NewGame()
{
	//初始化角色
	RoleNum = GameIni.ReadInt("角色","主角数量");
	char* strTemp = GameIni.ReadText("角色","主角");
	for(int i=0; i<RoleNum; i++)
	{
		role[i].LoadRoleIni(strTemp, GameIni.ReadText("角色", i));
	}

	_FREE(strTemp);
	map.LoadMap("maps\\word\\null.map");
	role[0].bShowed = false;
	Script.RestoreValue();
}

//--------------------------------------------------

//结束游戏，释放内存
BOOL ReleaseGame()
{
	int i;

	CCursor::Release();
	CFindPath cfp(&map);
	cfp.Release();

	//地面图素
	for(i=0; i<Max_GroundPic; i++)
	{
		_RELEASE( lpDDSMap[i] );
	}
	_DELETE( lpDDSMap );

	//释放
	_RELEASE( lpDDSBackGround );
	_RELEASE( lpDDSSour );
	_RELEASE( lpDDSTemp );

	if(pmmParent)
		DestroyMenu(pmmParent);

	_FREE(CGetStringDlgBuffer);	
	Script.Release();
	return true;
}

//--------------------------------------------------

//获取输入信息
void GetInputData()
{
	//获取鼠标当前的格子坐标
	GetCursorPos(&point);
	if( WindowMode != 0 )	//窗口
	{
		point.x-=RectWindow.left;
		point.y-=RectWindow.top;

		if( point.x < 0 ) 
			point.x = 0;
		if( point.x >= ScreenWidth ) 
			point.x = ScreenWidth-1;

		if( point.y < 0 ) 
			point.y = 0;
		if( point.y >= ScreenHeight ) 
			point.y = ScreenHeight-1;

	}

	cx=map.GetCellX(point.x, point.y);
	cy=map.GetCellY(point.x, point.y);

	Mouse.GetMouseState();			//获取鼠标立即信息
	mouse=Mouse.GetMouseData();		//获取鼠标缓冲信息
	Key.GetKeyData();				//获取键盘缓冲数据(附带即时数据)
	key=Key.GetKey();				//获取键盘数据
	g_Joystick.GetJoystickState();	//获取手柄立即数据
	g_Joystick.GetJoystickData();	//获取手柄缓冲数据
	joystick=g_Joystick.GetKey();
}

//主循环
void MainLoop()
{
	//延时控速
	NowTick=timeGetTime();
	if( NowTick < OldTick+DelayTick ) 
		return;
	OldTick=NowTick;

	//获取输入信息
	GetInputData();

	switch( g_ePlayState )
	{
	case PS_Null:	//空循环
		break;

	case PS_MAIN:	//游戏主循环
		GameLoop();
		break;

	case PS_SCROLLMAP:	//卷动地图
		map.ScrollLoop();
		break;

	case PS_EDIT:	//编辑器主循环
		EditLoop();
		break;
	}
	//取NPC的坐标
	for(int i=0; i<map.m_vNpc.size(); i++)
	{
		map.m_vNpc[i]->State.X=_grid_x(map.m_vNpc[i]->State.x);
		map.m_vNpc[i]->State.Y=_grid_y(map.m_vNpc[i]->State.y);
	}

	//计算当前地图左上角坐标
	map.sx=_point_x(map.SX)-map.DX;
	map.sy=_point_y(map.SY)-map.DY;

	//更新屏幕
	if( g_ePlayState == g_ePlayState_Old )
	{
		switch( g_ePlayState )
		{
		case PS_Null:	//空循环
			break;

		case PS_MAIN:	//游戏主循环
			if( Message.IsEmpty() && bControl )
				ShowMouseCursor();
			else
				ShowMouseCursor(lpDDSBack,false);
			UpdateScreen();
			break;

		case PS_SCROLLMAP:	//卷动地图
			ShowMouseCursor();
			UpdateScreen();
			break;

		case PS_EDIT:	//编辑器主循环
			UpdateScreen();
			break;
		}
	}
	else
		g_ePlayState_Old = g_ePlayState;


	// 运行脚本消息
	Message.Run();
}

//--------------------------------------------------

//--------------------------------------------------

//游戏主循环
void GameLoop()
{
	if( Message.IsEmpty() && bControl )	//是否可以控制
	{
		for(int i =0;i<map.m_vNpc.size();i++)
			//获取当前指向NPC
			if( cx == map.m_vNpc[i]->State.X && cy == map.m_vNpc[i]->State.Y )
			{
				CurrentNpc=i;
			}
	//移动
		if( (!role[0].bMoving|| map.Cell[map.Width * role[0].State.Y + 
								role[0].State.X].CP == BLOCK_CP ))
		{
			if( LEFT_ON )
			{
				role[0].MoveTo(role[0].State.x-CellWidth, role[0].State.y);
				Fight.RandomStart(map.GetEnemyFrequency());	//踩地雷
			}

			else if( RIGHT_ON )
			{
				role[0].MoveTo(role[0].State.x+CellWidth, role[0].State.y);
				Fight.RandomStart(map.GetEnemyFrequency());	//踩地雷
			}

			else if( UP_ON )
			{
				role[0].MoveTo(role[0].State.x, role[0].State.y-CellHeight);
				Fight.RandomStart(map.GetEnemyFrequency());	//踩地雷
			}
			
			else if( DOWN_ON )
			{
				role[0].MoveTo(role[0].State.x, role[0].State.y+CellHeight);
				Fight.RandomStart(map.GetEnemyFrequency());	//踩地雷
			}

			//触发脚本
			if( A_UP )
			{
				// 触发NPC脚本
				static int dir[4][2]={ 0,1, -1,0, 0,-1, 1,0 };
				int npcx=role[0].State.X+dir[role[0].State.CurrentDir][0];
				int npcy=role[0].State.Y+dir[role[0].State.CurrentDir][1];
				for(int i=0; i<map.m_vNpc.size(); i++)
				{
					if( map.m_vNpc[i]->State.X==npcx && map.m_vNpc[i]->State.Y==npcy )
					{
						CurrentNpc=i;
						char *NpcName=new char[32];
						strcpy(NpcName, map.m_vNpc[CurrentNpc]->Name);
						//调整NPC的位置，让他停下来说话
						map.m_vNpc[CurrentNpc]->AdjustPosition();
						BOOL bAct=map.m_vNpc[CurrentNpc]->State.bActive;
						//npc停止活动
						Message.PutMessage( MSG_SETNPCACTIVE, false, CurrentNpc, 1);
						char *ScriptFile = (char*)malloc(sizeof(char)*64);;
						strcpy(ScriptFile,map.m_vNpc[CurrentNpc]->ScriptFile);
						//调整主角和NPC的方向,发生NPC脚本
						Message.PutMessage( MSG_TALKDIR, CurrentNpc,0,0,ScriptFile );
						//npc恢复活动
						Message.PutMessage( MSG_SETNPCACTIVE, bAct, CurrentNpc, 1);
						_DELETE_ARRAY(NpcName);
					}
				}

				// 触发陷阱脚本
				role[0].RunScript();
			}
		}

		if( mouse == LB_UP )
		{
			if( CurrentNpc != -1 )	//和NPC对话
			{
				BOOL bAct=map.m_vNpc[CurrentNpc]->State.bActive;
				char *NpcName=new char[32];
				strcpy(NpcName, map.m_vNpc[CurrentNpc]->Name);
				map.m_vNpc[CurrentNpc]->AdjustPosition();
				Message.PutMessage( MSG_SETNPCACTIVE, false, CurrentNpc, 1);//npc停止活动
				int x,y;
				//计算坐标
				if(map.m_vNpc[CurrentNpc]->Path)
				{
					x = map.m_vNpc[CurrentNpc]->Path[map.m_vNpc[CurrentNpc]->PathSteps-1].x;
					y = map.m_vNpc[CurrentNpc]->Path[map.m_vNpc[CurrentNpc]->PathSteps-1].y;
				}
				else
				{
					x = map.m_vNpc[CurrentNpc]->State.X;
					y = map.m_vNpc[CurrentNpc]->State.Y;
				}
				Message.PutMessage( MSG_MOVE, x,y,1);	//主角移动过去
				char *ScriptFile = (char*)malloc(sizeof(char)*64);;
				strcpy(ScriptFile,map.m_vNpc[CurrentNpc]->ScriptFile);
				//调整主角和NPC的方向，发生NPC脚本
				Message.PutMessage( MSG_TALKDIR, CurrentNpc,bAct,0,ScriptFile);	
				_DELETE_ARRAY(NpcName);
			}
			else	//行走
			{
				//踩地雷
				if( Fight.RandomStart(map.GetEnemyFrequency()<<2) )
					return;

				//窗口
				if( WindowMode!=0 )
				{
					if( point.x >= 0 && point.x<ScreenWidth && point.y>=0 && point.y<ScreenHeight )
						role[0].MoveTo(_point_x(cx), _point_y(cy));
				}
				else	//全屏
				{
					role[0].MoveTo(_point_x(cx), _point_y(cy));
				}

				//显示目的地光标
				CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetPos(map.sx+point.x, map.sy+point.y);
				CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(3);
			}
		}

		//菜单
		if( B_UP || mouse == RB_UP )
		{
			Menu.DoMenuLoop();
		}

		if( mouse == RB_UP && Key.KeyDown(DIK_LCONTROL) )
		{
			Message.PutMessage( MSG_SETPOSITION, cx, cy);
			Message.PutMessage( MSG_CENTERMAP, cx, cy );
			role[0].bMoving=false;
		}
	}

	if( key == DIK_F11 )		//test战斗
	{
		key = 0;
		Fight.RandomStart(12000);
	}

	//显示地图
	map.ShowMap();
}

//*****************************
//处理主窗口消息
LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static bool bMusicPlaying=false;
	static bool bFMusicPlaying = false;
	switch( message )
	{
	case WM_ACTIVATE:	//应用程序是否活动

		if(LOWORD(wParam)==WA_INACTIVE)
			bActive = false;
		else if(HIWORD(wParam))
		{
			bActive = false;
			return 0;
		}
		else
			bActive = true;
		//重新获取输入设备
		Mouse.SetAcquire(hWnd);
		Key.SetAcquire(hWnd);
		g_Joystick.SetAcquire(hWnd);
		Mouse.GetMouseData();
		Mouse.ReleaseMouseBuffer();

		//保护页面
		if( g_eDisplayState==DS_NIGHT && WindowMode !=0 && !bActive )
		{
			GetPrimarySurface(lpDDSBack);
		}

		//背景音乐处理
		{
			if( !bActive )
			{
				if( Music.IsPlay() )
				{
					bMusicPlaying=true;
					Music.Pause();
				}
				else 
					bMusicPlaying=false;
				if(g_ePlayState==PS_FIGHT)
				{
					if(Fight.IsPlayingMusic())
					{
						bFMusicPlaying = true;
						Fight.PauseMusic();
					}
					else
						bFMusicPlaying = false;
				}
			}

			else
			{
				if(bMusicPlaying )
					Music.Resume();
				if(g_ePlayState == PS_FIGHT&&bFMusicPlaying)
				{
					Fight.ResumeMusic();
				}
			}
		}
		return 0;

	case WM_SYSKEYDOWN:
		switch( wParam )
		{
		case VK_RETURN:		//切换全屏或窗口
			if ( bSwitch && bActive )
			{
				if( WindowMode == 0 )
					lpDD->RestoreDisplayMode();
				GetWindowRect(hWnd, &RectWindow);
				WindowMode = 1-WindowMode;
				ChangeCoopLevel(hWnd);
			}
			break;

		case 'Q':
		case 'q':
		case VK_F4:
			if( MessageBox(hWnd, "真的要退出吗？", "圣剑英雄传 II", MB_YESNO) == IDYES )
				RunGame=false;
			return 0;

		case VK_F12:
			g_bDebug = !g_bDebug;
			break;

		case 18:	// 屏蔽alt
			return 0;
		}
		break;

    case WM_KEYDOWN://击键消息
        switch( wParam )
        {

		case VK_SUBTRACT:
			DelayTick+=33;
			if( DelayTick>1000 )
				DelayTick=1000;
			break;

		case VK_ADD:
			DelayTick-=33;
			if( DelayTick<0 )
				DelayTick=0;
			break;

		case VK_F2:
			Script.Run("script\\test.gps");
			break;

		case VK_F5:			//切换模式
			if( g_ePlayState==PS_EDIT )
			{
				_DELETE(UndoMap);
				g_ePlayState=PS_MAIN;
				bShowHardCursor=false;
				ShowCursor(FALSE);
			}
			else if( g_ePlayState==PS_MAIN )
			{
				UndoMap=new Cell_Struct[map.Width*map.Height];
				memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
				g_ePlayState=PS_EDIT;
				bShowHardCursor=true;
				ShowCursor(TRUE);
			}
			break;

		case VK_F6:			//查看变量
			{
				char *Var=GetStringDlg("请输入要查看的变量名称:");
				if( Var )
					ShowMessage("%s = %d", Var, Script.GetValue(Var));
			}
			break;

		case VK_F7:			//切换模式
			if( g_eDisplayState==DS_NORMAL )
			{
				g_eDisplayState=DS_NIGHT;
				map.CountDisplayGrid();
				Clrscr(lpDDSPrimary, RGB(0,0,0));
			}
			else if( g_eDisplayState==DS_NIGHT )
			{
				g_eDisplayState=DS_NORMAL;
				map.CountDisplayGrid();
			}
			break;

		case VK_F8:			//执行脚本
			{
				char *Var=GetStringDlg("请输入要执行的脚本语句:");
				if( Var == NULL ) break;
				Script.RunCommand(Var);
			}
			break;

		case VK_F4:		//切换全屏或窗口
			if ( bSwitch && bActive )
			{
				if( WindowMode == 0 )
					lpDD->RestoreDisplayMode();
				GetWindowRect(hWnd, &RectWindow);
				WindowMode = 1-WindowMode;
				ChangeCoopLevel(hWnd);
			}
			break;

		case VK_F12:		//显示FPS
			bShowFps=!bShowFps;
			break;
		
		case 'h':			//隐藏模式的加mp，hp方法
		case 'H':
			{
				if(g_ePlayState!=PS_MAIN||!g_bDebug||!GetAsyncKeyState(VK_CONTROL))
					break;
				for(int i =0 ;i<RoleNum;i++)
				{
					role[i].Propertiy.Hp = role[i].Propertiy.MaxHp;
					role[i].Propertiy.Mp = role[i].Propertiy.MaxMp;
				}
			}
			break;

		case 'l':			//直接升级
		case 'L':
			{
				if(g_ePlayState!=PS_MAIN||!g_bDebug||!GetAsyncKeyState(VK_CONTROL))
					break;
				for(int i =0 ;i<RoleNum;i++)
				{
					role[i].Propertiy.Level++;						
					//算下升一级所需要的经验值
					role[i].Propertiy.MaxExp += 10*(role[i].Propertiy.Level+1)*
														(role[i].Propertiy.Level)+30;
					role[i].Propertiy.Exp = role[i].Propertiy.MaxExp;
					role[i].Propertiy.MaxHp+=200;						//HP加20
					role[i].Propertiy.Hp=role[i].Propertiy.MaxHp;				//把HP加满
					role[i].Propertiy.MaxMp+=60;
					role[i].Propertiy.Mp=role[i].Propertiy.MaxMp;
					
					role[i].oldAttack+=80;
					role[i].oldDefend+=40;
					role[i].Propertiy.Magic+=3;
					
					role[i].Propertiy.Stamina+=3;
					role[i].Propertiy.Luck+=3;
					role[i].Propertiy.Speed+=3;
					role[i].Propertiy.Bp=100;
					role[i].Propertiy.Ip=100;
					role[i].ReCalcPro();
				}
			}
			break;
		}
		break;

	case WM_TIMER:	//定时器消息
		if(!bActive)
			return 0;
		switch( wParam )
		{
		case 1:		//Music
			if( g_ePlayState!=PS_FIGHT && CMusic::bPlay && 
							!Music.IsPlay() &&!bMusicPlaying)
			{
				Music.Play();
			}
			break;

		case 2:		//Fight
			if( g_ePlayState==PS_FIGHT )
			{
				Fight.OnTimer();
			}
			break;
		}
		return 0;

	// 处理地图编辑器的右键命令
	case WM_COMMAND:
		Mouse.ReleaseMouseBuffer();
		MapEditProc(LOWORD(wParam));
		Mouse.ReleaseMouseBuffer();
		break;

	case WM_PAINT:		//在后台窗口模式时
		if( !bActive &&WindowMode!=0 )
		{
		    while (TRUE)
            {
				int hRet = lpDDSPrimary->Blt(&RectWindow, lpDDSBack, 
											&RectScreen, DDBLT_WAIT, NULL);
                if (hRet == DD_OK)
                    break;
                if (hRet == DDERR_SURFACELOST)
                {
                    hRet = lpDDSPrimary->Restore();
                    if (hRet != DD_OK )
                    	break;
                }
                if (hRet != DDERR_WASSTILLDRAWING)
                    break;
            }
        }
        break;

	case WM_SIZE:
		if(wParam==SIZE_RESTORED)
		{
			GetWindowRect(hWnd,&RectWindow);
			RectWindow.top += g_pointClient.y;
			RectWindow.left += g_pointClient.x;
			RectWindow.right = RectWindow.left + ScreenWidth;
			RectWindow.bottom = RectWindow.top + ScreenHeight;
		}
		return 0;

	case WM_ENTERMENULOOP:
		if(Music.IsPlay())
		{
			bMusicPlaying=true;
			Music.Pause();
		}
		else
			bMusicPlaying=false;
		if(Fight.IsPlayingMusic())
		{
			bFMusicPlaying = true;
			Fight.PauseMusic();
		}
		else
			bFMusicPlaying = false;

		return 0;

	case WM_EXITMENULOOP:
		if(g_ePlayState == PS_FIGHT&&bFMusicPlaying)
			Fight.ResumeMusic();
		if(bMusicPlaying)
			Music.Resume();

		return 0;

	case WM_MOVE:		//窗口移动
        if ( bActive && WindowMode!=0 )
        {
            GetWindowRect(hWnd, &RectWindow);
			RectWindow.top += g_pointClient.y;
			RectWindow.left += g_pointClient.x;
			RectWindow.right = RectWindow.left + ScreenWidth;
			RectWindow.bottom = RectWindow.top + ScreenHeight;
		}
		break;

	case WM_SETCURSOR:	 //设置光标
		if( bActive && !bShowHardCursor )
		{
			SetCursor(NULL);
			return true;
		}
		break;

    case WM_DESTROY:	//退出消息
		PostQuitMessage( 0 );
		RunGame=false;
        break;

	case WM_GETMINMAXINFO:
		{
			// 窗口模式下不让改变大小 
			// 窗口固定到 640x480 (客户区)
			MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;
			
			DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
			DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
			//DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
			DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
			
			pMinMax->ptMinTrackSize.x = MainWidth  + dwFrameWidth * 2;
			pMinMax->ptMinTrackSize.y = MainHeight + dwFrameHeight * 2 + 
				dwCaptionHeight;
			
			pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
			pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
		}
		return 0L;
    }
	
	//调用缺省消息处理过程
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//-----------------------------------------------

//更新到屏幕或表面lpSurf
void UpdateScreen(LPDIRECTDRAWSURFACE lpSurf, RECT DestRect)
{
	if( g_eDisplayState==DS_NORMAL )	//正常显示
	{
		if( bShowFps ) 
			font.PrintText(lpDDSBack, 20, 460,"FPS:%d", CountSpeed());
		//debug模式的显示
		if( g_bDebug&&g_ePlayState == PS_MAIN)
		{
			DebugFont.PrintText(lpDDSBack,500,400,"Cheating Enable");
			DebugFont.PrintText(lpDDSBack,500,440,
							"Process = %d",Script.GetValue("$process"));
			DebugFont.PrintText(lpDDSBack,500,410,"Hp = %d...Mp = %d",role[0].Propertiy.Hp,role[0].Propertiy.Mp);
			DebugFont.PrintText(lpDDSBack,500,420,"Ap = %d...Dp = %d",role[0].Propertiy.Attack,role[0].Propertiy.Defend);
			DebugFont.PrintText(lpDDSBack,500,430,"Level = %d",role[0].Propertiy.Level);
			DebugFont.PrintText(lpDDSBack,100,100,"(%d,%d)",point.x,point.y);
		}
		
		if( WindowMode==0 )	//全屏
		{
			Blt(lpSurf, 0, 0, lpDDSBack, RectScreen, false);
		}
		else	//窗口
		{
			lpSurf->Blt( &DestRect, lpDDSBack, &RectScreen, DDBLTFAST_NOCOLORKEY, 0);
		}
	}
	else if( g_eDisplayState==DS_NIGHT )	//夜晚
	{
		if( bShowFps ) 
			font.PrintText(lpDDSBack, 220, 260, "FPS:%d", CountSpeed());
		
		int sx= role[0].State.x - map.sx - _point_x(6);
		int sy= role[0].State.y - map.sy - _point_y(5);

		Alpha_Blt(lpDDSBack, sx, sy, lpDDSMask, GetRect(0,0,384,288));

		if( WindowMode==0 )	//全屏
		{
			RECT s_rect={sx, sy, sx+384, sy+288};
			Blt(lpSurf, sx, sy, lpDDSBack, s_rect, false);
		}
		else	//窗口
		{

			RECT d_rect={DestRect.left+sx, DestRect.top+sy, 
							DestRect.left+sx+384, DestRect.top+sy+288};
			RECT s_rect={sx, sy, sx+384, sy+288};
			
			if( d_rect.left<DestRect.left ) 
			{
				s_rect.left=s_rect.left+DestRect.left-d_rect.left;
				d_rect.left=DestRect.left;
			}
			if( d_rect.top<DestRect.top )
			{
				s_rect.top=s_rect.top+DestRect.top-d_rect.top;
				d_rect.top=DestRect.top;
			}
			if( d_rect.right>DestRect.right )
			{
				s_rect.right-=d_rect.right-DestRect.right;
				d_rect.right=DestRect.right;
			}
			if( d_rect.bottom>DestRect.bottom )
			{
				s_rect.bottom-=d_rect.bottom-DestRect.bottom;
				d_rect.bottom=DestRect.bottom;
			}

			_SBlt(lpSurf, d_rect, lpDDSBack, s_rect, false);
			
		}
	}
	nFrames++;	//桢数
}

//更新到屏幕-对话
void UpdateScreen_Dialog(LPDIRECTDRAWSURFACE lpDDSurf)
{
	if( WindowMode==0 )		//全屏
		Blt(lpDDSPrimary, 0, 0, lpDDSurf, RectScreen, false);
	else
		lpDDSPrimary->Blt( &RectWindow, lpDDSurf, &RectScreen, DDBLTFAST_NOCOLORKEY, 0);

	nFrames++;	//桢数
}

//画鼠标光标
void ShowMouseCursor(LPDIRECTDRAWSURFACE lpSurf, bool bNormal)
{
	//目的地光标
	CCursor::Show(CCursor::CS_CHANGEMAP, lpSurf,
				CCursor::GetCursor(1).pPic.Getx()-map.sx, 
				CCursor::GetCursor(1).pPic.Gety()-map.sy );	//显示目的地光标

	//画鼠标光标
	if( g_ePlayState == PS_MAIN )
	{
		int n = map.Width*cy+cx;
		if( bNormal && cx<map.Width && cy<map.Height )
		{
			CCursor::Show(map.Cell[n].MouseType, lpSurf, point.x, point.y);
		}
		else
		{
			CCursor::Show(CCursor::CS_NORMAL, lpSurf, point.x, point.y);
		}
	}
	else
	{
		CCursor::Show(CCursor::CS_NORMAL, lpSurf, point.x, point.y);
	}
}

//把当前屏幕复制到后台表面
void GetPrimarySurface(LPDIRECTDRAWSURFACE surf)
{
	if( WindowMode==0 )	//全屏
		Blt(surf, 0, 0, lpDDSPrimary, RectScreen, false);
	else
		Blt(surf, 0, 0, lpDDSPrimary, RectWindow, false);
}

//把游戏画面复制到一个表面上（不画鼠标和FPS等）
void GetGameSurface(LPDIRECTDRAWSURFACE surf)
{
	if( g_ePlayState == PS_MAIN ||g_ePlayState == PS_Null)
	{
		if( g_eDisplayState == DS_NIGHT )		//夜晚模式
		{
			FillSurface(surf,RGB16(0));			//清除
			map.ShowMap(false,surf);			//显示地图
			//计算alpha显示的位置
			//下面是把光圈alpha显示到表面上
			int sx= role[0].State.x - map.sx - _point_x(6);		
			int sy= role[0].State.y - map.sy - _point_y(5);
			RECT rect;
			rect = GetRect(0,0,384,288);
			int xTemp=sx,yTemp=sy;
			if(sx<0)
			{
				rect.left = 0-sx;
				sx =0;
			}
			if(sy<0)
			{
				rect.top = 0-sy;
				sy =0;
			}
			if(xTemp>ScreenWidth-384)
				rect.right = xTemp+384-ScreenWidth+sx;
			if(yTemp>ScreenHeight-288)
				rect.bottom = yTemp+288-ScreenHeight+sy;
			
			Alpha_Blt(surf, sx, sy, lpDDSMask,rect);
		}
		else
		{
			map.ShowMap(false);			//直接显示
			Blt(surf, 0, 0, lpDDSBack, RectScreen, false);
		}
	}
	else if(g_ePlayState == PS_FIGHT)
		Fight.GetFightGameSurface(surf);		//战斗的时候
}
