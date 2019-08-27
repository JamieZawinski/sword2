//********************************************
//	列表窗口 相关函数
//  创建于2000年7月07日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "window.h"
#include "scroll.h"
#include "listwindow.h"

//构造
CListWindow::CListWindow()
{
	Column=NULL;
}

//析构
CListWindow::~CListWindow()
{
	for(int i=0; i<ColumnNum; i++)
	{
		free( Column[i].Name );
	}
	_DELETE_ARRAY( Column );
}

//读入对话框
bool CListWindow::LoadListWindowIni(char *ini, char *index)
{
	char *str;
	CIniSet Ini(ini);
	Id=Ini.ReadInt(index,"Id");			//ID
	x=Ini.ReadInt(index,"x");			//显示位置
	y=Ini.ReadInt(index,"y");			//显示位置
	Width=Ini.ReadInt(index,"Width");	//大小
	Height=Ini.ReadInt(index,"Height");	//大小
	LineHeight=Ini.ReadInt(index,"LineHeight");	//行高

	str=Ini.ReadText(index, "SelectColor");
	SelectColor=RGB16(Ini.ReadInt(str, "Red"), Ini.ReadInt(str, "Green"), Ini.ReadInt(str,"Blue"));
	_FREE(str);

	int MinLine=Ini.ReadInt(index, "MinLine");
	int MaxLine=Ini.ReadInt(index, "MaxLine");
	int CurLine=Ini.ReadInt(index, "CurLine");
	int PageLine=Ini.ReadInt(index, "PageLine");

	//标题高度
	ColumnHeight=Ini.ReadInt(index, "ColumnHeight");

	//标题字体
	str=Ini.ReadText(index, "ColumnColor");
	ColumnColor=RGB16(Ini.ReadInt(str, "Red"), Ini.ReadInt(str, "Green"), Ini.ReadInt(str,"Blue"));
	_FREE(str);

	//读取标题列表(纵向排列)
	int xxx=0;		//下一个内容的显示位置
	str=Ini.ReadText(index, "Column");
	ColumnNum=Ini.ReadInt(str, "ColumnNum");
	Column=new stColumn[ColumnNum];
	for(int i=0; i<ColumnNum; i++)
	{
		Column[i].Name=Ini.ReadText(str, i*3);
		Column[i].Align=Ini.ReadInt(str, i*3+1);
		Column[i].Width=Ini.ReadInt(str, i*3+2);
		Column[i].x=xxx;
		xxx+=Column[i].Width+1;
	}
	_FREE(str);

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

	//初始化滚动条
	file=Ini.ReadText(index, "ScrollFile");
	tile=Ini.ReadText(index, "ScrollIndex");
	ScrollBar.LoadIni(file, tile);
	_FREE(file);
	_FREE(tile);

	ScrollBar.SetStyle(0);
	ScrollBar.x=x+Width-ScrollBar.Width;
	ScrollBar.y=y;
	ScrollBar.SetHeight(Height);
	ScrollBar.SetMinNum(MinLine);
	ScrollBar.SetMaxNum(MaxLine);
	ScrollBar.SetCurNum(CurLine);
	ScrollBar.SetPageNum(PageLine);

	//初始化标题字体
	file=Ini.ReadText(index, "ColumnFontFile");
	tile=Ini.ReadText(index, "ColumnFontIndex");
	ColumnFont.LoadFont(file, tile);
	_FREE(file);
	_FREE(tile);

	return true;
}

//显示对话框
void CListWindow::ShowListWindow(LPDIRECTDRAWSURFACE lpDDSTemp, bool bShowWindow, int left, int top)
{
	//画窗口
	ShowWindow(lpDDSTemp, bShowWindow, left, top);

	//画滚动条
	if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum ) 
	{
		ScrollBar.CheckLoop();
	}
}
