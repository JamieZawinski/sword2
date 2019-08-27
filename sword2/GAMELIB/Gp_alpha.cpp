//********************************************
//	���ؼ��Ĳ�����ش�����
//  softboy ������2000��1��25��
//********************************************

#include <windows.h>
#include <fstream.h>
#include <math.h>
#include <ddraw.h>
#include "gp_draw.h"
#include "gp_input.h"
#include "gp_alpha.h"

//==============================alpha=============================
//	alpha�����غ���
//================================================================
WORD RMask, GMask, BMask;	//��ɫ����
BYTE RMove,GMove;			//�ƶ������ұߵ�λ��
DWORD rgbMask, rgbTemp;

//****************************
//���ܣ��������ALPHA��ϣ�0-31��
//������Դ�㣬Ŀ��㣬ALPHA����
//���أ���Ϻ�ĵ�
WORD Alpha_Pixel(WORD sour, WORD dest, int alpha)
{
	//C++�� ��������Ļ����ע��
	rgbTemp =  ((((sour<<16)|sour) & rgbMask ) * alpha +
						(((dest<<16)|dest) & rgbMask ) * (32-alpha) ) >> 5;
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
	
/*
	//����
	int ialpha=32-alpha;
	_asm{
		movzx eax, sour			//����չ��32λ
		mov bx, ax				//��sour�ŵ�bx��
		sal eax, 16				//��������16λ����λ��0
		mov ax, bx				//��sour�ŵ�ax�У����ڵ�eax�зŵ�������sour
		and eax, rgbMask		//����rgbMask
		mul alpha				//����alphaֵ
		mov esi, eax			//���浽esi
	
		movzx eax, dest			//ͬ���ķ���
		mov bx, ax
		sal eax, 16
		mov ax, bx
		and eax, rgbMask
		mul ialpha
		add eax, esi			//���ϸոյ�ֵ
		shr eax, 5				//�߼�����5λ����λ��0
		 
		//rgbTemp = rgbTemp & rgbMask;
		and eax, rgbMask		//����rgbMask
		mov esi, eax			//�ŵ�esi��
		shr eax, 16				//�߼����ƶ�16λ����λ��0
		or  eax, esi			//����esi
		mov rgbTemp, eax		//����
	}
	return (WORD)rgbTemp;
	*/
	/*--------------------------------------------------------
	��˵��㣬����rgbMask����ʽ��ʲô��������655��ʽ
	00000|gggggg|00000|rrrrr|000000|bbbbb
	5��0  ��λ�� 5��0 |��λ |6��0  |��λ
		  ������      |������      |������

	���ˣ������Ǹ�ʲô���
	Ҫ�ǽ�����һ������Ļ���Ҳֻ�Ǳ���grbλ����
	�������������ƶ�16λ������ԭ������ȥ����λ�Ļ�
	�����ʲô�أ�16λ��rgb��ֵ
	�����˾ͺð���ˡ�����
	��������ô����alpha��ϵ��أ�
	
	��Ҫ�����Ǹ��˷����棬ע���ȳ���һ��alpha��Ȼ���и�����5λ
	Ҳ���ǳ���32�ˡ����Ը�����ɫ�ɷֵ�ֵ�������Ҳ������Խλ��
	������ɫ���Ƕ����˵ġ�

	Ȼ���������˵���Ǹ��ˣ���ԭ��16λ����ɫֵ
	----------------------------------------------------------*/
}

