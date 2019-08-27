//********************************************
//	DirectDraw相关处理函数
//  softboy 创建于2000年1月25日
//********************************************

#include <windows.h>
#include <stdio.h>
#include <fstream.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_draw.h"
#include "gp_input.h"
#include "gp_alpha.h"
#include "gp_text.h"
#include "gp_other.h"

//*****************游戏显示属性*****************
DWORD ColorKey=RGB(255,0,255);	//透明色
WORD ColorKey16;				//16位透明色
bool Is555 = true;				//是否是555格式
long nFrames=0;					//帧数测试
RECT RectScreen={0,0,ScreenWidth,ScreenHeight};	//全屏显示的目标矩形
RECT RectWindow;								//窗口显示的目标矩阵
POINT g_pointClient;							//窗口和客户区的偏移

//*****************DDraw变量********************
LPDIRECTDRAW        lpDD;			//DirectDraw对象
LPDIRECTDRAWSURFACE	lpDDSPrimary;	// 主页面
LPDIRECTDRAWSURFACE	lpDDSBack=NULL;	// 后台页面
extern LPDIRECTDRAWSURFACE lpDDSSour, lpDDSTemp;

//****************Lock 相关*********************
WORD *GraphBuffer = 0;		// 绘图缓冲区
int GraphPitch = 0;			// 缓冲区跨度
int GraphWidth = 0;			//页面宽度
int GraphHeight = 0;		//页面高度

DDSURFACEDESC		ddsd;
HRESULT				ddrval;

//16位RGB换算
WORD RGB16(WORD r, WORD g, WORD b)
{
	//简单
	if( Is555 )
		//rrrrr|ggggg|bbbbb    0xf8 = 11111000b
		return ((r&0xf8)<<7) | ((g&0xf8)<<2) | ((b&0xf8)>>3);
	else
		//rrrrr|gggggg|bbbbb    0xfc = 11111100
		return ((r&0xf8)<<8) | ((g&0xfc)<<3) | ((b&0xf8)>>3);
}

//24位转16位
WORD RGB16(DWORD color)
{
	WORD r,g,b;
	//也比较简单
	r=(WORD)(color>>16);	
	g=(WORD)(color>>8);
	b=(WORD)color;

	if( Is555 )
		return ((r&0xf8)<<7) | ((g&0xf8)<<2) | ((b&0xf8)>>3);
	else
		return ((r&0xf8)<<8) | ((g&0xfc)<<3) | ((b&0xf8)>>3);
}

//功能：获得位图文件的尺寸大小
//参数：窗口句柄，位图文件名，返回位图宽度，返回位图高度(引用参数)
BOOL LoadBitmapFileInfo(HWND hwnd, LPCTSTR filename, int &dx, int &dy)
{
	FILE *fp;
	if( (fp=fopen(filename, "rb"))==NULL )
	{
			char b[MAX_PATH*2];
			sprintf(b,"Error open BMP file: %s",filename);
			MessageBox(hwnd,b,"Load file info!",MB_OK);
			return FALSE;
	}
	//读入文件头
	BITMAPFILEHEADER bmpFileHeader;		
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//检查BM标志
	char *ptr=(char*)&bmpFileHeader.bfType;
	if(*ptr!='B' || *++ptr!='M')
	{
		MessageBox(hwnd,"Invalid bitmap file!","error",MB_OK);
		return FALSE;
	}
	//信息头
	BITMAPINFOHEADER bmpInfoHeader;
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);
	fclose(fp);
	//宽度，高度
	dx=bmpInfoHeader.biWidth;
	dy=bmpInfoHeader.biHeight;
	return TRUE;
}

