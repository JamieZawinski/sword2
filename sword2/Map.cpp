//********************************************
//	地图 相关函数
//  创建于2000年3月14日
//********************************************
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include "gamelib\goldpoint.h"
#include "mapedit\mapedit.h"
#include "findpath.h"
#include "main.h"
#include "cursor.h"
#include "role.h"
#include "npc.h"
#include "talk.h"
#include "map.h"


#define GetX(n,w) ((n)%(w))
#define GetY(n,w) ((n)/(w))

int CMap::AniCellNum=0;
stAniCell *CMap::AniCell=NULL;
vectorAniCell CMap::m_vAniCell[4096];	// 动画CELL
int CMap::m_nAniCount[4096];			// 动画的进度
int CMap::m_nAniFrame[4096];			// 动画当前桢

//***************
//填写坐标表
CMap::CMap()
{
	Cell = NULL;
	strcpy(CurrentMapFileName,"Unknow");	//当前地图文件名
	strcpy(CurrentMapFilePath,"Unknow");	//当前地图路径
	m_pBackBmp = NULL;
}

CMap::~CMap()
{
	ClearNpc();
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY(Block);
	_DELETE_ARRAY( AniCell );
}

//读入动态元素
void CMap::LoadAniCell(char *file, char *index)
{
	ifstream stream;
	stream.open(file);
	if( stream.is_open() )
	{
		stream >> AniCellNum;		//AniCell的数量
		for(int i=0; i<AniCellNum; i++)
		{
			m_vAniCell[i].clear();	//清除

			long num=0;
			stream >> num;			//动画序列	
			for(int j=0; j<num; j++)
			{
				stAniCell st;
				stream >> st.page >> st.cell >> st.time;		//页，图块，时间
				m_vAniCell[i].push_back(st);
			}
		}
		stream.close();
	}

	memset(m_nAniCount, 0, 4096*4);
	memset(m_nAniFrame, 0, 4096*4);
}

//***************
//初始化地图
bool CMap::InitMap(int w, int h)
{
	Id=0;
	strcpy(Name,"");
	Width=w+1;
	Height=h+1;
	
	SX=SY=0; //所处场景的左上坐标，明确的说是可见区域的左上坐标
	DX=DY=0; //生成场景时的左上坐标

	CountDisplayGrid();	//计算显示格子

	//声请格子内存
	_DELETE_ARRAY(Cell);
	_DELETE_ARRAY(Block);
	Block = new bool[Width*Height];
	Cell = new stCell[Width*Height];
	//初始化格子
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].GroundPic=0;
		Cell[i].Ground=6;
		Cell[i].ObjPic=0;
		Cell[i].Obj=BLOCK_CELL;
		Cell[i].Obj2Pic=0;
		Cell[i].Obj2=BLOCK_CELL;
		Cell[i].Level=0;
		Cell[i].Level2=0;
		Cell[i].Block=0;
		Cell[i].CP=BLOCK_CP;
		Cell[i].MouseType=0;
		Block[i] = 0;
	}		
	//陷阱数目
	Hook_Num=0;

	//初始化字体
	char *strTemp = GameIni.ReadText("配置文件","字体");
	ShowNameFont.LoadFont(strTemp, "地图名称字体");
	_FREE(strTemp);

	//初始化该地图的寻路节点数组
	CFindPath cfp(&map);
	cfp.Init(Width, Height);

	// 初始化敌人设置
	m_nEnemyNum = 0;
	m_nEnemyFrequency = 0;
	ClearEnemyList();

	return true;
}

//***************
//读入地图
bool CMap::LoadMap(char *FileName)
{
	FILE *fp;

	fp=fopen(FileName, "rb");
	if( fp==NULL )
	{
		ShowMessage("Load map file error!\n找不到文件:%s",FileName);
		return false;
	}

	fseek(fp, 32, SEEK_SET);		//挑过文件头
	fread(&Id,4,1,fp);
	fread(Name,32,1,fp);
	fread(&Width,4,1,fp);
	fread(&Height,4,1,fp);

	//声请格子内存
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY(Block);
	Cell=new stCell[Width*Height];
	Block = new bool[Width*Height];
	fread(Cell, sizeof(stCell), Width*Height, fp);
	//读入陷阱列表
	fread(&Hook_Num,4,1,fp);
	fread(Hook, 64, Hook_Num, fp);
	for(int i =0;i<Width*Height;i++)
		Block[i]=0;

	// 读入敌人列表
	m_nEnemyNum = 0;
	m_nEnemyFrequency = 0;
	fread(&m_nEnemyNum, 4, 1, fp);
	fread(m_pEnemyList, 4, 16, fp);
	fread(&m_nEnemyFrequency, 4, 1, fp);

	// 背景图片
	m_strBackBmpFilename[0]=0;
	_RELEASE(map.m_pBackBmp);
	fread(m_strBackBmpFilename, 128, 1, fp);
	if( m_strBackBmpFilename[0] )
	{
		CreateBitmap(map.m_pBackBmp, 0,0, map.m_strBackBmpFilename);
	}

	fclose(fp);

	//清空NPC
	ClearNpc();
	
	//获取当前地图文件名和路径名
	sprintf(CurrentMapFileName, "%s\\%s", CurrentPath, FileName);
	char* str = GetFilePath(CurrentMapFileName);
	strcpy(CurrentMapFilePath, str);
	_FREE( str );

	SX=SY=DX=DY=0;
	CountDisplayGrid();
	ClearEditData();	//清空编辑数据－避免出错
	role[0].bMoving=false;

	//读取当前地图的对话文件
	char dialogfile[MAX_PATH];
	sprintf(dialogfile, "%s%s", CurrentMapFilePath, "talk.txt");
	talk.OpenFile( dialogfile );

	//显示地图名字数秒种
	ShowNameCount=70;
	ShowNameAlpha=32;

	//初始化该地图的寻路节点数组
	CFindPath cfp(&map);
	cfp.Init(Width, Height);
	//初始话鼠标
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	return true;
}

