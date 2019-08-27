//********************************************
//	角色 相关函数
//  创建于2000年4月14日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "menu\menu_inc.h"
#include "main.h"
#include "map.h"
#include "findpath.h"
#include "talk.h"
#include "message.h"
#include "script.h"
#include "role.h"

//构造
CRole::CRole()
{
	Path=NULL;
	State.Surface=State.lpShadow=NULL;
	bCuted = FALSE;
	bCancelTalk = FALSE;
}

//析构
CRole::~CRole()
{
	_RELEASE( State.Surface );			//行走的页面
	_RELEASE( State.lpShadow );
	_DELETE_ARRAY( Path );
}

//释放
void CRole::Release()
{
	_RELEASE( State.Surface );
	_RELEASE( State.lpShadow );
	_DELETE_ARRAY( Path );
}

//***************
//初始化角色
bool CRole::InitRole()
{
	strcpy(Name,"无名");
	Width=32;
	Height=48;
	Max_Step=4;
	Max_Dir=4;
	//行走序列
	StepList[0]=0;
	StepList[1]=1;
	StepList[2]=0;
	StepList[3]=2;

	DirList[0]=0;
	DirList[1]=1;
	DirList[2]=2;
	DirList[3]=3;
	//位置
	State.x=State.oldx=CenterX;
	State.y=State.oldy=CenterY;

	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);

	map.Block[map.Width*State.Y+State.X] = 1;
	StepSize=8;
	StepSize2=3;
	MoveX=0;
	MoveY=0;
	//脚底碰撞矩形
	FootRect[0].x=-8;		
	FootRect[0].y=-8;
	FootRect[1].x=8;		
	FootRect[1].y=-8;	
	FootRect[2].x=-8;		
	FootRect[2].y=8;
	FootRect[3].x=8;		
	FootRect[3].y=8;

	//页面
	char* strTemp = GameIni.ReadText("角色","Pic");
	strcpy(State.SurfaceFileName, strTemp);
	_FREE(strTemp);
	CreateBitmap(State.Surface, 0, 0, State.SurfaceFileName);
	DDSetColorKey(State.Surface, ColorKey);
	
	//生成角色影子页面
	CreateShadow();

	bMoving=false;
	State.CurrentStep=0;
	State.CurrentDir=2;
	State.LastDir=2;

	//对应脚本
	strcpy(ScriptFile, "script\\null.gps");
	Path=NULL;
	StepStep=0;
	return true;
}

