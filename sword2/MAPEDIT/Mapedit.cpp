//********************************************
//	��ͼ�༭ ��غ���
//  ������2000��4����10��
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
//�����༭��ר�ñ���
///////////////////////////////////
int cx2, cy2;				//��ʱ������

bool bShowBackGround=true;	//��ʾ����
bool bShowCell=true;		//��ʾ���
bool bShowObject=true;		//��ʾ����
bool bShowBlock=true;		//��ʾ�赲��ϵ
bool bShowCurrent=true;		//��ʾ��ǰ����
bool bShowEdit=true;		//��ʾ�༭��ʾ�����塢�л��㡢����
bool bShowOnlyObject=false;	//��ʾ����
bool bFillCell=false;		//�Ƿ��������
bool bShowHardCursor=false;	//�Ƿ���ʾӲ�����
bool bShowFps=false;		//�Ƿ���ʾFPS
bool bMenuPop=false;		//�˵��Ƿ����
bool bShowEditInfo=true;	//��ʾ�༭����

int Max_GroundPic;			//ͼ��ҳ����
int Cur_GroundPic=0;		//��ǰѡ��ͼ�صı��
int Beg_Ground=-1;			//��ѡ����ʼ�� -1=��ѡ��
int Cur_Ground=203;			//��ǰѡ�е�ͼ��(��Beg_Ground != -1 ���ǿ�ѡ�Ľ�����)
int Sel_Ground=0;			//ѡ�����ĵط� 0=ѡ��ҳ 1=�༭ҳ
int Cur_GroundY=0;			//��ǰѡ������ƫ�Ƶ�

int Cur_MouseType=3;		//��ǰѡ��������ʽ

int Max_Level=3;			//�ܲ���
int Cur_Level=0;			//��ǰ�༭�Ĳ�(0=���� 1=���� 2=����2 3=��̬ͼ�� 4=��̬ͼ��)
int Cur_EditLevel=0;		//��ǰ�޸ĵĲ�(0-2)
char *Level[]={"�����","�����(1)","�����(2)","��̬ͼ��","��̬ͼ��"};

