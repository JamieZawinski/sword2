
//------------------------------------------------------------
// 队列相关函数
// 创建于2002年4月3日
//------------------------------------------------------------

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <windows.h>

#include "Node.h"
class CTree;

class CQueue  
{
public:
	BOOL IsEmpty();			//是否是空的
	CTree* DeQueue();		//按优先权出队
	void EnQueue(CTree* data,int f);	//按优先权入队
	CQueue();
	virtual ~CQueue();

private:
	CNode* m_pRear;			//尾指针
	CNode* m_pFront;		//头指针
};

#endif 