//读取角色设定
bool CRole::LoadRoleIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	//------通用数据----------
	Id=Ini.ReadInt(index,"Id");					//角色编号
	char* strTemp = Ini.ReadText(index, "Name");
	strcpy(Name, strTemp);	//名字
	_FREE(strTemp);
	Width=Ini.ReadInt(index,"Width");
	Height=Ini.ReadInt(index,"Height");			//单张尺寸
	Max_Step=Ini.ReadInt(index,"Max_Step");		//每个方向行走的步数
	Max_Dir=Ini.ReadInt(index,"Max_Dir");		//方向数

	//行走图片变换列表
	char _Steps[32];
	strTemp = Ini.ReadText(index, "StepList");
	strcpy(_Steps, strTemp);
	_FREE(strTemp);
	for(int i=0; i<Max_Step; i++)
	{
		StepList[i]=Ini.ReadInt(_Steps, i);
	}

	//行走方向列表
	strTemp = Ini.ReadText(index, "DirList");
	strcpy(_Steps, strTemp);
	_FREE(strTemp);
	for(i=0; i<4; i++)
	{
		DirList[i]=Ini.ReadInt(_Steps, i);
	}

	//脚底碰撞矩阵
	FootRect[0].x=-8;
	FootRect[0].y=-8;
	FootRect[1].x=8;
	FootRect[1].y=-8;
	FootRect[2].x=-8;
	FootRect[2].y=8;
	FootRect[3].x=8;
	FootRect[3].y=8;

	StepSize=Ini.ReadInt(index,"StepSize");		//每步走的距离
	StepSize2=Ini.ReadInt(index,"StepSize2");	//斜向走的距离

	//对应脚本
	strTemp = Ini.ReadText(index, "Script");
	strcpy( ScriptFile, strTemp);
	_FREE(strTemp);

	//人物的参数结构
	Propertiy.Level=Ini.ReadInt(index,"Level");
	Propertiy.Exp=Ini.ReadInt(index,"Exp");
	Propertiy.MaxExp=Ini.ReadInt(index,"MaxExp");
	Propertiy.Hp=Ini.ReadInt(index,"Hp");
	Propertiy.MaxHp=Ini.ReadInt(index,"MaxHp");
	Propertiy.Mp=Ini.ReadInt(index,"Mp");
	Propertiy.MaxMp=Ini.ReadInt(index,"MaxMp");

	oldAttack = Propertiy.Attack=Ini.ReadInt(index,"Attack");
	oldDefend = Propertiy.Defend=Ini.ReadInt(index,"Defend");
	Propertiy.Magic=Ini.ReadInt(index,"MagicAct");
	
	Propertiy.Stamina=Ini.ReadInt(index,"Stamina");
	Propertiy.Luck=Ini.ReadInt(index,"Luck");
	Propertiy.Speed=Ini.ReadInt(index,"Speed");
	Propertiy.Bp=Ini.ReadInt(index,"Bp");
	Propertiy.Ip=Ini.ReadInt(index,"Ip");

	//金钱
	Propertiy.Money=Ini.ReadInt(index,"Money");
	
	//随身物品
	char _Goods[32];
	strTemp = Ini.ReadText(index,"Goods");
	strcpy(_Goods, strTemp );
	_FREE(strTemp);
	Propertiy.CurGoodsNum=Ini.ReadInt(_Goods, "Max_Good");
	for(i=0; i<Propertiy.CurGoodsNum; i++)
	{
		char* str = Ini.ReadData(_Goods, i);
		Propertiy.Goods[i]=atoi(str);
		_DELETE(str);
		Propertiy.GoodsNum[i]=Ini.ReadInt(_Goods, i);
	}

	//装备
	char _Arm[32];
	strTemp = Ini.ReadText(index,"Arm");
	strcpy(_Arm, strTemp );
	_FREE(strTemp);
	Propertiy.CurArmNum=Ini.ReadInt(_Arm, "Max_Arm");
	for(i=0; i<Propertiy.CurArmNum; i++)
	{
		Propertiy.Arm[i]=atoi(Ini.ReadData(_Arm, i));
		Propertiy.ArmNum[i]=Ini.ReadInt(_Arm, i);
	}

	//魔法
	strTemp = Ini.ReadText(index,"Magics");
	strcpy(_Goods, strTemp );
	_FREE(strTemp);
	Propertiy.MagicsNum=Ini.ReadInt(_Goods, "Max_Magics");
	for(i=0; i<Propertiy.MagicsNum; i++)
	{
		Propertiy.Magics[i]=Ini.ReadInt(_Goods, i);
	}

	Propertiy.Arms=Ini.ReadInt(index, "Arms");
	SetupArm(Propertiy.Arms);
	Propertiy.Corselet=Ini.ReadInt(index, "Corselet");
	SetupArm(Propertiy.Corselet);

	//战斗
	strcpy(Propertiy.Fight_Ini_Name, Name);

	//------状态数据----------
	State.x=Ini.ReadInt(index,"x");			//当前坐标 脚底中心点
	State.y=Ini.ReadInt(index,"y");
	State.oldx=Ini.ReadInt(index,"oldx");
	State.oldy=Ini.ReadInt(index,"oldy");				//上一步的坐标
	State.CurrentStep=Ini.ReadInt(index,"CurrentStep");	//当前步数
	State.CurrentDir=Ini.ReadInt(index,"CurrentDir");	//当前方向
	State.LastDir=Ini.ReadInt(index,"LastDir");			//上一步的方向
	State.bActive=Ini.ReadInt(index,"bActive");			//是否活动
	State.bShow=Ini.ReadInt(index,"bShow");				//是否可见
	
	//取主角的格子坐标
	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);

	map.Block[map.Width*State.Y+State.X] = 1;
	//行走的页面
	strTemp = Ini.ReadText(index,"SurfaceFileName");
	strcpy(State.SurfaceFileName, strTemp );	//对应文件
	_FREE(strTemp);
	CreateBitmap(State.Surface, 0, 0, State.SurfaceFileName);
	DDSetColorKey(State.Surface, ColorKey);
	
	//生成角色影子页面
	CreateShadow();

	//静止
	bMoving=false;
	Path=NULL;
	StepStep=0;

	return true;
}

