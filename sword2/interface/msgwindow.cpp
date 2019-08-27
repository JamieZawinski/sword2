//********************************************
//	��Ϣ���� ��غ���
//  ������2000��10��08��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "..\mapedit\mapedit.h"
#include "window.h"
#include "msgwindow.h"
#include "..\cursor.h"

//����
CMsgWindow::CMsgWindow()
{
}

//����
CMsgWindow::~CMsgWindow()
{
}

//����Ի���
bool CMsgWindow::LoadMsgWindowIni(char *ini, char *index)
{
	CIniSet Ini(ini);
	Id=Ini.ReadInt(index,"Id");			//ID
	x=Ini.ReadInt(index,"x");			//��ʾλ��
	y=Ini.ReadInt(index,"y");			//��ʾλ��
	Width=Ini.ReadInt(index,"Width");	//��С
	Height=Ini.ReadInt(index,"Height");	//��С

	//��ʼ������
	char *file=Ini.ReadText(index, "WindowFile");
	char *tile=Ini.ReadText(index, "WindowIndex");
	LoadWindowIni(file, tile);
	_FREE(file);
	_FREE(tile);

	dialogx=x-32;
	dialogy=y-32;
	dialogw=Width+64;
	dialogh=Height+64;

	return true;
}

//��ʾ�Ի���
void CMsgWindow::ShowMsgWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	//������
	ShowWindow(surf, bShowWindow, left, top);
}

//ѭ��
void CMsgWindow::DoMenu(LPDIRECTDRAWSURFACE& surf, char *string)
{
	//������
	GetGameSurface(surf);		//�õ�һ����Ϸ����
	PopWindow(surf,PWS_RANDOM);	//���ϴ���
	Blt(lpDDSBackGround, 0, 0,surf , RectScreen, false);	//���浽lpDDSBackGround
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	MSG msg;
	while(bActive)
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
			//�ָ�����
			Blt(surf, 0, 0, lpDDSBackGround, RectScreen, false);
			//��ʾ����
			Font.SetColor(RGB(255,255,255));
			Font.PrintText(surf, dialogx+40, dialogy+40,  string);

			ShowMouseCursor(surf,false);
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(surf, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();

			if( A_UP || B_UP || mouse == LB_UP )
			{
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				return;
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
}