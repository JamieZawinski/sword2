//********************************************
//	地图 相关函数
//  创建于2000年3月14日
//********************************************

#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include <list>
using namespace std;

//动画CELL
struct stAniCell
{
	int page;		//页面
	int cell;		//图块
	int time;		//延时
};

class CNpc;
class CGpFont;

typedef vector<stAniCell>	vectorAniCell;
typedef vector<stAniCell>::iterator	itAniCell;
typedef vector<CNpc*> vectorNpc;
typedef vector<CNpc*>::iterator itNpc;

#define MAP_HEAD "Gold Point Sword II MapFile V1.0"

#define BLOCK_CELL 4095	//空格子
#define BLOCK_CP 127	//空陷阱
#define MAX_HOOK 32		//最大陷阱数量
#define CP_CHANGE 1		//切换点

//=============
//地图类
class CMap
{
public:

	//物体层次
	enum ObjLevel
	{
		OL_UP=1,		//人物上面
		OL_NORMAL=0,	//人物同级
	};

	//格子结构
	struct stCell
	{
		unsigned GroundPic:6;	//地面页面编号（普通0-59  动态60-63）
		unsigned Ground:12;		//地面编号
		unsigned ObjPic:6;		//物体页面编号（普通0-59  动态60-63）
		unsigned Obj:12;		//物体编号
		unsigned Obj2Pic:6;		//物体2页面编号（普通0-59  动态60-63）
		unsigned Obj2:12;		//物体2编号
		unsigned Block:1;		//是否阻挡
		unsigned Level:4;		//物体所在层次(0-15)
		unsigned CP:7;			//陷阱
		unsigned Level2:4;		//物体2所在层次(0-15)
		unsigned MouseType:3;	//鼠标类型
		unsigned CPType:1;		//陷阱类型
		unsigned res:13;		//保留
	};	

	static vectorAniCell m_vAniCell[4096];	// 动画CELL
	static int m_nAniCount[4096];			// 动画的进度
	static int m_nAniFrame[4096];			// 动画当前桢

public:	
	static int AniCellNum;			//动画CELL数量
	static stAniCell *AniCell;		//动画CELL
	long GetAniCellPage(const long ground, RECT& rect);	// 获取动态图素的当前页和RECT
	long GetAniCellPageObj(const long ground, RECT& rect);	// 获取动态图素的当前页和RECT
	long GetAniCellPageObj2(const long ground, RECT& rect);	// 获取动态图素的当前页和RECT
	long GetAniPage(const long num, RECT& rect);		// 获取动态图素的当前页和RECT
	long AniLoop();					// 动画过一桢

	int Id;				//编号
	char Name[32];		//名字
	int Width, Height;	//大小 单位：格子
	stCell *Cell;		//格子

	int SX,SY;		//所处场景的左上坐标(格子)
	int DX,DY;		//生成场景时的左上坐标偏移(像素 -32<xxx<=0 )
	int sx,sy;		//所处场景的左上绝对坐标(像素 sx=SX*32-DX)

	int startx, starty;	//绘图时的起始格子
	int endx, endy;		//绘图时的结束格子

	int Hook_Num;					//陷阱数量
	char Hook[MAX_HOOK][64];		//陷阱列表

	vectorNpc m_vNpc;		//NPC
	vectorNpc m_vSortNpc;

	int ScrollX, ScrollY;	//卷动地图的目标点
	int ScrollStep;			//卷动的幅度

	CGpFont ShowNameFont;	//地图名字显示字体对象
	int ShowNameCount;		//地图名字显示记数器(每一桢减一)
	int ShowNameAlpha;		//地图名字显示alpha度

	char CurrentMapFileName[MAX_PATH];	//当前地图文件名
	char CurrentMapFilePath[MAX_PATH];	//当前地图路径

	char m_strBackBmpFilename[128];	// 背景图片名称
	LPDIRECTDRAWSURFACE m_pBackBmp;	// 背景图片

public:
	CMap();		//构造（填写显示坐标列表）
	~CMap();	//析构

