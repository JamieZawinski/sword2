//********************************************
//	滚动条 相关函数
//  创建于2000年7月06日
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "scroll.h"

//构造
CScroll::CScroll()
{
	Pic=NULL;
	bDrop=false;
	bCheck=false;
	lastcount=timeGetTime();
}

//析构
CScroll::~CScroll()
{
	_RELEASE( Pic );
}

//从INI文件读入
bool CScroll::LoadIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	Id=Ini.ReadInt(index,"Id");			//ID
	x=Ini.ReadInt(index,"x");			//显示位置
	y=Ini.ReadInt(index,"y");			//显示位置
	Style=Ini.ReadInt(index, "Style");	//风格
	Width=Ini.ReadInt(index,"Width");	//大小
	Height=Ini.ReadInt(index,"Height");	//大小

	MinNum=Ini.ReadInt(index, "MinNum");
	MaxNum=Ini.ReadInt(index, "MaxNum");
	CurNum=Ini.ReadInt(index, "CurNum");
	PageNum=Ini.ReadInt(index, "PageNum");

	char *str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);				//图片文件名
	_FREE(str);
	CreateBitmap(Pic, 0, 0, PicFileName);	//创建页面
	DDSetColorKey(Pic, ColorKey);

	MakeRect();

	return true;
}

//生成各个矩形
void CScroll::MakeRect()
{
	if( Style == SS_VAR )
	{
		//生成各部件在图片中的位置
		Sub_Rect=GetRect(0, 0, Width, Width);
		Sub2_Rect=GetRect(Width, 0, Width*2, Width);
		Add_Rect=GetRect(0, Width, Width, Width*2);
		Add2_Rect=GetRect(Width, Width, Width*2, Width*2);
		Bar_Rect=GetRect(Width*2, 0, Width*3, Width);
		Space_Rect=GetRect(Width*2, Width, Width*3, Width*2);

		//生成的滚动条各部件位置
		CountNewRect();
		RectSub=GetRect(x, y, x+Width, y+Width);
		RectAdd=GetRect(x, y+Height-Width, x+Width, y+Height);
		RectSpace=GetRect(x, y+Width, x+Width, y+Height-Width);
	}
	else if( Style == SS_HOR )
	{
		//生成各部件在图片中的位置
		Sub_Rect=GetRect(0, Width*2, Width, Width*3);
		Sub2_Rect=GetRect(0, Width*3, Width, Width*4);
		Add_Rect=GetRect(Width, Width*2, Width*2, Width*3);
		Add2_Rect=GetRect(Width, Width*3, Width*2, Width*4);
		Bar_Rect=GetRect(Width*2, Width*2, Width*3, Width*3);
		Space_Rect=GetRect(Width*2, Width*3, Width*3, Width*4);

		//生成的滚动条各部件位置
		CountNewRect();
		RectSub=GetRect(x, y, x+Width, y+Width);
		RectAdd=GetRect(x+Height-Width, y, x+Height, y+Width);
		RectSpace=GetRect(x+Width, y, x+Height-Width, y+Width);
	}
}

//修正三个部件的区域
void CScroll::CountNewRect()
{
	if( Style == SS_VAR )
	{
		StepSize=(((float)Height-(float)Width*2)-(float)Width)/((float)MaxNum-(float)MinNum-(float)PageNum);
		RectPageSub=GetRect(x, y+Width, x+Width, int(y+Width+((CurNum-MinNum)*StepSize)));
		RectPageAdd=GetRect(x, y+Width+int(((CurNum-MinNum)*StepSize)+Width), x+Width, y+Height-Width);
		RectBar=GetRect(x, int(y+Width+((CurNum-MinNum)*StepSize)), x+Width, int(y+Width+((CurNum-MinNum)*StepSize)+Width));
		Rect=GetRect(x, y, x+Width, y+Height);
	}
	else if( Style == SS_HOR )
	{
		StepSize=(((float)Height-(float)Width*2)-(float)Width)/((float)MaxNum-(float)MinNum-(float)PageNum);
		RectPageSub=GetRect(x+Width, y, int(x+Width+((CurNum-MinNum)*StepSize)), y+Width);
		RectPageAdd=GetRect(x+Width+int(((CurNum-MinNum)*StepSize)+Width), y, x+Height-Width, y+Width);
		RectBar=GetRect(int(x+Width+((CurNum-MinNum)*StepSize)), y, int(x+Width+((CurNum-MinNum)*StepSize)+Width),  y+Width);
		Rect=GetRect(x, y, x+Height, y+Width);
	}
}

//显示
void CScroll::Show()
{

}

