//********************************************
//	������ʾ��ش�����
//  softboy ������2000��1��25��
//********************************************
#include <windows.h>
#include <stdio.h>
#include <fstream.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_other.h"
#include "gp_iniset.h"
#include "gp_draw.h"
#include "gp_alpha.h"
#include "gp_text.h"

/*****************************************************************/
/*��������ر���
/*****************************************************************/

DWORD CGpFont::FONTCOLOR[10];	//10��ϵͳ�趨��������ɫ
DWORD CGpFont::DELAYTICK[10];	//���ֳ��ֵ���ʱ

CGpFont::CGpFont(bool set)
{
	if( set )
	{
		FONTCOLOR[0]=RGB(0,0,0);		//��ɫ
		FONTCOLOR[1]=RGB(255,0,0);
		FONTCOLOR[2]=RGB(0,255,0);
		FONTCOLOR[3]=RGB(0,0,255);
		FONTCOLOR[4]=RGB(255,255,0);
		FONTCOLOR[5]=RGB(255,0,255);
		FONTCOLOR[6]=RGB(0,255,255);
		FONTCOLOR[7]=RGB(255,0,255);
		FONTCOLOR[8]=RGB(127,127,127);
		FONTCOLOR[9]=RGB(225,255,225);

		DELAYTICK[0]=0;					//��ʱ
		DELAYTICK[1]=10;
		DELAYTICK[2]=20;
		DELAYTICK[3]=30;
		DELAYTICK[4]=50;
		DELAYTICK[5]=80;
		DELAYTICK[6]=100;
		DELAYTICK[7]=200;
		DELAYTICK[8]=500;
		DELAYTICK[9]=1000;
	}
}

CGpFont::~CGpFont()
{
	if(font_Font!=NULL)					//�ͷ���Դ
		DeleteObject(font_Font);
}


//****��ʼ������******
void CGpFont::InitFont(int FontSize,char *FontType)
{
	font_Size=FontSize;		//��С
	font_Family=FontType;	//����
	font_Color=RGB(255,255,255);
	font_BK=TRANSPARENT;	//�ֵı����Ƿ��ο�
	font_BKColor=RGB(255,255,255);	//����ɫ
	font_Width=70;			//һ�е������������ģ�width/2
	font_LineHeight=20;		//�м��
	bChangeColor=true;
	DelayTick=30;

	if( font_Font != NULL ) DeleteObject( font_Font );

	font_Font = CreateFont( FontSize,				// �ַ��߶�(�߼���λ)
							0,						// �ַ����(�߼���λ) 0Ϊ�Զ�ƥ��
							0,						// �ַ��нǶ�
							0,						// �ַ����߽Ƕ�
							font_Weight,			// 0--1000 ����Ũ���̶�, 400Ϊ����, 700Ϊ����
							FALSE,					// �Ƿ�Ϊб����
							FALSE,					// �Ƿ���»���
							0,						// �Ƿ���л���
							GB2312_CHARSET,			// ָ���ַ���
							OUT_DEFAULT_PRECIS,		// ָ���������
							CLIP_DEFAULT_PRECIS,	// ָ�����о���
							DEFAULT_QUALITY,		// ָ���������
							DEFAULT_PITCH|FF_SWISS, // ָ�����, ������
							FontType				// ������
							);
}

//��������
void CGpFont::LoadFont(char *filename, char *index)
{
	//��ʼ������
	CIniSet FontIni(filename);
	int size=FontIni.ReadInt(index, "font_Size");
	char *font=FontIni.ReadText(index, "font_Family");
	font_Weight=FontIni.ReadInt(index, "font_Weight");

	InitFont(size, font);
	if( font_Weight >= 700 ) font_Exp=1;
	else font_Exp=0;
	_FREE( font );
	
	font_LineHeight=FontIni.ReadInt(index,"font_LineHeight");
	font_Width=FontIni.ReadInt(index,"font_Width");
	
	char *Col=FontIni.ReadText(index, "font_Color");
	font_Color=RGB(FontIni.ReadInt(Col, "Red"), FontIni.ReadInt(Col, "Green"), FontIni.ReadInt(Col,"Blue"));
	_FREE( Col );
	
	char *BkCol=FontIni.ReadText(index, "font_BKColor");
	font_BKColor=RGB(FontIni.ReadInt(BkCol, "Red"), FontIni.ReadInt(BkCol, "Green"), FontIni.ReadInt(BkCol,"Blue"));
	_FREE( BkCol );

	if( FontIni.ReadInt(index, "font_BK") )
		font_BK=TRANSPARENT;	//�ֵı����Ƿ��ο�
	else
		font_BK=OPAQUE;
}