extern char strHelp[];	//����
Cell_Struct *UndoMap;	//���ڵ�ͼ
HMENU pmmParent;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//�༭����ѭ��
void EditLoop()
{
	unsigned char Button=mouse;

	if( key==DIK_F1 )	//����
		MessageBox(hWnd, strHelp, "Help", MB_OK);
	//����������ֶ�����ˣ��Ҳ���˵�ˣ�����soft������ֻ���������������
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

	if( key==DIK_C )	//��������
		CreateDlg( IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);

	if( key==DIK_D )	//ɾ������
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].CP=BLOCK_CP;
	}

	if(key == DIK_A)	//�ı���������
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].CPType=1-map.Cell[map.Width*cy+cx].CPType;
	}

	if( key==DIK_T )	//�ı������ʽ���
	{
		memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		map.Cell[map.Width*cy+cx].MouseType = Cur_MouseType;
	}

	if( key==DIK_P )	//��������
		CreateDlg( IDD_DIALOG_Object, (DLGPROC)DialogObjectProc);

	if( key==DIK_Q )	//����������
		CreateDlg( IDD_DIALOG_EnemySet, (DLGPROC)DialogEnemySetListProc);

	if( key==DIK_L || key==DIK_TAB )	//�ı䵱ǰ�༭��Σ�ѭ���ķ�ʽ
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
		map.ShowFullMap();	//����ͼ

	if( key==DIK_F )	//�Ե�ǰ�������
	{
		//׼����UndoMap
		memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		if( Cur_Level==0 )		//������
		for(int i=0; i<map.Height; i++)
			for(int j=0; j<map.Width; j++)
			{
				map.Cell[map.Width*i+j].Ground=Cur_Ground;	//���
				map.Cell[map.Width*i+j].GroundPic=Cur_GroundPic;	//ҳ��
			}
	}

	// �༭����
	if( key==DIK_F11 )
	{
		bShowEditInfo = !bShowEditInfo;
	}

	//����
	if( key==DIK_Z )
		if( Key.KeyDown(DIK_LCONTROL) )
		{
			Cell_Struct *tmp=map.Cell;
			map.Cell=UndoMap;
			UndoMap=tmp;
		}

	//���˵�
	if( key==DIK_ESCAPE )
		CreateDlg(IDD_DIALOG_Main, (DLGPROC)DialogProc);

	// ������Ϊ��ͼ����
	if( key == DIK_RETURN )
	{
		map.CenterMap( role[0].State.X, role[0].State.Y );
	}

	// �ƶ����ǵ����λ��
	if( Key.KeyDown(DIK_LCONTROL) && mouse == RB_DOWN )
	{
		role[0].State.X = map.GetCellX(point.x, point.y);
		role[0].State.x = _point_x(role[0].State.X) + 16;
		role[0].State.Y = map.GetCellY(point.x, point.y);
		role[0].State.y = _point_y(role[0].State.Y) + 16;
	}

	//ѡ�����
	if( key == DIK_1 )
	{
		if( Cur_GroundPic >= 60 )		//��̬�Ļ�����0
		{
			Cur_GroundPic = 0;
			Cur_Ground = 0;
		}

		if( Cur_Level == CL_PIC_SEL )		//ѡ��̬ͼ�أ��ص��༭״̬
		{
			Cur_Level = Cur_EditLevel;		//���óɵ�ǰ�༭��
		}
		else
		{
			if( Cur_Level < 3 )				//���Ǿ�̬��Ͷ�̬��
				Cur_EditLevel = Cur_Level;	//������ǰ�༭��
			Cur_Level=CL_PIC_SEL;			//���ó�ѡ��̬ͼ�أ�����ѡͼ״̬
		}
	}

	if( key == DIK_2 )
	{
		if( Cur_Level == CL_ANI_SEL )			//ͬ�����߼�
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

	//�༭��ѡ��
	switch( Cur_Level )
	{
	case CL_GROUND:		//����
		EditGround(Button);
		break;

	case CL_OBJECT:		//����
		EditObj(Button);
		break;

	case CL_OBJECT2:	//����2
		EditObj(Button);
		break;

	case CL_PIC_SEL:	//ѡ��̬ͼ��
		SelectGround(Button);
		break;

	case CL_ANI_SEL:	//ѡ��̬ͼ��
		SelectAni(Button);
		break;
	}
	//��ʾDX��SX�ȵ�
	if( bShowEditInfo )
	{
		font.PrintText(lpDDSBack, 500, 60, "SX,SY: %d,%d", map.SX, map.SY);
		font.PrintText(lpDDSBack, 500, 80, "DX,DY: %d,%d", map.DX, map.DY);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//�༭�����
void EditGround(char Button)
{
	//����
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
	//��ѡ�����
	int x1,y1, x2,y2;
	if( Sel_Ground==0 )	//ѡ�����ѡ��ҳ
	{
		x1=Beg_Ground % ShowCellW;
		y1=Beg_Ground / ShowCellW;
		x2=Cur_Ground % ShowCellW;
		y2=Cur_Ground / ShowCellW;
	}
	else if( Sel_Ground==1 )	//ѡ����ڱ༭ҳ
	{
		x1=Beg_Ground % map.Width;
		y1=Beg_Ground / map.Width;
		x2=Cur_Ground % map.Width;
		y2=Cur_Ground / map.Width;
	}
		
	static bool MouseDown=false;

	//�������
	if( Button==LB_DOWN )
	{
		//����UndoMap
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		MouseDown=true;
		if( Key.KeyDown( DIK_SPACE) )	//�ı���
		{
			bFillCell=true;
		}
		else if( Key.GetState(DIK_LSHIFT) )	//��ȡΪ��ǰ����
		{
			Beg_Ground=cy*map.Width+cx;
			Sel_Ground=1;
		}
		else if( Beg_Ground==-1 || x1>x2 || y1>y2 || (x1==x2 && y1==y2) )
		{
			//����ǵ�ѡ�Ļ�����������ģʽ
			bFillCell=true;
		}
		else	//��ѡ
		{
			if( Sel_Ground==0 )		//ѡ�����ѡ��ҳ
			for(int i=0; i<=x2-x1; i++)
				for(int j=0; j<=y2-y1; j++)
				{
					map.Cell[(cy+j)*map.Width+cx+i].Ground=Beg_Ground+(j*ShowCellW)+i;
					map.Cell[(cy+j)*map.Width+cx+i].GroundPic=Cur_GroundPic;
				}
			else if( Sel_Ground==1 )	//ѡ����ڱ༭ҳ
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
	else if( Button==LB_UP )	//����ɿ�
	{
		MouseDown=false;
		if( Key.GetState(DIK_LSHIFT) )	//��ȡΪ��ǰ����
		{
			Cur_Ground=map.GetCell(point.x, point.y);
			x2=cx;
			y2=cy;

			//���ǿ�ѡ
			if( x1>x2 || y1>y2 || (x1==x2 && y1==y2) )
			{
				Cur_GroundPic=map.Cell[Cur_Ground].GroundPic;
				Beg_Ground=Cur_Ground=map.Cell[Beg_Ground].Ground;
				Sel_Ground=0;
			}
			else	//��ѡ
			{
				Cur_Ground=map.GetCell(point.x, point.y);
				Sel_Ground=1;
			}
		}

		bFillCell=false;
		cx2=-1;
	}

	//��ʾ��ͼ
	map.ShowEditMap();

	//Ԥ�Ի�ȡ��ǰ����
	if( Key.GetState(DIK_LSHIFT) && MouseDown )
	{
		int ax1=Beg_Ground % map.Width ;
		int ay1=Beg_Ground / map.Width ;
		int ax2=cx;
		int ay2=cy;

		//���ǿ�ѡ
		if( ax1>ax2 || ay1>ay2 || (ax1==ax2 && ay1==ay2) )
		{
			ABlt(lpDDSBack, CellWidth*(ax1-map.SX), CellHeight*(ay1-map.SY), 
					lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 16);
		}
		else	//��ѡ
		{
			for(int i=0; i<=ax2-ax1; i++)
				for(int j=0; j<=ay2-ay1; j++)
				{
					ABlt(lpDDSBack, CellWidth*(ax1-map.SX+i), CellHeight*(ay1-map.SY+j), 
							lpDDSTools, map._GetRectangle(0, ShowCellW), 0, 15);
				}
		}
	}

	//���Ч��
	if( bFillCell && (cx!=cx2 || cy!=cy2) )
	{
		if( Key.KeyDown( DIK_LALT ) )	//�ı��赲��ϵ
		{
			int t=cy*map.Width+cx;
			map.Cell[t].Block=1-map.Cell[t].Block;
		}
		else if( Key.KeyDown( DIK_LCONTROL ) )	//ɾ��
		{
			map.Cell[cy*map.Width+cx].Obj=BLOCK_CELL;
			map.Cell[cy*map.Width+cx].Block=false;
		}
		else if( Key.KeyDown( DIK_SPACE) )		//�ı���
		{
			int t=cy*map.Width+cx;
			map.Cell[t].Level=1-map.Cell[t].Level;
			map.Cell[t].Level2=1-map.Cell[t].Level2;
		}
		else	//����
		{
			map.Cell[cy*map.Width+cx].Ground=Cur_Ground;
			map.Cell[cy*map.Width+cx].GroundPic=Cur_GroundPic;
		}
		cx2=cx;
		cy2=cy;
	}

	//�����˵�
	if( Button==RB_UP && !Key.KeyDown( DIK_LCONTROL ) )
	{
		PopupMenu();
	}

	//��ʾ����
	if( bShowCell )
		ShowCell(lpDDSBack);

	//Ԥ�Ե�ǰͼ��
	if( bShowCurrent && Cur_GroundPic!=BLOCK_CELL )
	{
		int x= (cx-map.SX) * CellWidth + map.DX;
		int y= (cy-map.SY) * CellHeight + map.DY;

		if( Key.KeyDown( DIK_LSHIFT ) )	//��ȡ
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(4, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_LALT) )	//�赲��ϵ
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(5, ShowCellW), ColorKey16, 15);
		}
		else if( Key.KeyDown(DIK_LCONTROL) )	//ɾ��
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(3, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_SPACE) )	//���
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
						map._GetRectangle(6, ShowCellW), ColorKey16, 0);
		}
		//����ǵ�ѡ�Ļ�����������ģʽ
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
			if( Sel_Ground==0 )		//ѡ�����ѡ��ҳ
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
			else if( Sel_Ground==1 )	//ѡ����ڱ༭ҳ
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
		font.PrintText(lpDDSBack,   0,   0, "����:(%d,%d)", point.x, point.y);
		font.PrintText(lpDDSBack,   0,  14, "����:(%d,%d)=%d", cx, cy, n);
		font.PrintText(lpDDSBack, 150,   0, "����:%4d[%4d(%d)][%4d(%d)]", Cur_Ground, map.Cell[n].Obj, map.Cell[n].ObjPic, map.Cell[n].Obj2, map.Cell[n].Obj2Pic);
		font.PrintText(lpDDSBack, 150,  14, "����:%4d[%4d(%d)]", Cur_Ground, map.Cell[n].Ground, map.Cell[n].GroundPic);
		font.PrintText(lpDDSBack,  80, 460, "��ǰ��:%d[%s]", Cur_Level, Level[Cur_Level]);
		font.PrintText(lpDDSBack, 550,   0, "Beg:%d", Beg_Ground);
		font.PrintText(lpDDSBack, 550,  20, "End:%d", Cur_Ground);
		font.PrintText(lpDDSBack, 550,  40, "Sel:%d", Sel_Ground);
	}
}

