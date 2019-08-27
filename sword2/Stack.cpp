
//------------------------------------------------------------
// ջ��غ���
// ������2002��4��3��
//------------------------------------------------------------


#include "Stack.h"
#include "gamelib\goldpoint.h"

CStack::CStack():m_pTop(NULL)
{
	m_nCount = 0;
}

CStack::~CStack()
{
	CNode* p ;			//���γ�ջ������delete
	while(m_pTop)
	{
		p = m_pTop;
		m_pTop = m_pTop->m_pNext;
		_DELETE(p);
	}
}

void CStack::Push(CTree *data)
{
	m_pTop = new CNode(data,m_pTop);	//��ջ
	m_nCount++;
}

BOOL CStack::IsEmpty()
{
	return m_pTop == NULL;
}

CTree* CStack::Pop()
{
	CTree* temp = m_pTop->m_pData;		//��ջ
	CNode* p = m_pTop;
	m_pTop = m_pTop->m_pNext;			//�޸�ָ��
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
