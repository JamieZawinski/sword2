//********************************************
//	�����ʼ����ش�����
//  softboy ������2000��1��25��
//********************************************
#include <windows.h>

#define _RELEASE(A)			try{ if( (A) != NULL ) { (A) -> Release(); (A) = NULL; } } catch(...) { PutDebugString("error ->Release!"); }
#define _DELETE(X)			try{ if( (X) != NULL ) { delete (X); (X) = NULL; } } catch(...) { PutDebugString("error delete!"); }
#define _DELETE_ARRAY(X)	try{ if( (X) != NULL ) { delete[] (X); (X) = NULL; } } catch(...) { PutDebugString("error delete[]!"); }
#define _FREE(X)			try{ if( (X) != NULL ) { free(X); (X)=NULL; } } catch(...) { PutDebugString("error free()!"); }

extern HWND	hWnd;				//�����ھ��
extern HINSTANCE hInst;			//Ӧ�ó���ʵ��
extern BOOL	bActive;		//Ӧ�ó����Ƿ��Ծ
extern int RunGame;	
extern int WindowMode;
extern BOOL bSwitch;
extern BOOL g_bDebug;

//��������
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
HWND InitWindow( HINSTANCE hInstance, int nCmdShow );
BOOL InitDDraw( void );
BOOL InitGame( void );
BOOL ReleaseGame( void );
HRESULT FreeDDraw( void );
HRESULT ChangeCoopLevel(HWND hWnd );
BOOL CreateClipper();			//����һ��GDI�ü���
void MainLoop();