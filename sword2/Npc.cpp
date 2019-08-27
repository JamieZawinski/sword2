//********************************************
//	Npc角色 相关函数
//  创建于2000年4月7日
//********************************************
#include "gamelib\goldpoint.h"
#include "main.h"
#include "role.h"
#include "map.h"
#include "npc.h"
//注释参看CRole类
//读取角色设定
bool CNpc::LoadNpcIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	//------通用数据----------
	Id=Ini.ReadInt(index,"Id");					//角色编号
	char* strTemp = Ini.ReadText(index, "Name");
	strcpy(Name, strTemp);						//名字
	free(strTemp);
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
	strcpy( ScriptFile, strTemp );
	_FREE(strTemp);

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
	
	//取格子坐标
	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);
	//阻挡关系
	map.Block[map.Width*State.Y+State.X] = 1;

	//行走的页面
	strTemp = Ini.ReadText(index,"SurfaceFileName");
	strcpy(State.SurfaceFileName, strTemp);	//对应文件
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

//*************************
//显示NPC tx,ty=当前地图坐标
void CNpc::Move(int nx, int ny)
{
	//正在行走中……
	if( bMoving)
	{
		int subx= _point_x(Path[PathCurrentStep].x) +(CellWidth>>1);
		int suby= _point_y(Path[PathCurrentStep].y) +(CellHeight>>1);

		if( abs(State.x-subx) <= 2 && abs(State.y-suby) <= 2 )
		{
			//到达下一点
			if(PathCurrentStep<PathSteps-1&&map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width]==1)
			{
				map.Block[ map.Width*State.oldY + State.oldX ] = 0;//上一个点可以通过
				
				if(random(100)<30)				//一定的机会重新寻路
				{								//参看CRole
					int xTemp = Path[PathCurrentStep+1].x;
					int yTemp = Path[PathCurrentStep+1].y;
					map.Cell[xTemp +yTemp*map.Width].Block=1;
					MoveTo(_point_x(DesX),_point_y(DesY));
					map.Cell[xTemp +yTemp*map.Width].Block=0;
				}
				map.Block[ map.Width*ny + nx ] = 1;		//该点不能通过
				State.oldX=State.X;
				State.oldY=State.Y;
				return;
			}
			if(PathCurrentStep<PathSteps-1)
				map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width] = 1;

			map.Block[ map.Width*State.oldY + State.oldX ] = 0;//上一个点可以通过
			map.Block[ map.Width*ny + nx ] = 1;		//该点不能通过
			State.oldX=State.X;
			State.oldY=State.Y;

			if( PathCurrentStep+1==PathSteps )	//目标点
			{
				State.x=subx;
				State.y=suby;
				State.CurrentStep=0;
				bMoving=false;
				MoveX=0;
				MoveY=0;

				return;
			}
			else	//下一点
			{
				int mx=nx - Path[PathCurrentStep+1].x;
				int my=ny - Path[PathCurrentStep+1].y;
				if( mx == 0 && my>0 )	//上
				{
					State.CurrentDir=2;
					MoveX=0;
					MoveY=-StepSize;
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
					State.x=subx;
					State.y=suby;
					State.LastDir=State.CurrentDir;
				}
			}
	
			PathCurrentStep++;	//步数加一
		}

		State.x+=MoveX;
		State.y+=MoveY;
		State.X=_grid_x(State.x);
		State.Y=_grid_y(State.y);

		StepStep++;
		if( StepStep==2 )
		{
			StepStep=0;
			State.CurrentStep=(State.CurrentStep+1)%(Max_Step);
		}
	}
}

//*******************
//显示角色
void CNpc::ShowNpcPic(int nx, int ny, int NumId)
{
	//坐标计算
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*DirList[State.CurrentDir];

	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//显示影子
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*DirList[State.CurrentDir];
	RECT r={x2, y2, x2+State.SW, y2+State.SH};
	ABlt(lpDDSBack, sx, sy+Height-6, State.lpShadow, r, 31, 20);

	//显示人
	RECT bs={ x1, y1, x1+Width, y1+Height};
	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);
	//显示边缘
	static WORD ColorFriend=RGB16(0,255,0);
	if(cx==State.X && cy==State.Y )
	{
		Edge_Blt(lpDDSBack, sx, sy, State.Surface, bs, ColorKey16, ColorFriend);
	}

}