//*********************************
//功能：创建页面
//参数：要创建的页面指针，宽度，高度，文件名，内存标志
BOOL CreateBitmap(LPDIRECTDRAWSURFACE &lpTemp,int x,int y,char *BitmapFile, DWORD MemoryFlag )
{
	DDSURFACEDESC	 ddsd;
	HRESULT          ddrval;

	//获得位图文件的尺寸
	if( x == 0 && y == 0 )
		if( LoadBitmapFileInfo(hWnd, BitmapFile, x, y)==FALSE )
			return FALSE;
	//创建一个页面
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | MemoryFlag;
	ddsd.dwWidth = x;
	ddsd.dwHeight = y;
	ddrval = lpDD->CreateSurface( &ddsd, &lpTemp, NULL );
	if( ddrval != DD_OK )
	{
		return initFail(BitmapFile);
	}
	
	if( BitmapFile!=NULL )
		DDReLoadBitmap(lpTemp,BitmapFile);
	
	return TRUE;
}

//***********************************************************
//功能：位图拷贝
//参数：目标表面，目标x，目标y，原表面，拷贝范围，是否带透明色
void Blt(LPDIRECTDRAWSURFACE SS,int x,int y,
		 LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flag)
{
	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}
	else 
		if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
		{ 
			rcRect.right = rcRect.left + ScreenWidth - x; 
		}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}
	else 
		if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
		{ 
			rcRect.bottom = rcRect.top + ScreenHeight - y; 
		}
	
	while( 1 )
	{
		ddrval = SS->BltFast( x, y, DS, &rcRect, Flag);

		if(	ddrval == DD_OK )
		{
			return;
		}
		
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
		    {
				return;
		    }
		}
		
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
		    return;
		}
    }
}

//***********************************************************
//功能：快速位图拷贝
//参数：目标表面，目标x，目标y，原表面，拷贝范围，是否带透明色
void BltFast(LPDIRECTDRAWSURFACE SS,int x,int y,
		 LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flag)
{
	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}
	else 
		if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
		{ 
			rcRect.right = rcRect.left + ScreenWidth - x; 
		}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}
	else 
		if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
		{
			rcRect.bottom = rcRect.top + ScreenHeight - y; 
		}

	SS->BltFast( x, y, DS, &rcRect, Flag);
}

//***********************************************************
//功能：带缩放的位图拷贝
//参数：目标表面，目标矩阵，原表面，拷贝范围，是否带透明色
BOOL SBlt(LPDIRECTDRAWSURFACE SS,RECT sr,
		 LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flag)
{
  DWORD Flags= (Flag==FALSE)?0:DDBLT_KEYSRC;

  //边界检查
  int SWidth=sr.right-sr.left, SHeight=sr.bottom-sr.top;

  int DWidth=dr.right-dr.left, DHeight=dr.bottom-dr.top;

  if( sr.left<0 )
  {
	  dr.left += (DWidth * (-sr.left)) / SWidth;
	  sr.left=0;
  }
  if( sr.top<0 ) 
  {
	  dr.top += (DHeight * (-sr.top)) / SHeight;
	  sr.top=0; 
  }
  if( sr.right > ScreenWidth ) 
  {
	  dr.right -= (DWidth * (sr.right-ScreenWidth)) / SWidth;	
	  sr.right=ScreenWidth; 
  }
  if( sr.bottom > ScreenHeight ) 
  {  
	  dr.bottom -= (DHeight *(sr.bottom-ScreenHeight)) / SHeight;
	  sr.bottom=ScreenHeight;
  }

 while( 1 )
 {
    ddrval = SS->Blt( &sr, DS, &dr, Flags, 0);

	if( ddrval == DD_OK )
	{
	   return TRUE;
	}
	if( ddrval == DDERR_SURFACELOST )
	{
	    ddrval = restoreAll();
	    if( ddrval != DD_OK )
	    {
		return TRUE;
	    }
	}
	if( ddrval != DDERR_WASSTILLDRAWING )
	{
	    return FALSE;
	}
    }
    if(ddrval != DD_OK)
    {
	return FALSE;
    }
}

