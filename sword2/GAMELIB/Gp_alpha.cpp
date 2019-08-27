//********************************************
//	像素级的操作相关处理函数
//  softboy 创建于2000年1月25日
//********************************************

#include <windows.h>
#include <fstream.h>
#include <math.h>
#include <ddraw.h>
#include "gp_draw.h"
#include "gp_input.h"
#include "gp_alpha.h"

//==============================alpha=============================
//	alpha混合相关函数
//================================================================
WORD RMask, GMask, BMask;	//颜色遮罩
BYTE RMove,GMove;			//移动到最右边的位数
DWORD rgbMask, rgbTemp;

//****************************
//功能：两个点的ALPHA混合（0-31）
//参数：源点，目标点，ALPHA级别
//返回：混合后的点
WORD Alpha_Pixel(WORD sour, WORD dest, int alpha)
{
	//C++版 ：看下面的汇编版的注释
	rgbTemp =  ((((sour<<16)|sour) & rgbMask ) * alpha +
						(((dest<<16)|dest) & rgbMask ) * (32-alpha) ) >> 5;
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
	
/*
	//汇编版
	int ialpha=32-alpha;
	_asm{
		movzx eax, sour			//零扩展成32位
		mov bx, ax				//把sour放到bx中
		sal eax, 16				//算术左移16位，低位填0
		mov ax, bx				//把sour放到ax中，现在的eax中放的是两个sour
		and eax, rgbMask		//与上rgbMask
		mul alpha				//乘上alpha值
		mov esi, eax			//保存到esi
	
		movzx eax, dest			//同样的方法
		mov bx, ax
		sal eax, 16
		mov ax, bx
		and eax, rgbMask
		mul ialpha
		add eax, esi			//加上刚刚的值
		shr eax, 5				//逻辑右移5位，高位填0
		 
		//rgbTemp = rgbTemp & rgbMask;
		and eax, rgbMask		//与上rgbMask
		mov esi, eax			//放到esi中
		shr eax, 16				//逻辑右移动16位，高位填0
		or  eax, esi			//或上esi
		mov rgbTemp, eax		//返回
	}
	return (WORD)rgbTemp;
	*/
	/*--------------------------------------------------------
	多说点点，看看rgbMask的形式是什么：比如是655格式
	00000|gggggg|00000|rrrrr|000000|bbbbb
	5个0  六位的 5个0 |五位 |6个0  |五位
		  绿掩码      |红掩码      |蓝掩码

	好了，看看是个什么结果
	要是将他与一个数与的话，也只是保留grb位的数
	如果把这个数右移动16位，再与原数或，再去掉高位的话
	结果是什么呢？16位的rgb的值
	明白了就好办多了。：）
	那又是怎么做到alpha混合的呢？
	
	主要是在那个乘法上面，注意先乘了一个alpha，然后有个右移5位
	也就是除以32了。所以各个颜色成分的值算出来了也不会有越位，
	各个颜色还是对齐了的。

	然后就是上面说的那个了，还原成16位的颜色值
	----------------------------------------------------------*/
}

//***************************
//功能：两个表面的Alpha混合
//参数：（SS：目标表面 DS：源表面 Color_Key：透明色[0：无] iAlpha：alpha值）
void ABlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, char iAlpha)
{
	if( iAlpha <= 0 )
	{										//不要惊奇:)看看下面就知道了，想想true和false的值
		Blt(SS, x, y, DS, rcRect, true);	//#define DDBLTFAST_NOCOLORKEY 0x00000000
		return;								//#define DDBLTFAST_SRCCOLORKEY 0x00000001
	}										//明白了吗？用的是SRCCOLORKEY
	else if( iAlpha >= 31 )
	{
		return;
	}

	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//取源页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//相关的坐标计算
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;			//想想跨度和宽度的区别
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//无透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				Sour[t1]=_Alpha_Pixel(Sour[t1], Dest[t2], iAlpha);//alpha混合
				t1++;
				t2++;
			}
			t1+=SW;			//跳过跨度和宽度的差值
			t2+=DW;
		}
	}
	else	//透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2] != Color_Key )		//不进行透明色的混合
					Sour[t1]=_Alpha_Pixel(Sour[t1], Dest[t2], iAlpha);
				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
}

