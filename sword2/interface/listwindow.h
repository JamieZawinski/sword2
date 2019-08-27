//********************************************
//	图形列表框 相关函数
//  创建于2000年7月07日
//********************************************

#ifndef _LISTBOX_H_
#define _LISTBOX_H_

#include "window.h"
#include "scroll.h"

class CScroll;
class CGpFont;

//列表框类
class CListWindow : public CWindow
{
public:
	//标题结构
	struct stColumn{
		int x;			//显示位置
		int Align;		//对齐方式 0=left 1=right 2=center
		int Width;		//宽度
		char *Name;		//名字
	};

	int Id;
	int x, y, Width, Height;
	int LineHeight;					//行高

	int ColumnHeight;				//标题高度
	WORD ColumnColor;				//颜色
	CGpFont ColumnFont;				//标题字体

	int ColumnNum;					//标题数量
	stColumn *Column;				//标题

	CScroll ScrollBar;				//滚动条

	int Select;						//当前选择
	WORD SelectColor;				//被选单元的底色

public:
	CListWindow();
	~CListWindow();

	bool LoadListWindowIni(char *, char *);	//读入列表窗口
	void ShowListWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示对话框
};

#endif
