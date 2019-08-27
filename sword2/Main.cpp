//********************************************
//	��ģ�� ��غ���
//  ������2000��4����10��
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
//ͨ�ñ���
//////////////////////////////////

LPDIRECTDRAWSURFACE lpDDSSour=NULL, lpDDSTemp=NULL;	//��ʱҳ��
LPDIRECTDRAWSURFACE lpDDSBackGround;				// ����ҳ��
LPDIRECTDRAWSURFACE lpDDSTools;						// �༭��ר��Сͼ��
LPDIRECTDRAWSURFACE *lpDDSMap;						// ��ͼ����ҳ��
LPDIRECTDRAWSURFACE lpDDSMask;						// ����ͼ

enum ePLAYSTATE g_ePlayState=PS_MAIN;				//��Ϸ��ǰ״̬
enum ePLAYSTATE g_ePlayState_Old=PS_MAIN;
enum eDISPLAYSTATE g_eDisplayState=DS_NORMAL;		//��ǰ��ʾģʽ

POINT point;		//�������
int cx,cy;			//������

int StartX=0,StartY=0;				//������ʼ����
int EndX=639,EndY=479;				//������������
int MainWidth=640,MainHeight=480;	//������С
int ShowCellW=20, ShowCellH=15;		//��ʾ���������

int RoleNum;				//����������1-4��

CIniSet GameIni;			//��Ϸ�����ļ�
CScript Script;				//��Ϸ�ű���
CMessage Message;			//��Ϸ��Ϣ����
CEvents Events;				//�����¼�
CTitle Title;				//����
CMenu Menu;					//�˵�
CMap map;					//��ǰ��ͼ
CGpFont font(true);			//�������
CGpFont DebugFont(true);	//�����������
CRole role[4];				//��ɫ
CTalk talk;					//�Ի�
CMsgWindow GameMessage;		//��Ϸ��Ϣ��
CSound Sound;				//��Ч
CMusic Music;				//��������
CProess Proess;				//������
unsigned mouse, key, joystick;	//��ꡢ���̺��ֱ��Ļ��������ݽ���
bool bControl=true;			//�Ƿ�ɿ���
CFight Fight;				//ս��

