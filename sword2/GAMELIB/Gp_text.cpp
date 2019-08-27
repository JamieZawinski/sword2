//********************************************
//	文字显示相关处理函数
//  softboy 创建于2000年1月25日
//********************************************
#include <windows.h>
#include <stdio.h>
#include <fstream.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_other.h"
#include "gp_iniset.h"
#include "gp_draw.h"
#include "gp_alpha.h"
#include "gp_text.h"

/*****************************************************************/
/*和文字相关变量
/*****************************************************************/

DWORD CGpFont::FONTCOLOR[10];	//10种系统设定的字体颜色
DWORD CGpFont::DELAYTICK[10];	//文字出现的延时

CGpFont::CGpFont(bool set)
{
	if( set )
	{
		FONTCOLOR[0]=RGB(0,0,0);		//颜色
		FONTCOLOR[1]=RGB(255,0,0);
		FONTCOLOR[2]=RGB(0,255,0);
		FONTCOLOR[3]=RGB(0,0,255);
		FONTCOLOR[4]=RGB(255,255,0);
		FONTCOLOR[5]=RGB(255,0,255);
		FONTCOLOR[6]=RGB(0,255,255);
		FONTCOLOR[7]=RGB(255,0,255);
		FONTCOLOR[8]=RGB(127,127,127);
		FONTCOLOR[9]=RGB(225,255,225);

		DELAYTICK[0]=0;					//延时
		DELAYTICK[1]=10;
		DELAYTICK[2]=20;
		DELAYTICK[3]=30;
		DELAYTICK[4]=50;
		DELAYTICK[5]=80;
		DELAYTICK[6]=100;
		DELAYTICK[7]=200;
		DELAYTICK[8]=500;
		DELAYTICK[9]=1000;
	}
}

CGpFont::~CGpFont()
{
	if(font_Font!=NULL)					//释放资源
		DeleteObject(font_Font);
}


//****初始化字体******
void CGpFont::InitFont(int FontSize,char *FontType)
{
	font_Size=FontSize;		//大小
	font_Family=FontType;	//字体
	font_Color=RGB(255,255,255);
	font_BK=TRANSPARENT;	//字的背景是否镂空
	font_BKColor=RGB(255,255,255);	//背景色
	font_Width=70;			//一行的文字数　中文＝width/2
	font_LineHeight=20;		//行间距
	bChangeColor=true;
	DelayTick=30;

	if( font_Font != NULL ) DeleteObject( font_Font );

	font_Font = CreateFont( FontSize,				// 字符高度(逻辑单位)
							0,						// 字符宽度(逻辑单位) 0为自动匹配
							0,						// 字符行角度
							0,						// 字符基线角度
							font_Weight,			// 0--1000 字体浓暗程度, 400为正常, 700为粗体
							FALSE,					// 是否为斜体字
							FALSE,					// 是否加下划线
							0,						// 是否加中划线
							GB2312_CHARSET,			// 指定字符集
							OUT_DEFAULT_PRECIS,		// 指定输出精度
							CLIP_DEFAULT_PRECIS,	// 指定剪切精度
							DEFAULT_QUALITY,		// 指定输出质量
							DEFAULT_PITCH|FF_SWISS, // 指定间距, 字体族
							FontType				// 字体名
							);
}

//读入字体
void CGpFont::LoadFont(char *filename, char *index)
{
	//初始化字体
	CIniSet FontIni(filename);
	int size=FontIni.ReadInt(index, "font_Size");
	char *font=FontIni.ReadText(index, "font_Family");
	font_Weight=FontIni.ReadInt(index, "font_Weight");

	InitFont(size, font);
	if( font_Weight >= 700 ) font_Exp=1;
	else font_Exp=0;
	_FREE( font );
	
	font_LineHeight=FontIni.ReadInt(index,"font_LineHeight");
	font_Width=FontIni.ReadInt(index,"font_Width");
	
	char *Col=FontIni.ReadText(index, "font_Color");
	font_Color=RGB(FontIni.ReadInt(Col, "Red"), FontIni.ReadInt(Col, "Green"), FontIni.ReadInt(Col,"Blue"));
	_FREE( Col );
	
	char *BkCol=FontIni.ReadText(index, "font_BKColor");
	font_BKColor=RGB(FontIni.ReadInt(BkCol, "Red"), FontIni.ReadInt(BkCol, "Green"), FontIni.ReadInt(BkCol,"Blue"));
	_FREE( BkCol );

	if( FontIni.ReadInt(index, "font_BK") )
		font_BK=TRANSPARENT;	//字的背景是否镂空
	else
		font_BK=OPAQUE;
}