//***************
//保存地图
bool CMap::SaveMap(char *FileName)
{
	//清除人物的阻挡
	//Cell[map.Width*role[0].State.Y+role[0].State.X].Block=0;

	//for(int i=0; i<m_vNpc.size(); i++)
	//{
//		Cell[map.Width*m_vNpc[i]->State.Y+m_vNpc[i]->State.X].Block=0;
//	}

	//开文件
	FILE *fp;

	fp=fopen(FileName, "wb");
	if( fp==NULL )
	{
		MessageBox(hWnd,"Save map file error!","",MB_OK);
		return false;
	}

	fwrite(MAP_HEAD, 32, 1, fp);	//文件头
	fwrite(&Id,4,1,fp);
	fwrite(Name,32,1,fp);
	fwrite(&Width,4,1,fp);
	fwrite(&Height,4,1,fp);
	fwrite(Cell, sizeof(stCell), Width*Height, fp);

	//保存陷阱列表
	fwrite(&Hook_Num,4,1,fp);
	fwrite(Hook, 64, Hook_Num, fp);

	// 保存敌人列表
	fwrite(&m_nEnemyNum, 4, 1, fp);
	fwrite(m_pEnemyList, 4, 16, fp);
	fwrite(&m_nEnemyFrequency, 4, 1, fp);

	fwrite(m_strBackBmpFilename, 128, 1, fp);

	fclose(fp);

	//恢复人物的阻挡
	//Cell[map.Width*role[0].State.Y+role[0].State.X].Block=1;

	//for(i=0; i<m_vNpc.size(); i++)
//	{
	//	Cell[map.Width*m_vNpc[i]->State.Y+m_vNpc[i]->State.X].Block=1;
//	}
	return true;
}

//***************
//读入进度
void CMap::Load(char *FileName)
{
	FILE *fp;

	fp=fopen(FileName, "rb");
	if( fp==NULL )
	{
		ShowMessage("Load map record file %s error!",FileName);
		return;
	}

	fread(&Id,4,1,fp);
	fread(Name,32,1,fp);
	fread(&Width,4,1,fp);
	fread(&Height,4,1,fp);

	//声请格子内存
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY( Block );
	Cell=new stCell[Width*Height];
	Block=new bool[Width*Height];
	fread(Cell, sizeof(stCell), Width*Height, fp);
	//陷阱列表
	fread(&Hook_Num,4,1,fp);
	fread(Hook, 64, Hook_Num, fp);
	for(int i =0;i<Width*Height;i++)
		Block[i]=0;

	// 读入敌人列表
	m_nEnemyNum = 0;
	m_nEnemyFrequency = 0;
	fread(&m_nEnemyNum, 4, 1, fp);
	fread(m_pEnemyList, 4, 16, fp);
	fread(&m_nEnemyFrequency, 4, 1, fp);

	fread(&SX,4,1,fp);
	fread(&SY,4,1,fp);
	fread(&DX,4,1,fp);
	fread(&DY,4,1,fp);
	fread(&sx,4,1,fp);
	fread(&sy,4,1,fp);

	//当前地图文件名和路径名
	char str[128];
	memset(str, 0, 128);
	fread(str, 128, 1, fp);
	sprintf(CurrentMapFileName, "%s\\%s", CurrentPath, str);
	
	memset(str, 0, 128);
	fread(str, 128, 1, fp);
	sprintf(CurrentMapFilePath, "%s\\%s", CurrentPath, str);

	//读取NPC
	long lNpcNum;
	fread(&lNpcNum, 4, 1, fp);
	ClearNpc();

	char temp[MAX_PATH];
	for(i=0; i<lNpcNum; i++)
	{
		sprintf(temp, "%s%d", FileName, i);
		CNpc* p = new CNpc;
		p->LoadRole(temp);
		m_vNpc.push_back(p);
	}
	for(i=0; i<m_vNpc.size(); i++)
	{
		Block[map.Width*m_vNpc[i]->State.Y+m_vNpc[i]->State.X]=1;
	}
	
	// 背景图片
	m_strBackBmpFilename[0]=0;
	_RELEASE(map.m_pBackBmp);
	fread(m_strBackBmpFilename, 128, 1, fp);
	if( m_strBackBmpFilename[0] )
	{
		CreateBitmap(map.m_pBackBmp, 0,0, map.m_strBackBmpFilename);
	}
	fclose(fp);


	CountDisplayGrid();

	//读取当前地图的对话文件
	char dialogfile[MAX_PATH];
	sprintf(dialogfile, "%s%s", CurrentMapFilePath, "talk.txt");
	talk.OpenFile( dialogfile );

	//显示地图名字数秒种
	ShowNameCount=70;
	ShowNameAlpha=32;

	//初始化该地图的寻路节点数组
	CFindPath cfp(&map);
	cfp.Init(Width, Height);
}