	void LoadAniCell(char *file, char *index);	//读入动态元素

	bool InitMap(int,int);	//初始化一张地图
	bool LoadMap(char *);	//装入地图
	bool SaveMap(char *);	//保存地图
	void Load(char *);		//读入进度
	void Save(char *);		//保存进度
	void ShowMap(bool active=true,LPDIRECTDRAWSURFACE suft = lpDDSBack);//显示地图
	void ShowEditMap();		//显示编辑时的地图

	bool ChangeScene(int cp, bool bCenter=true);	//场景切换(cp=切换点编号)
	void FadeIn(int Style=2, int time=50, WORD color=0);	//淡入
	void FadeOut(int Style=2, int time=50);					//淡出

	void CenterMap(int, int);		//设置地图中心点
	void ScrollTo(int, int, int);	//卷动地图到某点
	void ScrollLoop();				//卷动循环

	void CountDisplayGrid();		//计算显示格子

	void AddHook(int x, int y, char *string);	//添加地图陷阱
	void SetHook(int x, int y, int num);		//设置地图陷阱

	void SetCell(int level, int x, int y, int num, int page);	//设置地图某格地面
	void SetBlock(int x, int y, int block);						//设置地图阻挡
	void SetLevel(int x, int y, int level1, int level2);		//设置地图层次

	bool LoadIniNpcList(char *);	//从Ini文件读取NPC列表
	int GetNpcId(char *);			//从名字获取NPC的编号
	int AddNpc(char *, char *);		//加入一个NPC(返回编号)
	int DelNpc(int);				//删除一个NPC
	void ClearNpc();				//清空NPC
	void SortNpc();					//排序NPC

	int _GetCell(int,int,int);		//获取一个点所在的绝对格子
	int _GetCellX(int,int);			//获取一个点所在的格子X
	int _GetCellY(int,int);			//获取一个点所在的格子Y
	int GetCell(int,int);			//获取一个点所在的格子
	int GetCellX(int,int);			//获取一个点所在的格子X
	int GetCellY(int,int);			//获取一个点所在的格子Y
	RECT GetRectangle(int);			//获取一个格子的坐标
	RECT _GetRectangle(int,int);	//获取一个格子的绝对坐标

	void FillCell(int, int, WORD);	//填充一个格子

	void CreateFullMap(LPDIRECTDRAWSURFACE lpSurf, int lSurfWidth, int lSurfHeight);	//把整张地图复制到lpSurf
	void ShowFullMap();									//显示整张地图
	void SaveFullMap(char* filename, float fPer=1.f);	// 保存整张地图

	void RestoreLevel(int n=0);		//恢复所有层次关系
	void RestoreBlock(int n=0);		//恢复所有阻挡关系
	void RestoreMouseType(int n=0);	//恢复所有鼠标状态关系

////////////////////////////////////////////////////////////////
// 敌人列表
////////////////////////////////////////////////////////////////
private:
	BOOL m_bScrollRight;	// 向右卷地图
	BOOL m_bScrollDown;		// 向下卷地图
	int m_pEnemyList[16];	// 敌人编号列表
	int m_nEnemyNum;		// 敌人数量
	int m_nEnemyFrequency;	// 敌人出现频率	(0-100)

public:
	bool* Block;			//移动时候的阻挡数组
	void SetCPType(int x,int y,int type);//设置陷阱类型
	int GetEnemyNum()				{ return m_nEnemyNum; }
	void SetEnemyNum(int n)			{ m_nEnemyNum = n; }
	int GetEnemyList(int n)			{ return m_pEnemyList[n]; }
	void SetEnemyList(int n, int m)	{ m_pEnemyList[n] = m; }
	void ClearEnemyList()			{ memset(m_pEnemyList, 16*4, 0); }

	int GetEnemyFrequency()			{ return m_nEnemyFrequency; }
	void SetEnemyFrequency(int n)	{ m_nEnemyFrequency = n; }
};

#endif