//***************
//生成人物阴影
bool CRole::CreateShadow()
{
	int m,n;

	int W,H;
	LoadBitmapFileInfo(hWnd, State.SurfaceFileName, W, H);
	int nRows = W / Width;
	int nLines = H / Height;
	int W2=W;			
	int H2=H/4;			
	State.SW=W2/nRows;	//宽度不变
	State.SH=Height/4;	//高度为1/4

	//创建角色影子页面
	CreateBitmap(State.lpShadow, W2, H2, NULL, DDSCAPS_SYSTEMMEMORY);
	DDSetColorKey(State.lpShadow, ColorKey);
	FillSurface(State.lpShadow, ColorKey16);

	LPDIRECTDRAWSURFACE lpTemp;
	CreateBitmap(lpTemp, State.SW, State.SH, NULL, DDSCAPS_SYSTEMMEMORY);

	// 分块压扁
	for(m=0; m<nLines; m++)
		for(n=0; n<nRows; n++)
		{
			RECT rt={n*State.SW, m*State.SH, n*State.SW+State.SW, (m+1)*State.SH};
			RECT rs={n*Width, m*Height, (n+1)*Width, (m+1)*Height};
			SBlt(State.lpShadow, rt, State.Surface, rs, false);
		}

	// 锁定表面
	WORD* pShadow;
	WORD* pTemp;
	if( BeginDraw(State.lpShadow) )
	{
		pShadow = (WORD*)GraphBuffer;
		EndDraw(State.lpShadow);
	}
	if( BeginDraw(lpTemp) )
	{
		pTemp = (WORD*)GraphBuffer;
		EndDraw(lpTemp);
	}

	int i,j;
	for(m=0; m<nLines; m++)
		for(n=0; n<nRows; n++)
		{
			//清色处理
			for(i=0; i<State.SH; i++)
				for(j=0; j<State.SW; j++)
				{
					int p=(i+m*State.SH)*W2+(j+n*State.SW);
					if( pShadow[p] != ColorKey16 )
					{
						pShadow[p] = RGB16(0, 0, 0);		//全部做成黑色
					}
					else
					{
						pShadow[p] = 31;		//0x001f，蓝色五位都为1，将来显示的透明色
					}
				}
				
				//上下反转
				Blt(lpTemp, 0, 0, State.lpShadow, 
					GetRect(n*State.SW, m*State.SH, n*State.SW+State.SW, m*State.SH+State.SH)
					, false);

				for(i=0; i<State.SW; i++)
					for(j=0; j<State.SH; j++)
					{
						int p1=(j+m*State.SH)*W2+(i+n*State.SW);
						int p2=(State.SH-1-j)*State.SW+i;
						pShadow[p1] = pTemp[p2];
					}
		}

	_RELEASE(lpTemp);

	return true;
}

//***************
//读入进度时候
bool CRole::LoadRole(char *FileName)
{
	FILE *fp;

	fp=fopen(FileName, "rb");
	if( fp==NULL )
	{
		ShowMessage("Can't open file <%s>!",FileName);
		return false;
	}

	fread(&Id, 4, 1, fp);
	fread(Name,32,1,fp);
	fread(&Width,4,1,fp);
	fread(&Height,4,1,fp);
	fread(&Max_Step, sizeof(byte), 1, fp);
	fread(&Max_Dir, sizeof(byte), 1, fp);
	fread(StepList, Max_Step, sizeof(byte), fp);
	fread(DirList, 4, 1, fp);
	fread(FootRect, sizeof(POINT), 4, fp);
	fread(&StepSize, sizeof(int), 1, fp);
	fread(&StepSize2, sizeof(int), 1, fp);

	fread(ScriptFile, 64, 1, fp);
	fread(&Propertiy, sizeof(stRolePropertiy), 1, fp);
	fread(&State, sizeof(stRoleState), 1, fp);

	CreateBitmap(State.Surface, 0, 0, State.SurfaceFileName);
	DDSetColorKey(State.Surface, ColorKey);
	CreateShadow();		//生成角色影子页面


	//------移动数据-----------
	fread(&PathSteps, 4, 1, fp);
	fread(&PathCurrentStep, 4, 1, fp);
	fread(&DesX, sizeof(int), 1, fp);
	fread(&DesY, sizeof(int), 1, fp);
	fread(&bMoving, 1, 1, fp);
	fread(&StepStep, sizeof(int), 1, fp);
	fread(&MoveX, sizeof(int), 1, fp);
	fread(&MoveY, sizeof(int), 1, fp);
	fread(&bShowed, 1, 1, fp);
	fread(&oldAttack,sizeof(int),1,fp);
	fread(&oldDefend,sizeof(int),1,fp);

	if( PathSteps > 0 && bMoving )
	{
		_DELETE_ARRAY( Path );
		Path=new POINT[PathSteps];
		fread(Path, sizeof(POINT) , PathSteps, fp);
	}
	else 
		Path=NULL;

	fclose(fp);

	return true;
}