//***************
//保存进度
void CMap::Save(char *FileName)
{
	//开文件
	FILE *fp;

	fp=fopen(FileName, "wb");
	if( fp==NULL )
	{
		ShowMessage("Save map record file %s error!",FileName);
		return;
	}

	fwrite(&Id,4,1,fp);
	fwrite(Name,32,1,fp);
	fwrite(&Width,4,1,fp);
	fwrite(&Height,4,1,fp);
	fwrite(Cell, sizeof(stCell), Width*Height, fp);

	//保存陷阱列表
	fwrite(&Hook_Num,4,1,fp);
	fwrite(Hook, 64, Hook_Num, fp);

	// 保存敌人列表
	fwrite(&m_nEnemyNum, 4, 1, fp);
	fwrite(m_pEnemyList, 4, 16, fp);
	fwrite(&m_nEnemyFrequency, 4, 1, fp);

	fwrite(&SX,4,1,fp);
	fwrite(&SY,4,1,fp);
	fwrite(&DX,4,1,fp);
	fwrite(&DY,4,1,fp);
	fwrite(&sx,4,1,fp);
	fwrite(&sy,4,1,fp);

	//当前地图文件名和路径名
	char str[128];
	memset(str, 0, 128);
	strcpy(str, &CurrentMapFileName[strlen(CurrentPath)+1]);
	fwrite(str, 128, 1, fp);
	memset(str, 0, 128);
	strcpy(str, &CurrentMapFilePath[strlen(CurrentPath)+1]);
	fwrite(str, 128, 1, fp);

	//保存NPC
	char temp[MAX_PATH];
	long lNpcNum = m_vNpc.size();
	fwrite(&lNpcNum, 4, 1, fp);
	for(int i=0; i<lNpcNum; i++)
	{
		sprintf(temp, "%s%d", FileName, i);
		m_vNpc[i]->SaveRole(temp);
	}

	// 背景图片
	fwrite(m_strBackBmpFilename, 128, 1, fp);
	fclose(fp);
}