//�༭�����
void EditObj(char Button)
{
	//����
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
	//��ѡ�����
	int x1=Beg_Ground % ShowCellW;
	int y1=Beg_Ground / ShowCellW;
	int x2=Cur_Ground % ShowCellW;
	int y2=Cur_Ground / ShowCellW;

	//�������
	if( Button==LB_DOWN )
	{
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		if( Key.KeyDown( DIK_SPACE) )	//�ı���
		{
			bFillCell=true;
		}
		else if( Key.KeyDown(DIK_LSHIFT) )		//��ȡ
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
		//����ǵ�ѡ�Ļ�����������ģʽ
		else if( Key.KeyDown(DIK_LSHIFT) || Key.KeyDown(DIK_LCONTROL) 
							|| Key.KeyDown(DIK_LALT) || Beg_Ground==-1 
							|| x1>x2 || y1>y2 || (x1==x2 && y1==y2))
		{
			bFillCell=true;
		}
		else 	//�����
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

	//����ɿ�
	if( Button==LB_UP )
	{
		bFillCell=false;
		cx2=-1;
	}

	//���Ч��
	if( bFillCell && (cx!=cx2 || cy!=cy2) )
	{
		switch( Cur_Level )
		{
		case CL_OBJECT:
			{
				if( Key.KeyDown( DIK_LALT ) )		//�ı��赲��ϵ
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Block=1-map.Cell[t].Block;
				}
				else if( Key.KeyDown( DIK_LCONTROL ) )	//ɾ��
				{
					map.Cell[cy*map.Width+cx].Obj=BLOCK_CELL;
					map.Cell[cy*map.Width+cx].Block=false;
				}
				else if( Key.KeyDown( DIK_SPACE) )		//�ı���
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Level=1-map.Cell[t].Level;
				}
				else	//����
				{
					map.Cell[cy*map.Width+cx].Obj=Cur_Ground;
					map.Cell[cy*map.Width+cx].ObjPic=Cur_GroundPic;
					map.Cell[cy*map.Width+cx].Block=true;
				}
			}
			break;

		case CL_OBJECT2:
			{
				if( Key.KeyDown( DIK_LALT ) )		//�ı��赲��ϵ
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Block=1-map.Cell[t].Block;
				}
				else if( Key.KeyDown( DIK_LCONTROL ) )	//ɾ��
				{
					map.Cell[cy*map.Width+cx].Obj2=BLOCK_CELL;
				}
				else if( Key.KeyDown( DIK_SPACE) )		//�ı���
				{
					int t=cy*map.Width+cx;
					map.Cell[t].Level2=1-map.Cell[t].Level2;
				}
				else	//����
				{
					map.Cell[cy*map.Width+cx].Obj2=Cur_Ground;
					map.Cell[cy*map.Width+cx].Obj2Pic=Cur_GroundPic;
				}
			}
			break;
		}
		
		cx2=cx;
		cy2=cy;

		//����undo
		memcpy( UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
	}

	//�����˵�
	if( Button==RB_UP && !Key.KeyDown( DIK_LCONTROL ) )
	{
		PopupMenu();		
	}

	//��ʾ��ͼ
	map.ShowEditMap();

	//��ʾ����
	if( bShowCell )
		ShowCell(lpDDSBack);

	//Ԥ�Ե�ǰ����
	if( bShowCurrent && Cur_GroundPic!=BLOCK_CELL )
	{
		static Alpha=8;

		int x= (cx-map.SX) * CellWidth + map.DX;
		int y= (cy-map.SY) * CellHeight + map.DY;

		if( Key.KeyDown( DIK_LSHIFT ) )	//��ȡ
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(4, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_LALT) )	//�赲��ϵ
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(5, ShowCellW), ColorKey16, 15);
		}
		else if( Key.KeyDown(DIK_LCONTROL) )	//ɾ��
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(3, ShowCellW), ColorKey16, 0);
		}
		else if( Key.KeyDown(DIK_SPACE) )	//���
		{
			ABlt(lpDDSBack, x, y, lpDDSTools, 
					map._GetRectangle(6, ShowCellW), ColorKey16, 0);
		}
		//����ǵ�ѡ�Ļ�����������ģʽ
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
		font.PrintText(lpDDSBack,   0,   0, "����:(%d,%d)", point.x, point.y);
		font.PrintText(lpDDSBack,   0,  14, "����:(%d,%d)=%d", cx, cy, n);
		font.PrintText(lpDDSBack, 150,   0, "����:%4d[%4d(%d)][%4d(%d)]", Cur_Ground, map.Cell[n].Obj, map.Cell[n].ObjPic, map.Cell[n].Obj2, map.Cell[n].Obj2Pic);
		font.PrintText(lpDDSBack, 150,  14, "����:%4d[%4d(%d)]", Cur_Ground, map.Cell[n].Ground, map.Cell[n].GroundPic);
		font.PrintText(lpDDSBack,  80, 460, "��ǰ��:%d[%s]", Cur_Level, Level[Cur_Level]);
		font.PrintText(lpDDSBack, 550,   0, "Beg:%d", Beg_Ground);
		font.PrintText(lpDDSBack, 550,  20, "End:%d", Cur_Ground);
		font.PrintText(lpDDSBack, 550,  40, "Sel:%d", Sel_Ground);
	}
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

