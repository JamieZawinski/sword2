//********************************************
//	地图编辑 相关函数
//  创建于2000年4月日10日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "..\resource.h"
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "mapedit.h"
#include "dialog.h"

///////////////////////////////////
//场景编辑器专用变量
///////////////////////////////////
int cx2, cy2;				//临时鼠标格子

bool bShowBackGround=true;	//显示背景
bool bShowCell=true;		//显示表格
bool bShowObject=true;		//显示物体
bool bShowBlock=true;		//显示阻挡关系
bool bShowCurrent=true;		//显示当前物体
bool bShowEdit=true;		//显示编辑提示（物体、切换点、方格）
bool bShowOnlyObject=false;	//显示物体
bool bFillCell=false;		//是否连续填充
bool bShowHardCursor=false;	//是否显示硬件光标
bool bShowFps=false;		//是否显示FPS
bool bMenuPop=false;		//菜单是否存在
bool bShowEditInfo=true;	//显示编辑数据

int Max_GroundPic;			//图素页面数
int Cur_GroundPic=0;		//当前选中图素的编号
int Beg_Ground=-1;			//块选的起始点 -1=点选择
int Cur_Ground=203;			//当前选中的图素(如Beg_Ground != -1 则是块选的结束点)
int Sel_Ground=0;			//选择地面的地方 0=选择页 1=编辑页
int Cur_GroundY=0;			//当前选择地面的偏移点

int Cur_MouseType=3;		//当前选择的鼠标样式

int Max_Level=3;			//总层数
int Cur_Level=0;			//当前编辑的层(0=地面 1=物体 2=物体2 3=静态图素 4=动态图素)
int Cur_EditLevel=0;		//当前修改的层(0-2)
char *Level[]={"地面层","物体层(1)","物体层(2)","静态图素","动态图素"};