// 获取动态图素的当前页和RECT(地面层)
long CMap::GetAniCellPage(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Ground;		//编号
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;	//当前页
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;	//图块
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// 获取动态图素的当前页和RECT(物体层1)
long CMap::GetAniCellPageObj(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Obj;
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// 获取动态图素的当前页和RECT(物体层2)
long CMap::GetAniCellPageObj2(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Obj2;
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// 获取动态图素的当前页和RECT(直接用编号)
long CMap::GetAniPage(const long num, RECT& rect)
{
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// 动画过一桢
long CMap::AniLoop()
{
	for(int i=0; i<AniCellNum; i++)
	{
		if( ++m_nAniCount[i] > m_vAniCell[i][m_nAniFrame[i]].time )	//延时
		{
			m_nAniCount[i] = 0;
			if( ++m_nAniFrame[i] >= m_vAniCell[i].size() )
			{
				m_nAniFrame[i] = 0;			//当前桢
			}
		}
	}
	return true;
}

//*******************
//显示地图到surf上
void CMap::ShowMap(bool active,LPDIRECTDRAWSURFACE surf)
{
	int i,j, cellnum, obj, obj2, level;
	RECT rect = {0,0,0,0};

	// 显示背景
	if( m_pBackBmp )
	{
		int w, h;
		GetSurfaceSize(m_pBackBmp, w, h);
		rect.right = w;
		rect.bottom = h;
		BltFast(surf, 0, 0, m_pBackBmp, rect, false);
	}
	//更新人物位置
	if( active )
	{
		//主角移动
		role[0].Move(role[0].State.X, role[0].State.Y);
		
		CurrentNpc=-1;
		for(i=0; i<m_vNpc.size(); i++)
		{
			//NPC移动
			if( m_vNpc[i]->State.bActive && !m_vNpc[i]->bMoving )
			{
				if( random(100)>98 )
				{
					m_vNpc[i]->MoveTo(random(Width)*CellWidth, random(Height)*CellHeight);
				}
			}
			
			m_vNpc[i]->Move(m_vNpc[i]->State.X, m_vNpc[i]->State.Y);
		}
	}
	
	//计算显示坐标 startx\endx\starty\endy
	CountDisplayGrid();

	//显示地面
	int ground=Width * starty + startx;
	for(i=starty; i<endy ; i++)
	{
		for(j=startx; j<endx ; j++)
		{
			if( Cell[ ground ].GroundPic < 60 )	//静态元素
			{
				//位于地图页面的位置矩形
				rect=_GetRectangle(Cell[ ground ].Ground, ShowCellW);
				//Blt到surf上
				BltFast(surf, DX+_point_x(j-SX), DY+_point_y(i-SY), 
					lpDDSMap[ Cell[ ground ].GroundPic ], rect, true);

				ground++;
			}
			else	// 动态图素
			{
				RECT rect = {0,0,0,0};
				long page = GetAniCellPage(ground, rect);
				BltFast(surf, DX+_point_x(j-SX), DY+_point_y(i-SY),
					lpDDSMap[ page ], rect, true);
				ground++;
			}
		}
		//下一行
		ground=ground+Width-(endx-startx);
	}

	//显示物体
	for(i=starty; i<endy ; i++)
		for(j=endx-1; j>=startx; j--)
		{
			//TILE坐标
			cellnum=Width * i + j;
			
			//画不能遮挡主角的物体 level1
			level=Cell[ cellnum ].Level;
			if( level==OL_NORMAL )			//人物同级
			{
				obj=Cell[ cellnum ].Obj;	//编号
				
				if( obj != BLOCK_CELL )		//不是空格子
				{
					if( Cell[ cellnum ].ObjPic < 60 )	//静态元素
					{
						rect=_GetRectangle(obj, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[Cell[ cellnum ].ObjPic], rect, TRUE);
					}
					else	// 动态图素
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
			
			//画不能遮挡主角的物体 level2
			level=Cell[ cellnum ].Level2;
			if( level==OL_NORMAL )		//人物同级
			{
				obj2=Cell[ cellnum ].Obj2;	//编号
				if( obj2 != BLOCK_CELL )	//不是空的
				{
					if( Cell[ cellnum ].Obj2Pic < 60 )	//静态元素
					{
						rect=_GetRectangle(obj2, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY),
							lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, TRUE);
					}
					else	// 动态图素
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj2(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
		}

	//显示NPC
	bool bShowPlayer = false;
	//NPC按y坐标排序，使下面的NPC挡住上面的NPC
	SortNpc();
	for(int n=0; n<m_vSortNpc.size(); n++)
	{
		//插入的方式显示显示主角，实现遮挡关系
		if( !bShowPlayer && role[0].State.y < m_vSortNpc[n]->State.y && role[0].State.bShow )
		{
			role[0].Show(j, i);
			bShowPlayer = true;
		}

		if( m_vSortNpc[n]->State.bShow )
		{
			m_vSortNpc[n]->ShowNpcPic(j, i, n);
		}
	}

	//如果上面没有显示主角，就显示主角。原因是因为主角的y坐标比所有的NPC都大
	if( !bShowPlayer && role[0].State.bShow )
	{
		role[0].Show(j, i);
	}

	for(i=starty; i<endy ; i++)
		for(j=startx; j<endx ; j++)
		{
			//TILE坐标
			cellnum=Width * i + j;
			
			//画能遮挡主角的物体 level1
			//一样的逻辑
			level=Cell[ cellnum ].Level;
			if( level==OL_UP )
			{
				obj=Cell[ cellnum ].Obj;
				if( obj != BLOCK_CELL )		
				{
					if( Cell[ cellnum ].ObjPic < 60 )
					{
						rect=_GetRectangle(obj, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[Cell[ cellnum ].ObjPic], rect, TRUE);
					}
					else	// 动态图素
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
			
			//画能遮挡主角的物体 level2
			level=Cell[ cellnum ].Level2;
			if( level==OL_UP )
			{
				obj2=Cell[ cellnum ].Obj2;
				if( obj2 != BLOCK_CELL )
				{
					if( Cell[ cellnum ].Obj2Pic < 60 )
					{
						rect=_GetRectangle(obj2, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, TRUE);
					}
					else	// 动态图素
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj2(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
		}

	//人物到物体后面的半透明显示
	if( role[0].State.Y < map.Height && role[0].State.X < map.Width )
	{
		//两层物体都不是空的
		if( map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj != BLOCK_CELL ||
			map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj2 != BLOCK_CELL )
			//Alpha显示主角
			role[0].ShowRole(24);
	}

	//显示当前地图名字
	if( ShowNameCount>0 )
	{
		ShowNameCount--;
		if( ShowNameCount > 16 && ShowNameAlpha >0 )	
			ShowNameAlpha-=2;	//淡入

		if( ShowNameCount < 16 )	
			ShowNameAlpha+=2;	//淡出
		//阴影显示
		ShowNameFont.SetColor(0);
		ShowNameFont.PrintAlphaText(surf, 17, 15, ShowNameAlpha, Name);

		ShowNameFont.SetColor(RGB(225,255,225));
		ShowNameFont.PrintAlphaText(surf, 15, 13, ShowNameAlpha, Name);
	}

	// 动画
	AniLoop();
}

//*******************
//显示编辑时的地图，和ShowMap一样的逻辑
void CMap::ShowEditMap()
{
	int i,j, cellnum, obj, obj2, level, level2, block;
	RECT rect;

	// 显示背景
	if( m_pBackBmp )
	{
		int w, h;
		GetSurfaceSize(m_pBackBmp, w, h);
		rect.right = w;
		rect.bottom = h;
		BltFast(lpDDSBack, 0, 0, m_pBackBmp, rect, false);
	}

	//主角移动
	role[0].Move(role[0].State.X, role[0].State.Y);

	CurrentNpc=-1;
	for(i=0; i<m_vNpc.size(); i++)
	{
		//获取当前指向NPC
		if( cx == m_vNpc[i]->State.X && cy == m_vNpc[i]->State.Y )
		{
			CurrentNpc=i;
		}

		//NPC移动
		if( m_vNpc[i]->State.bActive && !m_vNpc[i]->bMoving )
		{
			if( random(100)>98 )
			{
				m_vNpc[i]->MoveTo(random(Width)*CellWidth, random(Height)*CellHeight);
			}
		}
		m_vNpc[i]->Move(m_vNpc[i]->State.X, m_vNpc[i]->State.Y);
	}
	
	//计算显示坐标 startx\endx\starty\endy
	CountDisplayGrid();

	//显示地面
	int ground=Width * starty + startx;
	
	if( bShowOnlyObject )	//只显示物体
		Clrscr(lpDDSBack, 0);
	else
		for(i=starty; i<endy ; i++)
		{
			for(j=startx; j<endx ; j++)
			{
				if( Cell[ ground ].GroundPic < 60 )
				{
					rect=_GetRectangle(Cell[ ground ].Ground, ShowCellW);
					BltFast(lpDDSBack, DX+_point_x(j-SX), DY+_point_y(i-SY), 
							lpDDSMap[ Cell[ ground ].GroundPic ], rect, true);
					ground++;
				}
				else	// 动态图素
				{
					RECT rect = {0,0,0,0};
					long page = GetAniCellPage(ground, rect);
					BltFast(lpDDSBack, DX+_point_x(j-SX), DY+_point_y(i-SY), 
							lpDDSMap[ page ], rect, true);
					ground++;
				}
			}
			ground=ground+Width-(endx-startx);
		}
		
		//显示物体和人物
	for(i=starty; i<endy ; i++)
		for(j=endx-1; j>=startx; j--)
		{
			if( bShowObject )	//显示物体
			{
				cellnum=Width * i + j;
				
				//画不能遮挡主角的物体
				level=Cell[ cellnum ].Level;
				if( level==OL_NORMAL )
				{
					obj=Cell[ cellnum ].Obj;							
					if( obj != BLOCK_CELL )
					{
						if( Cell[ cellnum ].ObjPic < 60 )
						{
							rect=_GetRectangle(obj, ShowCellW);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT )//NOCOLORKEY
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[Cell[ cellnum ].ObjPic], rect, false);
							else 
								if( !bShowOnlyObject )						//SRCCOLORKAY
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY),
										lpDDSMap[Cell[ cellnum ].ObjPic], rect, true);
						}
						else	// 动态图素
						{
							RECT rect = {0,0,0,0};
							long page = GetAniCellPageObj(cellnum, rect);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[page], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
										lpDDSMap[page], rect, true);
						}
					}
				}
				
				level=Cell[ cellnum ].Level2;
				if( level == OL_NORMAL )
				{
					obj2=Cell[ cellnum ].Obj2;
					if( obj2 != BLOCK_CELL )
					{
						if( Cell[ cellnum ].Obj2Pic < 60 )
						{
							rect=_GetRectangle(obj2, ShowCellW);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT2 )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
										lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, true);
						}
						else	// 动态图素
						{
							RECT rect = {0,0,0,0};
							long page = GetAniCellPageObj2(cellnum, rect);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT2 )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[page], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
										lpDDSMap[page], rect, true);
						}
					}
				}
			}
		}

	//显示NPC
	bool bShowPlayer = false;
	SortNpc();
	for(int n=0; n<m_vSortNpc.size(); n++)
	{
		//显示主角
		if( !bShowPlayer && role[0].State.y < m_vSortNpc[n]->State.y && role[0].State.bShow )
		{
			role[0].Show(j, i);
			bShowPlayer = true;
		}

		if( m_vSortNpc[n]->State.bShow )
		{
			m_vSortNpc[n]->ShowNpcPic(j, i, n);
		}
	}

	for(i=starty; i<endy ; i++)
		for(j=startx; j<endx ; j++)
		{
			cellnum=Width * i + j;
			level=Cell[ cellnum ].Level;
			level2=Cell[ cellnum ].Level2;
			block=Cell[ cellnum ].Block;
			
			if( bShowObject )	//显示物体
			{
				obj=Cell[ cellnum ].Obj;
				obj2=Cell[ cellnum ].Obj2;
				
				//画能遮挡主角的物体
				if( level==OL_UP )
				{
					if( obj != BLOCK_CELL )
					{
						if( Cell[ cellnum ].ObjPic < 60 )
						{
							rect=_GetRectangle(obj, ShowCellW);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[Cell[ cellnum ].ObjPic], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY),
										lpDDSMap[Cell[ cellnum ].ObjPic], rect, true);
						}
						else	// 动态图素
						{
							RECT rect = {0,0,0,0};
							long page = GetAniCellPageObj(cellnum, rect);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[page], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
										lpDDSMap[page], rect, true);
						}
					}
				}
				
				if( level2==OL_UP )
				{
					if( obj2 != BLOCK_CELL )
					{
						if( Cell[ cellnum ].Obj2Pic < 60 )
						{
							rect=_GetRectangle(obj2, ShowCellW);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT2 )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, false);
							else if( !bShowOnlyObject )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, true);
						}
						else	// 动态图素
						{
							RECT rect = {0,0,0,0};
							long page = GetAniCellPageObj2(cellnum, rect);

							if( bShowOnlyObject && Cur_Level == CL_OBJECT2 )
								Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
									lpDDSMap[page], rect, false);
							else 
								if( !bShowOnlyObject )
									Blt(lpDDSBack, DX+ _point_x(j-SX), DY+ _point_y(i-SY),
										lpDDSMap[page], rect, true);
						}
					}
				}
			}
			
			if( g_ePlayState == PS_EDIT )	//显示状态
			{
				//显示阻挡关系
				if( bShowBlock && Cell[cellnum].Block==1||Block[cellnum]==1)
					FillCell(j-startx, i-starty, 1);
				
				//显示陷阱
				if( bShowEdit && Cell[cellnum].CP != BLOCK_CP )
				{
					FillCell(j-startx, i-starty, 2);
					font.SetColor(RGB(255,255,0));
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX, _point_y(i-starty)+DY, "%d", Cell[cellnum].CP );
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX+16, _point_y(i-starty)+DY+16, "%d", Cell[cellnum].CPType );
				}
				
				//显示鼠标状态
				if( bShowEdit && Cell[cellnum].MouseType != CCursor::CS_NORMAL )
				{
					font.SetColor(RGB(255,255,255));
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX, _point_y(i-starty)+DY+16, "%d", Cell[cellnum].MouseType );
				}
				
				//显示物体层次
				if( bShowEdit )
					if( bShowOnlyObject )
					{
						if( Cur_Level == CL_OBJECT && Cell[cellnum].Level == OL_UP )
						{
							FillCell(j-startx, i-starty, 7);
						}
						
						if( Cur_Level == CL_OBJECT2 && Cell[cellnum].Level2 == OL_UP )
						{
							FillCell(j-startx, i-starty, 8);
						}
					}
					else
					{
						if( Cell[cellnum].Level==OL_UP )
						{
							FillCell(j-startx, i-starty, 7);
						}
						
						if( Cell[cellnum].Level2==OL_UP )
						{
							FillCell(j-startx, i-starty, 8);
						}
					}
			}
	}

	//人物到物体后面的半透明显示
	if( map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj != BLOCK_CELL )
		role[0].ShowRole(24);

	//显示当前地图名字
	if( ShowNameCount>0 )
	{
		ShowNameCount--;
		if( ShowNameCount > 16 && ShowNameAlpha >0 )	ShowNameAlpha-=2;	//淡入
		if( ShowNameCount < 16 )	ShowNameAlpha+=2;						//淡出

		ShowNameFont.SetColor(0);
		ShowNameFont.PrintAlphaText(lpDDSBack, 7, 5, ShowNameAlpha, Name);
		ShowNameFont.SetColor(RGB(255,255,255));
		ShowNameFont.PrintAlphaText(lpDDSBack, 5, 3, ShowNameAlpha, Name);
	}

	// 动画
	AniLoop();
}

