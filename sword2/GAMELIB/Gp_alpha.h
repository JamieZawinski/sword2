//********************************************
//	���ؼ��Ĳ�����ش�����
//  softboy ������2000��1��25��
//********************************************
#ifndef _GP_ALPHA_
#define _GP_ALPHA_

#include <windows.h>

extern WORD RMask, GMask, BMask;	//��ɫ����
extern BYTE RMove,GMove;			//�ƶ������ұߵ�λ��
extern DWORD rgbMask, rgbTemp;
//===============================================================
//����ļ������һЩinline��������Gp_alpah.cpp���滹�з�inline��
//����,Ҫ���Ҳ���ע�͵Ļ�����ο���inline������ע��
//��Ӧ��Ҫ�Ƿ�inline��û��ע�ͣ���ο�inline���ע��
//===============================================================

//
//softboy��д��Щ������ʱ��û��ע�͵ģ������ڼ�ע�͵�ʱ��һʱ
//���⣬û�з����������汾�����ԡ�������
//�����ˣ�---------tiamo
//

//==========================alpha================================

//�������ALPHA��ϣ�0-31��������
inline WORD _Alpha_Pixel(WORD sour, WORD dest, int alpha)
{
	//��inline��������������ϸ��ע��
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

//���ܣ��������Additive��ϣ������棩
//������Դ�㣬Ŀ���
//���أ���Ϻ�ĵ�
inline WORD _Additive_Pixel(WORD sour, WORD dest)
{
	//C++��		������inline��ע��
	rgbTemp = (((sour<<16)|sour) & rgbMask) + (((dest<<16)|dest) & rgbMask);
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
}

WORD Additive_Pixel(WORD sour, WORD dest);
inline WORD _Additive_Pixel(WORD sour, WORD dest);
void AdditiveBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key);

//===========================Gray================================

//���ܣ���һ��RGB��ı�ɻҶȣ������棩
//������Դ��
//���أ���Ϻ�ĵ�
inline WORD _Gray_Pixel(WORD sour)
{
	WORD t;
	WORD r, g, b;
	r= sour >> RMove;				//�õ���ɫ�ɷ֣�ֱ������RMoveλ
	g= (GMask & sour) >> GMove;		//�õ���ɫ�ɷ֣�����ɫ�����������㣬Ȼ������GMoveλ
	b= BMask & sour;				//ֱ������ɫ������������
	t= (r*3 + g*6 + b)/10;			//����0.3��0.6��0.1�ı����ϳ�

	return (t<<RMove)|(t<<GMove)|t;	//�ϳ�һ���㣬�����ɷ���ͬ
}

WORD Gray_Pixel(WORD sour);
inline WORD _Gray_Pixel(WORD sour);
void Gray_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key);

//===========================��ɫ================================
void Color_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key, WORD Color);

//���Ҿ����Ե
void FindEdge(LPDIRECTDRAWSURFACE lpDDSPict, WORD Color, WORD CK);
void Edge_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD CK, WORD Color);

#endif