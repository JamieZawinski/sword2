
//------------------------------------------------------------
// ������غ���
// ������2002��4��3��
//------------------------------------------------------------

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <windows.h>

#include "Node.h"
class CTree;

class CQueue  
{
public:
	BOOL IsEmpty();			//�Ƿ��ǿյ�
	CTree* DeQueue();		//������Ȩ����
	void EnQueue(CTree* data,int f);	//������Ȩ���
	CQueue();
	virtual ~CQueue();

private:
	CNode* m_pRear;			//βָ��
	CNode* m_pFront;		//ͷָ��
};

#endif 
