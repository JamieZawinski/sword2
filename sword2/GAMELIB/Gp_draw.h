//********************************************
//	DirectDraw相关处理函数
//  softboy 创建于2000年1月25日
//********************************************
#ifndef _GP_DRAW_
#define _GP_DRAW_

#include <windows.h>
#include <ddraw.h>

#define CK_NO	DDBLTFAST_NOCOLORKEY		//不使用透明色
#define CK_YES	DDBLTFAST_SRCCOLORKEY		//使用透明色

//********象素结构**********
struct stBit{
	unsigned R:5;
	unsigned G:6;
	unsigned B:5;
};

//************显示相关*********************
#define ScreenWidth     640		//屏幕宽
#define ScreenHeight    480		//屏幕高
#define ScreenColor		16		//颜色深度
//#define ColorKey    RGB(255,0,255)	//透明色

extern DWORD ColorKey;	//32位透明色
extern WORD ColorKey16;	//16位透明色
extern bool Is555;		//是否是555格式
extern long nFrames;	//帧数测试
extern RECT RectScreen;	//全屏显示的目标矩形 游戏中常用到的量，已经初始化
extern RECT RectWindow;	//窗口显示的目标矩阵
extern POINT g_pointClient;		//窗口和客户区的偏移

//******************DDraw*******************
extern LPDIRECTDRAW lpDD;
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWSURFACE lpDDSBack;
extern LPDIRECTDRAWCLIPPER lpClipper;   // Clipper for primary

extern WORD *GraphBuffer;		// 绘图缓冲区
extern int GraphPitch;			// 缓冲区跨度
extern int GraphWidth;		//页面宽度
extern int GraphHeight;		//页面高度

WORD RGB16(WORD r, WORD g, WORD b);
WORD RGB16(DWORD color);			//24位转16位

BOOL LoadBitmapFileInfo(HWND hwnd, LPCTSTR filename, int &dx, int &dy);
BOOL CreateBitmap(LPDIRECTDRAWSURFACE &lpTemp,int x,int y,char *BitmapFile=NULL, DWORD MemoryFlag=DDSCAPS_SYSTEMMEMORY );
void Blt(LPDIRECTDRAWSURFACE SS,int x,int y,LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flog);
void BltFast(LPDIRECTDRAWSURFACE SS,int x,int y,LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flog);	//快速位图拷贝
BOOL SBlt(LPDIRECTDRAWSURFACE SS,RECT sc, LPDIRECTDRAWSURFACE DS,RECT rcRect,BOOL Flog);
BOOL _SBlt(LPDIRECTDRAWSURFACE SS,RECT sr, LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flog);	//带缩放的位图拷贝
void Flip(void);
HRESULT restoreAll( void );
BOOL initFail(char *str);

void UpdateScreen(LPDIRECTDRAWSURFACE lpSurf=lpDDSPrimary, RECT DestRect=RectWindow);			//更新到屏幕
void _UpdateScreen(LPDIRECTDRAWSURFACE lpSurf=lpDDSBack, RECT DestRect=RectScreen);				//更新到屏幕

//------------------------

int GetSurfaceSize(LPDIRECTDRAWSURFACE surf,int &x,int &y);	//获取一个表面的大小
int GetSurfaceWidth(LPDIRECTDRAWSURFACE surf);	//获取一个表面的宽度
int GetSurfaceHeight(LPDIRECTDRAWSURFACE surf);	//获取一个表面的高度

bool BeginDraw(LPDIRECTDRAWSURFACE lpSur);	//直接在缓冲区中操作
bool EndDraw(LPDIRECTDRAWSURFACE lpSur);	//关闭缓冲区
void FillSurface(LPDIRECTDRAWSURFACE surf, DWORD color);	//以单色填充页面
void FillSurface(LPDIRECTDRAWSURFACE surf, WORD color);	//以单色填充页面

//------------------------

void PutPixel(LPDIRECTDRAWSURFACE, int, int, WORD);				//画点
void _PutPixel(int, int, WORD);									//不锁定表面画点
void Line(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);		//画直线
void Rectangle(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);	//画矩形
void Bar(LPDIRECTDRAWSURFACE, int, int, int, int, WORD);		//画实心矩形
void Circle(LPDIRECTDRAWSURFACE, int, int, int,WORD);			//画圆

//------------------------

//淡入淡出
void FadeIn(int Style=0, int time=50, WORD Color=0);
void FadeOut(int Style=0, int time=50, LPDIRECTDRAWSURFACE lpSurf=lpDDSBack);

void ColorIn(int time=50, WORD Color=0);
void ColorOut(LPDIRECTDRAWSURFACE dest=lpDDSBack, int time=50);

void ScreenIn(int Step=16,int Time=20);
void ScreenOut(LPDIRECTDRAWSURFACE dest=lpDDSBack, int Step=16,int Time=20);

//------------------------

//用某种颜色清屏
void Clrscr(LPDIRECTDRAWSURFACE,WORD=0);

//把一个表面顺时针旋转90度(本函数仅限于正方型矩阵)
void RolSurface(LPDIRECTDRAWSURFACE, int);

//保存一个页面到文件
bool SaveToBitmapFile(LPDIRECTDRAWSURFACE, char* filename);

#endif