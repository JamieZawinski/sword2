//********************************************
//	消息窗口 相关函数
//  创建于2000年10月08日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "..\mapedit\mapedit.h"
#include "window.h"
#include "msgwindow.h"
#include "..\cursor.h"

//构造
CMsgWindow::CMsgWindow()
{
}

//析构
CMsgWindow::~CMsgWindow()
{
}

//读入对话框
bool CMsgWindow::LoadMsgWindowIni(char *ini, char *index)
{
	CIniSet Ini(ini);
	Id=Ini.ReadInt(index,"Id");			//ID
	x=Ini.ReadInt(index,"x");			//显示位置
	y=Ini.ReadInt(index,"y");			//显示位置
	Width=Ini.ReadInt(index,"Width");	//大小
	Height=Ini.ReadInt(index,"Height");	//大小

	//初始化窗口
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

//显示对话框
void CMsgWindow::ShowMsgWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	//画窗口
	ShowWindow(surf, bShowWindow, left, top);
}

//循环
void CMsgWindow::DoMenu(LPDIRECTDRAWSURFACE& surf, char *string)
{
	//画窗口
	GetGameSurface(surf);		//得到一个游戏画面
	PopWindow(surf,PWS_RANDOM);	//画上窗口
	Blt(lpDDSBackGround, 0, 0,surf , RectScreen, false);	//保存到lpDDSBackGround
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
			//获取输入信息
			GetInputData();
			//恢复画面
			Blt(surf, 0, 0, lpDDSBackGround, RectScreen, false);
			//显示文字
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
		else WaitMessage();	//等待消息
	}
}