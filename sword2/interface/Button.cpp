//********************************************
//	按钮 相关函数
//  创建于2000年6月19日
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "button.h"
#include <stdio.h>

//构造
CCButton::CCButton()
{
	Pic=NULL;
	m_bTest = NULL;
}

//析构
CCButton::~CCButton()
{
	_RELEASE( Pic );
	_DELETE_ARRAY(m_bTest);
}

//从INI文件读入
bool CCButton::LoadIni(char *filename, char *index)
{
	CIniSet Ini(filename);
	char *str;

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

	Down_Rect.left=0;
	Down_Rect.top=Height*2;
	Down_Rect.right=Width;
	Down_Rect.bottom=Height*3;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);					//图片文件名
	_FREE(str);
	CreateBitmap(Pic, 0, 0, PicFileName);		//创建页面
	DDSetColorKey(Pic, ColorKey);

	int nUseRgn = Ini.ReadInt(index,"UseRgn");	//是否是不规则的按纽
	if(nUseRgn)
	{
		if(!CreateRgn())
			return false;
	}
	return true;
}

//显示
void CCButton::Show(LPDIRECTDRAWSURFACE surf, int bHighLight)
{
	if( bHighLight != -1 )
	{
		RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
		Blt(surf, x, y, Pic, rect, true);
	}
	else
		Blt(surf, x, y, Pic, Normal_Rect, true);
}

//按钮检测循环
// 返回 ret:( 0=NULL, 1=按下, 2=鼠标在上面 3=按住不放)
BOOL CCButton::CheckLoop(LPDIRECTDRAWSURFACE surf, int bHighLight)
{
	BOOL ret=false;

	//鼠标在按钮上方，不规则的用区域检查，另外的用长方形检查
	if(CheckIn())
	{
		if( Mouse.DownButton() == LBUTTON )	//按下
		{
			Blt(surf, x, y, Pic, Down_Rect, true);
			ret =3;
		}
		else
		{
			if( bHighLight != -1 )
			{
				RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
				Blt(surf, x, y, Pic, rect, true);
				ret=false;
			}
			else
			{
				Blt(surf, x, y, Pic, Over_Rect, true);
				ret=2;	//高亮状态
			}
		}
		
		if( mouse == LB_UP )
		{
			mouse=0;
			return 1;
		}
	}
	else	//鼠标没有在按钮上方
	{
		if( bHighLight != -1 )
		{
			RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
			Blt(surf, x, y, Pic, rect, true);
		}
		else
			Blt(surf, x, y, Pic, Normal_Rect, true);
	}

	return ret;
}

//创建区域
//return true:成功，false:失败
bool CCButton::CreateRgn()
{
	if(BeginDraw(Pic))
	{
		WORD* pSurf = GraphBuffer;
		int Pitch = GraphPitch;
		m_bTest = new bool[Width*Height];
		for(int i=0;i<Height;i++)
			for(int j =0;j<Width;j++)
			{
				if(pSurf[i*Pitch+j] == ColorKey16)
					m_bTest[j+i*Width] = false;
				else
					m_bTest[j+i*Width] = true;
			}
		EndDraw(Pic);
		return true;
	}
	return false;
}
//检测是不是在按纽区域里面
BOOL CCButton::CheckIn()
{
	if(point.x - x>=0&&point.x <x+Width&&point.y-y>=0&&point.y - y<Height)
		if(m_bTest)
			return m_bTest[(point.x - x)+(point.y - y)*Width];
		else
			return TRUE;
	return FALSE;
}
