//********************************************
//	���� ��غ���
//  ������2000��6��21��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "..\map.h"
#include "window.h"

//����
CWindow::CWindow()
{
	lpPic=NULL;
}

//����
CWindow::~CWindow()
{
	_RELEASE(lpPic);
}

//����Ի���
bool CWindow::LoadWindowIni(char *ini, char *index)
{
	CIniSet Ini(ini);
	dialogx=Ini.ReadInt(index, "dialogx");
	dialogy=Ini.ReadInt(index, "dialogy");
	dialogw=Ini.ReadInt(index, "dialogw");
	dialogh=Ini.ReadInt(index, "dialogh");
	Alpha=Ini.ReadInt(index, "Alpha");
	//���ڷ��
	PopWindowStyle=(enum POPWINDOWSTYLE)Ini.ReadInt(index, "PopWindowStyle");
	PopWindowStep=Ini.ReadInt(index, "PopWindowStep");
	//��ʼ��ͼƬ
	char *str=Ini.ReadText(index, "PicFileName");
	strcpy(PicFileName, str);
	_FREE(str);

	CreateBitmap(lpPic, 0, 0, PicFileName);
	DDSetColorKey(lpPic, ColorKey);
	
	//��ʼ������
	char *fontfile=Ini.ReadText(index, "FontFile");
	char *fontindex=Ini.ReadText(index, "FontIndex");
	Font.LoadFont(fontfile, fontindex);
	_FREE(fontfile);
	_FREE(fontindex);

	return true;
}

//��ʾ�Ի���
void CWindow::ShowWindow(LPDIRECTDRAWSURFACE lpDDSTemp, bool bShowWindow, int left, int top)
{
	if( !bShowWindow ) return;

	if( left==-1 ) left=dialogx;
	if( top==-1 ) top=dialogy;

	int Width=dialogw/32;
	int Height=dialogh/32;
	int LastWidth=dialogw%32;
	int LastHeight=dialogh%32;
	RECT rect;

	for(int i=0; i<=Height; i++)
	for(int j=0; j<=Width; j++)
	{
		if( i==0 )				//----------�ϱ�-----------
		{
			if( j==0 )			//����
			{
				rect=GetRect(0,0,32,32);
				Blt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width )	//����
			{
				rect=GetRect(64, 0, 96, 32);
				Blt(lpDDSTemp, left+32*j-32+LastWidth, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width-1 ) 
			{
				rect=GetRect(32,0,32+LastWidth, 32);
				Blt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, true);
				continue;
			}
			else				//����
			{
				rect=GetRect(32, 0, 64, 32);
				Blt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, true);
				continue;
			}
		}
		else if( i==Height )	//----------�±�-----------
		{
			if( j==0 )			//����
			{
				rect=GetRect(0, 64, 32, 96);
				Blt(lpDDSTemp, left+32*j, top+32*i-32+LastHeight, lpPic, rect, true);
				continue;
			}
			else if( j==Width )	//����
			{
				rect=GetRect(64,64,96,96);
				Blt(lpDDSTemp, left+32*j-32+LastWidth, top+32*i-32+LastHeight, lpPic, rect, true);
				continue;
			}
			else if( j==Width-1 )
			{
				rect=GetRect(32,64,32+LastWidth, 96);
				Blt(lpDDSTemp, left+32*j, top+32*i-32+LastHeight, lpPic, rect, true);
				continue;
			}
			else				//����
			{
				rect=GetRect(32, 64, 64, 96);
				Blt(lpDDSTemp, left+32*j, top+32*i-32+LastHeight, lpPic, rect, true);
				continue;
			}
		}
		else if( i==Height-1 )	//---------���µ�͸����----------
		{
			if( j==0 )			//��
			{
				rect=GetRect(0, 32, 32, 32+LastHeight);
				Blt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width)	//��
			{
				rect=GetRect(64, 32, 96, 32+LastHeight);
				Blt(lpDDSTemp, left+32*j-32+LastWidth, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width-1 )
			{
				rect=GetRect(32,32,32+LastWidth, 32+LastHeight);
				ABlt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, ColorKey16, Alpha);
				continue;
			}
			else				//��
			{
				rect=GetRect(32, 32, 64, 32+LastHeight);
				ABlt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, ColorKey16, Alpha);
				continue;
			}
		}
		else					//-----------�м�-----------
		{
			if( j==0 )			//����
			{
				rect=GetRect(0, 32, 32, 64);
				Blt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width)	//����
			{
				rect=GetRect(64, 32, 96, 64);
				Blt(lpDDSTemp, left+32*j-32+LastWidth, top+32*i, lpPic, rect, true);
				continue;
			}
			else if( j==Width-1 )
			{
				rect=GetRect(32,32,32+LastWidth, 64);
				ABlt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, ColorKey16, Alpha);
				continue;
			}
			else				//�в���͸������
			{
				rect=GetRect(32, 32, 64, 64);
				ABlt(lpDDSTemp, left+32*j, top+32*i, lpPic, rect, ColorKey16, Alpha);
				continue;
			}
		}
	}
}

//��������
void CWindow::PopWindow(LPDIRECTDRAWSURFACE& surf, enum POPWINDOWSTYLE pws, int Num)
{
	int i;

	//���
	if( pws==PWS_RANDOM )
	{
		pws=enum POPWINDOWSTYLE(random(3)+1);
	}

	//����
	switch( pws )
	{
	case PWS_NULL:
		return;

	case PWS_HOR:	//���򵯳�
		{
			
			int Old_Width=dialogw;
			float Step=(float)(dialogw-64)/Num;
			for(i=0; i<Num+1; i++)
			{
				dialogw=(int)(64+Step*i);
				GetGameSurface(surf);		//�õ�һ����Ϸ���棬�������滭����
				ShowWindow(surf, true, int(dialogx+Old_Width/2-32-Step/2*i));
				UpdateScreen_Dialog(surf);
				Delay(20);
			}
			
			dialogw=Old_Width;
		}
		break;

	case PWS_VER:	//���򵯳�
		{

			int Old_Height=dialogh;
			float Step=(float)(dialogh-64)/Num;
			for(i=0; i<Num+1; i++)
			{
				dialogh=int(64+Step*i);
				if( i==Num ) dialogh=Old_Height;
				GetGameSurface(surf);			//�õ�һ����Ϸ���棬�������滭����
				ShowWindow(surf, true, dialogx, int(dialogy+Old_Height/2-32-Step/2*i));
				UpdateScreen_Dialog(surf);
				Delay(20);
			}
			
			dialogh=Old_Height;
		}
		break;

	case PWS_BOTH:	//�Ŵ󵯳�
		{
			
			int Old_Width=dialogw;
			int Old_Height=dialogh;
			float StepW=(float)(dialogw-64)/Num;
			float StepH=(float)(dialogh-64)/Num;

			for(i=0; i<Num+1; i++)
			{
				dialogw=int(64+StepW*i);
				dialogh=int(64+StepH*i);
				GetGameSurface(surf);			//�õ�һ����Ϸ���棬�������滭����
				ShowWindow(surf, true, int(dialogx+Old_Width/2-32-StepW/2*i), int(dialogy+Old_Height/2-32-StepH/2*i));
				UpdateScreen_Dialog(surf);
				Delay(20);
			}
			
			dialogw=Old_Width;
			dialogh=Old_Height;
		}
		break;
	}
}

//�������������
int CWindow::PressAnyKey()
{
	if(	mouse == RB_UP || mouse == LB_UP )
	{
		return 1000;
	}
	
	if( key > 127 || A_UP )
		return key;
		
	return 0;
}