//*****************
//������ɫ
void CGpFont::SetColor(DWORD color)
{
	font_Color=color;
}

//******************
//��ȡ��ɫ
DWORD CGpFont::GetColor()
{
	return font_Color;
}

//�ָ�ȱʡ����
void CGpFont::Restore_Default_Font()
{
	font_BK=TRANSPARENT;	//�ֵı����Ƿ��ο�
	font_BKColor=RGB(255,255,255);	//����ɫ
	font_Width=80;			//һ�е������������ģ�width/2
	font_LineHeight=20;		//�м��
}

//�����ֵı����Ƿ��ο�
void CGpFont::SetBK(bool c)
{
	if( c )
		font_BK=TRANSPARENT;
	else
		font_BK=OPAQUE;
}

//����һ���ַ����м��Ƿ���Ա�ɫ
void CGpFont::SetChangeColor(bool c)
{
	bChangeColor=c;
}

//*********************************
//Alpha������ʾ
void CGpFont::PrintAlphaText(LPDIRECTDRAWSURFACE Surf, int x, int y, int alpha, char *Text,...)
{
	va_list va;
	char *str;
	str=(char *)malloc(strlen(Text)+256);	//Ԥ������256��char�Ŀռ�
	char *Str=str;							//�����ָ�뵽�Ժ��free��

	va_start(va,Text);
	vsprintf(str,Text,va);					//�ŵ�str��
	va_end(va);

	HDC hdc;
	int ChrLen;
	int ascii=0;
	char Tmp[256];

	if( str[0]==0 ) 
	{
		_FREE(Str);
		return;
	}

	//��ʱҳ��
	RECT rect={0,0,font_Width*font_Size , font_Size};
	LPDIRECTDRAWSURFACE TempSurface;
	CreateBitmap(TempSurface, rect.right , rect.bottom);
	DDSetColorKey(TempSurface, ColorKey);
	FillSurface(TempSurface, ColorKey16);

	//��ȡGDI����
	TempSurface->GetDC(&hdc);
	SetBkMode(hdc, font_BK);
	SetBkColor(hdc, font_BKColor);
	SetTextColor(hdc, font_Color);
	SelectObject(hdc,font_Font);

	ChrLen=strlen(str)+1;		

	for(int i=0; i<=ChrLen/font_Width; i++ )	//���д���
	{
		ascii=0;
		memset(Tmp,0,256);
		//����������һ�У�����һ�������ȣ�������ʣ��ĳ���
		int Chr=((i==ChrLen/font_Width)?ChrLen%font_Width:font_Width);
		for(int j=0; j<Chr; j++)
		{
			Tmp[j]=*str;
			str++;

			//����Ӣ������(0=˫�� 1=����)
			if( (unsigned char)Tmp[j]<128 )		//���ֵ���ʾ��ȫ�Ǵ���128��
				ascii=1-ascii;					//˵���Ǹ�Ӣ�ģ�����

			if( *str=='<' && *(str+1)=='C' && *(str+2)=='R' && *(str+3)=='>' )
			{
				str=str+4;		//��CR���ǻ���
				i--;
				goto _Show;
			}
		}

		//���������Ǻ��֣�Ҫ��Ӣ�ĵ���Ŀ�ǵ����Ļ�����Ȼ�ᵥ��һ����
		if( ascii==1 )
		{
			str--;			//������һ��
		}
		
_Show:	TextOut(hdc, 0, 0, Tmp, strlen(Tmp));		//��ʾ����ʱҳ��
		TempSurface->ReleaseDC(hdc);

		ABlt(Surf, x, y, TempSurface, rect, ColorKey16, alpha);		//AlphaBlt
		y+=font_LineHeight;							//��ʾ��һ�����ֵ�y����

		FillSurface(TempSurface, ColorKey16);		//���ҳ��
		//���»�ȡGDI����
		TempSurface->GetDC(&hdc);
		SetBkMode(hdc, font_BK);
		SetBkColor(hdc, font_BKColor);
		SetTextColor(hdc, font_Color);
		SelectObject(hdc,font_Font);
	}

	TempSurface->ReleaseDC(hdc);
	_RELEASE( TempSurface );
	_FREE(Str);
}