//*****************
//设置颜色
void CGpFont::SetColor(DWORD color)
{
	font_Color=color;
}

//******************
//获取颜色
DWORD CGpFont::GetColor()
{
	return font_Color;
}

//恢复缺省字体
void CGpFont::Restore_Default_Font()
{
	font_BK=TRANSPARENT;	//字的背景是否镂空
	font_BKColor=RGB(255,255,255);	//背景色
	font_Width=80;			//一行的文字数　中文＝width/2
	font_LineHeight=20;		//行间距
}

//设置字的背景是否镂空
void CGpFont::SetBK(bool c)
{
	if( c )
		font_BK=TRANSPARENT;
	else
		font_BK=OPAQUE;
}

//设置一个字符串中间是否可以变色
void CGpFont::SetChangeColor(bool c)
{
	bChangeColor=c;
}

//*********************************
//Alpha文字显示
void CGpFont::PrintAlphaText(LPDIRECTDRAWSURFACE Surf, int x, int y, int alpha, char *Text,...)
{
	va_list va;
	char *str;
	str=(char *)malloc(strlen(Text)+256);	//预先留下256个char的空间
	char *Str=str;							//保存该指针到以后的free用

	va_start(va,Text);
	vsprintf(str,Text,va);					//放到str中
	va_end(va);

	HDC hdc;
	int ChrLen;
	int ascii=0;
	char Tmp[256];

	if( str[0]==0 ) 
	{
		_FREE(Str);
		return;
	}

	//临时页面
	RECT rect={0,0,font_Width*font_Size , font_Size};
	LPDIRECTDRAWSURFACE TempSurface;
	CreateBitmap(TempSurface, rect.right , rect.bottom);
	DDSetColorKey(TempSurface, ColorKey);
	FillSurface(TempSurface, ColorKey16);

	//获取GDI表面
	TempSurface->GetDC(&hdc);
	SetBkMode(hdc, font_BK);
	SetBkColor(hdc, font_BKColor);
	SetTextColor(hdc, font_Color);
	SelectObject(hdc,font_Font);

	ChrLen=strlen(str)+1;		

	for(int i=0; i<=ChrLen/font_Width; i++ )	//分行处理
	{
		ascii=0;
		memset(Tmp,0,256);
		//如果不是最后一行，就用一行做长度，否则是剩余的长度
		int Chr=((i==ChrLen/font_Width)?ChrLen%font_Width:font_Width);
		for(int j=0; j<Chr; j++)
		{
			Tmp[j]=*str;
			str++;

			//计算英文数量(0=双数 1=单数)
			if( (unsigned char)Tmp[j]<128 )		//汉字的显示，全是大于128的
				ascii=1-ascii;					//说明是个英文，记数

			if( *str=='<' && *(str+1)=='C' && *(str+2)=='R' && *(str+3)=='>' )
			{
				str=str+4;		//〈CR〉是换行
				i--;
				goto _Show;
			}
		}

		//如果最后半个是汉字：要是英文的数目是单数的话，必然会单出一个来
		if( ascii==1 )
		{
			str--;			//留到下一行
		}
		
_Show:	TextOut(hdc, 0, 0, Tmp, strlen(Tmp));		//显示到临时页面
		TempSurface->ReleaseDC(hdc);

		ABlt(Surf, x, y, TempSurface, rect, ColorKey16, alpha);		//AlphaBlt
		y+=font_LineHeight;							//显示下一行文字的y坐标

		FillSurface(TempSurface, ColorKey16);		//清除页面
		//重新获取GDI表面
		TempSurface->GetDC(&hdc);
		SetBkMode(hdc, font_BK);
		SetBkColor(hdc, font_BKColor);
		SetTextColor(hdc, font_Color);
		SelectObject(hdc,font_Font);
	}

	TempSurface->ReleaseDC(hdc);
	_RELEASE( TempSurface );
	_FREE(Str);
}