extern char strHelp[];	//帮助
Cell_Struct *UndoMap;	//反悔地图
HMENU pmmParent;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//编辑器主循环
void EditLoop()
{
	unsigned char Button=mouse;

	if( key==DIK_F1 )	//帮助
		MessageBox(hWnd, strHelp, "Help", MB_OK);
	//看上面的名字定义好了，我不多说了，而且soft起的名字基本可以望文生意
	if( key==DIK_N ) 
		bShowCell=!bShowCell;

	if( key==DIK_O ) 
		bShowObject=!bShowObject;

	if( key==DIK_M ) 
		bShowBlock=!bShowBlock;

	if( key==DIK_B ) 
		bShowBackGround=!bShowBackGround;

	if( key==DIK_E ) 
		bShowEdit=!bShowEdit;

	if( key==DIK_S ) 
		bShowCurrent=!bShowCurrent;

	if( key==DIK_H ) 
		bShowOnlyObject=!bShowOnlyObject;

	if( key==DIK_C )	//增加陷阱
		CreateDlg( IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);

	if( key==DIK_D )	//删除陷阱
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].CP=BLOCK_CP;
	}

	if(key == DIK_A)	//改变陷阱类型
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].CPType=1-map.Cell[map.Width*cy+cx].CPType;
	}

	if( key==DIK_T )	//改变鼠标样式风格
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].MouseType = Cur_MouseType;
	}

	if( key==DIK_P )	//物体属性
		CreateDlg( IDD_DIALOG_Object, (DLGPROC)DialogObjectProc);

	if( key==DIK_Q )	//敌人设置性
		CreateDlg( IDD_DIALOG_EnemySet, (DLGPROC)DialogEnemySetListProc);

	if( key==DIK_L || key==DIK_TAB )	//改变当前编辑层次，循环的方式
	{
		if( Cur_Level == CL_GROUND ) 
			Cur_Level=CL_OBJECT;
		else 
			if( Cur_Level == CL_OBJECT ) 
				Cur_Level=CL_OBJECT2;
			else 
				if( Cur_Level == CL_OBJECT2 ) 
					Cur_Level=CL_GROUND;
	}

	if( key==DIK_F9 ) 
		map.ShowFullMap();	//缩略图

	if( key==DIK_F )	//以当前地面填充
	{
		//准备好UndoMap
		memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		if( Cur_Level==0 )		//填充地面
		for(int i=0; i<map.Height; i++)
			for(int j=0; j<map.Width; j++)
			{
				map.Cell[map.Width*i+j].Ground=Cur_Ground;	//编号
				map.Cell[map.Width*i+j].GroundPic=Cur_GroundPic;	//页面
			}
	}

	// 编辑数据
	if( key==DIK_F11 )
	{
		bShowEditInfo = !bShowEditInfo;
	}

	//反悔
	if( key==DIK_Z )
		if( Key.KeyDown(DIK_LCONTROL) )
		{
			Cell_Struct *tmp=map.Cell;
			map.Cell=UndoMap;
			UndoMap=tmp;
		}

	//主菜单
	if( key==DIK_ESCAPE )
		CreateDlg(IDD_DIALOG_Main, (DLGPROC)DialogProc);

	// 以主角为地图中心
	if( key == DIK_RETURN )
	{
		map.CenterMap( role[0].State.X, role[0].State.Y );
	}

	// 移动主角到鼠标位置
	if( Key.KeyDown(DIK_LCONTROL) && mouse == RB_DOWN )
	{
		role[0].State.X = map.GetCellX(point.x, point.y);
		role[0].State.x = _point_x(role[0].State.X) + 16;
		role[0].State.Y = map.GetCellY(point.x, point.y);
		role[0].State.y = _point_y(role[0].State.Y) + 16;
	}

	//选择材质
	if( key == DIK_1 )
	{
		if( Cur_GroundPic >= 60 )		//动态的话，置0
		{
			Cur_GroundPic = 0;
			Cur_Ground = 0;
		}

		if( Cur_Level == CL_PIC_SEL )		//选择静态图素，回到编辑状态
		{
			Cur_Level = Cur_EditLevel;		//设置成当前编辑层
		}
		else
		{
			if( Cur_Level < 3 )				//不是静态层和动态层
				Cur_EditLevel = Cur_Level;	//赋给当前编辑层
			Cur_Level=CL_PIC_SEL;			//设置成选择静态图素，进入选图状态
		}
	}

	if( key == DIK_2 )
	{
		if( Cur_Level == CL_ANI_SEL )			//同样的逻辑
		{
			Cur_Level = Cur_EditLevel;
		}
		else
		{
			if( Cur_Level < 3 )	
				Cur_EditLevel = Cur_Level;
			Cur_Level=CL_ANI_SEL;
		}
	}

	//编辑层选择
	switch( Cur_Level )
	{
	case CL_GROUND:		//地面
		EditGround(Button);
		break;

	case CL_OBJECT:		//物体
		EditObj(Button);
		break;

	case CL_OBJECT2:	//物体2
		EditObj(Button);
		break;

	case CL_PIC_SEL:	//选择静态图素
		SelectGround(Button);
		break;

	case CL_ANI_SEL:	//选择动态图素
		SelectAni(Button);
		break;
	}
	//显示DX，SX等等
	if( bShowEditInfo )
	{
		font.PrintText(lpDDSBack, 500, 60, "SX,SY: %d,%d", map.SX, map.SY);
		font.PrintText(lpDDSBack, 500, 80, "DX,DY: %d,%d", map.DX, map.DY);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//编辑地面层
void EditGround(char Button)
{
	//滚屏
	if( point.x>ScreenWidth-3 )
		if( map.SX < map.Width- ShowCellW-1 )
			map.SX++;
	if( point.x < 3 )
		if( map.SX >0 )
			map.SX--;
	if( point.y>ScreenHeight-3 )
		if( map.SY < map.Height- ShowCellH-1 )
			map.SY++;
	if( point.y < 3 )
		if( map.SY >0 )
			map.SY--;

	/////////////////////////////////////////////////////
	//块选点计算
	int x1,y1, x2,y2;
	if( Sel_Ground==0 )	//选择块在选择页
	{
		x1=Beg_Ground % ShowCellW;
		y1=Beg_Ground / ShowCellW;
		x2=Cur_Ground % ShowCellW;
		y2=Cur_Ground / ShowCellW;
	}
	else if( Sel_Ground==1 )	//选择块在编辑页
	{
		x1=Beg_Ground % map.Width;
		y1=Beg_Ground / map.Width;
		x2=Cur_Ground % map.Width;
		y2=Cur_Ground / map.Width;
	}
		
	static bool MouseDown=false;

	//左键按下
	if( Button==LB_DOWN )
	{
		//设置UndoMap
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		MouseDown=true;
		if( Key.KeyDown( DIK_SPACE) )	//改变层次
		{
			bFillCell=true;
		}
		else if( Key.GetState(DIK_LSHIFT) )	//获取为当前地面
		{
			Beg_Ground=cy*map.Width+cx;
			Sel_Ground=1;
		}
		else if( Beg_Ground==-1 || x1>x2 || y1>y2 || (x1==x2 && y1==y2) )
		{
			//如果是点选的话，就是连发模式
			bFillCell=true;
		}
		else	//块选
		{
			if( Sel_Ground==0 )		//选择块在选择页
			for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					map.Cell[(cy+j)*map.Width+cx+i].Ground=Beg_Ground+(j*ShowCellW)+i;
					map.Cell[(cy+j)*map.Width+cx+i].GroundPic=Cur_GroundPic;
				}
			else if( Sel_Ground==1 )	//选择块在编辑页
			{
				for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					map.Cell[(cy+j)*map.Width+cx+i].Ground=map.Cell[Beg_Ground+(j*map.Width)+i].Ground;
					map.Cell[(cy+j)*map.Width+cx+i].GroundPic=Cur_GroundPic;
				}
			}

		}
	}
	else if( Button==LB_UP )	//左键松开
	{
		MouseDown=false;
		if( Key.GetState(DIK_LSHIFT) )	//获取为当前地面
		{
			Cur_Ground=map.GetCell(point.x, point.y);
			x2=cx;
			y2=cy;

			//不是块选
			if( x1>x2 || y1>y2 || (x1==x2 && y1==y2) )
			{
				Cur_GroundPic=map.Cell[Cur_Ground].GroundPic;
				Beg_Ground=Cur_Ground=map.Cell[Beg_Ground].Ground;
				Sel_Ground=0;
			}
			else	//块选
			{
				Cur_Ground=map.GetCell(point.x, point.y);
				Sel_Ground=1;
			}
		}

		bFillCell=false;
		cx2=-1;
	}

	//显示地图
	map.ShowEditMap();

	//预显获取当前地面
	if( Key.GetState(DIK_LSHIFT) && MouseDown )
	{
		int ax1=Beg_Ground % map.Width ;
		int ay1=Beg_Ground / map.Width ;
		int ax2=cx;
		int ay2=cy;

		//不是块选
		if( ax1>ax2 || ay1>ay2 || (ax1==ax2 && ay1==ay2) )
		{
			ABlt(lpDDSBack, CellWidth*(ax1-map.SX), CellHeight*(ay1-map.SY), 
					lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 16);
		}
		else	//块选
		{
			for(int i=0; i<=ax2-ax1; i++)
				for(int j=0; j<=ay2-ay1; j++)
				{
					ABlt(lpDDSBack, CellWidth*(ax1-map.SX+i), CellHeight*(ay1-map.SY+j), 
							lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 15);
				}
		}
	}

	//点击效果
	if( bFillCell && (cx!=cx2 || cy!=cy2) )
	{
		if( Key.KeyDown( DIK_LALT ) )	//改变阻挡关系
		{
			int t=cy*map.Width+cx;
			map.Cell[t].Block=1-map.Cell[t].Block;
		}
		else if( Key.KeyDown( DIK_LCONTROL ) )	//删除
		{
			map.Cell[cy*map.Width+cx].Obj=BLOCK_CELL;
			map.Cell[cy*map.Width+cx].Block=false;
		}
		else if( Key.KeyDown( DIK_SPACE) )		//改变层次
		{
			int t=cy*map.Width+cx;
			map.Cell[t].Level=1-map.Cell[t].Level;
			map.Cell[t].Level2=1-map.Cell[t].Level2;
		}
		else	//放置
		{
			map.Cell[cy*map.Width+cx].Ground=Cur_Ground;
			map.Cell[cy*map.Width+cx].GroundPic=Cur_GroundPic;
		}
		cx2=cx;
		cy2=cy;
	}

	//弹出菜单
	if( Button==RB_UP && !Key.KeyDown( DIK_LCONTROL ) )
	{
		PopupMenu();
	}

	//显示网格
	if( bShowCell )
		ShowCell(lpDDSBack);

	//预显当前图块
	if( bShowCurrent && Cur_GroundPic!=BLOCK_CELL )
	{
		int x= (cx-map.SX) * CellWidth + map.DX;
		int y= (cy-map.SY) * CellHeight + map.DY;

		if( Key.KeyDown( DIK_LSHIFT ) )	//获取
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(4, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_LALT) )	//阻挡关系
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(5, ShowCellW), ColorKey16, 15);
		}
		else if( Key.KeyDown(DIK_LCONTROL) )	//删除
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(3, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_SPACE) )	//层次
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(6, ShowCellW), ColorKey16, 0);
		}
		//如果是点选的话，就是连发模式
		else if( Beg_Ground==-1 || x1>x2 || y1>y2 || (x1==x2 && y1==y2) )
		{
			if( Cur_GroundPic >= 60 )
			{
				RECT rect;
				long page = map.GetAniPage(Cur_Ground, rect);
				ABlt(lpDDSBack, x, y, lpDDSMap[page], rect, 0, 16);
			}
			else
			{
				ABlt(lpDDSBack, x, y, lpDDSMap[Cur_GroundPic], 
						map._GetRectangle(Cur_Ground, ShowCellW), 0, 16);
			}
		}
		else
		{
			if( Sel_Ground==0 )		//选择块在选择页
				for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					if( Cur_GroundPic >= 60 )
					{
						RECT rect;
						long page = map.GetAniPage(Cur_Ground, rect);
						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j, 
									lpDDSMap[page], rect, 0, 16);
					}
					else
					{
						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j,lpDDSMap[Cur_GroundPic]
							, map._GetRectangle(Beg_Ground+ShowCellW*j+i, ShowCellW), 0, 16);
					}
				}
			else if( Sel_Ground==1 )	//选择块在编辑页
			{
				for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					if( Cur_GroundPic >= 60 )
					{
						RECT rect;
						long page = map.GetAniPage(map.Cell[Beg_Ground+
									(j*map.Width)+i].Ground, rect);

						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j, 
										lpDDSMap[page], rect, 0, 16);
					}
					else
					{
						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j,lpDDSMap[Cur_GroundPic], 
								map._GetRectangle(map.Cell[Beg_Ground+(j*map.Width)+i].Ground,
							ShowCellW), 0, 16);
					}
				}
			}
		}
	}

	if( bShowEditInfo )
	{
		int n=map.GetCell(point.x, point.y);
		if( n < 0 ) n = 0;
		font.SetColor(RGB(255,255,255));
		font.PrintText(lpDDSBack,   0,   0, "坐标:(%d,%d)", point.x, point.y);
		font.PrintText(lpDDSBack,   0,  14, "格子:(%d,%d)=%d", cx, cy, n);
		font.PrintText(lpDDSBack, 150,   0, "物体:%4d[%4d(%d)][%4d(%d)]", Cur_Ground, map.Cell[n].Obj, map.Cell[n].ObjPic, map.Cell[n].Obj2, map.Cell[n].Obj2Pic);
		font.PrintText(lpDDSBack, 150,  14, "地面:%4d[%4d(%d)]", Cur_Ground, map.Cell[n].Ground, map.Cell[n].GroundPic);
		font.PrintText(lpDDSBack,  80, 460, "当前层:%d[%s]", Cur_Level, Level[Cur_Level]);
		font.PrintText(lpDDSBack, 550,   0, "Beg:%d", Beg_Ground);
		font.PrintText(lpDDSBack, 550,  20, "End:%d", Cur_Ground);
		font.PrintText(lpDDSBack, 550,  40, "Sel:%d", Sel_Ground);
	}
}