//***************
//保存进度的时候
bool CRole::SaveRole(char *FileName)
{
	FILE *fp;

	fp=fopen(FileName, "wb");
	if( fp==NULL )
	{
		MessageBox(hWnd,"save map file error!","",MB_OK);
		return false;
	}

	fwrite(&Id, 4, 1, fp);
	fwrite(Name,32,1,fp);
	fwrite(&Width,4,1,fp);
	fwrite(&Height,4,1,fp);
	fwrite(&Max_Step, sizeof(byte), 1, fp);
	fwrite(&Max_Dir, sizeof(byte), 1, fp);
	fwrite(StepList, Max_Step, sizeof(byte), fp);
	fwrite(DirList, 4, 1, fp);
	fwrite(FootRect, sizeof(POINT), 4, fp);
	fwrite(&StepSize, sizeof(int), 1, fp);
	fwrite(&StepSize2, sizeof(int), 1, fp);

	fwrite(ScriptFile, 64, 1, fp);
	fwrite(&Propertiy, sizeof(stRolePropertiy), 1, fp);
	fwrite(&State, sizeof(stRoleState), 1, fp);
	
	//------移动数据-----------
	fwrite(&PathSteps, 4, 1, fp);
	fwrite(&PathCurrentStep, 4, 1, fp);
	fwrite(&DesX, sizeof(int), 1, fp);
	fwrite(&DesY, sizeof(int), 1, fp);
	fwrite(&bMoving, 1, 1, fp);
	fwrite(&StepStep, sizeof(int), 1, fp);
	fwrite(&MoveX, sizeof(int), 1, fp);
	fwrite(&MoveY, sizeof(int), 1, fp);
	fwrite(&bShowed, 1, 1, fp);
	fwrite(&oldAttack,sizeof(int),1,fp);
	fwrite(&oldDefend,sizeof(int),1,fp);

	if( PathSteps > 0 && bMoving ) 
		fwrite(Path, sizeof(POINT) , PathSteps, fp);

	fclose(fp);
	return true;
}

//*******************
//显示角色
void CRole::ShowRole()
{
	//坐标计算

	//人物在图片上的位置
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*State.CurrentDir;

	//显示人在地图上的位置

	//脚底中心x坐标向左半个人宽
	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;	
	//脚底中心y坐标向上一个人高，再加上脚的高度
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//显示影子
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*State.CurrentDir;

	RECT r={x2, y2, x2+State.SW, y2+State.SH};
	//用0x001f为透明色，看看产生影子的函数
	//缩减一点点，因为影子有部分是在脚下的
	ABlt(lpDDSBack, sx, sy+Height-6, State.lpShadow, r, 31, 20);

	//显示人
	RECT bs={ x1, y1, x1+Width, y1+Height};

	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);

}

//*******************
//半透明显示角色
void CRole::ShowRole(int alpha)
{
	//坐标计算
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*State.CurrentDir;
	//一样的方法
	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//显示人
	RECT bs={ x1, y1, x1+Width, y1+Height};

	ABlt(lpDDSBack, sx, sy, State.Surface, bs, ColorKey16, alpha);
}

//*******************
//显示角色
void CRole::ShowRole(int lx, int ly, int lDir, int lStep)
{
	//坐标计算
	int x1=Width*StepList[lStep];
	int y1=Height*lDir;
	//…………
	int sx=lx-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=ly-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//显示影子
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*State.CurrentDir;
	RECT r={x2, y2, x2+State.SW, y2+State.SH};

	ABlt(lpDDSBack, sx-16, sy+Height-4, State.lpShadow, r, 31, 20);

	//显示人
	RECT bs={ x1, y1, x1+Width, y1+Height};
	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);
}

//*************************
//显示 tx,ty=当前地图坐标
void CRole::Show(int tx, int ty)
{
	ShowRole();
}

