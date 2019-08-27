
//------------------------------------------------------------
// A*Ѱ·��غ���
// ������2002��4��3��
//------------------------------------------------------------

#ifndef _FINDPATH_H_
#define _FINDPATH_H_

#include <windows.h>

class CMap;

#include "stack.h"
#include "queue.h"
#include "tree.h"

class CFindPath  
{
public:
	POINT * ThePath;			//·������
	int TheSteps;				//����
	CMap* Map;					//��ͼָ��
	void Release();				//�ͷ�
	void Init(int w,int h);		//��ʼ��
	CFindPath(CMap* cm);
	virtual ~CFindPath();
	bool Find(int sx, int sy, int dx, int dy);	//������
private:
	int Width;					//��ͼ���
	int Height;					//��ͼ�߶�
	int* dis_map;				//��ʷ��ü�¼
	CQueue* m_pOpen;			//open��
	CStack * m_pClose;			//close��
	int sx,sy,ex,ey;			//��ʼ��Ŀ�ĵ�����
	int Judge(int x,int y);		//��������
	void TryTile(int x,int y, CTree* father);	//������һ����
};

#endif 
