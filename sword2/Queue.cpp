
//------------------------------------------------------------
// 队列相关函数
// 创建于2002年4月3日
//------------------------------------------------------------


#include "Queue.h"
#include "gamelib\goldpoint.h"

CQueue::CQueue()			//建好头尾指针
{				
	m_pRear = new CNode(NULL,NULL,65535);
	m_pFront = new CNode(NULL,m_pRear,-1);
}

CQueue::~CQueue()
{
	CNode* p;
	while(m_pFront)				//依次出队
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
	while(p->m_nPriority<f)			//按照优先权查找应该插入的地方
	{
		father = p;
		p = p->m_pNext;
	}
	father->m_pNext = new CNode(data,p,f);	//插入
}

CTree* CQueue::DeQueue()			//将第二个元素出队
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