//***********************************************************
//功能：把一个表面上的特定颜色Alpha混合到另一个表面，其他不变
//参数：（SS：目标表面 DS：源表面 Color_Key：透明色[0：无] Color:混合色 iAlpha：alpha值）
void AlphaColorBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, WORD Color, char iAlpha)
{
	if( iAlpha <= 0 )				
	{
		Blt(SS, x, y, DS, rcRect, true);	//有不明白的看看上一个函数的注释
		return;
	}
	else if( iAlpha >= 31 )
	{
		return;
	}

	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//取源页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//计算相应的坐标
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;				//跨度和宽度
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//无透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2]==Color )		//只混合一种颜色
					Sour[t1]=_Alpha_Pixel(Sour[t1], Dest[t2], iAlpha);
				else
					Sour[t1]=Dest[t2];

				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
	else	//透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2] != Color_Key )
				{
					if( Dest[t2]==Color ) 
						Sour[t1]=_Alpha_Pixel(Sour[t1], Dest[t2], iAlpha);
					else
						Sour[t1]=Dest[t2];
				}

				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
}

//**************************
//功能：表面连续渐变Blt
//参数：(背景页面，源页面，x， y， 起始ALPHA值，步长，步数，延时，透明色)
void ChangeBlt(LPDIRECTDRAWSURFACE BackSurface, LPDIRECTDRAWSURFACE suf, int x, int y, int BeginAlpha, int StepSize, int Step,int Time, WORD Color_Key)
{
	long T1=0, T2=0;
	int W,H;
	int bAlpha=BeginAlpha;
	T1=T2=timeGetTime();

	//取表面宽高
	GetSurfaceSize(suf, W, H);
	
	RECT rect={0,0,W,H};
	RECT rect2={0,0,ScreenWidth, ScreenHeight};

	for( int j=0; j<Step+1; j++)
	{
		//延时
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;
		//先把BackSurface以NOCOLORKEY方式blt到Back表面上
		Blt(lpDDSBack, 0,0, BackSurface, rect2, false);
		//再把suf按照alpha的值和透明色blt到Back表面上
		ABlt(lpDDSBack, x, y, suf, rect, ColorKey16, bAlpha);
		//更新屏幕
		_UpdateScreen();
		//alpha的值增加一个单位
		bAlpha+=StepSize;
		//越界检查
		if( bAlpha<0 ) 
			bAlpha=0;
		if( bAlpha>31 ) 
			bAlpha=31;

		//一次到位
		if( Mouse.IsButton(0) )	//左键按下
		{
			//看上面的……
			bAlpha=BeginAlpha+StepSize*Step;

			if( bAlpha<0 ) 
				bAlpha=0;
			if( bAlpha>31 ) 
				bAlpha=31;

			Blt(lpDDSBack, 0,0, BackSurface, rect2, false);
			ABlt(lpDDSBack, x, y, suf, rect, ColorKey16, bAlpha);
			_UpdateScreen();
			return;
		}
	}
}

//***************************
//功能：表面和光圈的Alpha混合
//参数：（SS：目标表面 DS：光圈表面）
void Alpha_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect)
{
	WORD d=0x0000;		//想想是什么颜色：）黑
	WORD *Dest,*Sour;
	int t1, t2;
	int DestWidth, SourWidth;
	
	//取源页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}
	
	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//恩…………
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;					//不说了，这个……
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{	
			//看好哦，光圈表面的蓝色是alpha值，d是黑色
			Sour[t1]=_Alpha_Pixel(Sour[t1], d, Dest[t2]&BMask );
			t1++;
			t2++;
		}
		t1+=SW;
		t2+=DW;
	}
}

//==============================Additive==========================
//	additive混合相关函数
//================================================================

//****************************
//功能：两个点的Additive混合
//参数：源点，目标点
//返回：混合后的点
WORD Additive_Pixel(WORD sour, WORD dest)
{
	//C++版
	//简单了，明白了alpha混合，这个就不算什么了
	//不懂的话，恩，看看alpha混合的注释先
	rgbTemp = (((sour<<16)|sour) & rgbMask) + (((dest<<16)|dest) & rgbMask);
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
}

//***************************
//功能：两个表面的Additive混合
//参数：（SS：目标表面 DS：源表面 Color_Key：透明色[0：无]）
void AdditiveBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key)
{
	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//取目标页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//看上面的吧
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;					//没有语言了
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//无透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				Sour[t1]=_Additive_Pixel(Sour[t1], Dest[t2]);
				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
	else	//透明色检查
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2] != Color_Key )
					Sour[t1]=_Additive_Pixel(Sour[t1], Dest[t2]);
				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
}

//==============================Gray=============================
//	'RGB－>灰度'相关函数
//================================================================