//*********************************
//������ʾ
void CGpFont::PrintText(LPDIRECTDRAWSURFACE Surf,int x,int y,char *Text,...)
{
	char *str;
	str=(char *)malloc(strlen(Text)+256);
	char *BackStr=str;	//����ָ�룬�ȵ�freeʹ��

	va_list va;
	va_start(va,Text);
	vsprintf(str,Text,va);
	va_end(va);

	if( str=="" ) 
	{
		_FREE(BackStr);
		return;
	}

	//��ȡGDI����
	HDC hdc;
	Surf->GetDC(&hdc);
	SetBkMode(hdc, font_BK);
	SetBkColor(hdc, font_BKColor);
	SetTextColor(hdc, font_Color);
	SelectObject(hdc,font_Font);

	int StrLen, CurLen, LeftLen;
	int ascii=0, Chars=0;
	int showx=x;	//������ʾλ��
	char Tmp[256];

	StrLen=strlen(str);
	while( str < BackStr + StrLen )
	{
		ascii=0;
		Chars=0;
		showx=x;
		memset(Tmp,0,256);

		//���㱾�е��ַ���
		LeftLen=StrLen-(str-BackStr);	//ʣ����ַ�����
		if( LeftLen > font_Width ) 
			CurLen=font_Width;
		else 
			CurLen=LeftLen;

		while( Chars < CurLen&&*str )
		{
			//���⹦��
			if( *str=='<' )
			{
				//����������ɫ
				if( *(str+1)=='c' && *(str+3)=='>' )	//<c1>�ȵȣ�������ɫ����
				{
					if( bChangeColor )					//�����ɫ��ʾ
					{
						TextOut(hdc, showx, y, Tmp, strlen(Tmp));	//��ʾǰ��Ĳ���
						SetTextColor(hdc, FONTCOLOR[*(str+2)-'0'] );	//������ɫ
					
						showx+=strlen(Tmp)*(font_Size+font_Exp)/2;	//������ʾ��λ��
						CurLen-=strlen(Tmp);			//����ʣ�µĳ���
						memset(Tmp,0,256);				//��0
						Chars=0;						//λ�ø�λ
					}
					
					str+=4;								//���������ַ�
					continue;							//������ʾ���������
				}
				
				//�ָ�������ɫ
				if( *(str+1)=='/' && *(str+2)=='c' && *(str+3)=='>' )	//�ָ���ɫ
				{
					if( bChangeColor )					//�ܹ��ı���ɫ
					{
						TextOut(hdc, showx, y, Tmp, strlen(Tmp));		//��ʾ
						SetTextColor(hdc, font_Color);					//������ɫ

						showx+=strlen(Tmp)*(font_Size+font_Exp)/2;			
						CurLen-=strlen(Tmp);
						memset(Tmp,0,256);
						Chars=0;
					}

					str+=4;
					continue;
				}

				//������ʱ
				if( *(str+1)=='d' && *(str+3)=='>' )			//<d>��ʱ����
				{
					DelayTick=DELAYTICK[*(str+2)-'0'];			//���õ���ʱ����
					str+=4;										//����
					continue;
				}
				
				//�ָ�ԭ�����ٶ�
				if( *(str+1)=='/' && *(str+2)=='d' && *(str+3)=='>' )
				{
					DelayTick=DELAYTICK[3];
					str+=4;
					continue;
				}

				//����
				if( *(str+1)=='/' && *(str+2)=='n' && *(str+3)=='>' )
				{
					str+=4;
					goto _Show;
				}
			}

			Tmp[Chars]=*str;			//�����ַ���ֱ��copy
			str++;
			Chars++;

			//����Ӣ���ַ�����(0=˫�� 1=����)
			if( (unsigned char)Tmp[Chars-1]<128 )		//ͬ������Ӣ�Ĵ���
				ascii=1-ascii;
		}

		//���������Ǻ���
		if( ascii==1 && str < BackStr + StrLen )
		{
			str--;					//������һ��
			Tmp[Chars-1]=0;
		}

_Show:
		TextOut(hdc, showx, y, Tmp, strlen(Tmp));
		y+=font_LineHeight;			//��һ�е�y����
	}

	Surf->ReleaseDC(hdc);
	_FREE( BackStr );
}
// ������Ϣ
void CGpFont::GetFontInfo(LPDIRECTDRAWSURFACE surf, TEXTMETRIC *tm)
{
	HDC hdc;
	surf->GetDC(&hdc);
	GetTextMetrics(hdc,tm);
	surf->ReleaseDC(hdc);
}
