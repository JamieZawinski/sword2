
//------------------------------------------------------------
// 树相关函数
// 创建于2002年4月3日
//------------------------------------------------------------

#ifndef _TREE_H_
#define _TREE_H_

class CFindPath;

class CTree  
{
friend class CFindPath;
private:
	CTree(int f,int tile,CTree* father);
	CTree* m_pFather;
	int m_nTile;			//地图TILE值
	int m_nH;				//深度
	virtual ~CTree();
};

#endif 