//***********************************************************
//功能：带缩放的位图拷贝（不带边界检查）
//参数：目标表面，目标x，目标y，原表面，拷贝范围，是否带透明色
BOOL _SBlt(LPDIRECTDRAWSURFACE SS,RECT sr,
		 LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flag)
{
	DWORD Flags= (Flag==FALSE)?0:DDBLT_KEYSRC;
	while( 1 )
	{
	    ddrval = SS->Blt( &sr, DS, &dr, Flags, 0);

		if( ddrval == DD_OK )
		{
			return TRUE;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
			{
				return TRUE;
		    }
		}
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
			return FALSE;
		}
    }
}

//*****************************************************
//功能：翻转页面
void Flip(void)
{
	HRESULT ddrval;
	while( 1 )
	{
		ddrval = lpDDSPrimary->Flip( NULL, 0 );
		if( ddrval == DD_OK )
		{
		    break;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
		    {
				break;
		    }
		}
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
		    break;
		}
	}
}

//********************
//功能：恢复系统页面
HRESULT restoreAll( void )
{
    HRESULT	ddrval;
	
	if( lpDDSPrimary )
		ddrval = lpDDSPrimary->Restore();

	if( lpDDSBack )
		ddrval = lpDDSBack->Restore();
    return ddrval;
}

//********************************************
//功能：清屏
//参数：目标表面，颜色值
void Clrscr(LPDIRECTDRAWSURFACE surf,WORD color )
{
	DDBLTFX ddbltfx;
	ddbltfx.dwSize=sizeof(ddbltfx);
	ddbltfx.dwFillColor=color;
	surf->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx);
}

//-------------------------------

//功能：更新到屏幕
//参数：目标表面，更新范围
void _UpdateScreen(LPDIRECTDRAWSURFACE lpSurf, RECT DestRect)
{
	if( WindowMode==0 )	//全屏
	{
		Blt(lpDDSPrimary, 0, 0, lpSurf, DestRect, false);
	}
	else	//窗口
	{
		lpDDSPrimary->Blt( &RectWindow, lpSurf, &DestRect, DDBLTFAST_NOCOLORKEY, 0);
	}
}

/////////////////////////////////////////////////////////////////
//画元素
/////////////////////////////////////////////////////////////////
void PutPixel(LPDIRECTDRAWSURFACE surf, int x, int y, WORD color)
{
	BeginDraw(surf);
	EndDraw(surf);
	GraphBuffer[GraphPitch*y+x]=color;	
}
//设置一个点的颜色为color
void _PutPixel(int x, int y, WORD color)
{
	GraphBuffer[GraphPitch*y+x]=color;
}

//画圆
void Circle(LPDIRECTDRAWSURFACE surf, int x0, int y0, int r,WORD color)
{
	long flag;
	int x,y,xmax;
	static double SIN45=0.707106781186548;
	y=r;
	x=0;
	xmax=(int)(r*SIN45);			//只需要扫描45度的角
	flag=(1-r*2);
	//soft也是害人，一句注释都没有，我找了半天才知道：
	//这其实是一个很出名的算法
	//名字是Bresenham算法
	//我只说个大概
	//详细的就要自己去找这方面的解释
	if( BeginDraw(surf) )
	{
		while( x<=xmax )			//循环的条件，让x一直加，加到xmax为止，也就是从0度到45度
		{
			if(flag>=0)							//      |y          
			{									//      |           
				flag+=(6+((x-y)<<2));			//  \ ..... /
				y--;							//   .  |  .     
			}									//  . \ | / .       
			else								//  .  \|/  .     
				flag+=((x<<2)+2);				//--.---+---.--   仔细看看这些点的排列
												//  .  /|\  . x     
			//画8个点，也许是4个点(有重复的点)	//  . / | \ .    
			_PutPixel(x0+y, y0+x, color);		//   .  |  .        	
			_PutPixel(x0+x, y0+y, color);		//  / ..... \       
			_PutPixel(x0-x, y0+y, color);		//      |
			_PutPixel(x0-y, y0+x, color);		//      |
			_PutPixel(x0-y, y0-x, color);		//可以先看看下面的画线的函数然后再看这个，这
			_PutPixel(x0-x, y0-y, color);		//样比较好懂。想要画出这个圆来，关键是决定下
			_PutPixel(x0+x, y0-y, color);		//一个点该放到什么位置，这里有一个出名的判断
			_PutPixel(x0+y, y0-x, color);		//方法:Midpoint 方法…………
			x++;								//这儿放不下这么多的内容了，自己去找关于计算
		}										//机图形学的书来看吧。
		EndDraw(surf);							//或者要是你拿到的这份源代码中有一个叫圣二源
	}											//程序导读的文件的话，读读吧，里面有的。
}