//*******************************
//移动，参数是当前位置，相对与地图格子坐标的
void CRole::Move(int nx, int ny)
{
	//正在行走中……
	if( bMoving )
	{
		//下一个点的坐标，在地图格子的中心
		int subx= _point_x(Path[PathCurrentStep].x) +(CellWidth>>1);
		int suby= _point_y(Path[PathCurrentStep].y) +(CellHeight>>1);

		//到达下一点
		if( abs(State.x-subx) <= 2 && abs(State.y-suby) <= 2 )
		{
			if(PathCurrentStep<PathSteps-1&&map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width]==1)
			{
				int xTemp = Path[PathCurrentStep+1].x;		//下一个点
				int yTemp = Path[PathCurrentStep+1].y;
				map.Cell[xTemp +yTemp*map.Width].Block=1;	//不能通过（用于寻路）
				MoveTo(_point_x(DesX),_point_y(DesY));
				map.Cell[xTemp +yTemp*map.Width].Block=0;	//恢复

				map.Block[ map.Width*State.oldY + State.oldX ] = 0;//上一个点可以通过
				map.Block[ map.Width*ny + nx ] = 1;		//该点不能通过
				return;
			}
			if(PathCurrentStep<PathSteps-1)
				map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width] = 1;

			map.Block[ map.Width*State.oldY + State.oldX ] = 0;//上一个点可以通过
			map.Block[ map.Width*ny + nx ] = 1;		//该点不能通过
			if( PathCurrentStep+1==PathSteps )	//目标点
			{
				State.x=subx;	//更新当前脚底坐标
				State.y=suby;
				
				static bool left=true;
				if( left )	//先出左脚
				{
					left=!left;
					State.CurrentStep=0;	//设置人物的停止状态，保证两个脚并在一起
				}
				else		//先出右脚
				{
					left=!left;
					State.CurrentStep=2;
				}

				bMoving=false;				//停止移动
				MoveX=0;					//坐标变化为0
				MoveY=0;

				

				//如果目标点不能行走的话，那么主角转向目标点
				if(	/*map.Cell[map.Width*DesY+DesX].Block == 1
					&&*/ ((DesX!=State.X)
					|| (DesY!=State.Y))&&!bCuted)
				{
					if( DesX > State.X ) 
						State.CurrentDir = 3;		//向右转
					else 
						if( DesX < State.X ) 
							State.CurrentDir = 1;	//向左转
					if( DesY > State.Y ) 
						State.CurrentDir = 0;		//向下转
					else 
						if( DesY < State.Y ) 
							State.CurrentDir = 2;	//向上转
					RunScript();
				}

				State.oldX=State.X;
				State.oldY=State.Y;
				
				//判断当前点是否为场景切换点
				ChangeScene();
				return;
			}
			else	//到下一点
			{
				//保存上个位置
				State.oldX=State.X;
				State.oldY=State.Y;
				//需要移动的距离
				int mx=nx - Path[PathCurrentStep+1].x;
				int my=ny - Path[PathCurrentStep+1].y;
				if( mx == 0 && my>0 )		//上
				{
					State.CurrentDir=2;
					MoveX=0;				//x方向不需要移动
					MoveY=-StepSize;		//y方向移动一步的距离
				}
				else if( mx<0 && my==0 )	//右
				{
					State.CurrentDir=3;
					MoveX=StepSize;
					MoveY=0;
				}
				else if( mx==0 && my<0 )	//下
				{
					State.CurrentDir=0;
					MoveX=0;
					MoveY=StepSize;
				}
				else if( mx>0 && my==0 )	//左
				{
					State.CurrentDir=1;
					MoveX=-StepSize;
					MoveY=0;
				}

				//如果改变了方向就对齐中心点
				if( State.CurrentDir!=State.LastDir )
				{
					State.x=subx;		//直接更新
					State.y=suby;
					State.LastDir=State.CurrentDir;	//保存方向
				}
			}
			PathCurrentStep++;	//步数加一
			RunScript();
			if(PathCurrentStep>1)
				ChangeScene();
		}
		//滚屏
		switch( State.CurrentDir )
		{
		case 1:	//左：
				//两种情况：(1)地图比屏幕大,并且人物走到了中心点的左边，而且这时地图的X点比0
				//大(左边还有部分地图没有显示出来)；(2)显示地图到头了，但还是有部分没有显示出来
			if( State.x <= _point_x(map.Width-ShowCellW)+CenterX && (map.SX > 0 
								|| (map.SX==0 && map.DX<0) ) )
			{
				map.DX+=StepSize;		//移动人物的一步距离
				if( map.DX>0 )			//已经移动了一整个格子了
				{
					map.SX--;			//再向左一个格子
					map.DX-=CellWidth;	//恢复DX的值
				}
			}
			break;

		case 3:		//右,同样的逻辑
			if( State.x >= CenterX && ( map.SX < map.Width-ShowCellW-1 
				|| (map.SX==map.Width-ShowCellW-2 && map.DX>=-CellWidth)) )
			{
				map.DX-=StepSize;
				if( map.DX <= -CellWidth )
				{
					map.SX++;
					map.DX+=CellWidth;
				}
			}
			break;

		case 2:		//上,……
			if( State.y <= _point_y(map.Height-ShowCellH)+CenterY &&( map.SY > 0 
				|| (map.SY==0 && map.DY<0) ) )
			{
				map.DY+=StepSize;
				if( map.DY>0 )
				{
					map.SY--;
					map.DY-=CellHeight;
				}
			}
			break;
		
		case 0:		//下，不想说了：）
			if( State.y >= CenterY && ( map.SY < map.Height-ShowCellH-1 
				|| (map.SY==map.Height-ShowCellH-2 && map.DY>=-CellHeight)) )
			{
				map.DY-=StepSize;
				if( map.DY <= -CellHeight )
				{
					map.SY++;
					map.DY+=CellHeight;
				}
			}
			break;
		}

		State.x+=MoveX;		//脚底位置更新
		State.y+=MoveY;
		role[0].State.X=_grid_x(role[0].State.x);
		role[0].State.Y=_grid_y(role[0].State.y);

		StepStep++;
		if( StepStep==2 )
		{
			StepStep=0;			//2步一次
			State.CurrentStep=(State.CurrentStep+1)%(Max_Step);
		}
	}
}

