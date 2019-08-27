
//------------------------------------------------------------
// A*寻路相关函数
// 创建于2002年4月3日
//------------------------------------------------------------


#include "FindPath.h"
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "main.h"
#include "map.h"
#include <math.h>

#define TILE_X(x) x%Width		//得到x坐标
#define TILE_Y(y) y/Width		//得到y坐标


CFindPath::CFindPath(CMap* cm)
{
	Map = cm;					//保存map指针
	ThePath = NULL;				//路径数组
	dis_map = NULL;				//最好方案数组
}

CFindPath::~CFindPath()
{
	Release();
}

void CFindPath::Init(int w,int h)
{
	Width = w ;					//保存宽度和高度
	Height = h;
	Release();
	dis_map = new int[Width*Height];	//分配内存

}

void CFindPath::Release()
{
	_DELETE(dis_map);					//释放
	_DELETE(ThePath);
}

int CFindPath::Judge(int x, int y)		//评估函数
{
	return abs(x-ex)+abs(y-ey);			
}

void CFindPath::TryTile(int x, int y,CTree *father)
{
	if(x<0||x>=Width-1
		||y<0||y>=Height-1
		||Map->Cell[Width*y+x].Block ==1 )//越界或（x,y）不可走，
														
		return;
	CTree* p = father;					//指向父节点
	int temp = Width*y+x;				//合成TILE值
	while(p)							//遍历，如果是祖先就返回
	{
		if(temp == p->m_nTile)			//是祖先
			return;
		p = p->m_pFather;
	}
	int h = father->m_nH + 1;			//深度比父节点多1
	if(dis_map[Width*y+x]<=h)			//有更好的方法到（x,y）
		return;

	dis_map[Width*y+x] = h;				//保存历史最好记录
	p = new CTree(h,temp,father);		//将这个节点保存
	m_pOpen->EnQueue(p,h+Judge(x,y));	//按h+Judge（x,y）入队列

}
//主函数：寻找从（startx,starty）到（endx,endy）最短路径
bool CFindPath::Find(int startx, int starty, int endx, int endy)
{
	if(startx<0||endx>=Width-1||starty<0||endy>=Height-1)	//越界返回
	{
		_DELETE_ARRAY(ThePath);
		return false;
	}
	//保存数据，开始和结束坐标
	ex = endx;
	ey = endy;
	sx = startx;
	sy = starty;

	//创建open表和close表
	m_pOpen = new CQueue;
	m_pClose = new CStack;
	//初始化历史最好记录
	if(Map->Cell[ex+ey*Width].Block==1)			//目的点不可到，约束深度
	{
		for(int x = 0;x<Width;x++)
			for(int y = 0;y<Height;y++)
				dis_map[x+y*Width] = 3*(ShowCellW+ShowCellH);	//历史最好记录，3是个经验值
	}
	else
	{
		for(int x = 0;x<Width;x++)
			for(int y = 0;y<Height;y++)
				dis_map[x+y*Width] = (ShowCellW-1)*(ShowCellH-1);	//历史最好记录
	}
	//根节点
	CTree* root = new CTree(0,Width*sy+sx,NULL);
	CTree* p = root;
	//入队列
	m_pOpen->EnQueue(root,Judge(sx,sy));
	//开始
	while(1)
	{
		if(m_pOpen->IsEmpty())			//open表已经空了，表示没有路径到达目的地
			break;						//跳出循环，然后找一条最靠近目的地的路径

		root = m_pOpen->DeQueue();		//从open表中取出最好方案
		m_pClose->Push(root);			//放到close表中
		int tile = root->m_nTile;		//得到root的TILE值
		int i = TILE_X(tile);			//得到x,y值
		int j = TILE_Y(tile);
		int k = TILE_X(p->m_nTile);
		int l = TILE_Y(p->m_nTile);
		if(Judge(i,j)<Judge(k,l))		//比较，保证最近
			p = root;

		if(i == ex&&j == ey)			//到达目的地
			break;
		
		TryTile(i,j-1,root);
		TryTile(i,j+1,root);
		TryTile(i-1,j,root);			//向四个方向探索
		TryTile(i+1,j,root);
		
	}
	_DELETE_ARRAY(ThePath);				//除去先前的数组

	ThePath = new POINT[p->m_nH+1];		//从分配

	for(TheSteps = p->m_nH;p;)          // 回溯树，将求出的最佳路径保存在 path[] 中
	{									// TheSteps为path的最大下标
		ThePath[p->m_nH].x = TILE_X(p->m_nTile);
		ThePath[p->m_nH].y = TILE_Y(p->m_nTile);
		p = p->m_pFather;
	}
	TheSteps ++;						
	while(!m_pOpen->IsEmpty())			//释放队列
	{
		root = (CTree*)m_pOpen->DeQueue();
		delete root;
	}
	while(!m_pClose->IsEmpty())			//释放栈
	{
		root = (CTree*)m_pClose->Pop();
		delete root;
	}
	delete m_pOpen;
	delete m_pClose;
	return true;
}