//ѡ��̬ͼ��
void SelectGround(char Button)
{
	//�Ϸ�ҳ
	if( key==DIK_A )
	{
		Cur_GroundPic--;
		if( Cur_GroundPic < 0 ) 
			Cur_GroundPic=Max_GroundPic-1;
		Cur_GroundY=0;
	}

	//�·�ҳ
	if( key==DIK_Z )
	{
		Cur_GroundPic++;
		if( Cur_GroundPic >= Max_GroundPic ) 
			Cur_GroundPic=0;
		Cur_GroundY=0;
	}

	//���¹���
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

	//�������
	if( Button==LB_DOWN )
	{
		Beg_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		bShowSelect=true;
	}

	//����ɿ�
	if( Button==LB_UP )
	{
		Cur_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		Sel_Ground=0;
		Cur_Level=Cur_EditLevel;	//���ر༭
		bShowSelect=false;
	}
	
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
		bShowSelect=false;
	}

	//��ʾ
	RECT rect={0, Cur_GroundY, ScreenWidth, Cur_GroundY+ScreenHeight};
	Blt(lpDDSBack, 0, 0, lpDDSMap[Cur_GroundPic], rect, false);

	//��ʾ����
	if( bShowCell )
		ShowCell(lpDDSBack);

	//��ѡ����
	int x1=Beg_Ground % ShowCellW;
	int y1=Beg_Ground / ShowCellW;
	int x2=map._GetCellX(point.x, point.y+Cur_GroundY);
	int y2=map._GetCellY(point.x, point.y+Cur_GroundY);

	//��ʾѡ�񷽿�
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
	font.PrintText(lpDDSBack, 5, 5, "[A]=��һҳ [Z]=��һҳ");
	font.PrintText(lpDDSBack, 80, 460, "��ǰ��:%d[%s]", Cur_Level, Level[Cur_Level]);
	font.PrintText(lpDDSBack, 260, 460, "��ǰ����: %d / %d ҳ", map._GetCell(point.x, point.y+Cur_GroundY, ShowCellW), Cur_GroundPic);
	font.PrintText(lpDDSBack, 550, 0, "Beg:%d", Beg_Ground);
	font.PrintText(lpDDSBack, 550, 20, "End:%d", Cur_Ground);
}

