//********************************************
//	菜单 相关函数
//  创建于2000年7月09日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

int CMenu::SelectItem=0;			//当前处的菜单项
bool CMenu::bMenuRun=true;
CMenuTop CMenu::MenuTop;			//顶级菜单
CRoleSel CMenu::RoleSel;			//人物
CState CMenu::State;				//属性
CGoods CMenu::Goods;				//物品项
CArm CMenu::Arm;					//装备
CMagic CMenu::Magic;				//魔法
CSystem CMenu::System;				//系统
CLoadSave CMenu::LoadSave;			//进度
CHelp CMenu::Help;					//关于

CSound CMenu::Sound_TileClick;		//开头的音效
CSound CMenu::Sound_HighLight;		//鼠标划过
CSound CMenu::Sound_Click;			//音效
CSound CMenu::Sound_Change;			//切换
CSound CMenu::Sound_Pop;			//菜单弹出
CSound CMenu::Sound_Cancel;			//菜单取消

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
}

//读取
void CMenu::LoadMenuIni(char *filename, char *index)
{
	char* strFile, *strIndex;
	CIniSet Ini(filename);

	//人物
	strFile = Ini.ReadText(index, "RoleSelFile");
	strIndex = Ini.ReadText(index, "RoleSelIndex");
	RoleSel.LoadRoleSelIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//顶极菜单
	strFile = Ini.ReadText(index, "MenuTopFile");
	strIndex = Ini.ReadText(index, "MenuTopIndex");
	MenuTop.LoadMenuTopIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//属性
	strFile = Ini.ReadText(index, "StateFile");
	strIndex = Ini.ReadText(index, "StateIndex");
	State.LoadStateIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//物品
	strFile = Ini.ReadText(index, "GoodsFile");
	strIndex = Ini.ReadText(index, "GoodsIndex");
	Goods.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//魔法
	strFile = Ini.ReadText(index, "MagicFile");
	strIndex = Ini.ReadText(index, "MagicIndex");
	Magic.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//装备
	strFile = Ini.ReadText(index, "ArmFile");
	strIndex = Ini.ReadText(index, "ArmIndex");
	Arm.LoadArmIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//系统
	strFile = Ini.ReadText(index, "SystemFile");
	strIndex = Ini.ReadText(index, "SystemIndex");
	System.LoadSystemIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//进度
	strFile = Ini.ReadText(index, "LoadSaveFile");
	strIndex = Ini.ReadText(index, "LoadSaveIndex");
	LoadSave.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//关于
	strFile = Ini.ReadText(index, "HelpFile");
	strIndex = Ini.ReadText(index, "HelpIndex");
	Help.LoadHelpIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//读入音效
	Sound_TileClick.LoadSoundFX(hWnd, "Sound\\tileclick.wav");	//开头
	Sound_HighLight.LoadSoundFX(hWnd, "Sound\\high.wav");	//鼠标划过
	Sound_Click.LoadSoundFX(hWnd, "Sound\\click.wav");		//按键
	Sound_Change.LoadSoundFX(hWnd, "Sound\\change.wav");	//切换
	Sound_Pop.LoadSoundFX(hWnd, "Sound\\pop.wav");;			//菜单弹出
	Sound_Cancel.LoadSoundFX(hWnd, "Sound\\cancel.wav");	//菜单取消	
}

//------------------------------------

//菜单初始化
void CMenu::InitMenu()
{
	RoleSel.ButtonNum=RoleNum;
	bMenuRun=true;

	//初始化
	switch( SelectItem )
	{
	case 0:
		State.Init(lpDDSBack);			//弹出窗口
		State.Window_Magic.ShowWindow(lpDDSBack);		//魔法
		State.Window_Setup.ShowWindow(lpDDSBack);		//装备
		State.ShowStateWindow(lpDDSBack);				//状态
		break;

	case 1:
		Goods.Init(lpDDSBack);
		Goods.AboutWindow.ShowWindow(lpDDSBack, false);
		Goods.ScrollBar.CountNewRect();
		break;

	case 2:
		Arm.Init(lpDDSBack);
		Arm.Window_Setup.ShowWindow(lpDDSBack);
		Arm.ArmList.ShowWindow(lpDDSBack);
		Arm.ShowArmWindow(lpDDSBack);
		break;
	case 3:
		Magic.Init(lpDDSBack);
		Magic.AboutWindow.ShowWindow(lpDDSBack,false);
		Magic.ScrollBar.CountNewRect();
		break;
	}
	//保存到lpDDSBackGround
	Blt(lpDDSBackGround, 0, 0, lpDDSBack, RectScreen, false);
}

