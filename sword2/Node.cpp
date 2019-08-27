
//------------------------------------------------------------
// 节点相关函数
// 创建于2002年4月3日
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
