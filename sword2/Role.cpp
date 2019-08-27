//********************************************
//	��ɫ ��غ���
//  ������2000��4��14��
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

//����
CRole::CRole()
{
	Path=NULL;
	State.Surface=State.lpShadow=NULL;
	bCuted = FALSE;
	bCancelTalk = FALSE;
}

//����
CRole::~CRole()
{
	_RELEASE( State.Surface );			//���ߵ�ҳ��
	_RELEASE( State.lpShadow );
	_DELETE_ARRAY( Path );
}

//�ͷ�
void CRole::Release()
{
	_RELEASE( State.Surface );
	_RELEASE( State.lpShadow );
	_DELETE_ARRAY( Path );
}

//***************
//��ʼ����ɫ
bool CRole::InitRole()
{
	strcpy(Name,"����");
	Width=32;
	Height=48;
	Max_Step=4;
	Max_Dir=4;
	//��������
	StepList[0]=0;
	StepList[1]=1;
	StepList[2]=0;
	StepList[3]=2;

	DirList[0]=0;
	DirList[1]=1;
	DirList[2]=2;
	DirList[3]=3;
	//λ��
	State.x=State.oldx=CenterX;
	State.y=State.oldy=CenterY;

	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);

	map.Block[map.Width*State.Y+State.X] = 1;
	StepSize=8;
	StepSize2=3;
	MoveX=0;
	MoveY=0;
	//�ŵ���ײ����
	FootRect[0].x=-8;		
	FootRect[0].y=-8;
	FootRect[1].x=8;		
	FootRect[1].y=-8;	
	FootRect[2].x=-8;		
	FootRect[2].y=8;
	FootRect[3].x=8;		
	FootRect[3].y=8;

	//ҳ��
	char* strTemp = GameIni.ReadText("��ɫ","Pic");
	strcpy(State.SurfaceFileName, strTemp);
	_FREE(strTemp);
	CreateBitmap(State.Surface, 0, 0, State.SurfaceFileName);
	DDSetColorKey(State.Surface, ColorKey);
	
	//���ɽ�ɫӰ��ҳ��
	CreateShadow();

	bMoving=false;
	State.CurrentStep=0;
	State.CurrentDir=2;
	State.LastDir=2;

	//��Ӧ�ű�
	strcpy(ScriptFile, "script\\null.gps");
	Path=NULL;
	StepStep=0;
	return true;
}

