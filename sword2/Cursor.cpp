
//------------------------------------------------------------------
// 鼠标相关函数
// 创建于2000年7月10日
//------------------------------------------------------------------
#include "Cursor.h"

CCursor::stCursor* CCursor::m_pCursor=NULL;		// 光标指针
long CCursor::m_lCursor=1;						// 光标数量

CCursor::CCursor()
{
	m_pCursor = NULL;
}

CCursor::~CCursor()
{
	_DELETE_ARRAY(m_pCursor);
}

void CCursor::Init(char* file, char* index)
{
	CIniSet Ini(file);
	m_lCursor = Ini.GetContinueDataNum(index);

	char* cur;
	if( m_lCursor > 0 )
	{
		m_pCursor = new stCursor[m_lCursor];
		for(int i=0; i<m_lCursor; i++)
		{
			cur = Ini.ReadText(index, i);
			strcpy(m_pCursor[i].strName, cur);
			m_pCursor[i].pPic.LoadAni(file, cur);		//初始化光标
			_FREE(cur);
		}
	}
}

void CCursor::Release()
{
	_DELETE_ARRAY( m_pCursor );
}

void CCursor::Show(long n, LPDIRECTDRAWSURFACE lpSurf, int x, int y)
{
	m_pCursor[n].pPic.ShowAni(lpSurf, x, y);
}