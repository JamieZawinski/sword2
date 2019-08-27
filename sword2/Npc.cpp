//********************************************
//	Npc��ɫ ��غ���
//  ������2000��4��7��
//********************************************
#include "gamelib\goldpoint.h"
#include "main.h"
#include "role.h"
#include "map.h"
#include "npc.h"
//ע�Ͳο�CRole��
//��ȡ��ɫ�趨
bool CNpc::LoadNpcIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	//------ͨ������----------
	Id=Ini.ReadInt(index,"Id");					//��ɫ���
	char* strTemp = Ini.ReadText(index, "Name");
	strcpy(Name, strTemp);						//����
	free(strTemp);
	Width=Ini.ReadInt(index,"Width");
	Height=Ini.ReadInt(index,"Height");			//���ųߴ�
	Max_Step=Ini.ReadInt(index,"Max_Step");		//ÿ���������ߵĲ���
	Max_Dir=Ini.ReadInt(index,"Max_Dir");		//������

	//����ͼƬ�任�б�
	char _Steps[32];
	strTemp = Ini.ReadText(index, "StepList");
	strcpy(_Steps, strTemp);
	_FREE(strTemp);
	for(int i=0; i<Max_Step; i++)
	{
		StepList[i]=Ini.ReadInt(_Steps, i);
	}

	//���߷����б�
	strTemp = Ini.ReadText(index, "DirList");
	strcpy(_Steps, strTemp);
	_FREE(strTemp);
	for(i=0; i<4; i++)
	{
		DirList[i]=Ini.ReadInt(_Steps, i);
	}

	//�ŵ���ײ����
	FootRect[0].x=-8;
	FootRect[0].y=-8;
	FootRect[1].x=8;
	FootRect[1].y=-8;
	FootRect[2].x=-8;
	FootRect[2].y=8;
	FootRect[3].x=8;
	FootRect[3].y=8;

	StepSize=Ini.ReadInt(index,"StepSize");		//ÿ���ߵľ���
	StepSize2=Ini.ReadInt(index,"StepSize2");	//б���ߵľ���

	//��Ӧ�ű�
	strTemp = Ini.ReadText(index, "Script");
	strcpy( ScriptFile, strTemp );
	_FREE(strTemp);

	//------״̬����----------
	State.x=Ini.ReadInt(index,"x");			//��ǰ���� �ŵ����ĵ�
	State.y=Ini.ReadInt(index,"y");
	State.oldx=Ini.ReadInt(index,"oldx");
	State.oldy=Ini.ReadInt(index,"oldy");				//��һ��������
	State.CurrentStep=Ini.ReadInt(index,"CurrentStep");	//��ǰ����
	State.CurrentDir=Ini.ReadInt(index,"CurrentDir");	//��ǰ����
	State.LastDir=Ini.ReadInt(index,"LastDir");			//��һ���ķ���
	State.bActive=Ini.ReadInt(index,"bActive");			//�Ƿ�
	State.bShow=Ini.ReadInt(index,"bShow");				//�Ƿ�ɼ�
	
	//ȡ��������
	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);
	//�赲��ϵ
	map.Block[map.Width*State.Y+State.X] = 1;

	//���ߵ�ҳ��
	strTemp = Ini.ReadText(index,"SurfaceFileName");
	strcpy(State.SurfaceFileName, strTemp);	//��Ӧ�ļ�
	_FREE(strTemp);
	CreateBitmap(State.Surface, 0, 0, State.SurfaceFileName);
	DDSetColorKey(State.Surface, ColorKey);
	
	//���ɽ�ɫӰ��ҳ��
	CreateShadow();

	//��ֹ
	bMoving=false;
	
	Path=NULL;
	StepStep=0;

	return true;
}

//*************************
//��ʾNPC tx,ty=��ǰ��ͼ����
void CNpc::Move(int nx, int ny)
{
	//���������С���
	if( bMoving)
	{
		int subx= _point_x(Path[PathCurrentStep].x) +(CellWidth>>1);
		int suby= _point_y(Path[PathCurrentStep].y) +(CellHeight>>1);

		if( abs(State.x-subx) <= 2 && abs(State.y-suby) <= 2 )
		{
			//������һ��
			if(PathCurrentStep<PathSteps-1&&map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width]==1)
			{
				map.Block[ map.Width*State.oldY + State.oldX ] = 0;//��һ�������ͨ��
				
				if(random(100)<30)				//һ���Ļ�������Ѱ·
				{								//�ο�CRole
					int xTemp = Path[PathCurrentStep+1].x;
					int yTemp = Path[PathCurrentStep+1].y;
					map.Cell[xTemp +yTemp*map.Width].Block=1;
					MoveTo(_point_x(DesX),_point_y(DesY));
					map.Cell[xTemp +yTemp*map.Width].Block=0;
				}
				map.Block[ map.Width*ny + nx ] = 1;		//�õ㲻��ͨ��
				State.oldX=State.X;
				State.oldY=State.Y;
				return;
			}
			if(PathCurrentStep<PathSteps-1)
				map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width] = 1;

			map.Block[ map.Width*State.oldY + State.oldX ] = 0;//��һ�������ͨ��
			map.Block[ map.Width*ny + nx ] = 1;		//�õ㲻��ͨ��
			State.oldX=State.X;
			State.oldY=State.Y;

			if( PathCurrentStep+1==PathSteps )	//Ŀ���
			{
				State.x=subx;
				State.y=suby;
				State.CurrentStep=0;
				bMoving=false;
				MoveX=0;
				MoveY=0;

				return;
			}
			else	//��һ��
			{
				int mx=nx - Path[PathCurrentStep+1].x;
				int my=ny - Path[PathCurrentStep+1].y;
				if( mx == 0 && my>0 )	//��
				{
					State.CurrentDir=2;
					MoveX=0;
					MoveY=-StepSize;
				}
				else if( mx<0 && my==0 )	//��
				{
					State.CurrentDir=3;
					MoveX=StepSize;
					MoveY=0;
				}
				else if( mx==0 && my<0 )	//��
				{
					State.CurrentDir=0;
					MoveX=0;
					MoveY=StepSize;
				}
				else if( mx>0 && my==0 )	//��
				{
					State.CurrentDir=1;
					MoveX=-StepSize;
					MoveY=0;
				}

				//����ı��˷���Ͷ������ĵ�
				if( State.CurrentDir!=State.LastDir )
				{
					State.x=subx;
					State.y=suby;
					State.LastDir=State.CurrentDir;
				}
			}
	
			PathCurrentStep++;	//������һ
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
//��ʾ��ɫ
void CNpc::ShowNpcPic(int nx, int ny, int NumId)
{
	//�������
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*DirList[State.CurrentDir];

	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//��ʾӰ��
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*DirList[State.CurrentDir];
	RECT r={x2, y2, x2+State.SW, y2+State.SH};
	ABlt(lpDDSBack, sx, sy+Height-6, State.lpShadow, r, 31, 20);

	//��ʾ��
	RECT bs={ x1, y1, x1+Width, y1+Height};
	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);
	//��ʾ��Ե
	static WORD ColorFriend=RGB16(0,255,0);
	if(cx==State.X && cy==State.Y )
	{
		Edge_Blt(lpDDSBack, sx, sy, State.Surface, bs, ColorKey16, ColorFriend);
	}

}
