//********************************************
//	�˵� ��غ���
//  ������2000��7��09��
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

int CMenu::SelectItem=0;			//��ǰ���Ĳ˵���
bool CMenu::bMenuRun=true;
CMenuTop CMenu::MenuTop;			//�����˵�
CRoleSel CMenu::RoleSel;			//����
CState CMenu::State;				//����
CGoods CMenu::Goods;				//��Ʒ��
CArm CMenu::Arm;					//װ��
CMagic CMenu::Magic;				//ħ��
CSystem CMenu::System;				//ϵͳ
CLoadSave CMenu::LoadSave;			//����
CHelp CMenu::Help;					//����

CSound CMenu::Sound_TileClick;		//��ͷ����Ч
CSound CMenu::Sound_HighLight;		//��껮��
CSound CMenu::Sound_Click;			//��Ч
CSound CMenu::Sound_Change;			//�л�
CSound CMenu::Sound_Pop;			//�˵�����
CSound CMenu::Sound_Cancel;			//�˵�ȡ��

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
}

//��ȡ
void CMenu::LoadMenuIni(char *filename, char *index)
{
	char* strFile, *strIndex;
	CIniSet Ini(filename);

	//����
	strFile = Ini.ReadText(index, "RoleSelFile");
	strIndex = Ini.ReadText(index, "RoleSelIndex");
	RoleSel.LoadRoleSelIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//�����˵�
	strFile = Ini.ReadText(index, "MenuTopFile");
	strIndex = Ini.ReadText(index, "MenuTopIndex");
	MenuTop.LoadMenuTopIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//����
	strFile = Ini.ReadText(index, "StateFile");
	strIndex = Ini.ReadText(index, "StateIndex");
	State.LoadStateIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//��Ʒ
	strFile = Ini.ReadText(index, "GoodsFile");
	strIndex = Ini.ReadText(index, "GoodsIndex");
	Goods.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//ħ��
	strFile = Ini.ReadText(index, "MagicFile");
	strIndex = Ini.ReadText(index, "MagicIndex");
	Magic.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//װ��
	strFile = Ini.ReadText(index, "ArmFile");
	strIndex = Ini.ReadText(index, "ArmIndex");
	Arm.LoadArmIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//ϵͳ
	strFile = Ini.ReadText(index, "SystemFile");
	strIndex = Ini.ReadText(index, "SystemIndex");
	System.LoadSystemIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//����
	strFile = Ini.ReadText(index, "LoadSaveFile");
	strIndex = Ini.ReadText(index, "LoadSaveIndex");
	LoadSave.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//����
	strFile = Ini.ReadText(index, "HelpFile");
	strIndex = Ini.ReadText(index, "HelpIndex");
	Help.LoadHelpIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	//������Ч
	Sound_TileClick.LoadSoundFX(hWnd, "Sound\\tileclick.wav");	//��ͷ
	Sound_HighLight.LoadSoundFX(hWnd, "Sound\\high.wav");	//��껮��
	Sound_Click.LoadSoundFX(hWnd, "Sound\\click.wav");		//����
	Sound_Change.LoadSoundFX(hWnd, "Sound\\change.wav");	//�л�
	Sound_Pop.LoadSoundFX(hWnd, "Sound\\pop.wav");;			//�˵�����
	Sound_Cancel.LoadSoundFX(hWnd, "Sound\\cancel.wav");	//�˵�ȡ��	
}

//------------------------------------