//画线
void Line(LPDIRECTDRAWSURFACE surf, int left, int top, int right, int bottom, WORD color)
{
    register int t;
    int distance;
    int x=0,y=0,delta_x,delta_y,incx,incy;
    delta_x =right-left;
    delta_y=bottom-top;
	//根据x和y的变化量设置x和y的增长方式(每次加1还是减1或者是不变)
    if(delta_x>0) 
		incx=1;
    else 
		if (delta_x==0) 
			incx=0;
		else 
		{
			delta_x=-delta_x; 
			incx=-1;
		}

    if (delta_y>0) 
		incy=1;
    else 
		if (delta_y==0) 
			incy=0;
		else 
		{
			delta_y=-delta_y; 
			incy=-1;
		}

    if(delta_x>delta_y)
        distance=delta_x;
    else
        distance=delta_y;

    //开始画线了	
	//一样的Bresenham算法
	//看看右边的图先
	if( BeginDraw(surf) )
	{
		//两个端点也要画
		for (t=0; t<distance+2; t++)
		{
			//画点
			_PutPixel(left, top, color);	 //  o-------                         |
			x+=delta_x;					     // p1       --------                 | delta_y
			y+=delta_y;						 //                  -------      p2  |
			if(x>distance)					 //                         -------o  |
			{								 //------------------------------------	
				x-=distance;				 //                delta_x
				left+=incx;					 //这是一个基本的示意图
			}								 //接着来，看当delta_y<delta_x的时候，就会有x轴
			if(y>distance)					 //方向的水平线，那么什么时候才让y++呢？
			{								 //比较简单的就是让x增加delta_x/delta_y的时候才
				y-=distance;				 //让y++,当然这是个不精确的想法,不过这个算法
				top+=incy;					 //确是以这个为基础的。只不过是换了种方式而已，用
			}								 //的是加减法，不是除法。对照这个图，看看算法，
		}									 //明白了吗？--------------------------------
		EndDraw(surf);						 //害人的soft，我为了看懂这个算法，翻了好多的书	
	}										 //哦：）一句注释都没有，怎么说也要给个算法的名字
}											 //嘛，真是的。不是哪，开玩笑的。
											 //不过为了放这个大面积的注释，我可花了不少的功夫								

//画矩形
void Rectangle(LPDIRECTDRAWSURFACE surf, int x1, int y1, int x2, int y2, WORD color)
{
	Line(surf, x1, y1, x2, y1, color);
	Line(surf, x1, y1, x1, y2, color);
	Line(surf, x2, y1, x2, y2, color);
	Line(surf, x1, y2, x2, y2, color);
}

//功能：画实心矩形
//参数：目标表面，x1,y1,x2,y2,颜色
void Bar(LPDIRECTDRAWSURFACE surf, int x1, int y1, int x2, int y2, WORD color)
{
    DDBLTFX             ddbltfx;
    RECT                dest;
	ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = color;			//用指定的颜色填充
    dest.left = x1;
    dest.top = y1;
    dest.right = x2;
    dest.bottom = y2;

    if ( FAILED( surf->Blt( &dest, NULL, NULL,
                    DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx ) ) )
	{
        return;
    }
}