//***************************
//���ܣ����������Alpha���
//��������SS��Ŀ����� DS��Դ���� Color_Key��͸��ɫ[0����] iAlpha��alphaֵ��
void ABlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, char iAlpha)
{
	if( iAlpha <= 0 )
	{										//��Ҫ����:)���������֪���ˣ�����true��false��ֵ
		Blt(SS, x, y, DS, rcRect, true);	//#define DDBLTFAST_NOCOLORKEY 0x00000000
		return;								//#define DDBLTFAST_SRCCOLORKEY 0x00000001
	}										//���������õ���SRCCOLORKEY
	else if( iAlpha >= 31 )
	{
		return;
	}

	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//ȡԴҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//��ص��������
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;			//�����ȺͿ�ȵ�����
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//��͸��ɫ���
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				Sour[t1]=_Alpha_Pixel(Sour[t1], Dest[t2], iAlpha);//alpha���
				t1++;
				t2++;
			}
			t1+=SW;			//������ȺͿ�ȵĲ�ֵ
			t2+=DW;
		}
	}
	else	//͸��ɫ���
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2] != Color_Key )		//������͸��ɫ�Ļ��
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
//���ܣ���һ�������ϵ��ض���ɫAlpha��ϵ���һ�����棬��������
//��������SS��Ŀ����� DS��Դ���� Color_Key��͸��ɫ[0����] Color:���ɫ iAlpha��alphaֵ��
void AlphaColorBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key, WORD Color, char iAlpha)
{
	if( iAlpha <= 0 )				
	{
		Blt(SS, x, y, DS, rcRect, true);	//�в����׵Ŀ�����һ��������ע��
		return;
	}
	else if( iAlpha >= 31 )
	{
		return;
	}

	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//ȡԴҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//������Ӧ������
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;				//��ȺͿ��
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//��͸��ɫ���
	{
		for(int i=0; i<rcRect.bottom-rcRect.top; i++)
		{
			for( int j=0; j<rectWidth; j++)
			{
				if( Dest[t2]==Color )		//ֻ���һ����ɫ
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
	else	//͸��ɫ���
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
//���ܣ�������������Blt
//������(����ҳ�棬Դҳ�棬x�� y�� ��ʼALPHAֵ����������������ʱ��͸��ɫ)
void ChangeBlt(LPDIRECTDRAWSURFACE BackSurface, LPDIRECTDRAWSURFACE suf, int x, int y, int BeginAlpha, int StepSize, int Step,int Time, WORD Color_Key)
{
	long T1=0, T2=0;
	int W,H;
	int bAlpha=BeginAlpha;
	T1=T2=timeGetTime();

	//ȡ������
	GetSurfaceSize(suf, W, H);
	
	RECT rect={0,0,W,H};
	RECT rect2={0,0,ScreenWidth, ScreenHeight};

	for( int j=0; j<Step+1; j++)
	{
		//��ʱ
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;
		//�Ȱ�BackSurface��NOCOLORKEY��ʽblt��Back������
		Blt(lpDDSBack, 0,0, BackSurface, rect2, false);
		//�ٰ�suf����alpha��ֵ��͸��ɫblt��Back������
		ABlt(lpDDSBack, x, y, suf, rect, ColorKey16, bAlpha);
		//������Ļ
		_UpdateScreen();
		//alpha��ֵ����һ����λ
		bAlpha+=StepSize;
		//Խ����
		if( bAlpha<0 ) 
			bAlpha=0;
		if( bAlpha>31 ) 
			bAlpha=31;

		//һ�ε�λ
		if( Mouse.IsButton(0) )	//�������
		{
			//������ġ���
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
//���ܣ�����͹�Ȧ��Alpha���
//��������SS��Ŀ����� DS����Ȧ���棩
void Alpha_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect)
{
	WORD d=0x0000;		//������ʲô��ɫ������
	WORD *Dest,*Sour;
	int t1, t2;
	int DestWidth, SourWidth;
	
	//ȡԴҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}
	
	//�߽���
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
	//����������
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;					//��˵�ˣ��������
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{	
			//����Ŷ����Ȧ�������ɫ��alphaֵ��d�Ǻ�ɫ
			Sour[t1]=_Alpha_Pixel(Sour[t1], d, Dest[t2]&BMask );
			t1++;
			t2++;
		}
		t1+=SW;
		t2+=DW;
	}
}

//==============================Additive==========================
//	additive�����غ���
//================================================================

//****************************
//���ܣ��������Additive���
//������Դ�㣬Ŀ���
//���أ���Ϻ�ĵ�
WORD Additive_Pixel(WORD sour, WORD dest)
{
	//C++��
	//���ˣ�������alpha��ϣ�����Ͳ���ʲô��
	//�����Ļ�����������alpha��ϵ�ע����
	rgbTemp = (((sour<<16)|sour) & rgbMask) + (((dest<<16)|dest) & rgbMask);
	rgbTemp = rgbTemp & rgbMask;
	return (WORD)((rgbTemp>>16)|rgbTemp);
}

//***************************
//���ܣ����������Additive���
//��������SS��Ŀ����� DS��Դ���� Color_Key��͸��ɫ[0����]��
void AdditiveBlt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect, WORD Color_Key)
{
	WORD *Dest,*Sour;
	int t1,t2;
	int DestWidth, SourWidth;
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//������İ�
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;					//û��������
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//��͸��ɫ���
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
	else	//͸��ɫ���
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
//	'RGB��>�Ҷ�'��غ���
//================================================================

//****************************
//���ܣ���һ��RGB��ı�ɻҶ�
//������Դ��
//���أ���Ϻ�ĵ�
WORD Gray_Pixel(WORD sour)
{
	WORD t;						//�����Ǹ�inline���ע�͡���
	WORD r, g, b;
	r= sour >> RMove;
	g= (GMask & sour) >> GMove;
	b= BMask & sour;
	t= (r*3 + g*6 + b)/10;

	/*
	//����					//ͬ������ע�Ͱɣ��������������θ�ɾ����
	_asm{
		mov ax, sour			//��sour�ŵ�ax��
		mov bx, ax				//�ŵ�bx��
		mov dx, ax				//�ŵ�dx��
		
		mov cl, RMove			//��λ��Ŀ
		shr ax, cl				//�߼�����
		
		and bx, GMask			//������ɫ������
		mov cl,GMove			//����
		shr bx, cl				//����
		
		and dx, BMask			//������ɫ������
		
		add ax, bx				
		add ax, dx				//�ӵ�һ��
		mov bl, 3
		div bl					//����3����������㷨��һ��
		xor ah, ah				//ah�ŵ�������������Ҫ�ģ���0������
		mov t, ax				//�ŵ�t��
	}
*/	
	return (t<<RMove)|(t<<GMove)|t;	//�ϳ�
}

//****************************
//���ܣ���ͼƬ��ɻ�ɫ��BLT
//��������SS��Ŀ����� DS��Ŀ����� Color_Key��͸��ɫ[0����]��
void Gray_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//������������
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;
	int SW=SourWidth-rectWidth;				
	int DW=DestWidth-rectWidth;

	if( Color_Key == 0 )	//û��͸��ɫ
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
	else	//��͸��ɫ
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

//==============================��ɫ=============================
//	��ɫ������غ���
//================================================================

//**************************************
//���ܣ���ͼƬ��ɵ�ɫ(color)��BLT Color=��ɫ
//��������SS��Ŀ����� DS��Ŀ����� Color_Key��͸��ɫ[0����] Color=��ɫ��
void Color_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD Color_Key, WORD Color)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//��������
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;

	int SW=SourWidth-rectWidth;
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{
			if( Dest[t2] != Color_Key )		//ֻҪ����͸��ɫ�ͱ����Ҫ��ĵ�ɫ
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
//���ܣ����Ҿ����Ե
//����������ҳ�棬��Եɫ��͸��ɫ
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
				n=Pitch*i+j;			//��˳��ɨ���
				if( Pict[n]!=CK && (Pict[n+1]==CK || Pict[n-1]==CK 
						|| Pict[n+Pitch]==CK || Pict[n-Pitch]==CK ) )
				{
					Pict[n]=Color;			//����㲻��͸���ģ�����������Χ�ĸ�����һ��
				}							//��͸���ġ�
				else						//�����ǼӼ�һ
					Pict[n]=CK;				//�����ǼӼ�Pitch
			}								//��Ե���ָ������ɫ
		EndDraw(lpDDSPict);					//���������͸��ɫ
	}
}

//**************************************
//���ܣ�Blt�����Ե
//��������SS��Ŀ����� DS��Դ���� CK��͸��ɫ Color����Ե��ɫ��
void Edge_Blt(LPDIRECTDRAWSURFACE SS,int x,int y, LPDIRECTDRAWSURFACE DS,RECT rcRect,WORD CK, WORD Color)
{
  	WORD *Dest,*Sour;
	int DestWidth,SourWidth;
	int t1,t2;
	
	//ȡԴҳ��ָ��
	if( BeginDraw(DS) )
	{
		Dest=(WORD *)GraphBuffer;
		DestWidth=GraphPitch;
		EndDraw(DS);
	}
	
	//ȡĿ��ҳ��ָ��
	if( BeginDraw(SS) )
	{
		Sour=(WORD *)GraphBuffer;
		SourWidth=GraphPitch;
		EndDraw(SS);
	}

	//�߽���
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
	//�������÷�������ļ����кܶ���
	t1=SourWidth*y+x;
	t2=DestWidth*rcRect.top+rcRect.left;
	int rectWidth=rcRect.right-rcRect.left;

	int SW=SourWidth-rectWidth;
	int DW=DestWidth-rectWidth;

	for(int i=0; i<rcRect.bottom-rcRect.top; i++)
	{
		for( int j=0; j<rectWidth; j++)
		{
			//���ϸ�����һ�����߼���ֻ�����ı�Ķ���ͬ����
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