//*******************************
//移动到某点
bool CRole::MoveTo(int lx, int ly)
{
	CFindPath fp(&map);
	fp.Init(map.Width,map.Height);
	int Sx=State.X;
	int Sy=State.Y;

	//边界检查
	if( lx < 0 ) 
		lx=0;
	else 
		if( lx >= _point_x(map.Width-1) ) 
			lx=_point_x(map.Width-1)-1;

	if( ly < 0 ) 
		ly=0;
	else 
		if( ly >= _point_y(map.Height-1) ) 
			ly=_point_y(map.Height-1)-1;

	//目的点坐标
	DesX=_grid_x(lx);
	DesY=_grid_y(ly);

	//如果正在移动就把下一点做为起始点
	if( bMoving )
	{
		Sx=Path[PathCurrentStep].x;
		Sy=Path[PathCurrentStep].y;
		::map.Block[Sx+Sy*::map.Width]=1;
	}
	//寻路成功
	if( fp.Find(Sx, Sy, DesX, DesY) )
	{
		_DELETE_ARRAY( Path );
		Path=new POINT[fp.TheSteps];
		memcpy(Path, fp.ThePath, sizeof(POINT)*fp.TheSteps);
		PathSteps=fp.TheSteps;
		PathCurrentStep=0;
		bMoving=true;
		bCuted = FALSE;
		bCancelTalk = false;
	}
	return false;
}

//*******************************
//移动到某点
void CRole::Goto(int lx, int ly)
{
	//寻路
	MoveTo(lx, ly);

	//菜单循环
	MSG msg;
	while(bMoving)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) 
				return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else 
			if(bActive)
		{
			//获取输入信息
			GetInputData();
			
			//延时控速
			while( timeGetTime() < unsigned int(OldTick+DelayTick) ) {}
			OldTick=timeGetTime();

			//取主角的格子坐标
			State.X=_grid_x(State.x);
			State.Y=_grid_y(State.y);

			//计算当前地图左上角坐标
			map.sx=_point_x(map.SX)-map.DX;
			map.sy=_point_y(map.SY)-map.DY;

			map.ShowMap();
			UpdateScreen();

			Mouse.GetMouseData();		//获取鼠标立即信息
			Key.GetKeyData();			//获取键盘缓冲数据(附带即时数据)
			Key.GetKey();

			ShowMouseCursor(lpDDSBack, false);
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();
		}
		else WaitMessage();	//等待消息
	}
	bMoving=false;
}

//**************************
//场景切换
void CRole::ChangeScene()
{
	//得到人物的所在的格子坐标
	int nx=map._GetCellX(State.x,State.y);
	int ny=map._GetCellY(State.x,State.y);

	//切换
	int cp=map.Cell[map.Width*ny+nx].CP;
	int cptype = map.Cell[map.Width*ny+nx].CPType;
	if( cp != BLOCK_CP&&cptype)				//切换点
	{
		char GpsFile[MAX_PATH];
		memset(GpsFile,0, MAX_PATH);
		sprintf(GpsFile,"%s%s", map.CurrentMapFilePath, map.Hook[cp]);
		Script.Run( GpsFile );	//执行切换脚本
	}
}

//**************************
//触发角色前一个格子中的开关
void CRole::RunScript()
{
	int lx, ly;
	switch( State.CurrentDir )
	{
	case 0:				//下
		lx=State.x;
		ly=State.y+32;
		break;

	case 1:				//左
		lx=State.x-32;
		ly=State.y;
		break;

	case 2:				//上
		lx=State.x;		
		ly=State.y-32;
		break;

	case 3:				//右
		lx=State.x+32;	
		ly=State.y;
		break;
	}

	int nx=map._GetCellX(lx,ly);
	int ny=map._GetCellY(lx,ly);
	//越界
	if( nx < 0 || ny < 0 ) 
		return;

	//切换
	int cp=map.Cell[map.Width*ny+nx].CP;
	int cptype = map.Cell[map.Width*ny+nx].CPType;
	if( cp != BLOCK_CP&&cptype!=CP_CHANGE )			//陷阱要是个非切换型
	{
		char GpsFile[MAX_PATH];
		memset(GpsFile,0, MAX_PATH);
		sprintf(GpsFile,"%s%s", map.CurrentMapFilePath, map.Hook[cp]);
		AdjustPosition();		//调整位置
		Message.PutMessage(MSG_BREAK);		//本次不运行
		Script.Run( GpsFile );	//执行切换脚本
	}
}

//-----------------------------------------------------------