//编辑物体层
void EditObj(char Button)
{
	//滚屏
	if( point.x>ScreenWidth-3 )
		if( map.SX < map.Width- ShowCellW-1 )
			map.SX++;
	if( point.x < 3 )
		if( map.SX >0 )
			map.SX--;
	if( point.y>ScreenHeight-3 )
		if( map.SY < map.Height- ShowCellH-1 )
			map.SY++;
	if( point.y < 3 )
		if( map.SY >0 )
			map.SY--;

	/////////////////////////////////////////////////////
	//块选点计算
	int x1=Beg_Ground % ShowCellW;
	int y1=Beg_Ground / ShowCellW;
	int x2=Cur_Ground % ShowCellW;
	int y2=Cur_Ground / ShowCellW;

	//左键按下
	if( Button==LB_DOWN )
	{
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		if( Key.KeyDown( DIK_SPACE) )	//改变层次
		{
			bFillCell=true;
		}
		else if( Key.KeyDown(DIK_LSHIFT) )		//获取
		{
			switch( Cur_Level )
			{
			case CL_OBJECT:
				{
					int obj=map.Cell[cy*map.Width+cx].Obj;
					if( obj != BLOCK_CELL )
					{
						Cur_Ground=Beg_Ground=obj;
						Cur_GroundPic=map.Cell[cy*map.Width+cx].ObjPic;
					}
					else
					{
						obj=map.Cell[cy*map.Width+cx].Obj2;
						if( obj != BLOCK_CELL )
						{
							Cur_Ground=Beg_Ground=obj;
							Cur_GroundPic=map.Cell[cy*map.Width+cx].Obj2Pic;
						}
					}
				}
				break;

			case CL_OBJECT2:
				{
					int obj=map.Cell[cy*map.Width+cx].Obj2;
					if( obj != BLOCK_CELL )
					{
						Cur_Ground=Beg_Ground=obj;
						Cur_GroundPic=map.Cell[cy*map.Width+cx].Obj2Pic;
					}
					else
					{
						obj=map.Cell[cy*map.Width+cx].Obj;
						if( obj != BLOCK_CELL )
						{
							Cur_Ground=Beg_Ground=obj;
							Cur_GroundPic=map.Cell[cy*map.Width+cx].ObjPic;
						}
					}
				}
				break;
			}

		}
		//如果是点选的话，就是连发模式
		else if( Key.KeyDown(DIK_LSHIFT) || Key.KeyDown(DIK_LCONTROL) 
							|| Key.KeyDown(DIK_LALT) || Beg_Ground==-1 
							|| x1>x2 || y1>y2 || (x1==x2 && y1==y2))
		{
			bFillCell=true;
		}
		else 	//块填充
		{
			for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					switch( Cur_Level )
					{
					case CL_OBJECT:
						map.Cell[(cy+j)*map.Width+cx+i].Obj=Beg_Ground+(j*ShowCellW)+i;
						map.Cell[(cy+j)*map.Width+cx+i].ObjPic=Cur_GroundPic;
						break;

					case CL_OBJECT2:
						map.Cell[(cy+j)*map.Width+cx+i].Obj2=Beg_Ground+(j*ShowCellW)+i;
						map.Cell[(cy+j)*map.Width+cx+i].Obj2Pic=Cur_GroundPic;
						break;
					}
				}
		}
	}

	//左键松开
	if( Button==LB_UP )
	{
		bFillCell=false;
		cx2=-1;
	}

	//点击效果
	if( bFillCell && (cx!=cx2 || cy!=cy2) )
	{
		switch( Cur_Level )
		{
		case CL_OBJECT:
			{
				if( Key.KeyDown( DIK_LALT ) )		//改变阻挡关系
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Block=1-map.Cell[t].Block;
				}
				else if( Key.KeyDown( DIK_LCONTROL ) )	//删除
				{
					map.Cell[cy*map.Width+cx].Obj=BLOCK_CELL;
					map.Cell[cy*map.Width+cx].Block=false;
				}
				else if( Key.KeyDown( DIK_SPACE) )		//改变层次
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Level=1-map.Cell[t].Level;
				}
				else	//放置
				{
					map.Cell[cy*map.Width+cx].Obj=Cur_Ground;
					map.Cell[cy*map.Width+cx].ObjPic=Cur_GroundPic;
					map.Cell[cy*map.Width+cx].Block=true;
				}
			}
			break;

		case CL_OBJECT2:
			{
				if( Key.KeyDown( DIK_LALT ) )		//改变阻挡关系
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Block=1-map.Cell[t].Block;
				}
				else if( Key.KeyDown( DIK_LCONTROL ) )	//删除
				{
					map.Cell[cy*map.Width+cx].Obj2=BLOCK_CELL;
				}
				else if( Key.KeyDown( DIK_SPACE) )		//改变层次
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Level2=1-map.Cell[t].Level2;
				}
				else	//放置
				{
					map.Cell[cy*map.Width+cx].Obj2=Cur_Ground;
					map.Cell[cy*map.Width+cx].Obj2Pic=Cur_GroundPic;
				}
			}
			break;
		}
		
		cx2=cx;
		cy2=cy;

		//设置undo
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
	}

	//弹出菜单
	if( Button==RB_UP && !Key.KeyDown( DIK_LCONTROL ) )
	{
		PopupMenu();		
	}

	//显示地图
	map.ShowEditMap();

	//显示网格
	if( bShowCell )
		ShowCell(lpDDSBack);

	//预显当前物体
	if( bShowCurrent && Cur_GroundPic!=BLOCK_CELL )
	{
		static Alpha=8;

		int x= (cx-map.SX) * CellWidth + map.DX;
		int y= (cy-map.SY) * CellHeight + map.DY;

		if( Key.KeyDown( DIK_LSHIFT ) )	//获取
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(4, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_LALT) )	//阻挡关系
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(5, ShowCellW), ColorKey16, 15);
		}
		else if( Key.KeyDown(DIK_LCONTROL) )	//删除
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(3, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_SPACE) )	//层次
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(6, ShowCellW), ColorKey16, 0);
		}
		//如果是点选的话，就是连发模式
		else if( Beg_Ground==-1 || x1>x2 || y1>y2 || (x1==x2 && y1==y2))
		{
			if( Cur_GroundPic >= 60 )
			{
				RECT rect;
				long page = map.GetAniPage(Cur_Ground, rect);
				ABlt(lpDDSBack, x, y, lpDDSMap[page], rect, 0, 16);
			}
			else
			{
				ABlt(lpDDSBack, x, y, lpDDSMap[Cur_GroundPic],
						map._GetRectangle(Cur_Ground, ShowCellW), ColorKey16, Alpha);
			}
		}
		else
		{
			for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					if( Cur_GroundPic >= 60 )
					{
						RECT rect;
						long page = map.GetAniPage(Beg_Ground+ShowCellW*j+i, rect);
						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j,
									lpDDSMap[page], rect, 0, 16);
					}
					else
					{
						ABlt(lpDDSBack, x+CellWidth*i, y+CellHeight*j,lpDDSMap[Cur_GroundPic], 
							map._GetRectangle(Beg_Ground+ShowCellW*j+i, ShowCellW),
								ColorKey16, 16);
					}
				}
		}
	}

	if( bShowEditInfo )
	{
		int n=map.GetCell(point.x, point.y);
		if( n < 0 ) n = 0;
		font.SetColor(RGB(255,255,255));
		font.PrintText(lpDDSBack,   0,   0, "坐标:(%d,%d)", point.x, point.y);
		font.PrintText(lpDDSBack,   0,  14, "格子:(%d,%d)=%d", cx, cy, n);
		font.PrintText(lpDDSBack, 150,   0, "物体:%4d[%4d(%d)][%4d(%d)]", Cur_Ground, map.Cell[n].Obj, map.Cell[n].ObjPic, map.Cell[n].Obj2, map.Cell[n].Obj2Pic);
		font.PrintText(lpDDSBack, 150,  14, "地面:%4d[%4d(%d)]", Cur_Ground, map.Cell[n].Ground, map.Cell[n].GroundPic);
		font.PrintText(lpDDSBack,  80, 460, "当前层:%d[%s]", Cur_Level, Level[Cur_Level]);
		font.PrintText(lpDDSBack, 550,   0, "Beg:%d", Beg_Ground);
		font.PrintText(lpDDSBack, 550,  20, "End:%d", Cur_Ground);
		font.PrintText(lpDDSBack, 550,  40, "Sel:%d", Sel_Ground);
	}
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