//ѡ��̬ͼ��
void SelectAni(char Button)
{
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
	}

	static bShowSelect=false;

	//�������
	if( Button==LB_DOWN )
	{
		Beg_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		bShowSelect=true;
	}

	//����ɿ�
	if( Button==LB_UP )
	{
		Cur_Ground=map._GetCell(point.x, Cur_GroundY+point.y, ShowCellW);
		Sel_Ground=0;
		Cur_GroundPic = 60;
		Cur_Level=Cur_EditLevel;	//���ر༭
		bShowSelect=false;
	}
	
	if( Button==RB_UP )
	{
		Cur_Level=Cur_EditLevel;
		bShowSelect=false;
	}

	// �ض�anilist
	if( key == DIK_R )
	{
		map.LoadAniCell("ini\\anicell.ini", "");
	}

	//��ʾ
	Clrscr(lpDDSBack);

	for(int num=0; num<CMap::AniCellNum; num++)
	{
		RECT rect = {0,0,0,0};
		long page = map.GetAniPage(num, rect);
		BltFast(lpDDSBack, num%20*CellWidth, num/20*CellWidth, lpDDSMap[ page ], rect, true);
	}

	//��ʾ����
	if( bShowCell )
		ShowCell(lpDDSBack);

	font.SetColor(RGB(255,255,255));
	font.PrintText(lpDDSBack, 80, 460, "��ǰ��:%d[%s]", Cur_Level, Level[Cur_Level]);
	font.PrintText(lpDDSBack, 260, 460, "��ǰ����: %d / %d ҳ", map._GetCell(point.x, point.y+Cur_GroundY, ShowCellW), Cur_GroundPic);
	font.PrintText(lpDDSBack, 550, 0, "Beg:%d", Beg_Ground);
	font.PrintText(lpDDSBack, 550, 20, "End:%d", Cur_Ground);
	font.PrintText(lpDDSBack, 550, 40, "R : �ض�����ͼ���б�");

	map.AniLoop();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//��ʾ����
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

