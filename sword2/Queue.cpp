
//------------------------------------------------------------
// ������غ���
// ������2002��4��3��
//------------------------------------------------------------


#include "Queue.h"
#include "gamelib\goldpoint.h"

CQueue::CQueue()			//����ͷβָ��
{				
	m_pRear = new CNode(NULL,NULL,65535);
	m_pFront = new CNode(NULL,m_pRear,-1);
}

CQueue::~CQueue()
{
	CNode* p;
	while(m_pFront)				//���γ���
	{
		p = m_pFront;
		m_pFront = m_pFront->m_pNext;
		_DELETE(p);				//delete
	}
}

void CQueue::EnQueue(CTree *data, int f)
{
	CNode* p = m_pFront;
	CNode* father;
	while(p->m_nPriority<f)			//��������Ȩ����Ӧ�ò���ĵط�
	{
		father = p;
		p = p->m_pNext;
	}
	father->m_pNext = new CNode(data,p,f);	//����
}

CTree* CQueue::DeQueue()			//���ڶ���Ԫ�س���
{
	CTree* temp = m_pFront->m_pNext->m_pData;
	CNode* p = m_pFront->m_pNext; 
	m_pFront->m_pNext = m_pFront->m_pNext->m_pNext;
	_DELETE(p);
	return temp;
}

BOOL CQueue::IsEmpty()
{
	return m_pFront->m_pNext==m_pRear;
}