//选择静态图素
void SelectGround(char Button)
{
	//上翻页
	if( key==DIK_A )
	{
		Cur_GroundPic--;
		if( Cur_GroundPic < 0 ) 
			Cur_GroundPic=Max_GroundPic-1;
		Cur_GroundY=0;
	}

	//下翻页
	if( key==DIK_Z )
	{
		Cur_GroundPic++;
		if( Cur_GroundPic >= Max_GroundPic ) 
			Cur_GroundPic=0;
		Cur_GroundY=0;
	}

	//上下滚动
	if( point.y>ScreenHeight-3 )
	{
		if( Cur_GroundY+ScreenHeight < GetSurfaceHeight(lpDDSMap[Cur_GroundPic]) )
			Cur_GroundY+=CellHeight;
	}
	if( point.y < 3 )
	{
		if( Cur_GroundY >0 )
			Cur_GroundY-=CellHeight;
	}

	static bShowSelect=false;

	//左键按下
	if( Button==LB_DOWN )
	{
		Beg_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		bShowSelect=true;
	}

	//左键松开
	if( Button==LB_UP )
	{
		Cur_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		Sel_Ground=0;
		Cur_Level=Cur_EditLevel;	//返回编辑
		bShowSelect=false;
	}
	
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
		bShowSelect=false;
	}

	//显示
	RECT rect={0, Cur_GroundY, ScreenWidth, Cur_GroundY+ScreenHeight};
	Blt(lpDDSBack, 0, 0, lpDDSMap[Cur_GroundPic], rect, false);

	//显示网格
	if( bShowCell )
		ShowCell(lpDDSBack);

	//块选计算
	int x1=Beg_Ground % ShowCellW;
	int y1=Beg_Ground / ShowCellW;
	int x2=map._GetCellX(point.x, point.y+Cur_GroundY);
	int y2=map._GetCellY(point.x, point.y+Cur_GroundY);

	//显示选择方框
	int x= map._GetCellX(point.x, point.y) * CellWidth;
	int y= map._GetCellY(point.x, point.y) * CellHeight;
	if( !bShowSelect || (Beg_Ground==-1 || x1>x2 || y1>y2 || (x1==x2 && y1==y2) ) )
	{
		ABlt(lpDDSBack, x, y, lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 15);
	}
	else
	{
		for(int i=0; i<=x2-x1; i++)
			for(int j=0; j<=y2-y1; j++)
			{
				ABlt(lpDDSBack, CellWidth*(x1+i), CellHeight*(y1+j)-Cur_GroundY, 
						lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 16);
			}
	}

	font.SetColor(RGB(255,255,255));
	font.PrintText(lpDDSBack, 5, 5, "[A]=上一页 [Z]=下一页");
	font.PrintText(lpDDSBack, 80, 460, "当前层:%d[%s]", Cur_Level, Level[Cur_Level]);
	font.PrintText(lpDDSBack, 260, 460, "当前格子: %d / %d 页", map._GetCell(point.x, point.y+Cur_GroundY, ShowCellW), Cur_GroundPic);
	font.PrintText(lpDDSBack, 550, 0, "Beg:%d", Beg_Ground);
	font.PrintText(lpDDSBack, 550, 20, "End:%d", Cur_Ground);
}

