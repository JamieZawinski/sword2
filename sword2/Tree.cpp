
//------------------------------------------------------------
// 树相关函数
// 创建于2002年4月3日
//------------------------------------------------------------

#include "Tree.h"


CTree::~CTree()
{

}

CTree::CTree(int f, int tile, CTree *father):m_nH(f),m_nTile(tile),m_pFather(father)
{

}
