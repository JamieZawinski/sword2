//********************************************
//	��ͼ ��غ���
//  ������2000��3��14��
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
vectorAniCell CMap::m_vAniCell[4096];	// ����CELL
int CMap::m_nAniCount[4096];			// �����Ľ���
int CMap::m_nAniFrame[4096];			// ������ǰ��

//***************
//��д�����
CMap::CMap()
{
	Cell = NULL;
	strcpy(CurrentMapFileName,"Unknow");	//��ǰ��ͼ�ļ���
	strcpy(CurrentMapFilePath,"Unknow");	//��ǰ��ͼ·��
	m_pBackBmp = NULL;
}

CMap::~CMap()
{
	ClearNpc();
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY(Block);
	_DELETE_ARRAY( AniCell );
}

//���붯̬Ԫ��
void CMap::LoadAniCell(char *file, char *index)
{
	ifstream stream;
	stream.open(file);
	if( stream.is_open() )
	{
		stream >> AniCellNum;		//AniCell������
		for(int i=0; i<AniCellNum; i++)
		{
			m_vAniCell[i].clear();	//���

			long num=0;
			stream >> num;			//��������	
			for(int j=0; j<num; j++)
			{
				stAniCell st;
				stream >> st.page >> st.cell >> st.time;		//ҳ��ͼ�飬ʱ��
				m_vAniCell[i].push_back(st);
			}
		}
		stream.close();
	}

	memset(m_nAniCount, 0, 4096*4);
	memset(m_nAniFrame, 0, 4096*4);
}

//***************
//��ʼ����ͼ
bool CMap::InitMap(int w, int h)
{
	Id=0;
	strcpy(Name,"");
	Width=w+1;
	Height=h+1;
	
	SX=SY=0; //�����������������꣬��ȷ��˵�ǿɼ��������������
	DX=DY=0; //���ɳ���ʱ����������

	CountDisplayGrid();	//������ʾ����

	//��������ڴ�
	_DELETE_ARRAY(Cell);
	_DELETE_ARRAY(Block);
	Block = new bool[Width*Height];
	Cell = new stCell[Width*Height];
	//��ʼ������
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
	//������Ŀ
	Hook_Num=0;

	//��ʼ������
	char *strTemp = GameIni.ReadText("�����ļ�","����");
	ShowNameFont.LoadFont(strTemp, "��ͼ��������");
	_FREE(strTemp);

	//��ʼ���õ�ͼ��Ѱ·�ڵ�����
	CFindPath cfp(&map);
	cfp.Init(Width, Height);

	// ��ʼ����������
	m_nEnemyNum = 0;
	m_nEnemyFrequency = 0;
	ClearEnemyList();

	return true;
}

//***************
//�����ͼ
bool CMap::LoadMap(char *FileName)
{
	FILE *fp;

	fp=fopen(FileName, "rb");
	if( fp==NULL )
	{
		ShowMessage("Load map file error!\n�Ҳ����ļ�:%s",FileName);
		return false;
	}

	fseek(fp, 32, SEEK_SET);		//�����ļ�ͷ
	fread(&Id,4,1,fp);
	fread(Name,32,1,fp);
	fread(&Width,4,1,fp);
	fread(&Height,4,1,fp);

	//��������ڴ�
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY(Block);
	Cell=new stCell[Width*Height];
	Block = new bool[Width*Height];
	fread(Cell, sizeof(stCell), Width*Height, fp);
	//���������б�
	fread(&Hook_Num,4,1,fp);
	fread(Hook, 64, Hook_Num, fp);
	for(int i =0;i<Width*Height;i++)
		Block[i]=0;

	// ��������б�
	m_nEnemyNum = 0;
	m_nEnemyFrequency = 0;
	fread(&m_nEnemyNum, 4, 1, fp);
	fread(m_pEnemyList, 4, 16, fp);
	fread(&m_nEnemyFrequency, 4, 1, fp);

	// ����ͼƬ
	m_strBackBmpFilename[0]=0;
	_RELEASE(map.m_pBackBmp);
	fread(m_strBackBmpFilename, 128, 1, fp);
	if( m_strBackBmpFilename[0] )
	{
		CreateBitmap(map.m_pBackBmp, 0,0, map.m_strBackBmpFilename);
	}

	fclose(fp);

	//���NPC
	ClearNpc();
	
	//��ȡ��ǰ��ͼ�ļ�����·����
	sprintf(CurrentMapFileName, "%s\\%s", CurrentPath, FileName);
	char* str = GetFilePath(CurrentMapFileName);
	strcpy(CurrentMapFilePath, str);
	_FREE( str );

	SX=SY=DX=DY=0;
	CountDisplayGrid();
	ClearEditData();	//��ձ༭���ݣ��������
	role[0].bMoving=false;

	//��ȡ��ǰ��ͼ�ĶԻ��ļ�
	char dialogfile[MAX_PATH];
	sprintf(dialogfile, "%s%s", CurrentMapFilePath, "talk.txt");
	talk.OpenFile( dialogfile );

	//��ʾ��ͼ����������
	ShowNameCount=70;
	ShowNameAlpha=32;

	//��ʼ���õ�ͼ��Ѱ·�ڵ�����
	CFindPath cfp(&map);
	cfp.Init(Width, Height);
	//��ʼ�����
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	return true;
}