//选择动态图素
void SelectAni(char Button)
{
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
	}

	static bShowSelect=false;

	//左键按下
	if( Button==LB_DOWN )
	{
		Beg_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		bShowSelect=true;
	}

	//左键松开
	if( Button==LB_UP )
	{
		Cur_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		Sel_Ground=0;
		Cur_GroundPic = 60;
		Cur_Level=Cur_EditLevel;	//返回编辑
		bShowSelect=false;
	}
	
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
		bShowSelect=false;
	}

	// 重读anilist
	if( key == DIK_R )
	{
		map.LoadAniCell("ini\\anicell.ini", "");
	}

	//显示
	Clrscr(lpDDSBack);

	for(int num=0; num<CMap::AniCellNum; num++)
	{
		RECT rect = {0,0,0,0};
		long page = map.GetAniPage(num, rect);
		BltFast(lpDDSBack, num%20*CellWidth, num/20*CellWidth, lpDDSMap[ page ], rect, true);
	}

	//显示网格
	if( bShowCell )
		ShowCell(lpDDSBack);

	font.SetColor(RGB(255,255,255));
	font.PrintText(lpDDSBack, 80, 460, "当前层:%d[%s]", Cur_Level, Level[Cur_Level]);
	font.PrintText(lpDDSBack, 260, 460, "当前格子: %d / %d 页", map._GetCell(point.x, point.y+Cur_GroundY, ShowCellW), Cur_GroundPic);
	font.PrintText(lpDDSBack, 550, 0, "Beg:%d", Beg_Ground);
	font.PrintText(lpDDSBack, 550, 20, "End:%d", Cur_Ground);
	font.PrintText(lpDDSBack, 550, 40, "R : 重读动画图块列表");

	map.AniLoop();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//显示网格
