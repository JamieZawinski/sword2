//********************************************
//	���� ��غ���
//  ������2000��9��29��
//********************************************
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\mapedit\mapedit.h"
#include "..\script.h"
#include "menu_inc.h"
#include "..\cursor.h"
#include "..\role.h"

CTitle::CTitle()
{
	Select=0;
	Button = NULL;
	BackGroundFile = NULL;
}

CTitle::~CTitle()
{
	_DELETE_ARRAY(Button);
	_FREE( BackGroundFile );
}

//��ȡ
void CTitle::LoadTitleIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	BackGroundFile=Ini.ReadText(index, "BackGroundFile");

	ButtonNum=Ini.ReadInt(index, "ButtonNum");
	Button=new CCButton[ButtonNum];

	//��ʼ����ť
	char* strFile;
	char* strIndex;
	for(int i=0; i<ButtonNum; i++)
	{
		strFile = Ini.ReadText(index, i);
		strIndex = Ini.ReadData(index,i);
		Button[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
	}

	//��ʼ������
	char *strTemp = GameIni.ReadText("�����ļ�","����");
	ShowVersionFont.LoadFont(strTemp, "�汾����");
	_FREE(strTemp);
	
	strTemp = GameIni.ReadText("Start","version");
	strcpy(m_szVersion,strTemp);
	_FREE(strTemp);
}

//------------------------------------

//��ʾ
void CTitle::ShowTitle(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
}

//����ѭ��
void CTitle::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	int bHigh=-1, Contral=1;
	int oldx, oldy;

	g_ePlayState = PS_Null;
	bool b[3]={0,0,0};

	Music.Stop();
	WCHAR* str = WChar("music\\title.mid");
	Music.LoadMusic(str);
	_DELETE(str);
	Music.Play();

	//���뱳��ҳ��
	DDReLoadBitmap(lpDDSBackGround, BackGroundFile);
	for(int i=0; i<ButtonNum; i++)
	{
		Button[i].CheckLoop(lpDDSBackGround, -1);
	}

	ColorOut(lpDDSBackGround, 100);
	
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);

	//��ʾ�汾��
	int showversion_x,showversion_y;
	TEXTMETRIC tm;
	ShowVersionFont.GetFontInfo(lpDDSBack,&tm);
	showversion_x = MainWidth-tm.tmAveCharWidth*strlen(m_szVersion)-4;
	showversion_y = MainHeight-tm.tmHeight-tm.tmExternalLeading-4;
	ShowVersionFont.PrintText( lpDDSBackGround,showversion_x,showversion_y,m_szVersion);
	//�˵�ѭ��
	MSG msg;
	while(RunGame)
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
			
			//����
			Blt(lpDDSBack, 0, 0, lpDDSBackGround, RectScreen, false);
			
			if( Contral == 0 && oldx!=point.x || oldy!=point.y )
			{
				Contral=1;
				oldx=point.x;
				oldy=point.y;
			}
			else if( Contral == 1 && ( UP_DOWN || DOWN_DOWN ) )
			{
				Contral=0;
				Select=0;
				key=0;
			}

			if( Contral==0 )	//����
			{
				//ѡ��
				if( UP_DOWN )
				{
					Select--;
					if( Select<0 ) Select=ButtonNum-1;
					CMenu::Sound_HighLight.Play();
				}
				if( DOWN_DOWN )
				{
					Select++;
					if( Select>ButtonNum-1 ) Select=0;
					CMenu::Sound_HighLight.Play();
				}
				
				//��ʾ
				for(int i=0; i<ButtonNum; i++)
				{
					if( i==Select ) bHigh=1;
					else bHigh=-1;

					int ret=Button[i].CheckLoop(lpDDSBack, bHigh);
					if( ret==2 )	Select=i;
				}

				if( A_UP )
					Button[Select].CheckLoop(lpDDSBack, 2);

				if( A_UP )
				{
					switch( Select )
					{
					case 0:		//new game
						{
							CMenu::Sound_TileClick.Play();
							Music.Stop();
							NewGame();
							char *str=GameIni.ReadText("Start", "��ʼ�ű�");
							Script.Run( str );
							_FREE(str);
							return;
						}
					case 1:		//load game
						CMenu::Sound_TileClick.Play();
						if( CMenu::LoadSave.DoMenu(lpDDSBack, true, 168, 48 ) )
						{
							g_ePlayState=PS_MAIN;
							FadeIn(2);
							map.ShowMap();
							if( CMusic::bPlay )	//��������
							{
								Music.Resume();
							}
							return;
						}
						DDReLoadBitmap(lpDDSBackGround, BackGroundFile);
						ShowVersionFont.PrintText( lpDDSBackGround,showversion_x,showversion_y,m_szVersion);
						break;
					case 2:		//exit game
						CMenu::Sound_TileClick.Play();
						Music.Stop();
						RunGame=false;
						return;
					}
				}
			}
			else if( Contral==1 )	//���
			{
				int ret[3], ret2[3];
				for(int i=0; i<ButtonNum; i++)
				{
					ret[i]=Button[i].CheckLoop(lpDDSBack, -1);
					if( ret[i]==2 && ret2[i] != 2 )
					{
						Select=i;
						CMenu::Sound_HighLight.Play();
					}
					ret2[i] = ret[i];
				}

				for(i=0; i<ButtonNum; i++)
				{
					if( ret[i]==1 )
					{
						switch( Select )
						{
						case 0:		//new game
							{
								CMenu::Sound_TileClick.Play();
								Music.Stop();
								NewGame();
								char *str=GameIni.ReadText("Start", "��ʼ�ű�");
								Script.Run( str );
								_FREE(str);
								return;
							}
						case 1:		//load game
							CMenu::Sound_TileClick.Play();
							if( CMenu::LoadSave.DoMenu(lpDDSBack, true, 168, 48 ) )
							{
								g_ePlayState=PS_MAIN;
								FadeIn(2);
								map.ShowMap();
								if( CMusic::bPlay )	//��������
								{
									Music.Resume();
								}
								return;
							}
							DDReLoadBitmap(lpDDSBackGround, BackGroundFile);
							ShowVersionFont.PrintText( lpDDSBackGround,showversion_x,showversion_y,m_szVersion);
							break;

						case 2:		//exit game
							CMenu::Sound_TileClick.Play();
							Music.Stop();
							RunGame=false;
							return;
						}
					}
				}
			}
			ShowMouseCursor(lpDDSBack, false);
			_UpdateScreen();

			//ֱ�ӽ����ͼ�༭ģʽ
			if( key==DIK_F5 && Key.GetState(DIK_LALT) )
			{
				NewGame();
				Music.Stop();
				//��ʼ��Undomap
				UndoMap=new Cell_Struct[map.Width*map.Height];
				memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
				g_ePlayState=PS_EDIT;
				bShowHardCursor=true;
				ShowCursor(true);
				return;
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
}