//加入物体
void CRole::AddObject(char *name, int num)
{
	int goods=CGoods::GetGoodsNum(name);

	if(goods==-1)
		return;
	//如果是装备
	if( CMenu::Goods.GOODS[goods].Class >=0 
		&& CMenu::Goods.GOODS[goods].Class <=1 )
	{
		AddArm(name, num);
		return;
	}

	//已经有了就添加
	for(int i=0; i<Propertiy.CurGoodsNum; i++)
	{
		if( Propertiy.Goods[i] == goods )
		{
			Propertiy.GoodsNum[i]+=num;
			return;
		}
	}

	//新物体
	Propertiy.Goods[Propertiy.CurGoodsNum]=goods;
	Propertiy.GoodsNum[Propertiy.CurGoodsNum]=num;
	Propertiy.CurGoodsNum++;
}

//删除物体
void CRole::DelObject(char *name, int num)
{
	int goods=CGoods::GetGoodsNum(name);

	//如果是装备
	if( CMenu::Goods.GOODS[goods].Class >=0 
		&& CMenu::Goods.GOODS[goods].Class <=1 )
	{
		DelArm(name, num);
		return;
	}
	//是物品
	if( goods != 0 )
	{
		for(int i=0; i<Propertiy.CurGoodsNum; i++)
		{
			if( Propertiy.Goods[i] == goods )
			{
				Propertiy.GoodsNum[i]-=num;

				//把后面的物品向前移动一格
				if( Propertiy.GoodsNum[i] <= 0 )
				{
					int tmp;

					Propertiy.CurGoodsNum--;
					for(int j=i; j<Propertiy.CurGoodsNum; j++)
					{
						tmp=Propertiy.Goods[j+1];
						Propertiy.Goods[j]=tmp;

						tmp=Propertiy.GoodsNum[j+1];
						Propertiy.GoodsNum[j]=tmp;
					}
				}
				return;
			}
		}
	}
}

