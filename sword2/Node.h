
//------------------------------------------------------------
// 节点相关函数
// 创建于2002年4月3日
//------------------------------------------------------------

#ifndef _NOTE_H_
#define _NOTE_H_

class CStack;
class CQueue;
class CTree;

class CNode  
{
public:
friend class CStack;		//友员
friend class CQueue;

private:
	CTree* m_pData;			//数据
	CNode* m_pNext;			//下一个元素
	int m_nPriority;		//优先权
	CNode();
	CNode(CTree *data, CNode *link= 0, int Priority=0);
	virtual ~CNode();
};

#endif 