//-----------------------------------------------------------------

//淡入
void CMap::FadeIn(int Style, int time, WORD color)
{
	if( g_eDisplayState == DS_NIGHT )
		GetPrimarySurface(lpDDSBack);

	//特效淡入
	switch( Style )
	{
	case 0:	//无效果
		break;

	case 1:	//百叶窗
		ScreenIn(16, time);
		break;
	
	case 2:	//淡入淡出
		ColorIn(time, color);
		break;
	}
}

//淡出
void CMap::FadeOut(int Style, int time)
{
	switch( Style )
	{
	case 0:	//无效果
		break;

	case 1:	//百叶窗
		map.ShowMap(true);
		if( g_eDisplayState==DS_NIGHT ) 
			Clrscr(lpDDSBackGround, 0);
		//更新到lpDDSBackGround
		UpdateScreen(lpDDSBackGround, RectScreen);

		ScreenOut(lpDDSBackGround, 16, time);

	case 2:	//淡入淡出
		map.ShowMap(true);
		if( g_eDisplayState==DS_NIGHT ) 
			Clrscr(lpDDSBackGround, 0);
		UpdateScreen(lpDDSBackGround, RectScreen);
		ColorOut(lpDDSBackGround, time);
		break;
	}
}

//设置地图中心点
void CMap::CenterMap(int centx, int centy)
{
	SX=centx-10;		//向左10个格子
	SY=centy-7;			//向上7个格子

	//边界检查
	if( SX >= Width-1-ShowCellW ) 
		SX=Width-1-ShowCellW;

	if( SY >= Height-1-ShowCellH ) 
		SY=Height-1-ShowCellH;

	if( SX<0 ) 
		SX=0;

	if( SY<0 ) 
		SY=0;

	//计算当前地图左上角坐标
	sx=_point_x(SX)-DX;
	sy=_point_y(SY)-DY;
}

