//********************************************
//	�б��� ��غ���
//  ������2000��7��07��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "window.h"
#include "scroll.h"
#include "listwindow.h"

//����
CListWindow::CListWindow()
{
	Column=NULL;
}

//����
CListWindow::~CListWindow()
{
	for(int i=0; i<ColumnNum; i++)
	{
		free( Column[i].Name );
	}
	_DELETE_ARRAY( Column );
}

//����Ի���
bool CListWindow::LoadListWindowIni(char *ini, char *index)
{
	char *str;
	CIniSet Ini(ini);
	Id=Ini.ReadInt(index,"Id");			//ID
	x=Ini.ReadInt(index,"x");			//��ʾλ��
	y=Ini.ReadInt(index,"y");			//��ʾλ��
	Width=Ini.ReadInt(index,"Width");	//��С
	Height=Ini.ReadInt(index,"Height");	//��С
	LineHeight=Ini.ReadInt(index,"LineHeight");	//�и�

	str=Ini.ReadText(index, "SelectColor");
	SelectColor=RGB16(Ini.ReadInt(str, "Red"), Ini.ReadInt(str, "Green"), Ini.ReadInt(str,"Blue"));
	_FREE(str);

	int MinLine=Ini.ReadInt(index, "MinLine");
	int MaxLine=Ini.ReadInt(index, "MaxLine");
	int CurLine=Ini.ReadInt(index, "CurLine");
	int PageLine=Ini.ReadInt(index, "PageLine");

	//����߶�
	ColumnHeight=Ini.ReadInt(index, "ColumnHeight");

	//��������
	str=Ini.ReadText(index, "ColumnColor");
	ColumnColor=RGB16(Ini.ReadInt(str, "Red"), Ini.ReadInt(str, "Green"), Ini.ReadInt(str,"Blue"));
	_FREE(str);

	//��ȡ�����б�(��������)
	int xxx=0;		//��һ�����ݵ���ʾλ��
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

	//��ʼ��������
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

	//��ʼ����������
	file=Ini.ReadText(index, "ColumnFontFile");
	tile=Ini.ReadText(index, "ColumnFontIndex");
	ColumnFont.LoadFont(file, tile);
	_FREE(file);
	_FREE(tile);

	return true;
}

//��ʾ�Ի���
void CListWindow::ShowListWindow(LPDIRECTDRAWSURFACE lpDDSTemp, bool bShowWindow, int left, int top)
{
	//������
	ShowWindow(lpDDSTemp, bShowWindow, left, top);

	//��������
	if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum ) 
	{
		ScrollBar.CheckLoop();
	}
}