//--------------------------------------------------------

//初始化失败的处理函数
BOOL initFail(char *str)
{
    FreeDDraw();
    MessageBox( hWnd, str, "Init Fail", MB_OK );
    DestroyWindow( hWnd );
    return FALSE;

} 

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//功能：获取一个表面的大小
//参数：目标表面，返回宽，返回高
//返回：表面跨距
int GetSurfaceSize(LPDIRECTDRAWSURFACE surf,int &x,int &y)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		x=ddsd.dwWidth;
		y=ddsd.dwHeight;
		return ddsd.lPitch>>1;		//不说了
	}	
	
	return false;
}

//****************************
//获取一个表面的宽度
int GetSurfaceWidth(LPDIRECTDRAWSURFACE surf)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		return ddsd.dwWidth;
	}	
	
	return false;
}

//****************************
//获取一个表面的高度
int GetSurfaceHeight(LPDIRECTDRAWSURFACE surf)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		return ddsd.dwHeight;
	}	
	
	return false;
}

//************************************
//开页面缓冲区，然后直接在缓冲区中操作
bool BeginDraw(LPDIRECTDRAWSURFACE lpSur)
{
    ddsd.dwSize = sizeof(ddsd);
    
	while( 1 )
	{ 
		ddrval = lpSur->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );	//锁定
		if( ddrval == DD_OK )
		{
		    break;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		  	if( ddrval != DD_OK ) return( false );
		} 
	}

  	GraphBuffer = (WORD *)ddsd.lpSurface;		//内存指针
	GraphWidth = ddsd.dwWidth;					//宽度
	GraphHeight = ddsd.dwHeight;				//高度
	GraphPitch  = ddsd.lPitch >> 1;	//lPitch以Byte为单位计数的，
									//GraphPitch以WORD为单位。所以GraphPitch = lPitch / 2;
    return( true );
}

