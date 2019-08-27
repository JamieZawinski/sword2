//********************************************
//	文字显示相关处理函数
//  softboy 创建于2000年1月25日
//********************************************
#ifndef _GP_FONT_
#define _GP_FONT_

#include <windows.h>

//文字类
class CGpFont{
public:
	static DWORD FONTCOLOR[10];		//固定文字颜色
	static DWORD DELAYTICK[10];		//文字出现的延时

	HFONT font_Font;	//字体对象
	int font_Size;		//大小
	char *font_Family;	//字体
	int font_Weight;	//线宽(0-1000)
	int font_Exp;		//字体大小修正值(0-699=0 700-1000=1)
	DWORD font_Color;	//颜色
	DWORD font_BK;		//字的背景是否镂空
	DWORD font_BKColor;	//背景色
	int font_Width;		//一行的文字数　中文＝width/2
	int font_LineHeight;//行间距
	bool bChangeColor;	//在一段文字中是否可以变换颜色
	int DelayTick;		//在文字依次出现时的延时

public:
	void GetFontInfo(LPDIRECTDRAWSURFACE surf,TEXTMETRIC* tm);
	CGpFont(bool=false);

	void InitFont(int, char *);		//初始化字体
	void LoadFont(char *,char *);	//从配置文件读字体
	void SetColor(DWORD);			//设置颜色
	DWORD GetColor();				//获取颜色
	void SetBK(bool);				//设置字体背景是否镂空
	void Restore_Default_Font();	//恢复缺省字体
	void SetChangeColor(bool);		//设置在一段文字中是否可以变换颜色

	~CGpFont();
	void PrintText(LPDIRECTDRAWSURFACE,int,int,char *,...);					//文字显示
	void PrintAlphaText(LPDIRECTDRAWSURFACE, int, int, int, char *,...);	//Alpha文字显示
};

#endif