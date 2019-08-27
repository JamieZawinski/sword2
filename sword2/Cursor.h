
//------------------------------------------------------------------
// �����غ���
// ������2000��7��10��
//------------------------------------------------------------------

#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "gamelib\goldpoint.h"

class CCursor  
{
public:
	struct stCursor
	{
		char strName[32];	// ����
		CAni pPic;			// ͼƬ
	};

	enum CURSOR_STYLE
	{
		CS_NORMAL=3,		// ��ͨ���
		CS_CHANGEMAP=1,		// �л�����
	};

private:
	static stCursor* m_pCursor;		// ���ָ��
	static long m_lCursor;			// �������

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