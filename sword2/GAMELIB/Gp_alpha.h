//********************************************
//	像素级的操作相关处理函数
//  softboy 创建于2000年1月25日
//********************************************
#ifndef _GP_ALPHA_
#define _GP_ALPHA_

#include <windows.h>

extern WORD RMask, GMask, BMask;	//颜色遮罩
extern BYTE RMove,GMove;			//移动到最右边的位数
extern DWORD rgbMask, rgbTemp;
//===============================================================
//这个文件里包含一些inline函数，在Gp_alpah.cpp里面还有非inline的
//函数,要是找不到注释的话，请参考非inline函数的注释
//相应的要是非inline的没有注释，请参考inline版的注释
//===============================================================

//
//softboy在写这些函数的时候都没有注释的，而我在加注释的时候，一时
//大意，没有发现有两个版本，所以…………
//见谅了！---------tiamo
//

//==========================alpha================================

//两个点的ALPHA混合（0-31）内联版
inline WORD _Alpha_Pixel(WORD sour, WORD dest, int alpha)
{
	//非inline版的这个函数有详细的注释
	rgbTemp =  ((((sour<<16)|sour) & rgbMask ) * alpha + 
					(((dest<<16)|dest) & rgbMask ) * (32-alpha) ) >> 5;
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
}

WORD Alpha_Pixel(WORD sour, WORD dest, int alpha);
void ABlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, char iAlpha);
void AlphaColorBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, WORD Color, char iAlpha);
void ChangeBlt(LPDIRECTDRAWSURFACE BackSurface, LPDIRECTDRAWSURFACE suf, int x, int y, int BeginAlpha, int StepSize, int Step,int Time, WORD Color_Key);
void Alpha_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect);

//===========================additive============================

//功能：两个点的Additive混合（内联版）
//参数：源点，目标点
//返回：混合后的点
inline WORD _Additive_Pixel(WORD sour, WORD dest)
{
	//C++版		看看非inline的注释
	rgbTemp = (((sour<<16)|sour) & rgbMask) + (((dest<<16)|dest) & rgbMask);
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
}

WORD Additive_Pixel(WORD sour, WORD dest);
inline WORD _Additive_Pixel(WORD sour, WORD dest);
void AdditiveBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key);

//===========================Gray================================

//功能：把一个RGB点的变成灰度（内联版）
//参数：源点
//返回：混合后的点
inline WORD _Gray_Pixel(WORD sour)
{
	WORD t;
	WORD r, g, b;
	r= sour >> RMove;				//得到红色成分，直接右移RMove位
	g= (GMask & sour) >> GMove;		//得到绿色成分，与绿色的掩码与运算，然后右移GMove位
	b= BMask & sour;				//直接与蓝色的掩码与运算
	t= (r*3 + g*6 + b)/10;			//按照0.3：0.6：0.1的比例合成

	return (t<<RMove)|(t<<GMove)|t;	//合成一个点，三个成分相同
}

WORD Gray_Pixel(WORD sour);
inline WORD _Gray_Pixel(WORD sour);
void Gray_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key);

//===========================单色================================
void Color_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key, WORD Color);

//查找精灵边缘
void FindEdge(LPDIRECTDRAWSURFACE lpDDSPict, WORD Color, WORD CK);
void Edge_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD CK, WORD Color);

#endif