//��ձ༭����
void ClearEditData()
{
}

//�����˵�
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
	
	//��ʾ����
	if( bShowCell )
		CheckMenuItem(pmm, ID_MENUITEM_ShowCell, MF_CHECKED);

	//��ʾ����
	if( bShowObject )
		CheckMenuItem(pmm, ID_MENUITEM_ShowObject, MF_CHECKED);

	//��ʾ�赲��ϵ
	if( bShowBlock )
		CheckMenuItem(pmm, ID_MENUITEM_ShowBlock, MF_CHECKED);

	//��ʾ��ǰͼ��
	if( bShowCurrent )
		CheckMenuItem(pmm, ID_MENUITEM_ShowCurrent, MF_CHECKED);

	//��ʾ����
	if( bShowEdit )
		CheckMenuItem(pmm, ID_MENUITEM_ShowEdit, MF_CHECKED);

	//ֻ��ʾ����
	if( bShowOnlyObject )
		CheckMenuItem(pmm, ID_MENUITEM_ShowObjectOnly, MF_CHECKED);

	//�����
	if( Cur_Level == CL_GROUND )
		CheckMenuItem(pmm, ID_MENUITEM_Ground, MF_CHECKED);

	//�����1
	if( Cur_Level == CL_OBJECT )
		CheckMenuItem(pmm, ID_MENUITEM_Object, MF_CHECKED);

	//�����1
	if( Cur_Level == CL_OBJECT2 )
		CheckMenuItem(pmm, ID_MENUITEM_Object2, MF_CHECKED);
	
	TrackPopupMenu(pmm, TPM_LEFTALIGN|TPM_RIGHTBUTTON,
						point.x+l, point.y+t,
						0,
						hWnd,
						NULL);
	bMenuPop=true;
}