//检测一个物品的数量
int CRole::CheckObject(char *name)
{
	int goods=CGoods::GetGoodsNum(name);

	//如果是装备
	if( CMenu::Goods.GOODS[goods].Class >=0 
		&& CMenu::Goods.GOODS[goods].Class <=1 )
	{
		return CheckArm(name);
	}

	if( goods != 0 )
	{
		for(int i=0; i<Propertiy.CurGoodsNum; i++)
		{
			if( Propertiy.Goods[i] == goods )
			{
				return Propertiy.GoodsNum[i];
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------

//加入装备
void CRole::AddArm(char *name, int num)
{
	int arm=CGoods::GetGoodsNum(name);

	//添加
	for(int i=0; i<Propertiy.CurArmNum; i++)
	{
		if( Propertiy.Arm[i] == arm )
		{
			Propertiy.ArmNum[i]+=num;
			return;
		}
	}

	//新装备
	Propertiy.Arm[Propertiy.CurArmNum]=arm;
	Propertiy.ArmNum[Propertiy.CurArmNum]=num;
	Propertiy.CurArmNum++;
}

//删除物体
void CRole::DelArm(char *name, int num)
{
	int arm=CGoods::GetGoodsNum(name);
	if( arm != 0 )
	{
		for(int i=0; i<Propertiy.CurArmNum; i++)
		{
			if( Propertiy.Arm[i] == arm )
			{
				Propertiy.ArmNum[i]-=num;

				//把后面的物品向前移动一格
				if( Propertiy.ArmNum[i] <= 0 )
				{
					int tmp;

					Propertiy.CurArmNum--;
					for(int j=i; j<Propertiy.CurArmNum; j++)
					{
						tmp=Propertiy.Arm[j+1];
						Propertiy.Arm[j]=tmp;

						tmp=Propertiy.ArmNum[j+1];
						Propertiy.ArmNum[j]=tmp;
					}
				}
				return;
			}
		}

		// 如果是已经装备了的
		for(i=0; i<RoleNum; i++)
		{
			//如果是武器
			if( role[i].Propertiy.Arms == arm )
			{
				role[i].Propertiy.Arms = 0;	//卸除装备
				role[i].UnSetupArm(arm);
				return;
			}

			//如果是护具
			else if( role[i].Propertiy.Corselet == arm )
			{
				role[i].Propertiy.Corselet = 0;
				role[i].UnSetupArm(arm);
				return;
			}
		}
	}
}

// 检测一个装备的数量
int CRole::CheckArm(char *name)
{
	int num=0;
	int arm=CGoods::GetGoodsNum(name);
	if( arm != 0 )
	{
		for(int i=0; i<Propertiy.CurArmNum; i++)
		{
			if( Propertiy.Arm[i] == arm )
			{
				num = Propertiy.ArmNum[i];
			}
		}

		// 检查已经装备在身上了的
		for(i=0; i<RoleNum; i++)
		{
			if( role[i].Propertiy.Arms == arm )
			{
				num ++;
			}

			if( role[i].Propertiy.Corselet == arm )
			{
				num ++;
			}
		}
	}

	return num;
}

//装备
void CRole::SetupArm(char *name)
{
	int GoodNum=CGoods::GetGoodsNum(name);
	SetupArm(GoodNum);
}

//装备
void CRole::SetupArm(int GoodNum)
{
	Propertiy.Magic+=CMenu::Goods.GOODS[GoodNum].Magic;			//魔法

	Propertiy.Stamina+=CMenu::Goods.GOODS[GoodNum].Stamina;	//耐力
	Propertiy.Luck+=CMenu::Goods.GOODS[GoodNum].Luck;		//幸运
	Propertiy.Speed+=CMenu::Goods.GOODS[GoodNum].Speed;		//敏捷
	Propertiy.Bp+=CMenu::Goods.GOODS[GoodNum].Bp;		//体力
	Propertiy.Ip+=CMenu::Goods.GOODS[GoodNum].Ip;		//精神

	ReCalcPro();
}

//卸除装备
void CRole::UnSetupArm(char *name)
{
	int GoodNum=CGoods::GetGoodsNum(name);
	UnSetupArm(GoodNum);
}
//卸除装备
void CRole::UnSetupArm(int GoodNum)
{
	Propertiy.Magic-=CMenu::Goods.GOODS[GoodNum].Magic;			//魔法
	Propertiy.Stamina-=CMenu::Goods.GOODS[GoodNum].Stamina;	//耐力
	Propertiy.Luck-=CMenu::Goods.GOODS[GoodNum].Luck;		//幸运
	Propertiy.Speed-=CMenu::Goods.GOODS[GoodNum].Speed;		//敏捷
	Propertiy.Bp-=CMenu::Goods.GOODS[GoodNum].Bp;			//体力
	Propertiy.Ip-=CMenu::Goods.GOODS[GoodNum].Ip;			//精神

	ReCalcPro();
}

//调整位置
void CRole::AdjustPosition()
{
	bCancelTalk = true;		//取消很NPC的对话
	if(!bMoving)	//没有移动
		return ;
	PathSteps = PathCurrentStep+1;	//下一步是最后一步
	bCuted = TRUE;			//路径被减切
}
//加入一个魔法
void CRole::AddMagic(char *name)
{
	//得到魔法编号
	int magic = CMagic::GetMagicNum(name);

	if(magic==-1)
		return;

	for(int i=0; i<Propertiy.MagicsNum; i++)
	{
		if( Propertiy.Magics[i] == magic )//有了这个魔法
		{
			return;
		}
	}
	//没有学会的魔法
	Propertiy.Magics[Propertiy.MagicsNum]=magic;
	Propertiy.MagicsNum++;
}
//删除魔法
void CRole::DelMagic(char *name)
{
	// 魔法编号
	int magic=CMagic::GetMagicNum(name);
	// 是魔法
	if( magic >-1  )
	{
		for(int i=0; i<Propertiy.MagicsNum; i++)
		{
			if( Propertiy.Magics[i] == magic )
			{
				//把后面的魔法向前移动一格
				
				int tmp;
				
				Propertiy.MagicsNum--;
				for(int j=i; j<Propertiy.MagicsNum; j++)
				{
					tmp=Propertiy.Magics[j+1];
					Propertiy.Magics[j]=tmp;
				}
				return;
			}
		}
	}
}
//重新计算属性
void CRole::ReCalcPro()
{
	Propertiy.Attack = oldAttack;
	Propertiy.Defend = oldDefend;

	int GoodNum = Propertiy.Arms;
	Propertiy.Attack+=CMenu::Goods.GOODS[GoodNum].Attack;		//攻击
	Propertiy.Attack = (int)Propertiy.Attack*(100.0+CMenu::Goods.GOODS[GoodNum].AttackPer)/100.0;
	
	Propertiy.Defend+=CMenu::Goods.GOODS[GoodNum].Defend;		//防御
	Propertiy.Defend = (int)Propertiy.Defend*(100.0+CMenu::Goods.GOODS[GoodNum].DefendPer)/100.0;

	GoodNum = Propertiy.Corselet;
	Propertiy.Attack+=CMenu::Goods.GOODS[GoodNum].Attack;		//攻击
	Propertiy.Attack = (int)Propertiy.Attack*(100.0+CMenu::Goods.GOODS[GoodNum].AttackPer)/100.0;
	
	Propertiy.Defend+=CMenu::Goods.GOODS[GoodNum].Defend;		//防御
	Propertiy.Defend = (int)Propertiy.Defend*(100.0+CMenu::Goods.GOODS[GoodNum].DefendPer)/100.0;
}