//卷动地图到某点
void CMap::ScrollTo(int ox, int oy, int ostep)
{
	ScrollX=ox;
	ScrollY=oy;
	ScrollStep=ostep;
	
	//边界检查
	if( ScrollX > Width-ShowCellW-2 )
	{
		ScrollX=Width-ShowCellW-2;
	}
	if( ScrollX < 0 )
	{
		ScrollX=0;
	}

	if( ScrollY > Height-ShowCellH-2 )
	{
		ScrollY=Height-ShowCellH-2;
	}
	if( ScrollY < 0 )
	{
		ScrollY=0;
	}
	//进入卷地图状态
	if(ScrollX>SX)
		m_bScrollRight = TRUE;
	else
		m_bScrollRight = FALSE;

	if(ScrollY>SY)
		m_bScrollDown = TRUE;
	else
		m_bScrollDown = FALSE;
	g_ePlayState = PS_SCROLLMAP;
}

//卷动循环
void CMap::ScrollLoop()
{
	int ox=ScrollX-SX;
	int oy=ScrollY-SY;
	//先是DX的移动，超过一个格子后就移动一个格子，DX复位
	if( ox > 0 )		//右
	{
		DX-=ScrollStep;
		if( DX <= -32 )
		{
			DX+=32;
			SX++;
		}
	}
	else if( ox < 0 )	//左
	{
		DX+=ScrollStep;
		if( DX >=0 )
		{
			DX-=32;
			SX--;
		}
	}
	else		//SX到位，如果是向右边卷，就停止，反之，还卷一个格子
	{
		if(m_bScrollRight)
			DX=0;
		else
		{
			DX+=ScrollStep;
			if(DX>=0)
				DX=0;
		}
	}
	//和X方向一样
	if( oy > 0 )	//下
	{
		DY-=ScrollStep;
		if( DY <= -32 )
		{
			DY+=32;
			SY++;
		}
	}
	else if( oy < 0 )	//上
	{
		DY+=ScrollStep;
		if( DY >=0 )
		{
			DY-=32;
			SY--;
		}
	}
	else
	{
		if(m_bScrollDown)
			DY=0;
		else
		{
			DY+=ScrollStep;
			if(DY>=0)
				DY=0;
		}
	}
	
	//卷动到位了
	if( ox == 0 && oy == 0 && DX == 0 && DY == 0 )
	{
		g_ePlayState=PS_MAIN;		//返回
	}

	ShowMap();		//显示地图
}

//添加地图陷阱
void CMap::AddHook(int x, int y, char *string)
{
	if( Hook_Num==MAX_HOOK )
	{
		ShowMessage("地图陷阱数量已满( Hook_Num=32 )");
		return;
	}

	Cell[Width*y+x].CP=Hook_Num;
	strcpy(Hook[Hook_Num], string);
	Hook_Num++;
}

//设置地图陷阱
void CMap::SetHook(int x, int y, int num)
{
	if( num>=MAX_HOOK && num != BLOCK_CP )
	{
		ShowMessage("地图陷阱超过最大值( Max Hook_Num=32 )");
		return;
	}

	Cell[Width*y+x].CP=num;
}

