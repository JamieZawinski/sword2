//********************************************
//	动画 相关函数
//  创建于2000年4月16日
//********************************************
#include <windows.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_iniset.h"
#include "gp_draw.h"
#include "gp_other.h"
#include "gp_ani.h"

CAni::CAni()
{
	Pics=NULL;
}

//释放
CAni::~CAni()
{
	_RELEASE( Pics );
}

//读取一个动画
bool CAni::LoadAni(char *filename, char *index)
{
	CIniSet Ini(filename);

	x=Ini.ReadInt(index,"x");			//动画显示位置
	y=Ini.ReadInt(index,"y");			//动画显示位置
	tx=Ini.ReadInt(index,"tx");			//热点偏移
	ty=Ini.ReadInt(index,"ty");			//热点偏移
	Width=Ini.ReadInt(index,"Width");	//动画大小
	Height=Ini.ReadInt(index,"Height");	//动画大小

	Max_Pics=Ini.ReadInt(index,"Max_Pics");		//图片数目
	Cur_Pic=Ini.ReadInt(index,"Cur_Pic");		//显示的当前侦
	ShowCount=Ini.ReadInt(index,"ShowCount");	//记数器

 	char *str=Ini.ReadText(index,"FileName");
	strcpy(FileName, str);						//图片文件名
	_FREE( str );

	CreateBitmap(Pics, 0,0, FileName);			//创建页面
	DDSetColorKey(Pics, ColorKey);
	WLines=Ini.ReadInt(index,"WLines");
	HLines=Ini.ReadInt(index,"HLines");			//图片的横竖排列个数
	bArrayH=Ini.ReadInt(index,"bArrayH");		//横向优先

	Delay=Ini.ReadInt(index,"Delay");			//两帧间的间隔时间
	bColorKey=Ini.ReadInt(index,"bColorKey");	//两帧间的间隔时间
	bActive=Ini.ReadInt(index,"bActive");		//是否活动
	bShow=Ini.ReadInt(index,"bShow");			//是否显示
	
	Cur_Rect=GetRect( 0, 0, Width, Height );
	
	OldTick=timeGetTime();
	return true;
}

//显示
void CAni::ShowAni(LPDIRECTDRAWSURFACE Sour, int lx, int ly, int Cur_Picture)
{
	if( !bShow || ShowCount==0 )
		return;

	if( bActive && timeGetTime() > OldTick+Delay )	//时间到
	{
		OldTick=timeGetTime();

		if( Cur_Picture== -1 ) 
			Cur_Picture=Cur_Pic;

		int x1, y1;
		if( bArrayH )	//横向优先排列
		{
			x1=Cur_Picture%WLines;
			y1=Cur_Picture/WLines;
		}
		else
		{
			x1=Cur_Picture%HLines;
			y1=Cur_Picture/HLines;
		}

		int W=Width*x1;
		int H=Height*y1;
		Cur_Rect=GetRect( W, H, W+Width, H+Height );
	
		Cur_Pic=(Cur_Pic+1)%Max_Pics;		//下一帧
		if( ShowCount > 0 ) ShowCount--;	//记数器减1
	}

	Blt(Sour, lx+tx, ly+ty, Pics, Cur_Rect, bColorKey);	//显示
}

//显示
void CAni::ShowAni(LPDIRECTDRAWSURFACE Sour)
{
	if( !bShow || ShowCount==0 )
		return;

	if( bActive && timeGetTime() > OldTick+Delay )	//时间到
	{
		OldTick=timeGetTime();

		int x1, y1;
		if( bArrayH )	//横向优先排列
		{
			x1=Cur_Pic%WLines;
			y1=Cur_Pic/WLines;
		}
		else
		{
			x1=Cur_Pic/HLines;
			y1=Cur_Pic/HLines;
		}

		int W=Width*x1;
		int H=Height*y1;
		Cur_Rect=GetRect( W, H, W+Width, H+Height );
	
		Cur_Pic=(Cur_Pic+1)%Max_Pics;		//下一帧
		if( ShowCount > 0 ) ShowCount--;	//记数器减1
	}

	Blt(Sour, x+tx, y+ty, Pics, Cur_Rect, bColorKey);	//显示
}
