
//------------------------------------------------------------
// ����غ���
// ������2002��4��3��
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
	int m_nTile;			//��ͼTILEֵ
	int m_nH;				//���
	virtual ~CTree();
};

#endif 