char CurrentPath[MAX_PATH];	//��ǰ����Ŀ¼
int CurrentNpc=-1;			//��ǰ���ָ���NPC���(-1=NULL)
int NowTick=0, OldTick=0, DelayTick=40;
extern char strHelp[];		//����
extern char *CGetDlgMessage;	//��ʾ����
extern char *CGetStringDlgBuffer;	//�ַ���
extern HMENU pmmParent;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//��Ϸ��ʼ��
BOOL InitGame()
{
	char *strTemp=NULL;

	//������̨��ʱ����
	CreateBitmap(lpDDSBackGround, ScreenWidth, ScreenHeight);
	CreateBitmap(lpDDSSour, ScreenWidth, ScreenHeight);
	CreateBitmap(lpDDSTemp, ScreenWidth, ScreenHeight);
	Events.GoldPointLogo("pic\\interface\\logo.bmp", RGB(255,255,255), 
							190, 170, 272, 112, 7);
	//��ȡ��ǰ·��
	GetCurrentDirectory(256, CurrentPath);

	//����Ϸ�������ļ�
	GameIni.Open("ini\\heroland.ini");

	//͸��ɫ
	int r=GameIni.ReadInt("ColorKey","Red");
	int g=GameIni.ReadInt("ColorKey","Green");
	int b=GameIni.ReadInt("ColorKey","Blue");
	ColorKey=RGB(r,g,b);

	//��ʼ������
	font.InitFont(16, "����");

	//��ʼ������
	strTemp = GameIni.ReadText("�����ļ�","����");
	DebugFont.LoadFont(strTemp, "�汾����");
	_FREE(strTemp);

	// ������
	Proess.LoadIni("ini\\menu\\proess.ini", "������");

	//��ʼ���༭��ר��ͼ��ͼƬ
	strTemp=GameIni.ReadText("�༭��","ToolPic");
	CreateBitmap(lpDDSTools, 0,0, strTemp);
	DDSetColorKey(lpDDSTools, ColorKey);
	_FREE(strTemp);

	//��ʼ������ͼƬ
	CreateBitmap(lpDDSMask, 0,0, "pic\\mask.bmp" );

	//��ʼ��ͼ��ͼƬ
	Max_GroundPic=GameIni.ReadInt("��ͼԪ��","Max_GroundPic");
	lpDDSMap=new LPDIRECTDRAWSURFACE[Max_GroundPic];
	for(int i=0; i<Max_GroundPic; i++)
	{
		strTemp = GameIni.ReadText("��ͼԪ��",i);
		CreateBitmap(lpDDSMap[i], 0,0, strTemp);
		DDSetColorKey(lpDDSMap[i], ColorKey);
		_FREE(strTemp);
	}

	//��ʼ�����
	strTemp = GameIni.ReadText("�����ļ�","���");
	CCursor::Init(strTemp, "header");
	_FREE(strTemp);

	//��ʼ���Ի���
	strTemp = GameIni.ReadText("�����ļ�","�Ի���");
	talk.Init(strTemp, "��ͨ�Ի�");
	_FREE(strTemp);

	//��ʼ����Ϣ��
	strTemp = GameIni.ReadText("�����ļ�","��Ϣ��");
	GameMessage.LoadMsgWindowIni(strTemp, "��Ϸ��Ϣ");
	_FREE(strTemp);

	//��ʼ���˵�ϵͳ
	strTemp = GameIni.ReadText("�����ļ�","�˵�ϵͳ");
	Menu.LoadMenuIni(strTemp, "�˵�ϵͳ");
	_FREE(strTemp);

	strTemp = GameIni.ReadText("�����ļ�","��Ʒ�б�");
	Menu.Goods.LoadGoodsIni(strTemp);
	_FREE(strTemp);

	strTemp = GameIni.ReadText("�����ļ�","ħ���б�");
	Menu.Magic.LoadMagicIni(strTemp);
	_FREE(strTemp);

	//��ʼ����ͼ
	map.InitMap(32,32);
	strTemp = GameIni.ReadText("�����ļ�", "��̬ͼ��");
	map.LoadAniCell(strTemp, "��̬ͼ��");
	_FREE(strTemp);

	talk.OpenFile("maps\\talk.txt");

	//��ʼ���ű����ͻ�
	strTemp = GameIni.ReadText("�����ļ�","�ű�ָ��");
	char *index = GameIni.ReadText("�����ļ�","�ű�����");
	Script.Init(strTemp, index);
	_FREE(strTemp);
	_FREE(index);

	//��ʼ��ս��
	Fight.Init();

	//��ʼ�������б�
	CFight::LoadEnemyList("ini\\fight\\enemy.ini", "�����б�");

	//Ϊ����Ϸ��������
	NewGame();

	//����ģʽ
	g_ePlayState = g_ePlayState_Old = (enum ePLAYSTATE)GameIni.ReadInt("Start","PlayState");
	g_eDisplayState = (enum eDISPLAYSTATE)GameIni.ReadInt("Start","DisplayState");
	SetTimer(hWnd, 1, 5000, NULL);	//ʱ��

	//debug��ʽ
	g_bDebug = GameIni.ReadInt("Start","Debug");

	//����
	Title.LoadTitleIni( "ini\\menu\\title.ini", "����");
	Title.DoMenu(lpDDSBack);

	return true;
}

//����Ϸ��ʼ������
void NewGame()
{
	//��ʼ����ɫ
	RoleNum = GameIni.ReadInt("��ɫ","��������");
	char* strTemp = GameIni.ReadText("��ɫ","����");
	for(int i=0; i<RoleNum; i++)
	{
		role[i].LoadRoleIni(strTemp, GameIni.ReadText("��ɫ", i));
	}

	_FREE(strTemp);
	map.LoadMap("maps\\word\\null.map");
	role[0].bShowed = false;
	Script.RestoreValue();
}