//***************
//�����ͼ
bool CMap::SaveMap(char *FileName)
{
	//���������赲
	//Cell[map.Width*role[0].State.Y+role[0].State.X].Block=0;

	//for(int i=0; i<m_vNpc.size(); i++)
	//{
//		Cell[map.Width*m_vNpc[i]->State.Y+m_vNpc[i]->State.X].Block=0;
//	}

	//���ļ�
	FILE *fp;

	fp=fopen(FileName, "wb");
	if( fp==NULL )
	{
		MessageBox(hWnd,"Save map file error!","",MB_OK);
		return false;
	}

	fwrite(MAP_HEAD, 32, 1, fp);	//�ļ�ͷ
	fwrite(&Id,4,1,fp);
	fwrite(Name,32,1,fp);
	fwrite(&Width,4,1,fp);
	fwrite(&Height,4,1,fp);
	fwrite(Cell, sizeof(stCell), Width*Height, fp);

	//���������б�
	fwrite(&Hook_Num,4,1,fp);
	fwrite(Hook, 64, Hook_Num, fp);

	// ��������б�
	fwrite(&m_nEnemyNum, 4, 1, fp);
	fwrite(m_pEnemyList, 4, 16, fp);
	fwrite(&m_nEnemyFrequency, 4, 1, fp);

	fwrite(m_strBackBmpFilename, 128, 1, fp);

	fclose(fp);

	//�ָ�������赲
	//Cell[map.Width*role[0].State.Y+role[0].State.X].Block=1;

	//for(i=0; i<m_vNpc.size(); i++)
//	{
	//	Cell[map.Width*m_vNpc[i]->State.Y+m_vNpc[i]->State.X].Block=1;
//	}
	return true;
}

//***************
//�������
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

	//��������ڴ�
	_DELETE_ARRAY( Cell );
	_DELETE_ARRAY( Block );
	Cell=new stCell[Width*Height];
	Block=new bool[Width*Height];
	fread(Cell, sizeof(stCell), Width*Height, fp);
	//�����б�
	fread(&Hook_Num,4,1,fp);
	fread(Hook, 64, Hook_Num, fp);
	for(int i =0;i<Width*Height;i++)
		Block[i]=0;

	// ��������б�
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

	//��ǰ��ͼ�ļ�����·����
	char str[128];
	memset(str, 0, 128);
	fread(str, 128, 1, fp);
	sprintf(CurrentMapFileName, "%s\\%s", CurrentPath, str);
	
	memset(str, 0, 128);
	fread(str, 128, 1, fp);
	sprintf(CurrentMapFilePath, "%s\\%s", CurrentPath, str);

	//��ȡNPC
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
	
	// ����ͼƬ
	m_strBackBmpFilename[0]=0;
	_RELEASE(map.m_pBackBmp);
	fread(m_strBackBmpFilename, 128, 1, fp);
	if( m_strBackBmpFilename[0] )
	{
		CreateBitmap(map.m_pBackBmp, 0,0, map.m_strBackBmpFilename);
	}
	fclose(fp);


	CountDisplayGrid();

	//��ȡ��ǰ��ͼ�ĶԻ��ļ�
	char dialogfile[MAX_PATH];
	sprintf(dialogfile, "%s%s", CurrentMapFilePath, "talk.txt");
	talk.OpenFile( dialogfile );

	//��ʾ��ͼ����������
	ShowNameCount=70;
	ShowNameAlpha=32;

	//��ʼ���õ�ͼ��Ѱ·�ڵ�����
	CFindPath cfp(&map);
	cfp.Init(Width, Height);
}