//*********************************
//文字显示
void CGpFont::PrintText(LPDIRECTDRAWSURFACE Surf,int x,int y,char *Text,...)
{
	char *str;
	str=(char *)malloc(strlen(Text)+256);
	char *BackStr=str;	//备份指针，等到free使用

	va_list va;
	va_start(va,Text);
	vsprintf(str,Text,va);
	va_end(va);

	if( str=="" ) 
	{
		_FREE(BackStr);
		return;
	}

	//获取GDI表面
	HDC hdc;
	Surf->GetDC(&hdc);
	SetBkMode(hdc, font_BK);
	SetBkColor(hdc, font_BKColor);
	SetTextColor(hdc, font_Color);
	SelectObject(hdc,font_Font);

	int StrLen, CurLen, LeftLen;
	int ascii=0, Chars=0;
	int showx=x;	//文字显示位置
	char Tmp[256];

	StrLen=strlen(str);
	while( str < BackStr + StrLen )
	{
		ascii=0;
		Chars=0;
		showx=x;
		memset(Tmp,0,256);

		//计算本行的字符数
		LeftLen=StrLen-(str-BackStr);	//剩余的字符长度
		if( LeftLen > font_Width ) 
			CurLen=font_Width;
		else 
			CurLen=LeftLen;

		while( Chars < CurLen&&*str )
		{
			//特殊功能
			if( *str=='<' )
			{
				//设置字体颜色
				if( *(str+1)=='c' && *(str+3)=='>' )	//<c1>等等，设置颜色功能
				{
					if( bChangeColor )					//允许彩色显示
					{
						TextOut(hdc, showx, y, Tmp, strlen(Tmp));	//显示前面的部分
						SetTextColor(hdc, FONTCOLOR[*(str+2)-'0'] );	//设置颜色
					
						showx+=strlen(Tmp)*(font_Size+font_Exp)/2;	//接着显示的位置
						CurLen-=strlen(Tmp);			//本行剩下的长度
						memset(Tmp,0,256);				//清0
						Chars=0;						//位置复位
					}
					
					str+=4;								//跳过控制字符
					continue;							//接着显示后面的内容
				}
				
				//恢复字体颜色
				if( *(str+1)=='/' && *(str+2)=='c' && *(str+3)=='>' )	//恢复颜色
				{
					if( bChangeColor )					//能够改变颜色
					{
						TextOut(hdc, showx, y, Tmp, strlen(Tmp));		//显示
						SetTextColor(hdc, font_Color);					//设置颜色

						showx+=strlen(Tmp)*(font_Size+font_Exp)/2;			
						CurLen-=strlen(Tmp);
						memset(Tmp,0,256);
						Chars=0;
					}

					str+=4;
					continue;
				}

				//设置延时
				if( *(str+1)=='d' && *(str+3)=='>' )			//<d>延时功能
				{
					DelayTick=DELAYTICK[*(str+2)-'0'];			//查表得到延时长短
					str+=4;										//跳过
					continue;
				}
				
				//恢复原来的速度
				if( *(str+1)=='/' && *(str+2)=='d' && *(str+3)=='>' )
				{
					DelayTick=DELAYTICK[3];
					str+=4;
					continue;
				}

				//换行
				if( *(str+1)=='/' && *(str+2)=='n' && *(str+3)=='>' )
				{
					str+=4;
					goto _Show;
				}
			}

			Tmp[Chars]=*str;			//正常字符，直接copy
			str++;
			Chars++;

			//计算英文字符数量(0=双数 1=单数)
			if( (unsigned char)Tmp[Chars-1]<128 )		//同样的中英文处理
				ascii=1-ascii;
		}

		//如果最后半个是汉字
		if( ascii==1 && str < BackStr + StrLen )
		{
			str--;					//留到下一行
			Tmp[Chars-1]=0;
		}

_Show:
		TextOut(hdc, showx, y, Tmp, strlen(Tmp));
		y+=font_LineHeight;			//下一行的y坐标
	}

	Surf->ReleaseDC(hdc);
	_FREE( BackStr );
}
// 字体信息
void CGpFont::GetFontInfo(LPDIRECTDRAWSURFACE surf, TEXTMETRIC *tm)
{
	HDC hdc;
	surf->GetDC(&hdc);
	GetTextMetrics(hdc,tm);
	surf->ReleaseDC(hdc);
}
