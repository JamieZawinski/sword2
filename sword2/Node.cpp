
//------------------------------------------------------------
// �ڵ���غ���
// ������2002��4��3��
//------------------------------------------------------------

#include "Node.h"

CNode::CNode()
{

}

CNode::~CNode()
{

}

CNode::CNode(CTree *data, CNode *link, int Priority):m_pData(data),m_pNext(link),m_nPriority(Priority)
{

}