//******************
//关闭缓冲区
bool EndDraw(LPDIRECTDRAWSURFACE lpSur)
{
   	ddrval = lpSur->Unlock( NULL );				//解锁
	if( ddrval != DD_OK ) return( false );
	return true;
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//功能：以单色填充页面
//参数：目标表面，颜色
void FillSurface(LPDIRECTDRAWSURFACE surf, DWORD color)
{
	WORD color16=RGB16(color);				//转化成16色

	if( BeginDraw(surf) )
	{
	for( int i=0; i<GraphHeight; i++)
		for(int j=0; j<GraphWidth; j++)
			GraphBuffer[i*GraphPitch+j] = color16;		//直接填充
	EndDraw(surf);
	}
}

//以单色填充页面
void FillSurface(LPDIRECTDRAWSURFACE surf, WORD color)
{
	if( BeginDraw(surf) )
	{
	for( int i=0; i<GraphHeight; i++)
		for(int j=0; j<GraphWidth; j++)
			GraphBuffer[i*GraphPitch+j] = color;		//直接填充
	EndDraw(surf);
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//功能：特效淡入
//参数：风格，延时（缺省），颜色（缺省）
void FadeIn(int Style, int time, WORD Color)
{
	switch( Style )
	{
	case 0:	//无效果
		break;

	case 1:	//百叶窗
		ScreenIn();
		break;
	
	case 2:	//淡入淡出
		ColorIn(time, Color);
		break;
	}
}

//功能：特效淡出
//参数：风格，延时（缺省），目标表面（缺省）
void FadeOut(int Style, int time, LPDIRECTDRAWSURFACE lpSurf)
{
	switch( Style )
	{
	case 0:	//无效果
		break;

	case 1:	//百叶窗
		ScreenOut(lpSurf);
		break;
	
	case 2:	//淡入淡出
		ColorOut(lpSurf, time);
		break;
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//淡入(time=每一帧的时间)
void ColorIn(int time, WORD Color)
{
	unsigned int alpha=2, oldtick, newtick;
	WORD *lpSurf, *lpTemp, BlackColor=Color;

	//建立临时页面
	LPDIRECTDRAWSURFACE Temp;
	CreateBitmap(Temp, ScreenWidth, ScreenHeight);
	//保存到临时页面
	if( WindowMode==0 )	//全屏
		Blt(Temp, 0, 0, lpDDSPrimary, RectScreen, false);
	else
		Blt(Temp, 0, 0, lpDDSPrimary, RectWindow, false);

	if( BeginDraw(lpDDSBack) )
	{
		lpSurf=GraphBuffer;
		EndDraw(lpDDSBack);
	}

	if( BeginDraw(Temp) )
	{
		lpTemp=GraphBuffer;
		EndDraw(Temp);
	}

	oldtick=timeGetTime();	//起始时间

	for(int i=0; i<16; i++)
	{
		//把主表面的点和传近来的点进行alpha混合到lpDDSBack
		for(int j=0; j<ScreenWidth*ScreenHeight; j++)
		{
			lpSurf[j]=Alpha_Pixel(BlackColor, lpTemp[j], alpha);
		}

		//延时
		newtick=timeGetTime();
		while( newtick-oldtick < (unsigned int)(time * i) )
		{
			newtick=timeGetTime();
		}
		
		//更新屏幕
		_UpdateScreen();
		
		//自动跳桢
		i=(newtick-oldtick)/time;
		//alpha增加
		alpha=i*2+2;

		if( i >= 15 ) //结束
		{
			alpha=31;
			for(int j=0; j<ScreenWidth*ScreenHeight; j++)
			{
				lpSurf[j]=Alpha_Pixel(BlackColor, lpTemp[j], alpha);
			}
			_UpdateScreen();
			break;
		}
	}
	
	//释放
	_RELEASE( Temp );
}

//淡出(time=每一帧的时间)
void ColorOut(LPDIRECTDRAWSURFACE dest, int time)
{
	unsigned int alpha=2, oldtick, newtick;
	WORD *lpSour, *lpDest, *lpBack;

	if( WindowMode != 0 )	//窗口
	{
		Blt(lpDDSSour, 0,0, lpDDSPrimary, RectWindow, false);
	}
	else
	{
		Blt(lpDDSSour, 0,0, lpDDSPrimary, RectScreen, false);
	}

	if( BeginDraw(lpDDSSour) )
	{
		lpSour=GraphBuffer;
		EndDraw(lpDDSSour);
	}

	if( BeginDraw(dest) )
	{
		lpDest=GraphBuffer;
		EndDraw(dest);
	}

	if( BeginDraw(lpDDSTemp) )
	{
		lpBack=GraphBuffer;
		EndDraw(lpDDSTemp);
	}

	oldtick=timeGetTime();

	for(int i=0; i<16; i++)
	{
		for(int j=0; j<ScreenWidth*ScreenHeight; j++)
		{
			//看看上面的函数对比一下
			//这个是从屏幕逐渐变化到dest页面
			lpBack[j]=Alpha_Pixel(lpDest[j], lpSour[j], alpha);
		}

		//延时
		newtick=timeGetTime();
		while( newtick-oldtick < (unsigned int)(time*i) )	
		{
			newtick=timeGetTime();
		}

		//更新屏幕
		_UpdateScreen(lpDDSTemp);

		//自动跳桢
		i=(newtick-oldtick)/time;
		alpha=i*2+2;

		if( i >= 15 ) //结束
		{
			alpha=31;
			for(int j=0; j<ScreenWidth*ScreenHeight; j++)
			{
				lpBack[j]=Alpha_Pixel(lpDest[j], lpSour[j], alpha);
			}
			_UpdateScreen(lpDDSTemp);
			break;
		}
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//残像淡入(suf:目标页面，step:残像的多少，time:延时 20左右)
void ScreenIn(int Step,int Time)
{
	long T1=0, T2=0;
	int tmp=ScreenHeight/Step;
	WORD *Dest;
	T1=T2=timeGetTime();
	
	//取页面指针
	if( BeginDraw(lpDDSBack) )
	{
		Dest=(WORD *)GraphBuffer;
		EndDraw(lpDDSBack);
	}
	
	for( int j=0; j<tmp; j++)
	{
		//延时
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;

		for(int i=0; i<Step; i++)
			memset(&Dest[ScreenWidth*i*tmp+ScreenWidth*j], 0, ScreenWidth*2);
		
		_UpdateScreen();		//更新屏幕
	}
}

//残像淡出（开始页面，目标页面，残像数，延时）
void ScreenOut(LPDIRECTDRAWSURFACE dest,int Step,int Time)
{
	long T1, T2;
	WORD *Dest,*Sour;
	T1=T2=timeGetTime();
	
	//临时页面
	LPDIRECTDRAWSURFACE lpDDSTemp;
	CreateBitmap(lpDDSTemp, ScreenWidth, ScreenHeight);

	if( WindowMode != 0 )	//窗口
	{
		Blt(lpDDSTemp, 0,0, dest, RectScreen, false);
		Blt(lpDDSBack, 0,0, lpDDSPrimary, RectWindow, false);
	}
	else
	{
		Blt(lpDDSTemp, 0,0, dest, RectScreen, false);
		Blt(lpDDSBack, 0,0, lpDDSPrimary, RectScreen, false);
	}

	//取目标页面指针
	if( BeginDraw(lpDDSBack) )
	{
		Sour=(WORD *)GraphBuffer;
		EndDraw(lpDDSBack);
	}

	//取目标页面指针
	if( BeginDraw(lpDDSTemp) )
	{
		Dest=(WORD *)GraphBuffer;
		EndDraw(lpDDSTemp);
	}
	
	for( int j=0; j<ScreenHeight/Step; j++)
	{
		//延时
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;
	
		for(int i=0; i<Step; i++)
		{
			int sult=ScreenWidth*i*(ScreenHeight/Step)+ScreenWidth*j;
			memcpy( &Sour[sult], &Dest[sult], ScreenWidth*2);
		}

		_UpdateScreen();		//更新屏幕
	}

	//释放
	_RELEASE( lpDDSTemp );
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//把一个表面顺时针旋转90度(本函数仅限于正方型矩阵)
void RolSurface(LPDIRECTDRAWSURFACE SS,int w)
{
	WORD *Surf,*Temp;
	int Width;
	LPDIRECTDRAWSURFACE DS;
	
	//取页面指针
	if( BeginDraw(SS) )
	{
		Surf=(WORD *)GraphBuffer;
		Width=GraphPitch;
		EndDraw(SS);
	}

	CreateBitmap(DS, w, w, NULL, DDSCAPS_SYSTEMMEMORY);
	//取页面指针
	if( BeginDraw(DS) )
	{
		Temp=(WORD *)GraphBuffer;
		EndDraw(DS);
	}

	_asm{
		mov edi, Temp		//指针保存到esi和edi中
		mov esi, Surf

		mov ebx, 0			//两个循环变量i,j
_loop1:	mov ecx, 0	
_loop2:	mov eax, Width		//eax放的是要翻转的表面的跨度
		mul ebx				//i*Width+j
		add eax, ecx
		shl eax, 1			//乘2
		push eax			//压栈：(i*Width+j)*2
		
		mov eax, w			//正方行的边长
		dec eax				//减一
		sub eax, ecx		//减j
		mul Width			//乘上跨度
		add eax, ebx		//加上i
		shl eax, 1			//eax=((w-1-j)*Width+i)*2

		mov dx, [esi+eax]	//dx放的是当前点的位置
		pop eax				//恢复eax的值，eax = (i*Width+j)*2
		mov [edi+eax], dx	//放到新表面的相应点
		
		inc ecx				//j++
		cmp ecx, w			//j<w吗？
		jnz _loop2			//还没有就还是在第二层循环
		
		inc ebx				//否则，i++
		cmp ebx, w			//i<w吗？
		jnz _loop1			//还没有就继续循环
	}
		
	memcpy(Surf, Temp, (Width*w)*2);	//把temp复制到surf上
	//free(Temp);
}
//下面三个inline函数得到R，G，B的值
inline unsigned char GetRed(WORD color)
{
	if( Is555 )
		return (color>>7) & 0xff;
	else
		return (color>>8) & 0xff;
}

inline unsigned char GetGreen(WORD color)
{
	if( Is555 )
		return (color>>2) & 0xff;
	else
		return (color>>3) & 0xff;
}
	
inline unsigned char GetBlue(WORD color)
{
	return (color & 0x1f) << 3;
}

// 功能：将一个16位的DirectDraw表面，存为一张24位BMP位图
// 输入：表面指针，输出的文件名
// 输出：是否成功
bool SaveToBitmapFile(LPDIRECTDRAWSURFACE lpSurface, char* filename)
{
	WORD* lpBuffer;			// 表面指针
	int nPitch; 			// 表面跨距
	int nWidth, nHeight;	// 表面宽高
	
	// 打开文件s
	FILE* fp;
	if( (fp=fopen(filename, "wb")) != NULL )
	{
		// 锁定表面
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);
		HRESULT ddrval = lpSurface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
		if( ddrval == DD_OK )
		{
  			lpBuffer = (WORD *)ddsd.lpSurface;
			nWidth = ddsd.dwWidth;
			nHeight = ddsd.dwHeight;
			//lPitch以Byte为单位计数的，GraphPitch以WORD为单位。所以GraphPitch = lPitch / 2;
			nPitch  = ddsd.lPitch >> 1;	
		}

		// 保存文件头
		BITMAPFILEHEADER FileHeader;
		FileHeader.bfType = 'BM';
		FileHeader.bfSize = nWidth * nHeight * 3 + 0x36;
		FileHeader.bfReserved1 = 0;
		FileHeader.bfReserved2 = 0;
		FileHeader.bfOffBits = 0x36;
		fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		
		// 保存文件信息
		BITMAPINFOHEADER Header;
		Header.biSize = sizeof(BITMAPINFOHEADER);	// 结构的大小
		Header.biWidth = nWidth;					// 宽
		Header.biHeight = nHeight;					// 高
		Header.biPlanes = 1;						// 固定
		Header.biBitCount = 24;						// 颜色数
		Header.biCompression = BI_RGB;				// 是否压缩
		Header.biSizeImage = nWidth * nHeight * 3;	// 图片的大小
		Header.biXPelsPerMeter = 0;
		Header.biYPelsPerMeter = 0;
		Header.biClrUsed = 0;
		Header.biClrImportant = 0;
		fwrite(&Header, Header.biSize, 1, fp);

		// 写入具体内容(从下向上存放)
		fseek(fp, 0x36, SEEK_SET);
		WORD word;
		lpBuffer += nWidth * (nHeight - 1);
		for(int i=0; i<nHeight; i++)
		{
			for(int j=0; j<nWidth; j++)
			{
				word = *lpBuffer;
				fputc( GetBlue( word ), fp);	// 蓝
				fputc( GetGreen( word ), fp);	// 绿
				fputc( GetRed( word ), fp);		// 红
				lpBuffer++;
			}
			lpBuffer -= nPitch*2;	// 指针转到上一行的开始
		}

		fclose(fp);

		// 解锁表面
		ddrval = lpSurface->Unlock( NULL );
		return true;
	}

	return false;
}
