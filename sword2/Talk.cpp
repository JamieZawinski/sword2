//********************************************
//	�Ի� ��غ���
//  ������2000��5��01��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "main.h"
#include "role.h"
#include "map.h"
#include "talk.h"
#include "Cursor.h"

extern bool bShowFps;

//����
CTalk::CTalk()
{
	lpPhoto=NULL;
}

//����
CTalk::~CTalk()
{
	_RELEASE(lpPhoto);
}

//��ʼ��
bool CTalk::Init(char *ini, char *index)
{
	LoadWindowIni(ini, index);	//��ʼ������
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;

	CIniSet Ini(ini);
	photox=Ini.ReadInt(index, "photox");
	photoy=Ini.ReadInt(index, "photoy");
	photox2=Ini.ReadInt(index, "photox2");
	photoy2=Ini.ReadInt(index, "photoy2");
	photow=Ini.ReadInt(index, "photow");
	photoh=Ini.ReadInt(index, "photoh");
	PhotoPlace=0;
	
	//ͷ��ͼƬ
	char* strTemp = Ini.ReadText(index, "PhotoFilePath");
	strcpy(PhotoFilePath, strTemp);
	_FREE(strTemp);

	CreateBitmap(lpPhoto, photow, photoh);
	DDSetColorKey(lpPhoto, ColorKey);
	bPhoto=false;

	//�����õ�С��ͷ
	strTemp = Ini.ReadText(index, "ContinueFile");
	char* strTemp2 = Ini.ReadText(index, "ContinueIndex");
	Continue.LoadAni(strTemp, strTemp2);
	_FREE(strTemp);
	_FREE(strTemp2);

	//ʹ����ͼ�Ի�����
	bFullDialog=Ini.ReadInt(index, "bFullDialog");

	if( bFullDialog )
	{
		//��ȡ��ͼ�Ի��򱳾�
		strTemp = Ini.ReadText(index, "TalkPicFile");
		CreateBitmap(lpTalk, 0,0, strTemp);
		_FREE(strTemp);
		DDSetColorKey(lpTalk, ColorKey);
		//�õ����������С
		int ww, hh;
		GetSurfaceSize(lpTalk, ww, hh);
		TalkRect=GetRect(0, 0, ww, hh);

		TalkX=Ini.ReadInt(index, "TalkX");
		TalkY=Ini.ReadInt(index, "TalkY");

		//��ʼ��Alphaֵ
		TalkAlphaColor=RGB16(Ini.ReadInt(index, "Talk_Red"),
							 Ini.ReadInt(index, "Talk_Green"),
							 Ini.ReadInt(index, "Talk_Blue"));
	}
	//��ʱ
	NextTick=timeGetTime();

	return true;
}

//�򿪶Ի��ļ�
void CTalk::OpenFile(char *filename)
{
	Dialog.Open(filename);
	strcpy(FileName, filename);
}

//����ͷ��
void CTalk::CreatePhoto(char *filename)
{
	if( filename==NULL ) return;

	char File[256];
	sprintf(File, "%s%s%s", PhotoFilePath, filename, ".bmp");

	//��ͷ��
	if( strcmp(filename, "�԰�")==0 || CheckFile(File) == false )
	{
		bPhoto=false;
		return;
	}

	//����ͷ��
	LPDIRECTDRAWSURFACE Photo;
	BOOL ret=CreateBitmap(Photo, 0, 0, File);
	if( ret==TRUE )
	{
		Blt(lpPhoto, 0, 0, Photo, GetRect(0,0,photow,photoh), false);
		_RELEASE(Photo);
		bPhoto=true;
		return;
	}

	bPhoto=false;
}

//��ʾͷ��
void CTalk::ShowPhoto(LPDIRECTDRAWSURFACE lpDDSTemp, int bPhotoInLeft)
{
	if( bPhoto )
	{
		int left =0 , top = 0;
		if( bPhotoInLeft )			//ͷ�������
		{
			left=photox;
			top=photoy;
		}
		else
		{
			left=photox2;
			top=photoy2;
		}

		Blt(lpDDSTemp, left, top, lpPhoto, GetRect(0,0,photow,photoh), true);
	}
}

//��ʾ�Ի�
void CTalk::ShowText(LPDIRECTDRAWSURFACE lpDDSTemp, char *string)
{
	//��˫ɫ��ʾ
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	Font.SetChangeColor(false);
	Font.PrintText(lpDDSTemp, dialogx+42, dialogy+42, string);

	Font.SetColor(c);
	Font.SetChangeColor(true);
	Font.PrintText(lpDDSTemp, dialogx+40, dialogy+40, string);
}

