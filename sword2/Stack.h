
//------------------------------------------------------------
// 栈相关函数
// 创建于2002年4月3日
//------------------------------------------------------------

#ifndef _STACK_H_
#define _STACK_H_
#include "Node.h"
#include <windows.h>

class CStack  
{
public:
	int GetCount();			//返回个数
	CTree* GetTop();		//返回栈顶元素
	CTree* Pop();			//出栈
	BOOL IsEmpty();			//是否为空
	void Push(CTree* data);	//压栈
	CStack();
	virtual ~CStack();

private:
	int m_nCount;			//个数
	CNode* m_pTop;			//顶部指针
};

#endif 
