//********************************************
//	�¼� ��غ���
//  ������2000��8��03��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "main.h"
#include "events.h"

//������Ļ
void CEvents::TextTitle(char *filename, char *index)
{
	bool bRun=true, bShow=true;
	int sx, sy, i=0, alpha=31;
	
	CIniSet Ini(filename);
	char *String=Ini.ReadText(index, "String");		//��������
	char *MusicFile=Ini.ReadText(index, "Music");	//�����ļ�
	char *PicFile=Ini.ReadText(index, "Pic");		//�����ļ�
	int Lines=Ini.ReadInt(index, "Lines");			//����
	int DelayTime=Ini.ReadInt(index, "Delay");		//ÿ��ͣ����ʱ��
	
	char *Text=NULL;	//ÿ����ʾ������
	Text=Ini.ReadText(String, 0);
	sx=(ScreenWidth-strlen(Text)*font.font_Size/2)/2;
	sy=(ScreenHeight-font.font_Size)/2;

	//��������
	if( strcmp(MusicFile, "") != 0 )
	{
		if( Music.IsPlay() ) Music.Stop();
		WCHAR* str = WChar(MusicFile);
		Music.LoadMusic(str);
		_DELETE( str );
		Music.Play();
	}
	
	//���ɱ���������
	if( strcmp( PicFile, "" ) != 0 )
	{
		DDReLoadBitmap(lpDDSBackGround, PicFile);
		ColorOut(lpDDSBackGround, 100);
	}
	else
	{
		Clrscr(lpDDSBackGround, Ini.ReadInt(index, "BackColor"));
		ColorOut(lpDDSBackGround, 100);
	}

	MSG msg;
	while( bRun )
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

			if( bShow )	//����--------------------------------
			{
				alpha--;
				Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);
				font.PrintAlphaText(lpDDSBack, sx, sy, alpha, Text);

				if( alpha <= 0 )
				{
					Delay(DelayTime);
					alpha=0;
					bShow=false;
					continue;
				}
			}
			else	//����------------------------------------
			{
				alpha++;
				Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);
				font.PrintAlphaText(lpDDSBack, sx, sy, alpha, Text);

				if( alpha >= 31 )
				{
					alpha=31;
					bShow=true;

					i++;
					
					//����
					if( i>=Lines )
					{
						bRun=false;
						break;
					}
					
					//������һ��
					_FREE(Text);
					Text=Ini.ReadText(String, i);
					sx=(ScreenWidth-strlen(Text)*font.font_Size/2)/2;
					sy=(ScreenHeight-font.font_Size)/2;
					
					continue;
				}
			}

			_UpdateScreen();

			if( B_UP )
			{
				bRun=false;
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}

	Music.Stop();

	_FREE(String);
	_FREE(MusicFile);
	_FREE(PicFile);
	_FREE(Text);
}

//���LOGO
void CEvents::GoldPointLogo(char *PicFile, DWORD BkColor, int x, int y, int Width, int Height, int Num)
{
	LPDIRECTDRAWSURFACE LogoPic;
	CreateBitmap(LogoPic, 0,0, PicFile);

	FillSurface(lpDDSBackGround, BkColor);
	Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);
	ColorOut(lpDDSBack, 50);

	//��������
	if( Music.IsPlay() ) 
		Music.Stop();
	WCHAR* str = WChar("music\\logo.mid");
	Music.LoadMusic(str);
	_DELETE( str );
	Music.Play();

	SetCursor(NULL);

	//��������
	RECT rect={0, 0, Width, Height};
	for(int i=0; i<Num; i++)
	{
		Delay(350);
		Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);

		rect.top=Height*i;
		rect.bottom=Height*(i+1);
		Blt(lpDDSBack, x, y, LogoPic, rect, false);
		_UpdateScreen();
	}
	Delay(2500);
	Music.Stop();
	
	ColorIn(50, RGB16(0,0,0));
	Delay(1000);

	_RELEASE( LogoPic );
}
