//********************************************
//	DirectDraw��ش�����
//  softboy ������2000��1��25��
//********************************************
#ifndef _GP_DRAW_
#define _GP_DRAW_

#include <windows.h>
#include <ddraw.h>

#define CK_NO	DDBLTFAST_NOCOLORKEY		//��ʹ��͸��ɫ
#define CK_YES	DDBLTFAST_SRCCOLORKEY		//ʹ��͸��ɫ

//********���ؽṹ**********
struct stBit{
	unsigned R:5;
	unsigned G:6;
	unsigned B:5;
};

//************��ʾ���*********************
#define ScreenWidth     640		//��Ļ��
#define ScreenHeight    480		//��Ļ��
#define ScreenColor		16		//��ɫ���
//#define ColorKey    RGB(255,0,255)	//͸��ɫ

extern DWORD ColorKey;	//32λ͸��ɫ
extern WORD ColorKey16;	//16λ͸��ɫ
extern bool Is555;		//�Ƿ���555��ʽ
extern long nFrames;	//֡������
extern RECT RectScreen;	//ȫ����ʾ��Ŀ����� ��Ϸ�г��õ��������Ѿ���ʼ��
extern RECT RectWindow;	//������ʾ��Ŀ�����
extern POINT g_pointClient;		//���ںͿͻ�����ƫ��

//******************DDraw*******************
extern LPDIRECTDRAW lpDD;
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWSURFACE lpDDSBack;
extern LPDIRECTDRAWCLIPPER lpClipper;   // Clipper for primary

extern WORD *GraphBuffer;		// ��ͼ������
extern int GraphPitch;			// ���������
extern int GraphWidth;		//ҳ����
extern int GraphHeight;		//ҳ��߶�

WORD RGB16(WORD r, WORD g, WORD b);
WORD RGB16(DWORD color);			//24λת16λ

BOOL LoadBitmapFileInfo(HWND hwnd, LPCTSTR filename, int &dx, int &dy);
BOOL CreateBitmap(LPDIRECTDRAWSURFACE &lpTemp,int x,int y,char *BitmapFile=NULL, DWORD MemoryFlag=DDSCAPS_SYSTEMMEMORY );
void Blt(LPDIRECTDRAWSURFACE SS,int x,int y,LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flog);
void BltFast(LPDIRECTDRAWSURFACE SS,int x,int y,LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flog);	//����λͼ����
BOOL SBlt(LPDIRECTDRAWSURFACE SS,RECT sc, LPDIRECTDRAWSURFACE DS,RECT rcRect,BOOL Flog);
BOOL _SBlt(LPDIRECTDRAWSURFACE SS,RECT sr, LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flog);	//�����ŵ�λͼ����
void Flip(void);
HRESULT restoreAll( void );
BOOL initFail(char *str);

void UpdateScreen(LPDIRECTDRAWSURFACE lpSurf=lpDDSPrimary, RECT DestRect=RectWindow);			//���µ���Ļ
void _UpdateScreen(LPDIRECTDRAWSURFACE lpSurf=lpDDSBack, RECT DestRect=RectScreen);				//���µ���Ļ

//------------------------

int GetSurfaceSize(LPDIRECTDRAWSURFACE surf,int &x,int &y);	//��ȡһ������Ĵ�С
int GetSurfaceWidth(LPDIRECTDRAWSURFACE surf);	//��ȡһ������Ŀ��
int GetSurfaceHeight(LPDIRECTDRAWSURFACE surf);	//��ȡһ������ĸ߶�

bool BeginDraw(LPDIRECTDRAWSURFACE lpSur);	//ֱ���ڻ������в���
bool EndDraw(LPDIRECTDRAWSURFACE lpSur);	//�رջ�����
void FillSurface(LPDIRECTDRAWSURFACE surf, DWORD color);	//�Ե�ɫ���ҳ��
void FillSurface(LPDIRECTDRAWSURFACE surf, WORD color);	//�Ե�ɫ���ҳ��

//------------------------

void PutPixel(LPDIRECTDRAWSURFACE, int, int, WORD);				//����
void _PutPixel(int, int, WORD);									//���������滭��
void Line(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);		//��ֱ��
void Rectangle(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);	//������
void Bar(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);		//��ʵ�ľ���
void Circle(LPDIRECTDRAWSURFACE, int, int, int,WORD);			//��Բ

//------------------------

//���뵭��
void FadeIn(int Style=0, int time=50, WORD Color=0);
void FadeOut(int Style=0, int time=50, LPDIRECTDRAWSURFACE lpSurf=lpDDSBack);

void ColorIn(int time=50, WORD Color=0);
void ColorOut(LPDIRECTDRAWSURFACE dest=lpDDSBack, int time=50);

void ScreenIn(int Step=16,int Time=20);
void ScreenOut(LPDIRECTDRAWSURFACE dest=lpDDSBack, int Step=16,int Time=20);

//------------------------

//��ĳ����ɫ����
void Clrscr(LPDIRECTDRAWSURFACE,WORD=0);

//��һ������˳ʱ����ת90��(�����������������;���)
void RolSurface(LPDIRECTDRAWSURFACE, int);

//����һ��ҳ�浽�ļ�
bool SaveToBitmapFile(LPDIRECTDRAWSURFACE, char* filename);

#endif