//--------------------------------------------------

//������Ϸ���ͷ��ڴ�
BOOL ReleaseGame()
{
	int i;

	CCursor::Release();
	CFindPath cfp(&map);
	cfp.Release();

	//����ͼ��
	for(i=0; i<Max_GroundPic; i++)
	{
		_RELEASE( lpDDSMap[i] );
	}
	_DELETE( lpDDSMap );

	//�ͷ�
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

//��ȡ������Ϣ
void GetInputData()
{
	//��ȡ��굱ǰ�ĸ�������
	GetCursorPos(&point);
	if( WindowMode != 0 )	//����
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

	Mouse.GetMouseState();			//��ȡ���������Ϣ
	mouse=Mouse.GetMouseData();		//��ȡ��껺����Ϣ
	Key.GetKeyData();				//��ȡ���̻�������(������ʱ����)
	key=Key.GetKey();				//��ȡ��������
	g_Joystick.GetJoystickState();	//��ȡ�ֱ���������
	g_Joystick.GetJoystickData();	//��ȡ�ֱ���������
	joystick=g_Joystick.GetKey();
}

//��ѭ��
void MainLoop()
{
	//��ʱ����
	NowTick=timeGetTime();
	if( NowTick < OldTick+DelayTick ) 
		return;
	OldTick=NowTick;

	//��ȡ������Ϣ
	GetInputData();

	switch( g_ePlayState )
	{
	case PS_Null:	//��ѭ��
		break;

	case PS_MAIN:	//��Ϸ��ѭ��
		GameLoop();
		break;

	case PS_SCROLLMAP:	//����ͼ
		map.ScrollLoop();
		break;

	case PS_EDIT:	//�༭����ѭ��
		EditLoop();
		break;
	}
	//ȡNPC������
	for(int i=0; i<map.m_vNpc.size(); i++)
	{
		map.m_vNpc[i]->State.X=_grid_x(map.m_vNpc[i]->State.x);
		map.m_vNpc[i]->State.Y=_grid_y(map.m_vNpc[i]->State.y);
	}

	//���㵱ǰ��ͼ���Ͻ�����
	map.sx=_point_x(map.SX)-map.DX;
	map.sy=_point_y(map.SY)-map.DY;

	//������Ļ
	if( g_ePlayState == g_ePlayState_Old )
	{
		switch( g_ePlayState )
		{
		case PS_Null:	//��ѭ��
			break;

		case PS_MAIN:	//��Ϸ��ѭ��
			if( Message.IsEmpty() && bControl )
				ShowMouseCursor();
			else
				ShowMouseCursor(lpDDSBack,false);
			UpdateScreen();
			break;

		case PS_SCROLLMAP:	//����ͼ
			ShowMouseCursor();
			UpdateScreen();
			break;

		case PS_EDIT:	//�༭����ѭ��
			UpdateScreen();
			break;
		}
	}
	else
		g_ePlayState_Old = g_ePlayState;


	// ���нű���Ϣ
	Message.Run();
}

//--------------------------------------------------

//--------------------------------------------------

//��Ϸ��ѭ��
void GameLoop()
{
	if( Message.IsEmpty() && bControl )	//�Ƿ���Կ���
	{
		for(int i =0;i<map.m_vNpc.size();i++)
			//��ȡ��ǰָ��NPC
			if( cx == map.m_vNpc[i]->State.X && cy == map.m_vNpc[i]->State.Y )
			{
				CurrentNpc=i;
			}
	//�ƶ�
		if( (!role[0].bMoving|| map.Cell[map.Width * role[0].State.Y + 
								role[0].State.X].CP == BLOCK_CP ))
		{
			if( LEFT_ON )
			{
				role[0].MoveTo(role[0].State.x-CellWidth, role[0].State.y);
				Fight.RandomStart(map.GetEnemyFrequency());	//�ȵ���
			}

			else if( RIGHT_ON )
			{
				role[0].MoveTo(role[0].State.x+CellWidth, role[0].State.y);
				Fight.RandomStart(map.GetEnemyFrequency());	//�ȵ���
			}

			else if( UP_ON )
			{
				role[0].MoveTo(role[0].State.x, role[0].State.y-CellHeight);
				Fight.RandomStart(map.GetEnemyFrequency());	//�ȵ���
			}
			
			else if( DOWN_ON )
			{
				role[0].MoveTo(role[0].State.x, role[0].State.y+CellHeight);
				Fight.RandomStart(map.GetEnemyFrequency());	//�ȵ���
			}

			//�����ű�
			if( A_UP )
			{
				// ����NPC�ű�
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
						//����NPC��λ�ã�����ͣ����˵��
						map.m_vNpc[CurrentNpc]->AdjustPosition();
						BOOL bAct=map.m_vNpc[CurrentNpc]->State.bActive;
						//npcֹͣ�
						Message.PutMessage( MSG_SETNPCACTIVE, false, CurrentNpc, 1);
						char *ScriptFile = (char*)malloc(sizeof(char)*64);;
						strcpy(ScriptFile,map.m_vNpc[CurrentNpc]->ScriptFile);
						//�������Ǻ�NPC�ķ���,����NPC�ű�
						Message.PutMessage( MSG_TALKDIR, CurrentNpc,0,0,ScriptFile );
						//npc�ָ��
						Message.PutMessage( MSG_SETNPCACTIVE, bAct, CurrentNpc, 1);
						_DELETE_ARRAY(NpcName);
					}
				}

				// ��������ű�
				role[0].RunScript();
			}
		}

		if( mouse == LB_UP )
		{
			if( CurrentNpc != -1 )	//��NPC�Ի�
			{
				BOOL bAct=map.m_vNpc[CurrentNpc]->State.bActive;
				char *NpcName=new char[32];
				strcpy(NpcName, map.m_vNpc[CurrentNpc]->Name);
				map.m_vNpc[CurrentNpc]->AdjustPosition();
				Message.PutMessage( MSG_SETNPCACTIVE, false, CurrentNpc, 1);//npcֹͣ�
				int x,y;
				//��������
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
				Message.PutMessage( MSG_MOVE, x,y,1);	//�����ƶ���ȥ
				char *ScriptFile = (char*)malloc(sizeof(char)*64);;
				strcpy(ScriptFile,map.m_vNpc[CurrentNpc]->ScriptFile);
				//�������Ǻ�NPC�ķ��򣬷���NPC�ű�
				Message.PutMessage( MSG_TALKDIR, CurrentNpc,bAct,0,ScriptFile);	
				_DELETE_ARRAY(NpcName);
			}
			else	//����
			{
				//�ȵ���
				if( Fight.RandomStart(map.GetEnemyFrequency()<<2) )
					return;

				//����
				if( WindowMode!=0 )
				{
					if( point.x >= 0 && point.x<ScreenWidth && point.y>=0 && point.y<ScreenHeight )
						role[0].MoveTo(_point_x(cx), _point_y(cy));
				}
				else	//ȫ��
				{
					role[0].MoveTo(_point_x(cx), _point_y(cy));
				}

				//��ʾĿ�ĵع��
				CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetPos(map.sx+point.x, map.sy+point.y);
				CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(3);
			}
		}

		//�˵�
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

	if( key == DIK_F11 )		//testս��
	{
		key = 0;
		Fight.RandomStart(12000);
	}

	//��ʾ��ͼ
	map.ShowMap();
}

