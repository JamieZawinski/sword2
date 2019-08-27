//********************************************
//	进度条 相关函数
//  创建于2001年5月12日
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "proess.h"

//构造
CProess::CProess()
{
	fCurCount=0.f;
	Pic=NULL;
}

//析构
CProess::~CProess()
{
	_RELEASE( Pic );
}

//从INI文件读入
bool CProess::LoadIni(char *filename, char *index)
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

	Normal_Rect.left=0;
	Normal_Rect.top=0;
	Normal_Rect.right=Width;
	Normal_Rect.bottom=Height;

	Over_Rect.left=0;
	Over_Rect.top=Height;
	Over_Rect.right=Width;
	Over_Rect.bottom=Height*2;

	DestRect.left=x;
	DestRect.top=y;
	DestRect.right=x+Width;
	DestRect.bottom=y+Height;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);	//图片文件名
	_FREE(str);

	str=Ini.ReadText(index,"Dest");
	strcpy(strDest, str);	//显示的str
	_FREE(str);
	
	_RELEASE(Pic);
	CreateBitmap(Pic, 0, 0, PicFileName);			//创建页面
	DDSetColorKey(Pic, ColorKey);

	return true;
}

//显示
void CProess::Show(LPDIRECTDRAWSURFACE surf)
{
	Over_Rect.right = int(Width * fCurCount);		//计算显示的多少，进度条的长度
	DestRect.right = x + Over_Rect.right;
	surf->Blt( &DestRect, Pic, &Over_Rect, DDBLT_KEYSRC, 0);

	DestRect.right = x + Width;
	surf->Blt( &DestRect, Pic, &Normal_Rect, DDBLT_KEYSRC, 0);	//画上边框
}

//按钮检测循环
bool CProess::CheckLoop(LPDIRECTDRAWSURFACE surf)
{
	return false;
}