//设置地图某格地面
void CMap::SetCell(int level, int x, int y, int num, int page)
{
	if( level <0 || level >2 )
	{
		ShowMessage("层次 %d 超过最大值( MaxLevel=%d )", level, 2);
		return;
	}

	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("坐标(%d,%d)超出范围(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}

	if( page >= Max_GroundPic )
	{
		ShowMessage("页面 %d 超过最大值( MaxPage=%d )", page, Max_GroundPic-1);
		return;
	}

	int w,h;
	//得到页面大小
	GetSurfaceSize(lpDDSMap[page], w, h);
	if( num >= (w/32)*(h/32) )
	{
		ShowMessage("编号 %d 超过最大值( MaxNum=%d )", num, (w/32)*(h/32)-1);
		return;
	}

	switch(level)
	{
	case 0:	//地面
		Cell[Width*y+x].GroundPic=page;
		Cell[Width*y+x].Ground=num;
		break;

	case 1:	//物体1
		Cell[Width*y+x].ObjPic=page;
		Cell[Width*y+x].Obj=num;
		break;

	case 2:	//物体2
		Cell[Width*y+x].Obj2Pic=page;
		Cell[Width*y+x].Obj2=num;
		break;
	}

}

//设置地图阻挡
void CMap::SetBlock(int x, int y, int block)
{
	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("坐标(%d,%d)超出范围(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}
	if( block < 0 || block > 1 )
	{
		ShowMessage("block(%d)超出范围(0,1)", block);
		return;
	}
	Cell[Width*y+x].Block=block;
}

//设置地图层次
void CMap::SetLevel(int x, int y, int level1, int level2)
{
	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("坐标(%d,%d)超出范围(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}
	if( level1 < 0 || level1 > 1 )
	{
		ShowMessage("level1(%d)超出范围(0,1)", level1);
		return;
	}
	if( level2 < 0 || level2 > 1 )
	{
		ShowMessage("level2(%d)超出范围(0,1)", level2);
		return;
	}
	Cell[Width*y+x].Level=level1;
	Cell[Width*y+x].Level2=level2;
}

//读入Npc列表从ini文件
bool CMap::LoadIniNpcList(char *filename)
{
	//如果文件不存
	if( !CheckFile( filename ) )
	{
		return true;
	}

	//读入NPC
	CIniSet Ini;
	Ini.Open(filename);

	ClearNpc();
	//NPC最大数目
	long lNpcNum = Ini.ReadInt("Npc", "Max_Npc");

	for(int i=0; i<lNpcNum; i++)
	{
		CNpc* p = new CNpc;
		char *NpcIndex=Ini.ReadText("Npc", i);
		p->LoadNpcIni(filename, NpcIndex);
		m_vNpc.push_back(p);
		_FREE(NpcIndex);
	}

	return true;
}

//加入一个Npc
int CMap::AddNpc(char *filename, char *index)
{
	//如果文件不存
	if( !CheckFile( filename ) )
	{
		return ERROR_DATA;
	}

	CNpc* p = new CNpc;
	p->LoadNpcIni(filename, index);
	m_vNpc.push_back(p);

	return m_vNpc.size()-1;
}

//删除一个NPC
int CMap::DelNpc(int num)
{
	if( num >= m_vNpc.size() )
	{
		return ERROR_DATA;
	}

	//脚底方格清空阻挡
	Block[ Width*m_vNpc[num]->State.Y+m_vNpc[num]->State.X ]=0;

	if(m_vNpc[num]->bMoving)
	{
		Block[m_vNpc[num]->Path[m_vNpc[num]->PathCurrentStep].x +m_vNpc[num]->Path[m_vNpc[num]->PathCurrentStep].y*Width] =0;
	}
	long l = 0;
	for(itNpc it=m_vNpc.begin(); it!=m_vNpc.end(); it++)
	{
		if( l == num )
		{
			m_vNpc.erase(it);
			return m_vNpc.size();
		}
		l++;
	}

	return m_vNpc.size();
}

//从npc名字获取ID
int CMap::GetNpcId(char *string)
{
	for(int i=0; i<m_vNpc.size(); i++)
	{
		if( stricmp( m_vNpc[i]->Name, string)==0 )
		{
			return i;
		}
	}
	return -1;	//没找到
}

//排序NPC
void CMap::SortNpc()
{
	m_vSortNpc.clear();
	for(int i=0; i<m_vNpc.size(); i++)
	{
		for(itNpc it=m_vSortNpc.begin(); it!=m_vSortNpc.end(); it++)
		{
			CNpc* p = (*it);
			if( m_vNpc[i]->State.y < p->State.y )	//y坐标排序
			{
				m_vSortNpc.insert(it, m_vNpc[i]);
				goto _Next;
			}
		}

		m_vSortNpc.push_back(m_vNpc[i]);
_Next:;
	}
}

//清空NPC
void CMap::ClearNpc()
{
	for(int i=0; i<m_vNpc.size(); i++)
	{
		CNpc* p = m_vNpc[i];
		_DELETE(p);
	}
	
	m_vNpc.clear();
	m_vSortNpc.clear();
}

//计算显示格子
void CMap::CountDisplayGrid()
{
	if( g_eDisplayState == DS_NORMAL )		//普通模式
	{
		startx=SX;							//全显
		starty=SY;
		endx=startx+ShowCellW+1;
		endy=starty+ShowCellH+1;
	}
	else if( g_eDisplayState == DS_NIGHT )	//夜晚模式
	{
		startx=role[0].State.X-5;			//显示主角周围的一部分
		endx=startx+11;	
		//边界检查
		if( startx<0 ) 
		{
			startx=0;
		}
		if( endx>=Width ) 
		{
			endx=Width-1;
		}

		starty=role[0].State.Y-4;
		endy=starty+8;

		if( starty<0 ) 
		{
			starty=0;
		}

		if( endy>=Height ) 
		{
			endy=Height-1;
		}
	}
}

//-----------------------------------------------------------------

//获得一个绝对点的格子
int CMap::_GetCell(int x, int y, int W)
{
	return _grid_y(y) * W + _grid_x(x);
}

//**********************
//获得一个点所在的格子X
int CMap::_GetCellX(int x, int y)
{
	return _grid_x(x);
}

//**********************
//获得一个点所在的格子Y
int CMap::_GetCellY(int x, int y)
{
	return _grid_y(y);
}


//**********************
//获得一个点所在的格子
int CMap::GetCell(int x, int y)
{
	return Width * (_grid_y(y)+SY) + _grid_x(x) + SX;
}

//**********************
//获得一个点所在的格子X
int CMap::GetCellX(int x, int y)
{
	return _grid_x(x)+SX;
}

//**********************
//获得一个点所在的格子Y
int CMap::GetCellY(int x, int y)
{
	return _grid_y(y)+SY;
}

//********************
//获取一个格子的坐标
RECT CMap::GetRectangle(int n)
{
	int x=_point_x(n%Width), y=_point_y(n/Width);
	RECT rect={ x, y, x+CellWidth, y+CellHeight};
	return rect;
}

//********************
//获取一个格子的绝对坐标矩形
RECT CMap::_GetRectangle(int n, int W)
{
	int x=_point_x(n%W), y=_point_y(n/W);
	RECT rect={ x, y, x+CellWidth, y+CellHeight};
	return rect;
}

//**********************
//填充一个格子
void CMap::FillCell(int x, int y, WORD num)
{
	Blt(lpDDSBack, _point_x(x)+DX, _point_y(y)+DY, lpDDSTools, _GetRectangle(num, ShowCellW), true);
}

/////////////////////////////////////////////////////////////////////////
// 整张地图
/////////////////////////////////////////////////////////////////////////

//把整张地图复制到lpSurf
void CMap::CreateFullMap(LPDIRECTDRAWSURFACE lpSurf, int lSurfWidth, int lSurfHeight)
{
	float LoopX, LoopY;				//小图块个数
	int lTileWidth, lTileHeight;	//小图块大小
	int x, y;

	int _SX=SX;	//保存
	int _SY=SY;
	//需要几个屏幕才能显示完整个地图
	LoopX=(float)(Width-1)/20;		//横着20个格子
	LoopY=(float)(Height-1)/15;		//竖着15个格子

	lTileWidth = (int)(lSurfWidth/LoopX);
	lTileHeight = (int)(lSurfHeight/LoopY);

	for( int j=0; j<=(int)LoopY; j++ )
	{
		SY = j*15;
		y  = j*lTileHeight;

		//最后一个屏幕的行修正
		if( j==(int)LoopY )
		{
			SY=Height - 16;			//用于地图的显示，所以要显示一个整的地图
			int H=((Height)%15);	//剩下的高度
			y=(j)*lTileHeight - lTileHeight*H/15;	//重叠部分修正
		}

		for( int i=0; i<=(int)LoopX; i++ )
		{
			SX = i*20;
			x  = i*lTileWidth;

			//最后一屏幕的列修正
			if( i == (int)LoopX )
			{
				SX=Width - 21;
				int W=((Width)%20);
				x=(i)*lTileWidth - lTileWidth*W/20;
			}

			ShowMap(false);
			RECT dr={x, y, x+lTileWidth, y+lTileHeight};
			lpSurf->Blt(&dr, lpDDSBack, &RectScreen, DDBLT_WAIT, 0);
		}
	}

	SX = _SX;
	SY = _SY;
}

//显示整张地图
void CMap::ShowFullMap()
{
	//变黑
	FillSurface(lpDDSBackGround, RGB16(0,0,0) );

	float LoopX, LoopY;		//小图块个数
	int ow, oh;				//小图块大小
	int FullWidth, FullHeight;	//缩略图的总尺寸

	LoopX=(float)(Width-1)/20;
	LoopY=(float)(Height-1)/15;

	//以宽度为准
	if( (float)Width/Height >= (float)4/3 ) 
	{
		ow=(int)(ScreenWidth/LoopX);
		oh=(int)(ScreenHeight/LoopX);
		FullWidth=ScreenWidth;
		FullHeight=ScreenWidth*(Height-1)/(Width-1);
	}
	else //以高度为准
	{
		ow=(int)(ScreenWidth/LoopY);
		oh=(int)(ScreenHeight/LoopY);
		FullWidth=ScreenHeight*(Width-1)/(Height-1);
		FullHeight=ScreenHeight;
	}
	CreateFullMap(lpDDSBackGround, FullWidth, FullWidth);
	_UpdateScreen(lpDDSBackGround);

	//等待返回
	int px=0,py=0;
	MSG msg;
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//获取输入信息
			GetInputData();
			if( B_UP || mouse == LB_UP)
			{
				//显示地图的坐标
				if( px==0 ) 
					SX=0;
				else 
					SX=Width*px/FullWidth;

				if( py==0 ) 
					SY=0;
				else 
					SY=Height*py/FullHeight;

				return;
			}

			if( mouse == RB_UP )
			{
				return;
			}
			//跟踪鼠标，产生小视口的位置
			px=point.x-ow/2;
			py=point.y-oh/2;
			//边界检查
			if( px < 0 ) 
				px=0;
			if( py < 0 ) 
				py=0;

			if( px >= FullWidth - ow ) 
				px = FullWidth - ow -2;

			if( py >= FullHeight - oh ) 
				py = FullHeight - oh -2;
			//显示缩略图
			Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);
			//画上视口矩形
			Rectangle(lpDDSBack, px, py, px+ow, py+oh, RGB16(255,255,255));
			_UpdateScreen();
		}
		else WaitMessage();	//等待消息
	}
}

// 保存整张地图
void CMap::SaveFullMap(char* filename, float fPer)
{
	LPDIRECTDRAWSURFACE lpTemp;
	//根据缩放比例计算大小
	int w = (int)((Width-1)*32*fPer);
	int h = (int)((Height-1)*32*fPer);
	CreateBitmap(lpTemp, w, h);
	CreateFullMap(lpTemp, w, h);
	//保存成BPM文件
	SaveToBitmapFile(lpTemp, filename);
	_RELEASE(lpTemp);
}

//恢复所有层次关系
void CMap::RestoreLevel(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].Level=n;
		Cell[i].Level2=n;
	}
}

//恢复所有阻挡关系
void CMap::RestoreBlock(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].Block=n;
	}
}

//恢复所有鼠标状态
void CMap::RestoreMouseType(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].MouseType=n;
	}
}
//设置陷阱类型
void CMap::SetCPType(int x,int y,int type)
{
	Cell[y*Width+x].CPType = type;
}
