
//------------------------------------------------------------
// A*寻路相关函数
// 创建于2002年4月3日
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
	POINT * ThePath;			//路径数组
	int TheSteps;				//步数
	CMap* Map;					//地图指针
	void Release();				//释放
	void Init(int w,int h);		//初始化
	CFindPath(CMap* cm);
	virtual ~CFindPath();
	bool Find(int sx, int sy, int dx, int dy);	//主函数
private:
	int Width;					//地图宽度
	int Height;					//地图高度
	int* dis_map;				//历史最好记录
	CQueue* m_pOpen;			//open表
	CStack * m_pClose;			//close表
	int sx,sy,ex,ey;			//开始和目的点坐标
	int Judge(int x,int y);		//评估函数
	void TryTile(int x,int y, CTree* father);	//测试下一个点
};

#endif 
