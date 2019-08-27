
//------------------------------------------------------------------
// 鼠标相关函数
// 创建于2000年7月10日
//------------------------------------------------------------------

#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "gamelib\goldpoint.h"

class CCursor  
{
public:
	struct stCursor
	{
		char strName[32];	// 名称
		CAni pPic;			// 图片
	};

	enum CURSOR_STYLE
	{
		CS_NORMAL=3,		// 普通光标
		CS_CHANGEMAP=1,		// 切换点光标
	};

private:
	static stCursor* m_pCursor;		// 光标指针
	static long m_lCursor;			// 光标数量

public:
	CCursor();
	virtual ~CCursor();

	static void Init(char* file, char* index);
	static void Release();
	static void Show(long n, LPDIRECTDRAWSURFACE lpSurf, int x, int y);
	static stCursor& GetCursor(long l)	{ return m_pCursor[l]; }
	static long GetCursorNum()			{ return m_lCursor; }
};

#endif 