//һ�ζԻ�
void CTalk::Talk(LPDIRECTDRAWSURFACE& surf, char *index, int bPhotoInLeft)
{
	CurrentLine=0;
	strcpy(Index, index);

	GetGameSurface(lpDDSBackGround);			//�õ�һ����Ϸ����
	if( !bFullDialog ) 
		PopWindow(lpDDSBack, PopWindowStyle, PopWindowStep);	//��������

	Font.DelayTick=CGpFont::DELAYTICK[3];	//���ֳ����ٶȻ�ԭ
	char *photo, *dlg=NULL;//
	int len=0, stringlen=0;
	char *str=NULL;
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
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
			//��ʱ����
			NowTick=timeGetTime();
			if( NowTick < OldTick+DelayTick ) continue;
			OldTick=NowTick;

			//��ȡ������Ϣ
			GetInputData();

			//ֱ����϶Ի�
			if( key==DIK_E + 128 ) 
			{
				g_ePlayState = PS_MAIN;
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
			}

			//��һ��
			if( PressAnyKey()!=0 || A_UP || CurrentLine==0 )
			{
				if( len < stringlen )	//ȫ�ԶԻ�
				{
					len=stringlen-2;
				}
				else
				{
					photo=Dialog.ReadData(Index, CurrentLine);	//��ȡͷ��
					if( photo[0]==0 )
					{
						g_ePlayState=PS_MAIN;
						if( g_eDisplayState==DS_NIGHT )
							Clrscr(lpDDSPrimary);
						return;		//����
					}
					
					CreatePhoto(photo);	//����ͷ��
					_DELETE(dlg);
					dlg=Dialog.ReadText(Index, CurrentLine);	//��ȡ�Ի�
					
					//���仺��
					stringlen=strlen(dlg);
					_DELETE(str);
					str=new char[stringlen+1];
					len=0;

					CurrentLine++;
				}
			}

			//��������
			if( len < stringlen && timeGetTime()>=NextTick )
			{
				NextTick=timeGetTime()+Font.DelayTick;

				len+=2;					//��������ַ�
				if( len > stringlen ) 
					len=stringlen;
				else if( dlg[len-2]=='<' )
				{
					len+=2;
				}
				else if( dlg[len-1]=='<' )
				{
					len+=3;
				}
				memcpy(str, dlg, len);
				memset(&str[len], 0, 1);
			}

			map.ShowMap();

			if( g_eDisplayState==DS_NIGHT )
			{
				Clrscr(lpDDSBackGround, 0);
				UpdateScreen(lpDDSBackGround, RectScreen);
			}
			else
				Blt(surf, 0,0, lpDDSBack, RectScreen, false);

			ShowPhoto(surf, bPhotoInLeft);				//��ʾͷ��
			if( !bFullDialog ) 
				ShowWindow(surf);		//��ʾ�Ի���
			else 
				AlphaColorBlt(surf, TalkX, TalkY, lpTalk, TalkRect, ColorKey16, TalkAlphaColor, Alpha);
			ShowText(surf, str);	//��ʾ����
			if( len == stringlen )
				Continue.ShowAni(surf, dialogx+dialogw+Continue.Getx(), 
					dialogy+dialogh+Continue.Gety());	//��˸��С��ͷ
			//����FPS
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}

			if( g_eDisplayState==DS_NIGHT )
			{
				ShowMouseCursor(surf,false);
				UpdateScreen_Dialog(surf);
			}
			else
			{
				ShowMouseCursor(surf,false);
				_UpdateScreen(surf);
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}

	_DELETE( str );
}

//һ�仰
void CTalk::Say(LPDIRECTDRAWSURFACE& surf, char *string, char *photo, int bPhotoInLeft)
{
	GetGameSurface(lpDDSBackGround);	//�õ�һ����Ϸ����
	if( !bFullDialog ) 
		PopWindow(lpDDSBack, PopWindowStyle, PopWindowStep);	//��������

	Font.DelayTick=CGpFont::DELAYTICK[3];	//���ֳ����ٶȻ�ԭ
	CreatePhoto(photo);						//����ͷ��
	int len=0, stringlen=strlen(string);
	char *str=new char[stringlen+1];
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
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
			//��ʱ����
			NowTick=timeGetTime();
			if( NowTick < OldTick+DelayTick ) continue;
			OldTick=NowTick;

			//��ȡ������Ϣ
			GetInputData();

			if( mouse==LB_UP || A_UP || B_UP )
			{
				if( len < stringlen )	//ȫ�ԶԻ�
				{
					len=stringlen-2;
				}
				else	//��϶Ի�
				{
					g_ePlayState=PS_MAIN;
					if( g_eDisplayState==DS_NIGHT )
						Clrscr(lpDDSPrimary);
					break;
				}
			}

			//��������
			if( len < stringlen && timeGetTime()>=NextTick )
			{
				NextTick=timeGetTime()+Font.DelayTick;

				len+=2;					//��������ַ�
				if( len > stringlen ) 
					len=stringlen;
				else if( string[len-2]=='<' )
				{
					len+=2;
				}
				else if( string[len-1]=='<' )
				{
					len+=3;
				}
				memcpy(str, string, len);
				memset(&str[len], 0, 1);
			}

			map.ShowMap();

			if( g_eDisplayState==DS_NIGHT )
			{
				Clrscr(lpDDSBackGround, 0);
				UpdateScreen(lpDDSBackGround, RectScreen);
			}
			else
				Blt(surf, 0,0, lpDDSBack, RectScreen, false);

			ShowPhoto(surf, bPhotoInLeft);		//��ʾͷ��
			if( !bFullDialog ) 
				ShowWindow(surf);	//��ʾ�Ի���
			else 
				AlphaColorBlt(surf, TalkX, TalkY, lpTalk, TalkRect, ColorKey16, TalkAlphaColor, Alpha);
			ShowText(surf, str);	//��ʾ����
			if( len == stringlen )
				Continue.ShowAni(surf, dialogx+dialogw+Continue.Getx(), dialogy+dialogh+Continue.Gety());	//��˸��С��ͷ

			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			if( g_eDisplayState==DS_NIGHT )
			{
				ShowMouseCursor(surf,false);
				UpdateScreen_Dialog(surf);
			}
			else
			{
				ShowMouseCursor(surf,false);
				_UpdateScreen(surf);
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
	
	_DELETE( str );
}