//按钮检测循环
bool CScroll::CheckLoop()
{
	//画部件
	if( InRect( &RectSub, point.x, point.y ) && Mouse.DownButton() == LBUTTON )	//鼠标在按钮上方并按下
		Blt(lpDDSBack, RectSub.left, RectSub.top, Pic, Sub2_Rect, true);
	else
		Blt(lpDDSBack, RectSub.left, RectSub.top, Pic, Sub_Rect, true);			//画减箭头

	if( InRect( &RectAdd, point.x, point.y ) && Mouse.DownButton() == LBUTTON )//鼠标在按钮上方并按下
		Blt(lpDDSBack, RectAdd.left, RectAdd.top, Pic, Add2_Rect, true);
	else
		Blt(lpDDSBack, RectAdd.left, RectAdd.top, Pic, Add_Rect, true);	//画加箭头

	SBlt(lpDDSBack, RectSpace, Pic, Space_Rect, true);					//画空白条
	Blt(lpDDSBack, RectBar.left, RectBar.top, Pic, Bar_Rect, true);		//画滚动条

	//按下鼠标左键
	if( InRect( &Rect, point.x, point.y) && mouse == LB_DOWN )
	{
		bCheck=1;
		lastcount=timeGetTime();
	}

	//松开鼠标左键
	if( mouse == LB_UP )
	{
		bCheck=0;
		bDrop=false;
	}

	//滚动处理
	if( !bDrop && ( bCheck==1 || ( bCheck==2 && timeGetTime() > lastcount + 500 ) ))
	{
		bCheck=2;

		//拖动
		if( InRect( &RectBar, point.x, point.y) )
		{
			bDrop=true;
		}
		//减按钮
		else if( InRect( &RectSub, point.x, point.y ) )
		{
			if( CurNum > MinNum )
			{
				CurNum--;
				CountNewRect();
			}
		}
		//加按钮
		else if( InRect( &RectAdd, point.x, point.y ) )
		{
			if( CurNum < MaxNum-PageNum )
			{
				CurNum++;
				CountNewRect();
			}
		}
		//上翻页
		else if( InRect( &RectPageSub, point.x, point.y ) )
		{
			CurNum-=PageNum;
			if( CurNum < MinNum ) CurNum=MinNum;
			CountNewRect();
		}
		//下翻页
		else if( InRect( &RectPageAdd, point.x, point.y ) )
		{
			CurNum+=PageNum;
			if( CurNum > MaxNum-PageNum ) CurNum=MaxNum-PageNum;
			CountNewRect();
		}
	}

	//拖动处理
	if( bDrop )
	{
		if( Style == SS_VAR )
		{
			if( point.y <= y+Width )
			{
				CurNum=MinNum;
			}
			else if( point.y >= y+Height-Width )
			{
				CurNum=MaxNum-PageNum;
			}
			else
			{
				CurNum=int( float(MaxNum-MinNum-PageNum) * float(point.y-y-Width) / float(Height-Width*3));
				if( CurNum > MaxNum-PageNum ) CurNum=MaxNum-PageNum;
				if( CurNum < MinNum ) CurNum=MinNum;
			}
		}
		else if( Style == SS_HOR )
		{
			if( point.x <= x+Width )
			{
				CurNum=MinNum;
			}
			else if( point.x >= x+Height-Width )
			{
				CurNum=MaxNum-PageNum;
			}
			else
			{
				CurNum=int( float(MaxNum-MinNum-PageNum) * float(point.x-x-Width) / float(Height-Width*3));
				if( CurNum > MaxNum-PageNum ) CurNum=MaxNum-PageNum;
				if( CurNum < MinNum ) CurNum=MinNum;
			}
		}
		
		CountNewRect();
	}

	return false;
}

//检测鼠标是否在一矩形范围
bool CScroll::InRect(RECT *rect, int px, int py)
{
	if( px > rect->left &&
		px < rect->right && 
		py > rect->top &&
		py < rect->bottom )
		return true;
	return false;
}

//-----------------------------

int CScroll::SetCurNum(int n) 
{
	CurNum=n;
	if( CurNum < MinNum )
		CurNum=MinNum;

	if( CurNum > MaxNum )
		CurNum=MaxNum;
	
	CountNewRect(); 
	return CurNum;
}

int CScroll::SetMinNum(int n) 
{
	MinNum=n;
	if( MinNum > MaxNum )
		MinNum=MaxNum;

	if( CurNum < MinNum )
		CurNum=MinNum;

	CountNewRect(); 
	return MinNum;
}

int CScroll::SetMaxNum(int n) 
{
	MaxNum=n;
	if( MaxNum < MinNum )
		MaxNum=MinNum;

	if( CurNum > MaxNum )
		CurNum=MaxNum;

	CountNewRect(); 
	return MaxNum;
}

int CScroll::SetPageNum(int n)
{
	PageNum=n;
	if( PageNum > MaxNum - MinNum )
		PageNum=MaxNum-MinNum;

	CountNewRect();
	return PageNum;
}