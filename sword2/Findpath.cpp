
//------------------------------------------------------------
// A*Ѱ·��غ���
// ������2002��4��3��
//------------------------------------------------------------


#include "FindPath.h"
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "main.h"
#include "map.h"
#include <math.h>

#define TILE_X(x) x%Width		//�õ�x����
#define TILE_Y(y) y/Width		//�õ�y����


CFindPath::CFindPath(CMap* cm)
{
	Map = cm;					//����mapָ��
	ThePath = NULL;				//·������
	dis_map = NULL;				//��÷�������
}

CFindPath::~CFindPath()
{
	Release();
}

void CFindPath::Init(int w,int h)
{
	Width = w ;					//�����Ⱥ͸߶�
	Height = h;
	Release();
	dis_map = new int[Width*Height];	//�����ڴ�

}

void CFindPath::Release()
{
	_DELETE(dis_map);					//�ͷ�
	_DELETE(ThePath);
}

int CFindPath::Judge(int x, int y)		//��������
{
	return abs(x-ex)+abs(y-ey);			
}

void CFindPath::TryTile(int x, int y,CTree *father)
{
	if(x<0||x>=Width-1
		||y<0||y>=Height-1
		||Map->Cell[Width*y+x].Block ==1 )//Խ���x,y�������ߣ�
														
		return;
	CTree* p = father;					//ָ�򸸽ڵ�
	int temp = Width*y+x;				//�ϳ�TILEֵ
	while(p)							//��������������Ⱦͷ���
	{
		if(temp == p->m_nTile)			//������
			return;
		p = p->m_pFather;
	}
	int h = father->m_nH + 1;			//��ȱȸ��ڵ��1
	if(dis_map[Width*y+x]<=h)			//�и��õķ�������x,y��
		return;

	dis_map[Width*y+x] = h;				//������ʷ��ü�¼
	p = new CTree(h,temp,father);		//������ڵ㱣��
	m_pOpen->EnQueue(p,h+Judge(x,y));	//��h+Judge��x,y�������

}
//��������Ѱ�Ҵӣ�startx,starty������endx,endy�����·��
bool CFindPath::Find(int startx, int starty, int endx, int endy)
{
	if(startx<0||endx>=Width-1||starty<0||endy>=Height-1)	//Խ�緵��
	{
		_DELETE_ARRAY(ThePath);
		return false;
	}
	//�������ݣ���ʼ�ͽ�������
	ex = endx;
	ey = endy;
	sx = startx;
	sy = starty;

	//����open���close��
	m_pOpen = new CQueue;
	m_pClose = new CStack;
	//��ʼ����ʷ��ü�¼
	if(Map->Cell[ex+ey*Width].Block==1)			//Ŀ�ĵ㲻�ɵ���Լ�����
	{
		for(int x = 0;x<Width;x++)
			for(int y = 0;y<Height;y++)
				dis_map[x+y*Width] = 3*(ShowCellW+ShowCellH);	//��ʷ��ü�¼��3�Ǹ�����ֵ
	}
	else
	{
		for(int x = 0;x<Width;x++)
			for(int y = 0;y<Height;y++)
				dis_map[x+y*Width] = (ShowCellW-1)*(ShowCellH-1);	//��ʷ��ü�¼
	}
	//���ڵ�
	CTree* root = new CTree(0,Width*sy+sx,NULL);
	CTree* p = root;
	//�����
	m_pOpen->EnQueue(root,Judge(sx,sy));
	//��ʼ
	while(1)
	{
		if(m_pOpen->IsEmpty())			//open���Ѿ����ˣ���ʾû��·������Ŀ�ĵ�
			break;						//����ѭ����Ȼ����һ�����Ŀ�ĵص�·��

		root = m_pOpen->DeQueue();		//��open����ȡ����÷���
		m_pClose->Push(root);			//�ŵ�close����
		int tile = root->m_nTile;		//�õ�root��TILEֵ
		int i = TILE_X(tile);			//�õ�x,yֵ
		int j = TILE_Y(tile);
		int k = TILE_X(p->m_nTile);
		int l = TILE_Y(p->m_nTile);
		if(Judge(i,j)<Judge(k,l))		//�Ƚϣ���֤���
			p = root;

		if(i == ex&&j == ey)			//����Ŀ�ĵ�
			break;
		
		TryTile(i,j-1,root);
		TryTile(i,j+1,root);
		TryTile(i-1,j,root);			//���ĸ�����̽��
		TryTile(i+1,j,root);
		
	}
	_DELETE_ARRAY(ThePath);				//��ȥ��ǰ������

	ThePath = new POINT[p->m_nH+1];		//�ӷ���

	for(TheSteps = p->m_nH;p;)          // ������������������·�������� path[] ��
	{									// TheStepsΪpath������±�
		ThePath[p->m_nH].x = TILE_X(p->m_nTile);
		ThePath[p->m_nH].y = TILE_Y(p->m_nTile);
		p = p->m_pFather;
	}
	TheSteps ++;						
	while(!m_pOpen->IsEmpty())			//�ͷŶ���
	{
		root = (CTree*)m_pOpen->DeQueue();
		delete root;
	}
	while(!m_pClose->IsEmpty())			//�ͷ�ջ
	{
		root = (CTree*)m_pClose->Pop();
		delete root;
	}
	delete m_pOpen;
	delete m_pClose;
	return true;
}