//��ȡ��ɫ�趨
bool CRole::LoadRoleIni(char *filename, char *index)
{
	CIniSet Ini(filename);

	//------ͨ������----------
	Id=Ini.ReadInt(index,"Id");					//��ɫ���
	char* strTemp = Ini.ReadText(index, "Name");
	strcpy(Name, strTemp);	//����
	_FREE(strTemp);
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
	strcpy( ScriptFile, strTemp);
	_FREE(strTemp);

	//����Ĳ����ṹ
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

	//��Ǯ
	Propertiy.Money=Ini.ReadInt(index,"Money");
	
	//������Ʒ
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

	//װ��
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

	//ħ��
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

	//ս��
	strcpy(Propertiy.Fight_Ini_Name, Name);

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
	
	//ȡ���ǵĸ�������
	State.X=State.oldX=_grid_x(State.x);
	State.Y=State.oldY=_grid_y(State.y);

	map.Block[map.Width*State.Y+State.X] = 1;
	//���ߵ�ҳ��
	strTemp = Ini.ReadText(index,"SurfaceFileName");
	strcpy(State.SurfaceFileName, strTemp );	//��Ӧ�ļ�
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

//***************
//����������Ӱ
bool CRole::CreateShadow()
{
	int m,n;

	int W,H;
	LoadBitmapFileInfo(hWnd, State.SurfaceFileName, W, H);
	int nRows = W / Width;
	int nLines = H / Height;
	int W2=W;			
	int H2=H/4;			
	State.SW=W2/nRows;	//��Ȳ���
	State.SH=Height/4;	//�߶�Ϊ1/4

	//������ɫӰ��ҳ��
	CreateBitmap(State.lpShadow, W2, H2, NULL, DDSCAPS_SYSTEMMEMORY);
	DDSetColorKey(State.lpShadow, ColorKey);
	FillSurface(State.lpShadow, ColorKey16);

	LPDIRECTDRAWSURFACE lpTemp;
	CreateBitmap(lpTemp, State.SW, State.SH, NULL, DDSCAPS_SYSTEMMEMORY);

	// �ֿ�ѹ��
	for(m=0; m<nLines; m++)
		for(n=0; n<nRows; n++)
		{
			RECT rt={n*State.SW, m*State.SH, n*State.SW+State.SW, (m+1)*State.SH};
			RECT rs={n*Width, m*Height, (n+1)*Width, (m+1)*Height};
			SBlt(State.lpShadow, rt, State.Surface, rs, false);
		}

	// ��������
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
			//��ɫ����
			for(i=0; i<State.SH; i++)
				for(j=0; j<State.SW; j++)
				{
					int p=(i+m*State.SH)*W2+(j+n*State.SW);
					if( pShadow[p] != ColorKey16 )
					{
						pShadow[p] = RGB16(0, 0, 0);		//ȫ�����ɺ�ɫ
					}
					else
					{
						pShadow[p] = 31;		//0x001f����ɫ��λ��Ϊ1��������ʾ��͸��ɫ
					}
				}
				
				//���·�ת
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
//�������ʱ��
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
	CreateShadow();		//���ɽ�ɫӰ��ҳ��


	//------�ƶ�����-----------
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
//������ȵ�ʱ��
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
	
	//------�ƶ�����-----------
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
//��ʾ��ɫ
void CRole::ShowRole()
{
	//�������

	//������ͼƬ�ϵ�λ��
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*State.CurrentDir;

	//��ʾ���ڵ�ͼ�ϵ�λ��

	//�ŵ�����x�����������˿�
	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;	
	//�ŵ�����y��������һ���˸ߣ��ټ��Ͻŵĸ߶�
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//��ʾӰ��
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*State.CurrentDir;

	RECT r={x2, y2, x2+State.SW, y2+State.SH};
	//��0x001fΪ͸��ɫ����������Ӱ�ӵĺ���
	//����һ��㣬��ΪӰ���в������ڽ��µ�
	ABlt(lpDDSBack, sx, sy+Height-6, State.lpShadow, r, 31, 20);

	//��ʾ��
	RECT bs={ x1, y1, x1+Width, y1+Height};

	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);

}

//*******************
//��͸����ʾ��ɫ
void CRole::ShowRole(int alpha)
{
	//�������
	int x1=Width*StepList[State.CurrentStep];
	int y1=Height*State.CurrentDir;
	//һ���ķ���
	int sx=State.x-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=State.y-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//��ʾ��
	RECT bs={ x1, y1, x1+Width, y1+Height};

	ABlt(lpDDSBack, sx, sy, State.Surface, bs, ColorKey16, alpha);
}

//*******************
//��ʾ��ɫ
void CRole::ShowRole(int lx, int ly, int lDir, int lStep)
{
	//�������
	int x1=Width*StepList[lStep];
	int y1=Height*lDir;
	//��������
	int sx=lx-(Width>>1)- _point_x(map.SX) +map.DX;
	int sy=ly-Height+(int)FootRect[3].y - _point_y(map.SY) +map.DY;

	//��ʾӰ��
	int x2=State.SW*StepList[State.CurrentStep];
	int y2=State.SH*State.CurrentDir;
	RECT r={x2, y2, x2+State.SW, y2+State.SH};

	ABlt(lpDDSBack, sx-16, sy+Height-4, State.lpShadow, r, 31, 20);

	//��ʾ��
	RECT bs={ x1, y1, x1+Width, y1+Height};
	Blt(lpDDSBack, sx, sy, State.Surface, bs, true);
}

//*************************
//��ʾ tx,ty=��ǰ��ͼ����
void CRole::Show(int tx, int ty)
{
	ShowRole();
}

//*******************************
//�ƶ��������ǵ�ǰλ�ã�������ͼ���������
void CRole::Move(int nx, int ny)
{
	//���������С���
	if( bMoving )
	{
		//��һ��������꣬�ڵ�ͼ���ӵ�����
		int subx= _point_x(Path[PathCurrentStep].x) +(CellWidth>>1);
		int suby= _point_y(Path[PathCurrentStep].y) +(CellHeight>>1);

		//������һ��
		if( abs(State.x-subx) <= 2 && abs(State.y-suby) <= 2 )
		{
			if(PathCurrentStep<PathSteps-1&&map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width]==1)
			{
				int xTemp = Path[PathCurrentStep+1].x;		//��һ����
				int yTemp = Path[PathCurrentStep+1].y;
				map.Cell[xTemp +yTemp*map.Width].Block=1;	//����ͨ��������Ѱ·��
				MoveTo(_point_x(DesX),_point_y(DesY));
				map.Cell[xTemp +yTemp*map.Width].Block=0;	//�ָ�

				map.Block[ map.Width*State.oldY + State.oldX ] = 0;//��һ�������ͨ��
				map.Block[ map.Width*ny + nx ] = 1;		//�õ㲻��ͨ��
				return;
			}
			if(PathCurrentStep<PathSteps-1)
				map.Block[Path[PathCurrentStep+1].x +Path[PathCurrentStep+1].y*map.Width] = 1;

			map.Block[ map.Width*State.oldY + State.oldX ] = 0;//��һ�������ͨ��
			map.Block[ map.Width*ny + nx ] = 1;		//�õ㲻��ͨ��
			if( PathCurrentStep+1==PathSteps )	//Ŀ���
			{
				State.x=subx;	//���µ�ǰ�ŵ�����
				State.y=suby;
				
				static bool left=true;
				if( left )	//�ȳ����
				{
					left=!left;
					State.CurrentStep=0;	//���������ֹͣ״̬����֤�����Ų���һ��
				}
				else		//�ȳ��ҽ�
				{
					left=!left;
					State.CurrentStep=2;
				}

				bMoving=false;				//ֹͣ�ƶ�
				MoveX=0;					//����仯Ϊ0
				MoveY=0;

				

				//���Ŀ��㲻�����ߵĻ�����ô����ת��Ŀ���
				if(	/*map.Cell[map.Width*DesY+DesX].Block == 1
					&&*/ ((DesX!=State.X)
					|| (DesY!=State.Y))&&!bCuted)
				{
					if( DesX > State.X ) 
						State.CurrentDir = 3;		//����ת
					else 
						if( DesX < State.X ) 
							State.CurrentDir = 1;	//����ת
					if( DesY > State.Y ) 
						State.CurrentDir = 0;		//����ת
					else 
						if( DesY < State.Y ) 
							State.CurrentDir = 2;	//����ת
					RunScript();
				}

				State.oldX=State.X;
				State.oldY=State.Y;
				
				//�жϵ�ǰ���Ƿ�Ϊ�����л���
				ChangeScene();
				return;
			}
			else	//����һ��
			{
				//�����ϸ�λ��
				State.oldX=State.X;
				State.oldY=State.Y;
				//��Ҫ�ƶ��ľ���
				int mx=nx - Path[PathCurrentStep+1].x;
				int my=ny - Path[PathCurrentStep+1].y;
				if( mx == 0 && my>0 )		//��
				{
					State.CurrentDir=2;
					MoveX=0;				//x������Ҫ�ƶ�
					MoveY=-StepSize;		//y�����ƶ�һ���ľ���
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
					State.x=subx;		//ֱ�Ӹ���
					State.y=suby;
					State.LastDir=State.CurrentDir;	//���淽��
				}
			}
			PathCurrentStep++;	//������һ
			RunScript();
			if(PathCurrentStep>1)
				ChangeScene();
		}
		//����
		switch( State.CurrentDir )
		{
		case 1:	//��
				//���������(1)��ͼ����Ļ��,���������ߵ������ĵ����ߣ�������ʱ��ͼ��X���0
				//��(��߻��в��ֵ�ͼû����ʾ����)��(2)��ʾ��ͼ��ͷ�ˣ��������в���û����ʾ����
			if( State.x <= _point_x(map.Width-ShowCellW)+CenterX && (map.SX > 0 
								|| (map.SX==0 && map.DX<0) ) )
			{
				map.DX+=StepSize;		//�ƶ������һ������
				if( map.DX>0 )			//�Ѿ��ƶ���һ����������
				{
					map.SX--;			//������һ������
					map.DX-=CellWidth;	//�ָ�DX��ֵ
				}
			}
			break;

		case 3:		//��,ͬ�����߼�
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

		case 2:		//��,����
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
		
		case 0:		//�£�����˵�ˣ���
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

		State.x+=MoveX;		//�ŵ�λ�ø���
		State.y+=MoveY;
		role[0].State.X=_grid_x(role[0].State.x);
		role[0].State.Y=_grid_y(role[0].State.y);

		StepStep++;
		if( StepStep==2 )
		{
			StepStep=0;			//2��һ��
			State.CurrentStep=(State.CurrentStep+1)%(Max_Step);
		}
	}
}

//*******************************
//�ƶ���ĳ��
bool CRole::MoveTo(int lx, int ly)
{
	CFindPath fp(&map);
	fp.Init(map.Width,map.Height);
	int Sx=State.X;
	int Sy=State.Y;

	//�߽���
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

	//Ŀ�ĵ�����
	DesX=_grid_x(lx);
	DesY=_grid_y(ly);

	//��������ƶ��Ͱ���һ����Ϊ��ʼ��
	if( bMoving )
	{
		Sx=Path[PathCurrentStep].x;
		Sy=Path[PathCurrentStep].y;
		::map.Block[Sx+Sy*::map.Width]=1;
	}
	//Ѱ·�ɹ�
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
//�ƶ���ĳ��
void CRole::Goto(int lx, int ly)
{
	//Ѱ·
	MoveTo(lx, ly);

	//�˵�ѭ��
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
			//��ȡ������Ϣ
			GetInputData();
			
			//��ʱ����
			while( timeGetTime() < unsigned int(OldTick+DelayTick) ) {}
			OldTick=timeGetTime();

			//ȡ���ǵĸ�������
			State.X=_grid_x(State.x);
			State.Y=_grid_y(State.y);

			//���㵱ǰ��ͼ���Ͻ�����
			map.sx=_point_x(map.SX)-map.DX;
			map.sy=_point_y(map.SY)-map.DY;

			map.ShowMap();
			UpdateScreen();

			Mouse.GetMouseData();		//��ȡ���������Ϣ
			Key.GetKeyData();			//��ȡ���̻�������(������ʱ����)
			Key.GetKey();

			ShowMouseCursor(lpDDSBack, false);
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
	bMoving=false;
}

//**************************
//�����л�
void CRole::ChangeScene()
{
	//�õ���������ڵĸ�������
	int nx=map._GetCellX(State.x,State.y);
	int ny=map._GetCellY(State.x,State.y);

	//�л�
	int cp=map.Cell[map.Width*ny+nx].CP;
	int cptype = map.Cell[map.Width*ny+nx].CPType;
	if( cp != BLOCK_CP&&cptype)				//�л���
	{
		char GpsFile[MAX_PATH];
		memset(GpsFile,0, MAX_PATH);
		sprintf(GpsFile,"%s%s", map.CurrentMapFilePath, map.Hook[cp]);
		Script.Run( GpsFile );	//ִ���л��ű�
	}
}

//**************************
//������ɫǰһ�������еĿ���
void CRole::RunScript()
{
	int lx, ly;
	switch( State.CurrentDir )
	{
	case 0:				//��
		lx=State.x;
		ly=State.y+32;
		break;

	case 1:				//��
		lx=State.x-32;
		ly=State.y;
		break;

	case 2:				//��
		lx=State.x;		
		ly=State.y-32;
		break;

	case 3:				//��
		lx=State.x+32;	
		ly=State.y;
		break;
	}

	int nx=map._GetCellX(lx,ly);
	int ny=map._GetCellY(lx,ly);
	//Խ��
	if( nx < 0 || ny < 0 ) 
		return;

	//�л�
	int cp=map.Cell[map.Width*ny+nx].CP;
	int cptype = map.Cell[map.Width*ny+nx].CPType;
	if( cp != BLOCK_CP&&cptype!=CP_CHANGE )			//����Ҫ�Ǹ����л���
	{
		char GpsFile[MAX_PATH];
		memset(GpsFile,0, MAX_PATH);
		sprintf(GpsFile,"%s%s", map.CurrentMapFilePath, map.Hook[cp]);
		AdjustPosition();		//����λ��
		Message.PutMessage(MSG_BREAK);		//���β�����
		Script.Run( GpsFile );	//ִ���л��ű�
	}
}

//-----------------------------------------------------------

//��������
void CRole::AddObject(char *name, int num)
{
	int goods=CGoods::GetGoodsNum(name);

	if(goods==-1)
		return;
	//�����װ��
	if( CMenu::Goods.GOODS[goods].Class >=0 
		&& CMenu::Goods.GOODS[goods].Class <=1 )
	{
		AddArm(name, num);
		return;
	}

	//�Ѿ����˾����
	for(int i=0; i<Propertiy.CurGoodsNum; i++)
	{
		if( Propertiy.Goods[i] == goods )
		{
			Propertiy.GoodsNum[i]+=num;
			return;
		}
	}

	//������
	Propertiy.Goods[Propertiy.CurGoodsNum]=goods;
	Propertiy.GoodsNum[Propertiy.CurGoodsNum]=num;
	Propertiy.CurGoodsNum++;
}

//ɾ������
void CRole::DelObject(char *name, int num)
{
	int goods=CGoods::GetGoodsNum(name);

	//�����װ��
	if( CMenu::Goods.GOODS[goods].Class >=0 
		&& CMenu::Goods.GOODS[goods].Class <=1 )
	{
		DelArm(name, num);
		return;
	}
	//����Ʒ
	if( goods != 0 )
	{
		for(int i=0; i<Propertiy.CurGoodsNum; i++)
		{
			if( Propertiy.Goods[i] == goods )
			{
				Propertiy.GoodsNum[i]-=num;

				//�Ѻ������Ʒ��ǰ�ƶ�һ��
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

//���һ����Ʒ������
int CRole::CheckObject(char *name)
{
	int goods=CGoods::GetGoodsNum(name);

	//�����װ��
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

//����װ��
void CRole::AddArm(char *name, int num)
{
	int arm=CGoods::GetGoodsNum(name);

	//���
	for(int i=0; i<Propertiy.CurArmNum; i++)
	{
		if( Propertiy.Arm[i] == arm )
		{
			Propertiy.ArmNum[i]+=num;
			return;
		}
	}

	//��װ��
	Propertiy.Arm[Propertiy.CurArmNum]=arm;
	Propertiy.ArmNum[Propertiy.CurArmNum]=num;
	Propertiy.CurArmNum++;
}

//ɾ������
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

				//�Ѻ������Ʒ��ǰ�ƶ�һ��
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

		// ������Ѿ�װ���˵�
		for(i=0; i<RoleNum; i++)
		{
			//���������
			if( role[i].Propertiy.Arms == arm )
			{
				role[i].Propertiy.Arms = 0;	//ж��װ��
				role[i].UnSetupArm(arm);
				return;
			}

			//����ǻ���
			else if( role[i].Propertiy.Corselet == arm )
			{
				role[i].Propertiy.Corselet = 0;
				role[i].UnSetupArm(arm);
				return;
			}
		}
	}
}

// ���һ��װ��������
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

		// ����Ѿ�װ���������˵�
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

//װ��
void CRole::SetupArm(char *name)
{
	int GoodNum=CGoods::GetGoodsNum(name);
	SetupArm(GoodNum);
}

//װ��
void CRole::SetupArm(int GoodNum)
{
	Propertiy.Magic+=CMenu::Goods.GOODS[GoodNum].Magic;			//ħ��

	Propertiy.Stamina+=CMenu::Goods.GOODS[GoodNum].Stamina;	//����
	Propertiy.Luck+=CMenu::Goods.GOODS[GoodNum].Luck;		//����
	Propertiy.Speed+=CMenu::Goods.GOODS[GoodNum].Speed;		//����
	Propertiy.Bp+=CMenu::Goods.GOODS[GoodNum].Bp;		//����
	Propertiy.Ip+=CMenu::Goods.GOODS[GoodNum].Ip;		//����

	ReCalcPro();
}

//ж��װ��
void CRole::UnSetupArm(char *name)
{
	int GoodNum=CGoods::GetGoodsNum(name);
	UnSetupArm(GoodNum);
}
//ж��װ��
void CRole::UnSetupArm(int GoodNum)
{
	Propertiy.Magic-=CMenu::Goods.GOODS[GoodNum].Magic;			//ħ��
	Propertiy.Stamina-=CMenu::Goods.GOODS[GoodNum].Stamina;	//����
	Propertiy.Luck-=CMenu::Goods.GOODS[GoodNum].Luck;		//����
	Propertiy.Speed-=CMenu::Goods.GOODS[GoodNum].Speed;		//����
	Propertiy.Bp-=CMenu::Goods.GOODS[GoodNum].Bp;			//����
	Propertiy.Ip-=CMenu::Goods.GOODS[GoodNum].Ip;			//����

	ReCalcPro();
}

//����λ��
void CRole::AdjustPosition()
{
	bCancelTalk = true;		//ȡ����NPC�ĶԻ�
	if(!bMoving)	//û���ƶ�
		return ;
	PathSteps = PathCurrentStep+1;	//��һ�������һ��
	bCuted = TRUE;			//·��������
}
//����һ��ħ��
void CRole::AddMagic(char *name)
{
	//�õ�ħ�����
	int magic = CMagic::GetMagicNum(name);

	if(magic==-1)
		return;

	for(int i=0; i<Propertiy.MagicsNum; i++)
	{
		if( Propertiy.Magics[i] == magic )//�������ħ��
		{
			return;
		}
	}
	//û��ѧ���ħ��
	Propertiy.Magics[Propertiy.MagicsNum]=magic;
	Propertiy.MagicsNum++;
}
//ɾ��ħ��
void CRole::DelMagic(char *name)
{
	// ħ�����
	int magic=CMagic::GetMagicNum(name);
	// ��ħ��
	if( magic >-1  )
	{
		for(int i=0; i<Propertiy.MagicsNum; i++)
		{
			if( Propertiy.Magics[i] == magic )
			{
				//�Ѻ����ħ����ǰ�ƶ�һ��
				
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
//���¼�������
void CRole::ReCalcPro()
{
	Propertiy.Attack = oldAttack;
	Propertiy.Defend = oldDefend;

	int GoodNum = Propertiy.Arms;
	Propertiy.Attack+=CMenu::Goods.GOODS[GoodNum].Attack;		//����
	Propertiy.Attack = (int)Propertiy.Attack*(100.0+CMenu::Goods.GOODS[GoodNum].AttackPer)/100.0;
	
	Propertiy.Defend+=CMenu::Goods.GOODS[GoodNum].Defend;		//����
	Propertiy.Defend = (int)Propertiy.Defend*(100.0+CMenu::Goods.GOODS[GoodNum].DefendPer)/100.0;

	GoodNum = Propertiy.Corselet;
	Propertiy.Attack+=CMenu::Goods.GOODS[GoodNum].Attack;		//����
	Propertiy.Attack = (int)Propertiy.Attack*(100.0+CMenu::Goods.GOODS[GoodNum].AttackPer)/100.0;
	
	Propertiy.Defend+=CMenu::Goods.GOODS[GoodNum].Defend;		//����
	Propertiy.Defend = (int)Propertiy.Defend*(100.0+CMenu::Goods.GOODS[GoodNum].DefendPer)/100.0;
}
