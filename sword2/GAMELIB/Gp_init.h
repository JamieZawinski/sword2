//********************************************
//	程序初始化相关处理函数
//  softboy 创建于2000年1月25日
//********************************************
#include <windows.h>

#define _RELEASE(A)			try{ if( (A) != NULL ) { (A) -> Release(); (A) = NULL; } } catch(...) { PutDebugString("error ->Release!"); }
#define _DELETE(X)			try{ if( (X) != NULL ) { delete (X); (X) = NULL; } } catch(...) { PutDebugString("error delete!"); }
#define _DELETE_ARRAY(X)	try{ if( (X) != NULL ) { delete[] (X); (X) = NULL; } } catch(...) { PutDebugString("error delete[]!"); }
#define _FREE(X)			try{ if( (X) != NULL ) { free(X); (X)=NULL; } } catch(...) { PutDebugString("error free()!"); }

extern HWND	hWnd;				//主窗口句柄
extern HINSTANCE hInst;			//应用程序实例
extern BOOL	bActive;		//应用程序是否活跃
extern int RunGame;	
extern int WindowMode;
extern BOOL bSwitch;
extern BOOL g_bDebug;

//函数声明
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
HWND InitWindow( HINSTANCE hInstance, int nCmdShow );
BOOL InitDDraw( void );
BOOL InitGame( void );
BOOL ReleaseGame( void );
HRESULT FreeDDraw( void );
HRESULT ChangeCoopLevel(HWND hWnd );
BOOL CreateClipper();			//创建一个GDI裁减器
void MainLoop();