//***************
//�������
void CMap::Save(char *FileName)
{
	//���ļ�
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

	//���������б�
	fwrite(&Hook_Num,4,1,fp);
	fwrite(Hook, 64, Hook_Num, fp);

	// ��������б�
	fwrite(&m_nEnemyNum, 4, 1, fp);
	fwrite(m_pEnemyList, 4, 16, fp);
	fwrite(&m_nEnemyFrequency, 4, 1, fp);

	fwrite(&SX,4,1,fp);
	fwrite(&SY,4,1,fp);
	fwrite(&DX,4,1,fp);
	fwrite(&DY,4,1,fp);
	fwrite(&sx,4,1,fp);
	fwrite(&sy,4,1,fp);

	//��ǰ��ͼ�ļ�����·����
	char str[128];
	memset(str, 0, 128);
	strcpy(str, &CurrentMapFileName[strlen(CurrentPath)+1]);
	fwrite(str, 128, 1, fp);
	memset(str, 0, 128);
	strcpy(str, &CurrentMapFilePath[strlen(CurrentPath)+1]);
	fwrite(str, 128, 1, fp);

	//����NPC
	char temp[MAX_PATH];
	long lNpcNum = m_vNpc.size();
	fwrite(&lNpcNum, 4, 1, fp);
	for(int i=0; i<lNpcNum; i++)
	{
		sprintf(temp, "%s%d", FileName, i);
		m_vNpc[i]->SaveRole(temp);
	}

	// ����ͼƬ
	fwrite(m_strBackBmpFilename, 128, 1, fp);
	fclose(fp);
}

// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT(�����)
long CMap::GetAniCellPage(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Ground;		//���
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;	//��ǰҳ
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;	//ͼ��
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT(�����1)
long CMap::GetAniCellPageObj(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Obj;
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT(�����2)
long CMap::GetAniCellPageObj2(const long ground, RECT& rect)
{
	long num = Cell[ ground ].Obj2;
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT(ֱ���ñ��)
long CMap::GetAniPage(const long num, RECT& rect)
{
	long page = m_vAniCell[num][ m_nAniFrame[num] ].page;
	long cell = m_vAniCell[num][ m_nAniFrame[num] ].cell;
	rect=_GetRectangle(cell, ShowCellW);
	return page;
}

// ������һ��
long CMap::AniLoop()
{
	for(int i=0; i<AniCellNum; i++)
	{
		if( ++m_nAniCount[i] > m_vAniCell[i][m_nAniFrame[i]].time )	//��ʱ
		{
			m_nAniCount[i] = 0;
			if( ++m_nAniFrame[i] >= m_vAniCell[i].size() )
			{
				m_nAniFrame[i] = 0;			//��ǰ��
			}
		}
	}
	return true;
}

//*******************
//��ʾ��ͼ��surf��
void CMap::ShowMap(bool active,LPDIRECTDRAWSURFACE surf)
{
	int i,j, cellnum, obj, obj2, level;
	RECT rect = {0,0,0,0};

	// ��ʾ����
	if( m_pBackBmp )
	{
		int w, h;
		GetSurfaceSize(m_pBackBmp, w, h);
		rect.right = w;
		rect.bottom = h;
		BltFast(surf, 0, 0, m_pBackBmp, rect, false);
	}
	//��������λ��
	if( active )
	{
		//�����ƶ�
		role[0].Move(role[0].State.X, role[0].State.Y);
		
		CurrentNpc=-1;
		for(i=0; i<m_vNpc.size(); i++)
		{
			//NPC�ƶ�
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
	
	//������ʾ���� startx\endx\starty\endy
	CountDisplayGrid();

	//��ʾ����
	int ground=Width * starty + startx;
	for(i=starty; i<endy ; i++)
	{
		for(j=startx; j<endx ; j++)
		{
			if( Cell[ ground ].GroundPic < 60 )	//��̬Ԫ��
			{
				//λ�ڵ�ͼҳ���λ�þ���
				rect=_GetRectangle(Cell[ ground ].Ground, ShowCellW);
				//Blt��surf��
				BltFast(surf, DX+_point_x(j-SX), DY+_point_y(i-SY), 
					lpDDSMap[ Cell[ ground ].GroundPic ], rect, true);

				ground++;
			}
			else	// ��̬ͼ��
			{
				RECT rect = {0,0,0,0};
				long page = GetAniCellPage(ground, rect);
				BltFast(surf, DX+_point_x(j-SX), DY+_point_y(i-SY),
					lpDDSMap[ page ], rect, true);
				ground++;
			}
		}
		//��һ��
		ground=ground+Width-(endx-startx);
	}

	//��ʾ����
	for(i=starty; i<endy ; i++)
		for(j=endx-1; j>=startx; j--)
		{
			//TILE����
			cellnum=Width * i + j;
			
			//�������ڵ����ǵ����� level1
			level=Cell[ cellnum ].Level;
			if( level==OL_NORMAL )			//����ͬ��
			{
				obj=Cell[ cellnum ].Obj;	//���
				
				if( obj != BLOCK_CELL )		//���ǿո���
				{
					if( Cell[ cellnum ].ObjPic < 60 )	//��̬Ԫ��
					{
						rect=_GetRectangle(obj, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[Cell[ cellnum ].ObjPic], rect, TRUE);
					}
					else	// ��̬ͼ��
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
			
			//�������ڵ����ǵ����� level2
			level=Cell[ cellnum ].Level2;
			if( level==OL_NORMAL )		//����ͬ��
			{
				obj2=Cell[ cellnum ].Obj2;	//���
				if( obj2 != BLOCK_CELL )	//���ǿյ�
				{
					if( Cell[ cellnum ].Obj2Pic < 60 )	//��̬Ԫ��
					{
						rect=_GetRectangle(obj2, ShowCellW);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY),
							lpDDSMap[Cell[ cellnum ].Obj2Pic], rect, TRUE);
					}
					else	// ��̬ͼ��
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj2(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
		}

	//��ʾNPC
	bool bShowPlayer = false;
	//NPC��y��������ʹ�����NPC��ס�����NPC
	SortNpc();
	for(int n=0; n<m_vSortNpc.size(); n++)
	{
		//����ķ�ʽ��ʾ��ʾ���ǣ�ʵ���ڵ���ϵ
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

	//�������û����ʾ���ǣ�����ʾ���ǡ�ԭ������Ϊ���ǵ�y��������е�NPC����
	if( !bShowPlayer && role[0].State.bShow )
	{
		role[0].Show(j, i);
	}

	for(i=starty; i<endy ; i++)
		for(j=startx; j<endx ; j++)
		{
			//TILE����
			cellnum=Width * i + j;
			
			//�����ڵ����ǵ����� level1
			//һ�����߼�
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
					else	// ��̬ͼ��
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
			
			//�����ڵ����ǵ����� level2
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
					else	// ��̬ͼ��
					{
						RECT rect = {0,0,0,0};
						long page = GetAniCellPageObj2(cellnum, rect);
						Blt(surf, DX+ _point_x(j-SX), DY+ _point_y(i-SY), 
							lpDDSMap[page], rect, TRUE);
					}
				}
			}
		}

	//���ﵽ�������İ�͸����ʾ
	if( role[0].State.Y < map.Height && role[0].State.X < map.Width )
	{
		//�������嶼���ǿյ�
		if( map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj != BLOCK_CELL ||
			map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj2 != BLOCK_CELL )
			//Alpha��ʾ����
			role[0].ShowRole(24);
	}

	//��ʾ��ǰ��ͼ����
	if( ShowNameCount>0 )
	{
		ShowNameCount--;
		if( ShowNameCount > 16 && ShowNameAlpha >0 )	
			ShowNameAlpha-=2;	//����

		if( ShowNameCount < 16 )	
			ShowNameAlpha+=2;	//����
		//��Ӱ��ʾ
		ShowNameFont.SetColor(0);
		ShowNameFont.PrintAlphaText(surf, 17, 15, ShowNameAlpha, Name);

		ShowNameFont.SetColor(RGB(225,255,225));
		ShowNameFont.PrintAlphaText(surf, 15, 13, ShowNameAlpha, Name);
	}

	// ����
	AniLoop();
}

//*******************
//��ʾ�༭ʱ�ĵ�ͼ����ShowMapһ�����߼�
void CMap::ShowEditMap()
{
	int i,j, cellnum, obj, obj2, level, level2, block;
	RECT rect;

	// ��ʾ����
	if( m_pBackBmp )
	{
		int w, h;
		GetSurfaceSize(m_pBackBmp, w, h);
		rect.right = w;
		rect.bottom = h;
		BltFast(lpDDSBack, 0, 0, m_pBackBmp, rect, false);
	}

	//�����ƶ�
	role[0].Move(role[0].State.X, role[0].State.Y);

	CurrentNpc=-1;
	for(i=0; i<m_vNpc.size(); i++)
	{
		//��ȡ��ǰָ��NPC
		if( cx == m_vNpc[i]->State.X && cy == m_vNpc[i]->State.Y )
		{
			CurrentNpc=i;
		}

		//NPC�ƶ�
		if( m_vNpc[i]->State.bActive && !m_vNpc[i]->bMoving )
		{
			if( random(100)>98 )
			{
				m_vNpc[i]->MoveTo(random(Width)*CellWidth, random(Height)*CellHeight);
			}
		}
		m_vNpc[i]->Move(m_vNpc[i]->State.X, m_vNpc[i]->State.Y);
	}
	
	//������ʾ���� startx\endx\starty\endy
	CountDisplayGrid();

	//��ʾ����
	int ground=Width * starty + startx;
	
	if( bShowOnlyObject )	//ֻ��ʾ����
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
				else	// ��̬ͼ��
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
		
		//��ʾ���������
	for(i=starty; i<endy ; i++)
		for(j=endx-1; j>=startx; j--)
		{
			if( bShowObject )	//��ʾ����
			{
				cellnum=Width * i + j;
				
				//�������ڵ����ǵ�����
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
						else	// ��̬ͼ��
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
						else	// ��̬ͼ��
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

	//��ʾNPC
	bool bShowPlayer = false;
	SortNpc();
	for(int n=0; n<m_vSortNpc.size(); n++)
	{
		//��ʾ����
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
			
			if( bShowObject )	//��ʾ����
			{
				obj=Cell[ cellnum ].Obj;
				obj2=Cell[ cellnum ].Obj2;
				
				//�����ڵ����ǵ�����
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
						else	// ��̬ͼ��
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
						else	// ��̬ͼ��
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
			
			if( g_ePlayState == PS_EDIT )	//��ʾ״̬
			{
				//��ʾ�赲��ϵ
				if( bShowBlock && Cell[cellnum].Block==1||Block[cellnum]==1)
					FillCell(j-startx, i-starty, 1);
				
				//��ʾ����
				if( bShowEdit && Cell[cellnum].CP != BLOCK_CP )
				{
					FillCell(j-startx, i-starty, 2);
					font.SetColor(RGB(255,255,0));
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX, _point_y(i-starty)+DY, "%d", Cell[cellnum].CP );
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX+16, _point_y(i-starty)+DY+16, "%d", Cell[cellnum].CPType );
				}
				
				//��ʾ���״̬
				if( bShowEdit && Cell[cellnum].MouseType != CCursor::CS_NORMAL )
				{
					font.SetColor(RGB(255,255,255));
					font.PrintText(lpDDSBack, _point_x(j-startx)+DX, _point_y(i-starty)+DY+16, "%d", Cell[cellnum].MouseType );
				}
				
				//��ʾ������
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

	//���ﵽ�������İ�͸����ʾ
	if( map.Cell[map.Width*role[0].State.Y+role[0].State.X].Obj != BLOCK_CELL )
		role[0].ShowRole(24);

	//��ʾ��ǰ��ͼ����
	if( ShowNameCount>0 )
	{
		ShowNameCount--;
		if( ShowNameCount > 16 && ShowNameAlpha >0 )	ShowNameAlpha-=2;	//����
		if( ShowNameCount < 16 )	ShowNameAlpha+=2;						//����

		ShowNameFont.SetColor(0);
		ShowNameFont.PrintAlphaText(lpDDSBack, 7, 5, ShowNameAlpha, Name);
		ShowNameFont.SetColor(RGB(255,255,255));
		ShowNameFont.PrintAlphaText(lpDDSBack, 5, 3, ShowNameAlpha, Name);
	}

	// ����
	AniLoop();
}

//-----------------------------------------------------------------

//����
void CMap::FadeIn(int Style, int time, WORD color)
{
	if( g_eDisplayState == DS_NIGHT )
		GetPrimarySurface(lpDDSBack);

	//��Ч����
	switch( Style )
	{
	case 0:	//��Ч��
		break;

	case 1:	//��Ҷ��
		ScreenIn(16, time);
		break;
	
	case 2:	//���뵭��
		ColorIn(time, color);
		break;
	}
}

//����
void CMap::FadeOut(int Style, int time)
{
	switch( Style )
	{
	case 0:	//��Ч��
		break;

	case 1:	//��Ҷ��
		map.ShowMap(true);
		if( g_eDisplayState==DS_NIGHT ) 
			Clrscr(lpDDSBackGround, 0);
		//���µ�lpDDSBackGround
		UpdateScreen(lpDDSBackGround, RectScreen);

		ScreenOut(lpDDSBackGround, 16, time);

	case 2:	//���뵭��
		map.ShowMap(true);
		if( g_eDisplayState==DS_NIGHT ) 
			Clrscr(lpDDSBackGround, 0);
		UpdateScreen(lpDDSBackGround, RectScreen);
		ColorOut(lpDDSBackGround, time);
		break;
	}
}

//���õ�ͼ���ĵ�
void CMap::CenterMap(int centx, int centy)
{
	SX=centx-10;		//����10������
	SY=centy-7;			//����7������

	//�߽���
	if( SX >= Width-1-ShowCellW ) 
		SX=Width-1-ShowCellW;

	if( SY >= Height-1-ShowCellH ) 
		SY=Height-1-ShowCellH;

	if( SX<0 ) 
		SX=0;

	if( SY<0 ) 
		SY=0;

	//���㵱ǰ��ͼ���Ͻ�����
	sx=_point_x(SX)-DX;
	sy=_point_y(SY)-DY;
}

//����ͼ��ĳ��
void CMap::ScrollTo(int ox, int oy, int ostep)
{
	ScrollX=ox;
	ScrollY=oy;
	ScrollStep=ostep;
	
	//�߽���
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
	//������ͼ״̬
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

//��ѭ��
void CMap::ScrollLoop()
{
	int ox=ScrollX-SX;
	int oy=ScrollY-SY;
	//����DX���ƶ�������һ�����Ӻ���ƶ�һ�����ӣ�DX��λ
	if( ox > 0 )		//��
	{
		DX-=ScrollStep;
		if( DX <= -32 )
		{
			DX+=32;
			SX++;
		}
	}
	else if( ox < 0 )	//��
	{
		DX+=ScrollStep;
		if( DX >=0 )
		{
			DX-=32;
			SX--;
		}
	}
	else		//SX��λ����������ұ߾���ֹͣ����֮������һ������
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
	//��X����һ��
	if( oy > 0 )	//��
	{
		DY-=ScrollStep;
		if( DY <= -32 )
		{
			DY+=32;
			SY++;
		}
	}
	else if( oy < 0 )	//��
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
	
	//����λ��
	if( ox == 0 && oy == 0 && DX == 0 && DY == 0 )
	{
		g_ePlayState=PS_MAIN;		//����
	}

	ShowMap();		//��ʾ��ͼ
}

//��ӵ�ͼ����
void CMap::AddHook(int x, int y, char *string)
{
	if( Hook_Num==MAX_HOOK )
	{
		ShowMessage("��ͼ������������( Hook_Num=32 )");
		return;
	}

	Cell[Width*y+x].CP=Hook_Num;
	strcpy(Hook[Hook_Num], string);
	Hook_Num++;
}

//���õ�ͼ����
void CMap::SetHook(int x, int y, int num)
{
	if( num>=MAX_HOOK && num != BLOCK_CP )
	{
		ShowMessage("��ͼ���峬�����ֵ( Max Hook_Num=32 )");
		return;
	}

	Cell[Width*y+x].CP=num;
}

//���õ�ͼĳ�����
void CMap::SetCell(int level, int x, int y, int num, int page)
{
	if( level <0 || level >2 )
	{
		ShowMessage("��� %d �������ֵ( MaxLevel=%d )", level, 2);
		return;
	}

	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("����(%d,%d)������Χ(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}

	if( page >= Max_GroundPic )
	{
		ShowMessage("ҳ�� %d �������ֵ( MaxPage=%d )", page, Max_GroundPic-1);
		return;
	}

	int w,h;
	//�õ�ҳ���С
	GetSurfaceSize(lpDDSMap[page], w, h);
	if( num >= (w/32)*(h/32) )
	{
		ShowMessage("��� %d �������ֵ( MaxNum=%d )", num, (w/32)*(h/32)-1);
		return;
	}

	switch(level)
	{
	case 0:	//����
		Cell[Width*y+x].GroundPic=page;
		Cell[Width*y+x].Ground=num;
		break;

	case 1:	//����1
		Cell[Width*y+x].ObjPic=page;
		Cell[Width*y+x].Obj=num;
		break;

	case 2:	//����2
		Cell[Width*y+x].Obj2Pic=page;
		Cell[Width*y+x].Obj2=num;
		break;
	}

}

//���õ�ͼ�赲
void CMap::SetBlock(int x, int y, int block)
{
	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("����(%d,%d)������Χ(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}
	if( block < 0 || block > 1 )
	{
		ShowMessage("block(%d)������Χ(0,1)", block);
		return;
	}
	Cell[Width*y+x].Block=block;
}

//���õ�ͼ���
void CMap::SetLevel(int x, int y, int level1, int level2)
{
	if( x>=Width-1 || y>=Height-1 || x<0 || y< 0 )
	{
		ShowMessage("����(%d,%d)������Χ(0,0 - %d,%d)", x, y, Width-1, Height-1);
		return;
	}
	if( level1 < 0 || level1 > 1 )
	{
		ShowMessage("level1(%d)������Χ(0,1)", level1);
		return;
	}
	if( level2 < 0 || level2 > 1 )
	{
		ShowMessage("level2(%d)������Χ(0,1)", level2);
		return;
	}
	Cell[Width*y+x].Level=level1;
	Cell[Width*y+x].Level2=level2;
}

//����Npc�б��ini�ļ�
bool CMap::LoadIniNpcList(char *filename)
{
	//����ļ�����
	if( !CheckFile( filename ) )
	{
		return true;
	}

	//����NPC
	CIniSet Ini;
	Ini.Open(filename);

	ClearNpc();
	//NPC�����Ŀ
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

//����һ��Npc
int CMap::AddNpc(char *filename, char *index)
{
	//����ļ�����
	if( !CheckFile( filename ) )
	{
		return ERROR_DATA;
	}

	CNpc* p = new CNpc;
	p->LoadNpcIni(filename, index);
	m_vNpc.push_back(p);

	return m_vNpc.size()-1;
}

//ɾ��һ��NPC
int CMap::DelNpc(int num)
{
	if( num >= m_vNpc.size() )
	{
		return ERROR_DATA;
	}

	//�ŵ׷�������赲
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

//��npc���ֻ�ȡID
int CMap::GetNpcId(char *string)
{
	for(int i=0; i<m_vNpc.size(); i++)
	{
		if( stricmp( m_vNpc[i]->Name, string)==0 )
		{
			return i;
		}
	}
	return -1;	//û�ҵ�
}

//����NPC
void CMap::SortNpc()
{
	m_vSortNpc.clear();
	for(int i=0; i<m_vNpc.size(); i++)
	{
		for(itNpc it=m_vSortNpc.begin(); it!=m_vSortNpc.end(); it++)
		{
			CNpc* p = (*it);
			if( m_vNpc[i]->State.y < p->State.y )	//y��������
			{
				m_vSortNpc.insert(it, m_vNpc[i]);
				goto _Next;
			}
		}

		m_vSortNpc.push_back(m_vNpc[i]);
_Next:;
	}
}

//���NPC
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

//������ʾ����
void CMap::CountDisplayGrid()
{
	if( g_eDisplayState == DS_NORMAL )		//��ͨģʽ
	{
		startx=SX;							//ȫ��
		starty=SY;
		endx=startx+ShowCellW+1;
		endy=starty+ShowCellH+1;
	}
	else if( g_eDisplayState == DS_NIGHT )	//ҹ��ģʽ
	{
		startx=role[0].State.X-5;			//��ʾ������Χ��һ����
		endx=startx+11;	
		//�߽���
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

//���һ�����Ե�ĸ���
int CMap::_GetCell(int x, int y, int W)
{
	return _grid_y(y) * W + _grid_x(x);
}

//**********************
//���һ�������ڵĸ���X
int CMap::_GetCellX(int x, int y)
{
	return _grid_x(x);
}

//**********************
//���һ�������ڵĸ���Y
int CMap::_GetCellY(int x, int y)
{
	return _grid_y(y);
}


//**********************
//���һ�������ڵĸ���
int CMap::GetCell(int x, int y)
{
	return Width * (_grid_y(y)+SY) + _grid_x(x) + SX;
}

//**********************
//���һ�������ڵĸ���X
int CMap::GetCellX(int x, int y)
{
	return _grid_x(x)+SX;
}

//**********************
//���һ�������ڵĸ���Y
int CMap::GetCellY(int x, int y)
{
	return _grid_y(y)+SY;
}

//********************
//��ȡһ�����ӵ�����
RECT CMap::GetRectangle(int n)
{
	int x=_point_x(n%Width), y=_point_y(n/Width);
	RECT rect={ x, y, x+CellWidth, y+CellHeight};
	return rect;
}

//********************
//��ȡһ�����ӵľ����������
RECT CMap::_GetRectangle(int n, int W)
{
	int x=_point_x(n%W), y=_point_y(n/W);
	RECT rect={ x, y, x+CellWidth, y+CellHeight};
	return rect;
}

//**********************
//���һ������
void CMap::FillCell(int x, int y, WORD num)
{
	Blt(lpDDSBack, _point_x(x)+DX, _point_y(y)+DY, lpDDSTools, _GetRectangle(num, ShowCellW), true);
}

/////////////////////////////////////////////////////////////////////////
// ���ŵ�ͼ
/////////////////////////////////////////////////////////////////////////

//�����ŵ�ͼ���Ƶ�lpSurf
void CMap::CreateFullMap(LPDIRECTDRAWSURFACE lpSurf, int lSurfWidth, int lSurfHeight)
{
	float LoopX, LoopY;				//Сͼ�����
	int lTileWidth, lTileHeight;	//Сͼ���С
	int x, y;

	int _SX=SX;	//����
	int _SY=SY;
	//��Ҫ������Ļ������ʾ��������ͼ
	LoopX=(float)(Width-1)/20;		//����20������
	LoopY=(float)(Height-1)/15;		//����15������

	lTileWidth = (int)(lSurfWidth/LoopX);
	lTileHeight = (int)(lSurfHeight/LoopY);

	for( int j=0; j<=(int)LoopY; j++ )
	{
		SY = j*15;
		y  = j*lTileHeight;

		//���һ����Ļ��������
		if( j==(int)LoopY )
		{
			SY=Height - 16;			//���ڵ�ͼ����ʾ������Ҫ��ʾһ�����ĵ�ͼ
			int H=((Height)%15);	//ʣ�µĸ߶�
			y=(j)*lTileHeight - lTileHeight*H/15;	//�ص���������
		}

		for( int i=0; i<=(int)LoopX; i++ )
		{
			SX = i*20;
			x  = i*lTileWidth;

			//���һ��Ļ��������
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

//��ʾ���ŵ�ͼ
void CMap::ShowFullMap()
{
	//���
	FillSurface(lpDDSBackGround, RGB16(0,0,0) );

	float LoopX, LoopY;		//Сͼ�����
	int ow, oh;				//Сͼ���С
	int FullWidth, FullHeight;	//����ͼ���ܳߴ�

	LoopX=(float)(Width-1)/20;
	LoopY=(float)(Height-1)/15;

	//�Կ��Ϊ׼
	if( (float)Width/Height >= (float)4/3 ) 
	{
		ow=(int)(ScreenWidth/LoopX);
		oh=(int)(ScreenHeight/LoopX);
		FullWidth=ScreenWidth;
		FullHeight=ScreenWidth*(Height-1)/(Width-1);
	}
	else //�Ը߶�Ϊ׼
	{
		ow=(int)(ScreenWidth/LoopY);
		oh=(int)(ScreenHeight/LoopY);
		FullWidth=ScreenHeight*(Width-1)/(Height-1);
		FullHeight=ScreenHeight;
	}
	CreateFullMap(lpDDSBackGround, FullWidth, FullWidth);
	_UpdateScreen(lpDDSBackGround);

	//�ȴ�����
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
			//��ȡ������Ϣ
			GetInputData();
			if( B_UP || mouse == LB_UP)
			{
				//��ʾ��ͼ������
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
			//������꣬����С�ӿڵ�λ��
			px=point.x-ow/2;
			py=point.y-oh/2;
			//�߽���
			if( px < 0 ) 
				px=0;
			if( py < 0 ) 
				py=0;

			if( px >= FullWidth - ow ) 
				px = FullWidth - ow -2;

			if( py >= FullHeight - oh ) 
				py = FullHeight - oh -2;
			//��ʾ����ͼ
			Blt(lpDDSBack, 0,0, lpDDSBackGround, RectScreen, false);
			//�����ӿھ���
			Rectangle(lpDDSBack, px, py, px+ow, py+oh, RGB16(255,255,255));
			_UpdateScreen();
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
}

// �������ŵ�ͼ
void CMap::SaveFullMap(char* filename, float fPer)
{
	LPDIRECTDRAWSURFACE lpTemp;
	//�������ű��������С
	int w = (int)((Width-1)*32*fPer);
	int h = (int)((Height-1)*32*fPer);
	CreateBitmap(lpTemp, w, h);
	CreateFullMap(lpTemp, w, h);
	//�����BPM�ļ�
	SaveToBitmapFile(lpTemp, filename);
	_RELEASE(lpTemp);
}

//�ָ����в�ι�ϵ
void CMap::RestoreLevel(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].Level=n;
		Cell[i].Level2=n;
	}
}

//�ָ������赲��ϵ
void CMap::RestoreBlock(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].Block=n;
	}
}

//�ָ��������״̬
void CMap::RestoreMouseType(int n)
{
	for(int i=0; i<Width*Height; i++)
	{
		Cell[i].MouseType=n;
	}
}
//������������
void CMap::SetCPType(int x,int y,int type)
{
	Cell[y*Width+x].CPType = type;
}
