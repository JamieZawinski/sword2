
//------------------------------------------------------------
// 栈相关函数
// 创建于2002年4月3日
//------------------------------------------------------------


#include "Stack.h"
#include "gamelib\goldpoint.h"

CStack::CStack():m_pTop(NULL)
{
	m_nCount = 0;
}

CStack::~CStack()
{
	CNode* p ;			//依次出栈，并且delete
	while(m_pTop)
	{
		p = m_pTop;
		m_pTop = m_pTop->m_pNext;
		_DELETE(p);
	}
}

void CStack::Push(CTree *data)
{
	m_pTop = new CNode(data,m_pTop);	//入栈
	m_nCount++;
}

BOOL CStack::IsEmpty()
{
	return m_pTop == NULL;
}

CTree* CStack::Pop()
{
	CTree* temp = m_pTop->m_pData;		//出栈
	CNode* p = m_pTop;
	m_pTop = m_pTop->m_pNext;			//修改指针
	_DELETE(p);							//delete
	m_nCount--;
	return temp;
}

CTree* CStack::GetTop()
{
	return m_pTop->m_pData;
}

int CStack::GetCount()
{
	return m_nCount;
}
