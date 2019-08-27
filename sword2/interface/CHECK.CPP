//********************************************
//	复选按钮 相关函数
//  创建于2000年7月10日
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "check.h"

//构造
CCheck::CCheck()
{
	Pic=NULL;
}

//析构
CCheck::~CCheck()
{
	_RELEASE( Pic );
}

//从INI文件读入
bool CCheck::LoadIni(char *filename, char *index)
{
	char *str;
	CIniSet Ini(filename);	

	Id=Ini.ReadInt(index,"Id");			//ID
	str=Ini.ReadText(index,"Name");
	strcpy(Name, str);					//按钮名
	_FREE(str);
	x=Ini.ReadInt(index,"x");			//显示位置
	y=Ini.ReadInt(index,"y");			//显示位置
	Width=Ini.ReadInt(index,"Width");	//大小
	Height=Ini.ReadInt(index,"Height");	//大小

	bChecked=(Ini.ReadInt(index, "bChecked"))?true:false;
	
	//初始化字体
	char *fontfile=Ini.ReadText(index, "FontFile");
	char *fontindex=Ini.ReadText(index, "FontIndex");
	Font.LoadFont(fontfile, fontindex);
	_FREE(fontfile);
	_FREE(fontindex);

	Normal_Rect.left=0;
	Normal_Rect.top=0;
	Normal_Rect.right=Width;
	Normal_Rect.bottom=Height;

	Over_Rect.left=0;
	Over_Rect.top=Height;
	Over_Rect.right=Width;
	Over_Rect.bottom=Height*2;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);	//图片文件名
	_FREE(str);
	
	_RELEASE(Pic);
	CreateBitmap(Pic, 0, 0, PicFileName);			//创建页面
	DDSetColorKey(Pic, ColorKey);

	return true;
}

//显示
void CCheck::Show(LPDIRECTDRAWSURFACE surf)
{
	if( !bChecked )
		Blt(surf, x, y, Pic, Normal_Rect, true);
	else
		Blt(surf, x, y, Pic, Over_Rect, true);

	Font.PrintText(surf, x+Width+4, y, Name);
}

//按钮检测循环
bool CCheck::CheckLoop(LPDIRECTDRAWSURFACE surf)
{
	if( point.x > x && point.x < x+Width && point.y > y && point.y < y+Height )
	{
		if( mouse == LB_UP )
		{
			bChecked=!bChecked;
		}
	}

	Show(surf);
	return bChecked;
}