//****************************
//功能：把一个RGB点的变成灰度
//参数：源点
//返回：混合后的点
WORD Gray_Pixel(WORD sour)
{
	WORD t;						//看看那个inline版的注释……
	WORD r, g, b;
	r= sour >> RMove;
	g= (GMask & sour) >> GMove;
	b= BMask & sour;
	t= (r*3 + g*6 + b)/10;

	/*
	//汇编版					//同样给个注释吧，本来我是想把这段给删掉的
	_asm{
		mov ax, sour			//把sour放到ax中
		mov bx, ax				//放到bx中
		mov dx, ax				//放到dx中
		
		mov cl, RMove			//移位数目
		shr ax, cl				//逻辑右移
		
		and bx, GMask			//与上绿色的掩码
		mov cl,GMove			//……
		shr bx, cl				//……
		
		and dx, BMask			//与上蓝色的掩码
		
		add ax, bx				
		add ax, dx				//加到一起
		mov bl, 3
		div bl					//除以3：和上面的算法不一样
		xor ah, ah				//ah放的是余数，不需要的，清0就是了
		mov t, ax				//放到t中
	}
*/	
	return (t<<RMove)|(t<<GMove)|t;	//合成
}

//****************************
//功能：把图片变成灰色后BLT
//参数：（SS：目标表面 DS：目标表面 Color_Key：透明色[0：无]）
void Gray_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//取目标页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//………………
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;				
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//没有透明色
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				Sour[t1]=_Gray_Pixel(Dest[t2] );
				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
	else	//带透明色
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2] != Color_Key )
					Sour[t1]=_Gray_Pixel(Dest[t2] );
				t1++;
				t2++;
			}
			t1+=SW;
			t2+=DW;
		}
	}
}

//==============================单色=============================
//	单色处理相关函数
//================================================================

//**************************************
//功能：把图片变成单色(color)后BLT Color=颜色
//参数：（SS：目标表面 DS：目标表面 Color_Key：透明色[0：无] Color=颜色）
void Color_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key, WORD Color)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//取目标页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x;
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//…………
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;

	int SW=SourWidth-rectWidth;
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{
			if( Dest[t2] != Color_Key )		//只要不是透明色就变成所要求的单色
				Sour[t1]=Color;
			t1++;
			t2++;
		}
		t1+=SW;
		t2+=DW;
	}
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
//**************************************
//功能：查找精灵边缘
//参数：精灵页面，边缘色，透明色
void FindEdge(LPDIRECTDRAWSURFACE lpDDSPict, WORD Color, WORD CK)
{
	int n, i, j;

	if( BeginDraw(lpDDSPict) )
	{
		WORD *Pict=GraphBuffer;
		int Width=GraphWidth;
		int Height=GraphHeight;
		int Pitch=GraphPitch;

		for(i=0; i<Height; i++)
			for(j=0; j<Width; j++)
			{
				n=Pitch*i+j;			//按顺序扫描点
				if( Pict[n]!=CK && (Pict[n+1]==CK || Pict[n-1]==CK 
						|| Pict[n+Pitch]==CK || Pict[n-Pitch]==CK ) )
				{
					Pict[n]=Color;			//这个点不是透明的，但是他的周围四个点有一个
				}							//是透明的。
				else						//左右是加减一
					Pict[n]=CK;				//上下是加减Pitch
			}								//边缘变成指定的颜色
		EndDraw(lpDDSPict);					//其他都变成透明色
	}
}

//**************************************
//功能：Blt精灵边缘
//参数：（SS：目标表面 DS：源表面 CK：透明色 Color：边缘颜色）
void Edge_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD CK, WORD Color)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//取源页面指针
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//取目标页面指针
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//边界检查
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0;
	}

	if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
	{ 
		rcRect.right = rcRect.left + ScreenWidth - x; 
	}

	if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
	{ 
		rcRect.bottom = rcRect.top + ScreenHeight - y; 
	}
	//这样的用法在这个文件里有很多了
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;

	int SW=SourWidth-rectWidth;
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{
			//和上个函数一样的逻辑，只不过改变的对象不同而已
			if( Dest[t2]!=CK && (Dest[t2+1]==CK || Dest[t2-1]==CK 
						|| Dest[t2+DestWidth]==CK || Dest[t2-DestWidth]==CK ) )
				Sour[t1]=Color;
			t1++;
			t2++;
		}
		t1+=SW;
		t2+=DW;
	}
}
