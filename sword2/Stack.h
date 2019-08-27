
//------------------------------------------------------------
// ջ��غ���
// ������2002��4��3��
//------------------------------------------------------------

#ifndef _STACK_H_
#define _STACK_H_
#include "Node.h"
#include <windows.h>

class CStack  
{
public:
	int GetCount();			//���ظ���
	CTree* GetTop();		//����ջ��Ԫ��
	CTree* Pop();			//��ջ
	BOOL IsEmpty();			//�Ƿ�Ϊ��
	void Push(CTree* data);	//ѹջ
	CStack();
	virtual ~CStack();

private:
	int m_nCount;			//����
	CNode* m_pTop;			//����ָ��
};

#endif 