//�˵���ʼ��
void CMenu::InitMenu()
{
	RoleSel.ButtonNum=RoleNum;
	bMenuRun=true;

	//��ʼ��
	switch( SelectItem )
	{
	case 0:
		State.Init(lpDDSBack);			//��������
		State.Window_Magic.ShowWindow(lpDDSBack);		//ħ��
		State.Window_Setup.ShowWindow(lpDDSBack);		//װ��
		State.ShowStateWindow(lpDDSBack);				//״̬
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
	//���浽lpDDSBackGround
	Blt(lpDDSBackGround, 0, 0, lpDDSBack, RectScreen, false);
}

//���ò˵�
void CMenu::ReInitMenu()
{
	RoleSel.ButtonNum=RoleNum;
	bMenuRun=true;
	//�õ�һ����Ϸ����
	GetGameSurface(lpDDSBack);

	//��ʼ��
	switch( SelectItem )
	{
	case 0:		//״̬
		State.ShowWindow(lpDDSBack);
		State.Window_Magic.ShowWindow(lpDDSBack);
		State.Window_Setup.ShowWindow(lpDDSBack);
		State.ShowStateWindow(lpDDSBack);
		break;

	case 1:		//��Ʒ
		Goods.ShowWindow(lpDDSBack);
		Goods.AboutWindow.ShowWindow(lpDDSBack);
		Goods.ScrollBar.MinNum=0;
		Goods.ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
		Goods.ScrollBar.CountNewRect();
		break;

	case 2:		//װ��
		Arm.ShowWindow(lpDDSBack);
		Arm.Window_Setup.ShowWindow(lpDDSBack);
		Arm.ArmList.ScrollBar.MinNum=0;
		Arm.ArmList.ScrollBar.MaxNum=role[0].Propertiy.CurArmNum;
		Arm.ArmList.ScrollBar.CountNewRect();
		Arm.ArmList.ShowWindow(lpDDSBack);
		Arm.ShowArmWindow(lpDDSBack);
		break;

	case 3:		//ħ��
		Magic.ShowWindow(lpDDSBack);
		Magic.AboutWindow.ShowWindow(lpDDSBack);
		Magic.ScrollBar.MinNum=0;
		Magic.ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;
		Magic.ScrollBar.CountNewRect();
		break;

	case 4:		//ϵͳ
		System.ShowWindow(lpDDSBack);
		break;

	case 5:		//����
		Help.ShowWindow(lpDDSBack);
		Help.ShowHelpWindow(lpDDSBack);
		break;
	}

	MenuTop.ShowMenuTop(lpDDSBack);		//��ʾ����˵�
	//����
	Blt(lpDDSBackGround, 0, 0, lpDDSBack, RectScreen, false);
}

//�˵�ѭ��
void CMenu::DoMenuLoop()
{
	Goods.ScrollBar.MinNum=0;
	Goods.ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
	Magic.ScrollBar.MinNum=0;
	Magic.ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;
	CMenu::Sound_Pop.Play();

	InitMenu();

	//�˵�ѭ��
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
			//��ȡ������Ϣ
			GetInputData();
			
			if( B_UP || mouse == RB_UP ) 
			{
				CMenu::Sound_Cancel.Play();
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				SelectItem=0;
				return;
			}
			
			//�ָ�����
			Blt(lpDDSBack, 0, 0, lpDDSBackGround, RectScreen, false);
			//��ʾ����˵�
			MenuTop.DoMenu(lpDDSBack, false);

			switch( SelectItem )
			{
			case 0:		//״̬
				State.DoMenu(lpDDSBack, false);
				break;

			case 1:		//װ��
				Goods.DoMenu(lpDDSBack, false);
				break;

			case 2:		//װ��
				Arm.DoMenu(lpDDSBack, false);
				break;
			case 3:		//ħ��
				Magic.DoMenu(lpDDSBack,false);
				break;
			case 4:		//ϵͳ
				switch( System.DoMenu(lpDDSBack, false) )
				{
				case 1:		//load
					SelectItem=0;
					FadeIn(2);
					map.ShowMap();

					if( CMusic::bPlay )	//��������
					{
						Music.Resume();
					}
					return;
					
				case 2:		//save
					break;

				case 3:		//�˳���Ϸ
					ColorIn(100, RGB16(0,0,0));
					Title.DoMenu(lpDDSBack);
					SelectItem=0;
					return;
				}
				break;

				case 5:		//����
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
		else WaitMessage();	//�ȴ���Ϣ
	}

	if( g_eDisplayState == DS_NIGHT )
		Clrscr(lpDDSPrimary);
}