void ShowCell(LPDIRECTDRAWSURFACE ds)
{
	int i;
	for(i=1; i<ShowCellH; i++)
	{
		Line(lpDDSBack, 0, i*CellWidth, ScreenWidth, i*CellWidth, RGB16(128,128,128));
	}
	
	for(i=1; i<ShowCellW; i++)
	{
		Line(lpDDSBack, i*CellHeight, 0, i*CellHeight, ScreenHeight, RGB16(128,128,128));
	}
}

//清空编辑数据
void ClearEditData()
{
}

//弹出菜单
void PopupMenu()
{
	int l,t;
	if( WindowMode == 0 )
	{
		l=t=0;
	}
	else
	{
		l=RectWindow.left;
		t=RectWindow.top;
	}
	pmmParent=LoadMenu(hInst, "IDR_MENU_POP");
	HMENU pmm=GetSubMenu(pmmParent, 0);
	
	//显示网格
	if( bShowCell )
		CheckMenuItem(pmm, ID_MENUITEM_ShowCell, MF_CHECKED);

	//显示物体
	if( bShowObject )
		CheckMenuItem(pmm, ID_MENUITEM_ShowObject, MF_CHECKED);

	//显示阻挡关系
	if( bShowBlock )
		CheckMenuItem(pmm, ID_MENUITEM_ShowBlock, MF_CHECKED);

	//显示当前图块
	if( bShowCurrent )
		CheckMenuItem(pmm, ID_MENUITEM_ShowCurrent, MF_CHECKED);

	//显示其他
	if( bShowEdit )
		CheckMenuItem(pmm, ID_MENUITEM_ShowEdit, MF_CHECKED);

	//只显示物体
	if( bShowOnlyObject )
		CheckMenuItem(pmm, ID_MENUITEM_ShowObjectOnly, MF_CHECKED);

	//地面层
	if( Cur_Level == CL_GROUND )
		CheckMenuItem(pmm, ID_MENUITEM_Ground, MF_CHECKED);

	//物体层1
	if( Cur_Level == CL_OBJECT )
		CheckMenuItem(pmm, ID_MENUITEM_Object, MF_CHECKED);

	//物体层1
	if( Cur_Level == CL_OBJECT2 )
		CheckMenuItem(pmm, ID_MENUITEM_Object2, MF_CHECKED);
	
	TrackPopupMenu(pmm, TPM_LEFTALIGN|TPM_RIGHTBUTTON,
						point.x+l, point.y+t,
						0,
						hWnd,
						NULL);
	bMenuPop=true;
}