//重置菜单
void CMenu::ReInitMenu()
{
	RoleSel.ButtonNum=RoleNum;
	bMenuRun=true;
	//得到一个游戏画面
	GetGameSurface(lpDDSBack);

	//初始化
	switch( SelectItem )
	{
	case 0:		//状态
		State.ShowWindow(lpDDSBack);
		State.Window_Magic.ShowWindow(lpDDSBack);
		State.Window_Setup.ShowWindow(lpDDSBack);
		State.ShowStateWindow(lpDDSBack);
		break;

	case 1:		//物品
		Goods.ShowWindow(lpDDSBack);
		Goods.AboutWindow.ShowWindow(lpDDSBack);
		Goods.ScrollBar.MinNum=0;
		Goods.ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
		Goods.ScrollBar.CountNewRect();
		break;

	case 2:		//装备
		Arm.ShowWindow(lpDDSBack);
		Arm.Window_Setup.ShowWindow(lpDDSBack);
		Arm.ArmList.ScrollBar.MinNum=0;
		Arm.ArmList.ScrollBar.MaxNum=role[0].Propertiy.CurArmNum;
		Arm.ArmList.ScrollBar.CountNewRect();
		Arm.ArmList.ShowWindow(lpDDSBack);
		Arm.ShowArmWindow(lpDDSBack);
		break;

	case 3:		//魔法
		Magic.ShowWindow(lpDDSBack);
		Magic.AboutWindow.ShowWindow(lpDDSBack);
		Magic.ScrollBar.MinNum=0;
		Magic.ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;
		Magic.ScrollBar.CountNewRect();
		break;

	case 4:		//系统
		System.ShowWindow(lpDDSBack);
		break;

	case 5:		//关于
		Help.ShowWindow(lpDDSBack);
		Help.ShowHelpWindow(lpDDSBack);
		break;
	}

	MenuTop.ShowMenuTop(lpDDSBack);		//显示顶层菜单
	//保存
	Blt(lpDDSBackGround, 0, 0, lpDDSBack, RectScreen, false);
}

//菜单循环
void CMenu::DoMenuLoop()
{
	Goods.ScrollBar.MinNum=0;
	Goods.ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
	Magic.ScrollBar.MinNum=0;
	Magic.ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;
	CMenu::Sound_Pop.Play();

	InitMenu();

	//菜单循环
	MSG msg;
	while(bMenuRun)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//获取输入信息
			GetInputData();
			
			if( B_UP || mouse == RB_UP ) 
			{
				CMenu::Sound_Cancel.Play();
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				SelectItem=0;
				return;
			}
			
			//恢复画面
			Blt(lpDDSBack, 0, 0, lpDDSBackGround, RectScreen, false);
			//显示顶层菜单
			MenuTop.DoMenu(lpDDSBack, false);

			switch( SelectItem )
			{
			case 0:		//状态
				State.DoMenu(lpDDSBack, false);
				break;

			case 1:		//装备
				Goods.DoMenu(lpDDSBack, false);
				break;

			case 2:		//装备
				Arm.DoMenu(lpDDSBack, false);
				break;
			case 3:		//魔法
				Magic.DoMenu(lpDDSBack,false);
				break;
			case 4:		//系统
				switch( System.DoMenu(lpDDSBack, false) )
				{
				case 1:		//load
					SelectItem=0;
					FadeIn(2);
					map.ShowMap();

					if( CMusic::bPlay )	//背景音乐
					{
						Music.Resume();
					}
					return;
					
				case 2:		//save
					break;

				case 3:		//退出游戏
					ColorIn(100, RGB16(0,0,0));
					Title.DoMenu(lpDDSBack);
					SelectItem=0;
					return;
				}
				break;

				case 5:		//关于
					Help.DoMenu(lpDDSBack, false);
					break;
			}

			ShowMouseCursor(lpDDSBack, false);
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();
		}
		else WaitMessage();	//等待消息
	}

	if( g_eDisplayState == DS_NIGHT )
		Clrscr(lpDDSPrimary);
}