//����༭���Ҽ��˵���Ϣ
void MapEditProc(long msg)
{
	switch (msg)
	{
	//ѡ��̬ͼ��
	case ID_MENUITEM_SelGround:
		if( Cur_Level < 3 )	Cur_EditLevel = Cur_Level;
		Cur_Level=CL_PIC_SEL;

		if( Cur_GroundPic >= 60 )
		{
			Cur_GroundPic = 0;
			Cur_Ground = 0;
		}

		break;

	//ѡ��̬ͼ��
	case ID_MENUITEM_SelAni:
		if( Cur_Level < 3 )	
			Cur_EditLevel = Cur_Level;
		Cur_Level=CL_ANI_SEL;
		break;

	//���õ���
	case ID_MENUITEM_Enemy:
		CreateDlg(IDD_DIALOG_EnemySet, DialogEnemySetListProc);
		break;

	//��������
	case ID_MENUITEM_Cell:
		CreateDlg(IDD_DIALOG_Object, DialogObjectProc);
		break;

	//��������
	case ID_MENUITEM_SetHook:
		CreateDlg(IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);
		break;

	//ɾ������
	case ID_MENUITEM_DelHook:
		map.Cell[map.Width*cy+cx].CP=BLOCK_CP;
		break;

	//���˵�
	case ID_MENUITEM_Map:
		CreateDlg(IDD_DIALOG_Main, (DLGPROC)DialogProc);
		break;
	
	//��ʾ����ͼ
	case ID_MENUITEM_FullMap:
		map.ShowFullMap();
		break;

	//��ʾ����
	case ID_MENUITEM_ShowCell:
		bShowCell=!bShowCell;
		break;

	//��ʾ����
	case ID_MENUITEM_ShowObject:
		bShowObject=!bShowObject;
		break;

	//��ʾ�赲��ϵ
	case ID_MENUITEM_ShowBlock:
		bShowBlock=!bShowBlock;
		break;

	//��ʾ��ǰͼ��
	case ID_MENUITEM_ShowCurrent:
		bShowCurrent=!bShowCurrent;
		break;

	//��ʾ����
	case ID_MENUITEM_ShowEdit:
		bShowEdit=!bShowEdit;
		break;

	//ֻ��ʾ����
	case ID_MENUITEM_ShowObjectOnly:
		bShowOnlyObject=!bShowOnlyObject;
		break;

	//�༭���-����
	case ID_MENUITEM_Ground:
		Cur_Level=CL_GROUND;
		break;

	//�༭���-����1
	case ID_MENUITEM_Object:
		Cur_Level=CL_OBJECT;
		break;

	//�༭���-����2
	case ID_MENUITEM_Object2:
		Cur_Level=CL_OBJECT2;
		break;

	//��ȡ��ͼ-new
	case ID_MENUITEM_New:
		_NewMap(hWnd);
		break;

	//��ȡ��ͼ-open
	case ID_MENUITEM_Load:
		_LoadMap(hWnd);
		break;

	//��ȡ��ͼ-save
	case ID_MENUITEM_Save:
		_SaveMap(hWnd);
		break;

	//��ȡ��ͼ-saveas
	case ID_MENUITEM_SaveAs:
		_SaveAsMap(hWnd);
		break;

	//�˳�
	case ID_MENUITEM_Exit:
		RunGame=false;
		break;

	//����
	case ID_MENUITEM_Undo:
		{
			Cell_Struct *tmp=map.Cell;
			map.Cell=UndoMap;
			UndoMap=tmp;
		}
		break;

	//ȡ��
	case ID_MENUITEM_Cancel:
		break;

	//�˳�
	case IDM_Exit:
		RunGame=false;
		break;
	}
}