//处理编辑器右键菜单消息
void MapEditProc(long msg)
{
	switch (msg)
	{
	//选择静态图素
	case ID_MENUITEM_SelGround:
		if( Cur_Level < 3 )	Cur_EditLevel = Cur_Level;
		Cur_Level=CL_PIC_SEL;

		if( Cur_GroundPic >= 60 )
		{
			Cur_GroundPic = 0;
			Cur_Ground = 0;
		}

		break;

	//选择动态图素
	case ID_MENUITEM_SelAni:
		if( Cur_Level < 3 )	
			Cur_EditLevel = Cur_Level;
		Cur_Level=CL_ANI_SEL;
		break;

	//设置敌人
	case ID_MENUITEM_Enemy:
		CreateDlg(IDD_DIALOG_EnemySet, DialogEnemySetListProc);
		break;

	//格子属性
	case ID_MENUITEM_Cell:
		CreateDlg(IDD_DIALOG_Object, DialogObjectProc);
		break;

	//设置陷阱
	case ID_MENUITEM_SetHook:
		CreateDlg(IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);
		break;

	//删除陷阱
	case ID_MENUITEM_DelHook:
		map.Cell[map.Width*cy+cx].CP=BLOCK_CP;
		break;

	//主菜单
	case ID_MENUITEM_Map:
		CreateDlg(IDD_DIALOG_Main, (DLGPROC)DialogProc);
		break;
	
	//显示缩略图
	case ID_MENUITEM_FullMap:
		map.ShowFullMap();
		break;

	//显示网格
	case ID_MENUITEM_ShowCell:
		bShowCell=!bShowCell;
		break;

	//显示物体
	case ID_MENUITEM_ShowObject:
		bShowObject=!bShowObject;
		break;

	//显示阻挡关系
	case ID_MENUITEM_ShowBlock:
		bShowBlock=!bShowBlock;
		break;

	//显示当前图块
	case ID_MENUITEM_ShowCurrent:
		bShowCurrent=!bShowCurrent;
		break;

	//显示其他
	case ID_MENUITEM_ShowEdit:
		bShowEdit=!bShowEdit;
		break;

	//只显示物体
	case ID_MENUITEM_ShowObjectOnly:
		bShowOnlyObject=!bShowOnlyObject;
		break;

	//编辑层次-地面
	case ID_MENUITEM_Ground:
		Cur_Level=CL_GROUND;
		break;

	//编辑层次-物体1
	case ID_MENUITEM_Object:
		Cur_Level=CL_OBJECT;
		break;

	//编辑层次-物体2
	case ID_MENUITEM_Object2:
		Cur_Level=CL_OBJECT2;
		break;

	//存取地图-new
	case ID_MENUITEM_New:
		_NewMap(hWnd);
		break;

	//存取地图-open
	case ID_MENUITEM_Load:
		_LoadMap(hWnd);
		break;

	//存取地图-save
	case ID_MENUITEM_Save:
		_SaveMap(hWnd);
		break;

	//存取地图-saveas
	case ID_MENUITEM_SaveAs:
		_SaveAsMap(hWnd);
		break;

	//退出
	case ID_MENUITEM_Exit:
		RunGame=false;
		break;

	//反悔
	case ID_MENUITEM_Undo:
		{
			Cell_Struct *tmp=map.Cell;
			map.Cell=UndoMap;
			UndoMap=tmp;
		}
		break;

	//取消
	case ID_MENUITEM_Cancel:
		break;

	//退出
	case IDM_Exit:
		RunGame=false;
		break;
	}
}