//*****************************
//������������Ϣ
LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static bool bMusicPlaying=false;
	static bool bFMusicPlaying = false;
	switch( message )
	{
	case WM_ACTIVATE:	//Ӧ�ó����Ƿ�

		if(LOWORD(wParam)==WA_INACTIVE)
			bActive = false;
		else if(HIWORD(wParam))
		{
			bActive = false;
			return 0;
		}
		else
			bActive = true;
		//���»�ȡ�����豸
		Mouse.SetAcquire(hWnd);
		Key.SetAcquire(hWnd);
		g_Joystick.SetAcquire(hWnd);
		Mouse.GetMouseData();
		Mouse.ReleaseMouseBuffer();

		//����ҳ��
		if( g_eDisplayState==DS_NIGHT && WindowMode !=0 && !bActive )
		{
			GetPrimarySurface(lpDDSBack);
		}

		//�������ִ���
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
		case VK_RETURN:		//�л�ȫ���򴰿�
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
			if( MessageBox(hWnd, "���Ҫ�˳���", "ʥ��Ӣ�۴� II", MB_YESNO) == IDYES )
				RunGame=false;
			return 0;

		case VK_F12:
			g_bDebug = !g_bDebug;
			break;

		case 18:	// ����alt
			return 0;
		}
		break;

    case WM_KEYDOWN://������Ϣ
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

		case VK_F5:			//�л�ģʽ
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

		case VK_F6:			//�鿴����
			{
				char *Var=GetStringDlg("������Ҫ�鿴�ı�������:");
				if( Var )
					ShowMessage("%s = %d", Var, Script.GetValue(Var));
			}
			break;

		case VK_F7:			//�л�ģʽ
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

		case VK_F8:			//ִ�нű�
			{
				char *Var=GetStringDlg("������Ҫִ�еĽű����:");
				if( Var == NULL ) break;
				Script.RunCommand(Var);
			}
			break;

		case VK_F4:		//�л�ȫ���򴰿�
			if ( bSwitch && bActive )
			{
				if( WindowMode == 0 )
					lpDD->RestoreDisplayMode();
				GetWindowRect(hWnd, &RectWindow);
				WindowMode = 1-WindowMode;
				ChangeCoopLevel(hWnd);
			}
			break;

		case VK_F12:		//��ʾFPS
			bShowFps=!bShowFps;
			break;
		
		case 'h':			//����ģʽ�ļ�mp��hp����
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

		case 'l':			//ֱ������
		case 'L':
			{
				if(g_ePlayState!=PS_MAIN||!g_bDebug||!GetAsyncKeyState(VK_CONTROL))
					break;
				for(int i =0 ;i<RoleNum;i++)
				{
					role[i].Propertiy.Level++;						
					//������һ������Ҫ�ľ���ֵ
					role[i].Propertiy.MaxExp += 10*(role[i].Propertiy.Level+1)*
														(role[i].Propertiy.Level)+30;
					role[i].Propertiy.Exp = role[i].Propertiy.MaxExp;
					role[i].Propertiy.MaxHp+=200;						//HP��20
					role[i].Propertiy.Hp=role[i].Propertiy.MaxHp;				//��HP����
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

	case WM_TIMER:	//��ʱ����Ϣ
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

	// �����ͼ�༭�����Ҽ�����
	case WM_COMMAND:
		Mouse.ReleaseMouseBuffer();
		MapEditProc(LOWORD(wParam));
		Mouse.ReleaseMouseBuffer();
		break;

	case WM_PAINT:		//�ں�̨����ģʽʱ
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

	case WM_MOVE:		//�����ƶ�
        if ( bActive && WindowMode!=0 )
        {
            GetWindowRect(hWnd, &RectWindow);
			RectWindow.top += g_pointClient.y;
			RectWindow.left += g_pointClient.x;
			RectWindow.right = RectWindow.left + ScreenWidth;
			RectWindow.bottom = RectWindow.top + ScreenHeight;
		}
		break;

	case WM_SETCURSOR:	 //���ù��
		if( bActive && !bShowHardCursor )
		{
			SetCursor(NULL);
			return true;
		}
		break;

    case WM_DESTROY:	//�˳���Ϣ
		PostQuitMessage( 0 );
		RunGame=false;
        break;

	case WM_GETMINMAXINFO:
		{
			// ����ģʽ�²��øı��С 
			// ���ڹ̶��� 640x480 (�ͻ���)
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
	
	//����ȱʡ��Ϣ�������
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//-----------------------------------------------

//���µ���Ļ�����lpSurf
void UpdateScreen(LPDIRECTDRAWSURFACE lpSurf, RECT DestRect)
{
	if( g_eDisplayState==DS_NORMAL )	//������ʾ
	{
		if( bShowFps ) 
			font.PrintText(lpDDSBack, 20, 460,"FPS:%d", CountSpeed());
		//debugģʽ����ʾ
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
		
		if( WindowMode==0 )	//ȫ��
		{
			Blt(lpSurf, 0, 0, lpDDSBack, RectScreen, false);
		}
		else	//����
		{
			lpSurf->Blt( &DestRect, lpDDSBack, &RectScreen, DDBLTFAST_NOCOLORKEY, 0);
		}
	}
	else if( g_eDisplayState==DS_NIGHT )	//ҹ��
	{
		if( bShowFps ) 
			font.PrintText(lpDDSBack, 220, 260, "FPS:%d", CountSpeed());
		
		int sx= role[0].State.x - map.sx - _point_x(6);
		int sy= role[0].State.y - map.sy - _point_y(5);

		Alpha_Blt(lpDDSBack, sx, sy, lpDDSMask, GetRect(0,0,384,288));

		if( WindowMode==0 )	//ȫ��
		{
			RECT s_rect={sx, sy, sx+384, sy+288};
			Blt(lpSurf, sx, sy, lpDDSBack, s_rect, false);
		}
		else	//����
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
	nFrames++;	//����
}

//���µ���Ļ-�Ի�
void UpdateScreen_Dialog(LPDIRECTDRAWSURFACE lpDDSurf)
{
	if( WindowMode==0 )		//ȫ��
		Blt(lpDDSPrimary, 0, 0, lpDDSurf, RectScreen, false);
	else
		lpDDSPrimary->Blt( &RectWindow, lpDDSurf, &RectScreen, DDBLTFAST_NOCOLORKEY, 0);

	nFrames++;	//����
}

//�������
void ShowMouseCursor(LPDIRECTDRAWSURFACE lpSurf, bool bNormal)
{
	//Ŀ�ĵع��
	CCursor::Show(CCursor::CS_CHANGEMAP, lpSurf,
				CCursor::GetCursor(1).pPic.Getx()-map.sx, 
				CCursor::GetCursor(1).pPic.Gety()-map.sy );	//��ʾĿ�ĵع��

	//�������
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

//�ѵ�ǰ��Ļ���Ƶ���̨����
void GetPrimarySurface(LPDIRECTDRAWSURFACE surf)
{
	if( WindowMode==0 )	//ȫ��
		Blt(surf, 0, 0, lpDDSPrimary, RectScreen, false);
	else
		Blt(surf, 0, 0, lpDDSPrimary, RectWindow, false);
}

//����Ϸ���渴�Ƶ�һ�������ϣ���������FPS�ȣ�
void GetGameSurface(LPDIRECTDRAWSURFACE surf)
{
	if( g_ePlayState == PS_MAIN ||g_ePlayState == PS_Null)
	{
		if( g_eDisplayState == DS_NIGHT )		//ҹ��ģʽ
		{
			FillSurface(surf,RGB16(0));			//���
			map.ShowMap(false,surf);			//��ʾ��ͼ
			//����alpha��ʾ��λ��
			//�����ǰѹ�Ȧalpha��ʾ��������
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
			map.ShowMap(false);			//ֱ����ʾ
			Blt(surf, 0, 0, lpDDSBack, RectScreen, false);
		}
	}
	else if(g_ePlayState == PS_FIGHT)
		Fight.GetFightGameSurface(surf);		//ս����ʱ��
}
