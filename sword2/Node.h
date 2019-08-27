
//------------------------------------------------------------
// �ڵ���غ���
// ������2002��4��3��
//------------------------------------------------------------

#ifndef _NOTE_H_
#define _NOTE_H_

class CStack;
class CQueue;
class CTree;

class CNode  
{
public:
friend class CStack;		//��Ա
friend class CQueue;

private:
	CTree* m_pData;			//����
	CNode* m_pNext;			//��һ��Ԫ��
	int m_nPriority;		//����Ȩ
	CNode();
	CNode(CTree *data, CNode *link= 0, int Priority=0);
	virtual ~CNode();
};

#endif 
