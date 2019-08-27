//********************************************
//	��Ϣ���� ��غ���
//  ������2000��5��8��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "fight\fight.h"
#include "menu\title.h"
#include "main.h"
#include "events.h"
#include "talk.h"
#include "map.h"
#include "role.h"
#include "npc.h"
#include "script.h"
#include "message.h"

//��ʼ��
CMessage::CMessage()
{
	ClearAllMessage();				//���
	m_bMessageRunning=false;		//û����Ϣ������
}

//���������Ϣ
void CMessage::ClearAllMessage()
{
	m_nStartId=m_nEndId=0;			//ͷβָ��
}

//��Ϣ�����Ƿ�Ϊ��
bool CMessage::IsEmpty()
{
	if( m_nStartId==m_nEndId )
		return true;
	else 
		return false;
}

//����һ����Ϣ
void CMessage::PutMessage(stMessage Message)
{
	m_stMsg[m_nEndId]=Message;		//���뵽������
	m_nEndId++;						//βָ���1
	if( m_nEndId >= MAX_MSG )
		m_nEndId=0;					//ѭ��
}

//����һ����Ϣ
void CMessage::PutMessage(enum MESSAGE Class, int p1, int p2, int p3, char *str1, char *str2, char *str3)
{
	//���ýṹ
	m_stMsg[m_nEndId].msg=Class;
	m_stMsg[m_nEndId].Param1=p1;
	m_stMsg[m_nEndId].Param2=p2;
	m_stMsg[m_nEndId].Param3=p3;
	m_stMsg[m_nEndId].String1=str1;
	m_stMsg[m_nEndId].String2=str2;
	m_stMsg[m_nEndId].String3=str3;
	
	m_nEndId++;
	if( m_nEndId >= MAX_MSG )
		m_nEndId=0;
}

//���һ����Ϣ����
enum MESSAGE CMessage::GetMessage(int Id)
{
	if( Id==-1 ) 
		Id=m_nStartId;	//ȱʡ��Ϣ

	if( Id < 0 || Id >= MAX_MSG ) 
		return MSG_NULL;

	if( m_nEndId==Id ) 
		return MSG_NULL;	//��Ϣ����Ϊ��

	return m_stMsg[Id].msg;
}

//ɾ��һ����Ϣ
void CMessage::DeleteMessage(int Id)
{
	if( Id==-1 ) 
		Id=m_nStartId;	//ȱʡ��Ϣ

	if( Id < 0 || Id >= MAX_MSG ) 
		return;

	if( m_nEndId==Id ) 
		return;		//��Ϣ����Ϊ��

	//�ͷ��ַ�������
	_FREE( m_stMsg[Id].String1 );
	_FREE( m_stMsg[Id].String2 );
	_FREE( m_stMsg[Id].String3 );

	m_nStartId++;				//ͷָ������ƶ�һ��
	if( m_nStartId>=MAX_MSG )
		m_nStartId=0;
}

//ִ��
void CMessage::Run()
{
	//-----------------------///////////////////////
	//Message.RunMessage();	//   ����ǰ��Ϣ	 //
	//---------------------///////////////////////

	//-----------------------//////////////////////
	//Message.BackMessage();// ������Ϣ�������� //
	//---------------------//////////////////////

	// ���SetPlayState(0)������ִ����Ϣ��һֱִ�е�SetPlayState(1)Ϊֹ
	do
	{
		if( m_nStartId == m_nEndId ) 
			return;	//��Ϣ����Ϊ��

		RunMessage();		//ִ��һ����Ϣ
		BackMessage();		//����һ����Ϣ
	}
	while( g_ePlayState == PS_Null && m_nStartId != m_nEndId );		//����ִ��
}

//ִ��һ����Ϣ
bool CMessage::RunMessage(int Id)
{
	if( m_bMessageRunning ) 
		return false;			//����Ϣ����ִ��

	if( Id==-1 ) 
		Id=m_nStartId;						//ȱʡ��Ϣ

	if( Id < 0 || Id >= MAX_MSG ) 
		return false;		//��ϢԽ��

	switch( m_stMsg[Id].msg )
	{
	case MSG_BREAK:
		return true;
	case MSG_BREAKSCRIPT:	//����ִ�нű�
		{
			for(int i=Script.RecursionLevel; i>=0; i--)
			{
				//�����ϴ���ͣ�ĵط�ִ�У������������ͣ���߽ű���ɾ��˳�
				if( Script.Run( Script.RecursionFile[i], "",
						Script.RecursionPoint[i] ) == CScript::SR_RECURSION )
					break;
			}
		}
		break;

	case MSG_SETVARIABLE:	//���ýű�������ֵ
		Script.SetValue( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_SETPLAYSTATE:	//��������״̬
		g_ePlayState=(enum ePLAYSTATE)m_stMsg[Id].Param1;
		break;

	case MSG_SETDISPLAYSTATE:	//������ʾ״̬
		g_eDisplayState=(enum eDISPLAYSTATE)m_stMsg[Id].Param1;
		break;

	case MSG_BACKTOTITLE:	//����Ϸ���˵�
		Title.DoMenu(lpDDSBack);
		break;

	case MSG_TEXTTITLE:		//��ʾ��Ļ
		Events.TextTitle( m_stMsg[Id].String1, m_stMsg[Id].String2 );
		break;

	case MSG_FIGHT:			//ս��
		{
			FadeIn(2, 20, RGB16(255,255,255));		//��Ļ���
			//��������
			if( strcmp(m_stMsg[Id].String1, "")==0 ) 
				m_stMsg[Id].String1=NULL;
			if( strcmp(m_stMsg[Id].String2, "")==0 ) 
				m_stMsg[Id].String2=NULL;
			if( strcmp(m_stMsg[Id].String3, "")==0 ) 
				m_stMsg[Id].String3=NULL;

			//��������
			stRolePropertiy *rps[3];
			if( m_stMsg[Id].Param1 == ERROR_DATA )	//û��ָ�����ǣ���ȫ����
			{
				rps[0]=&role[0].Propertiy;
				if( RoleNum >1 ) 
					rps[1]=&role[1].Propertiy;
				else rps[1]=NULL;

				if( RoleNum >2 ) 
					rps[2]=&role[2].Propertiy;
				else rps[2]=NULL;
			}
			else
			{										//ֻ��ָ��������
				if (m_stMsg[Id].Param1 != -1 ) 
					rps[0]=&role[m_stMsg[Id].Param1].Propertiy;
				else rps[0]=NULL;

				if( m_stMsg[Id].Param2 != -1 && m_stMsg[Id].Param2 != ERROR_DATA ) 
					rps[1]=&role[m_stMsg[Id].Param2].Propertiy;
				else rps[1]=NULL;

				if( m_stMsg[Id].Param3 != -1 && m_stMsg[Id].Param3 != ERROR_DATA ) 
					rps[2]=&role[m_stMsg[Id].Param3].Propertiy;
				else rps[2]=NULL;
			}

			Fight.bLeave=false;			//��������
			//ֻ֧���������ˣ�2λ�ò�������
			Fight.Start("ini\\fight\\background1.ini", rps[0], rps[1], rps[2], 
				m_stMsg[Id].String1, NULL, m_stMsg[Id].String2, m_stMsg[Id].String3);

			FadeOut(2);
		}
		break;

	case MSG_ENABLEFIGHT:	//�Ƿ�ȵ���
		Fight.bRandomFight=m_stMsg[Id].Param1;
		break;

	case MSG_BOSSFIGHT:	//�Ƿ�ȵ���
		Fight.SetBossFight(m_stMsg[Id].Param1);
		break;

	//-----------------------------------------------------------

	case MSG_LOADMAP:		//�����ͼ
		map.LoadMap( m_stMsg[Id].String1 );
		break;

	case MSG_SAVEMAP:		//�洢��ͼ
		map.SaveMap( m_stMsg[Id].String1 );
		break;

	case MSG_ADDMAPHOOK:	//��ӵ�ͼ����
		map.AddHook( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].String1);
		break;

	case MSG_SETMAPHOOK:	//���õ�ͼ����
		map.SetHook( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3);
		break;

	case MSG_SETMAPCELL:	//���õ�ͼĳ��
		{
			int n1=(int)m_stMsg[Id].String1;
			int n2=(int)m_stMsg[Id].String2;
			map.SetCell( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3, n1, n2);
			m_stMsg[Id].String1=NULL;
			m_stMsg[Id].String2=NULL;
		}
		break;

	case MSG_SETMAPBLOCK:	//���õ�ͼ�赲
		map.SetBlock( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3 );
		break;

	case MSG_SETMAPLEVEL:	//���õ�ͼ���
		{
			int n1=(int)m_stMsg[Id].String1;
			map.SetLevel( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3, n1 );
			m_stMsg[Id].String1=NULL;
		}
		break;

	case MSG_CENTERMAP:		//���õ�ͼ���ĵ�
		map.CenterMap( m_stMsg[Id].Param1, m_stMsg[Id].Param2 );
		break;

	case MSG_SCROLLMAPTO:	//����ͼ��ĳ��
		bControl=false;
		m_bMessageRunning=true;		//�ȵ���ͼ����λ�˲�ִ����һ����Ϣ

		if( m_stMsg[Id].Param3==ERROR_DATA ) //û��ָ������
			m_stMsg[Id].Param3=8;

		map.ScrollTo( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3 );
		break;
	
	case MSG_LOADNPC:		//��ȡ��ͼ��NPC
		map.LoadIniNpcList( m_stMsg[Id].String1 );
		break;

//------------------------ROLE------------------------------------

	case MSG_TALK:			//�Ի���Ϣ
		talk.Talk(lpDDSBackGround, m_stMsg[Id].String1);	//�Ի�
		break;

	case MSG_SAY:			//˵һ�仰
		talk.Say(lpDDSBackGround, m_stMsg[Id].String1, m_stMsg[Id].String2, 
					m_stMsg[Id].Param1);
		break;

	case MSG_TALKDIR:		//�������Ǻ�npc�Ի�ʱ�ķ���
		{
			int px=role[0].State.X;
			int py=role[0].State.Y;
			int nx=map.m_vNpc[m_stMsg[Id].Param1]->State.X;
			int ny=map.m_vNpc[m_stMsg[Id].Param1]->State.Y;
			if( abs(px-nx) >= abs(py-ny) )	//����
			{
				if( px < nx )	//���������
				{
					role[0].State.CurrentDir=3;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=1;
				}
				else			//�������ұ�
				{
					role[0].State.CurrentDir=1;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=3;
				}
			}
			else	//����
			{
				if( py < ny )	//�������ϱ�
				{
					role[0].State.CurrentDir=0;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=2;
				}
				else			//�������±�
				{
					role[0].State.CurrentDir=2;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=0;
				}
			}		//���нű�
			Script.Run(m_stMsg[Id].String1);
			Message.PutMessage( MSG_SETNPCACTIVE, m_stMsg[Id].Param2,m_stMsg[Id].Param1, 1);		//npc�ָ��
		}
		break;
	
	case MSG_MOVE:			//������Ϣ
		bControl=false;
		m_bMessageRunning=true;			//���ǵ�λ�Ž���ִ��
		if(m_stMsg[Id].Param3)
		{
			map.Cell[m_stMsg[Id].Param1+m_stMsg[Id].Param2*map.Width].Block =1;
			role[0].MoveTo(m_stMsg[Id].Param1*CellWidth, m_stMsg[Id].Param2*CellHeight);
			map.Cell[m_stMsg[Id].Param1+m_stMsg[Id].Param2*map.Width].Block =0;
		}
		else
			role[0].MoveTo(m_stMsg[Id].Param1*CellWidth, m_stMsg[Id].Param2*CellHeight);
		break;

	case MSG_MOVEDIRECTTO:		//��һ�������ƶ�
		{
			bControl = false;
			m_bMessageRunning=true;
			int x = role[0].State.X*CellWidth;
			int y = role[0].State.Y*CellHeight;
			switch(m_stMsg[Id].Param1)		//���ݷ������Ŀ�ĵ�
			{
			case 0:
				y += m_stMsg[Id].Param2*CellHeight;
				break;
			case 1:
				x -= m_stMsg[Id].Param2*CellWidth;
				break;
			case 2:
				y -= m_stMsg[Id].Param2*CellHeight;
				break;
			case 3:
				x += m_stMsg[Id].Param2*CellWidth;
				break;
			default:
				return false;
			}

			role[0].MoveTo(x,y);
			break;
		}
	case MSG_SETDIR:		//�������﷽��
		role[0].State.CurrentDir=m_stMsg[Id].Param1;
		break;

	case MSG_SETPOSITION:	//��������λ��
		if( m_stMsg[Id].Param3 != ERROR_DATA )		//���ԭλ�õ��赲��ϵ
			map.Block[map.Width*role[0].State.Y+role[0].State.X]=0;
		role[0].bMoving = false;
		role[0].State.x=role[0].State.oldx=m_stMsg[Id].Param1*CellWidth+CellWidth/2;
		role[0].State.y=role[0].State.oldy=m_stMsg[Id].Param2*CellHeight+CellHeight/2;
		//ȡ���ǵĸ�������
		role[0].State.X=role[0].State.oldX=_grid_x(role[0].State.x);
		role[0].State.Y=role[0].State.oldY=_grid_y(role[0].State.y);
		//�����µĵ�ǰ��״̬
		role[0].State.CurrentStep = 0;
		//�����µ��赲��ϵ
		map.Block[map.Width*role[0].State.Y+role[0].State.X]=1;
		break;

	case MSG_CENTERPLAYER:	//������Ϊ��ͼ����
		map.CenterMap( role[0].State.X, role[0].State.Y );
		break;

	case MSG_SETPLAYERSHOW:	//����������ʾ���
		role[0].State.bShow=m_stMsg[Id].Param1;
		break;

	case MSG_LOADPLAYER:	//��ini�ļ���������
		role[m_stMsg[Id].Param1].LoadRoleIni(m_stMsg[Id].String1, m_stMsg[Id].String2);
		break;

	case MSG_SWITCHPLAYER:	//��������
		{
			int r1=m_stMsg[Id].Param1;
			int r2=m_stMsg[Id].Param2;
			//�ȱ���
			role[r1].SaveRole("save\\temp1.rol");
			role[r2].SaveRole("save\\temp2.rol");
			//��������
			role[r1].LoadRole("save\\temp2.rol");
			role[r2].LoadRole("save\\temp1.rol");

			if( r2==0 )
				goto _Next;
			else 
				if( r1==0 )
				{ 
					r1=r2;				
					r2=0;		
				}
			else 
				break;

_Next:
			//�̳й���
			role[r2].Propertiy.Money=role[r1].Propertiy.Money;	//��Ǯ
			role[r2].Propertiy.CurGoodsNum=role[r1].Propertiy.CurGoodsNum;	//��Ʒ����
			//��Ʒ�����ɴ�64�֣�
			memcpy(role[r2].Propertiy.Goods, role[r1].Propertiy.Goods, MAXGOODS*2);
			//ÿ����Ʒ������
			memcpy(role[r2].Propertiy.GoodsNum, role[r1].Propertiy.GoodsNum, MAXGOODS*2);	
			role[r2].Propertiy.CurArmNum=role[r1].Propertiy.CurArmNum;	//װ������
			//װ�������ɴ�32�֣�
			memcpy(role[r2].Propertiy.Arm, role[r2].Propertiy.Arm, MAXARM*2);				
			//ÿ��װ��������
			memcpy(role[r2].Propertiy.ArmNum, role[r2].Propertiy.ArmNum, MAXARM*2);			
		}
		break;

	case MSG_SETPLAYERNUM:	//�������Ǹ���
		RoleNum=m_stMsg[Id].Param1;
		break;

	case MSG_ADDMONEY:		//�ı��Ǯ
		role[0].Propertiy.Money+=m_stMsg[Id].Param1;
		break;

	case MSG_ADDOBJECT:		//�������
		if( m_stMsg[Id].Param1==ERROR_DATA ) //��Ŀ
			m_stMsg[Id].Param1=1;

		role[0].AddObject( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_DELOBJECT:		//ɾ������
		if( m_stMsg[Id].Param1==ERROR_DATA ) //��Ŀ
			m_stMsg[Id].Param1=1;

		role[0].DelObject( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_ADDMAGIC:		//���ħ��
		if(m_stMsg[Id].Param1 == ERROR_DATA)
			m_stMsg[Id].Param1 = 0;		//ȱʡ�ǵ�һ����ɫ
		role[m_stMsg[Id].Param1].AddMagic(m_stMsg[Id].String1);

		break;

	case MSG_DELMAGIC:		//���ħ��
		if(m_stMsg[Id].Param1 == ERROR_DATA)
			m_stMsg[Id].Param1 = 0;		//ȱʡ�ǵ�һ����ɫ
		role[m_stMsg[Id].Param1].DelMagic(m_stMsg[Id].String1);

		break;
//------------------------NPC------------------------------------

	case MSG_MOVENPCTO:		//NPC�ƶ�
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);	//�õ�NPC�ı��
			if( num != -1 )
			{
				map.m_vNpc[num]->MoveTo(m_stMsg[Id].Param1*CellWidth, 
					m_stMsg[Id].Param2*CellHeight);

				m_stMsg[Id].Param1=num;					//���棬��BackMessageʱʹ��
				bControl=false;							//���ܿ���
				m_bMessageRunning=true;					//�ȵ�NPC�ƶ���λ
			}
		}
		break;

	case MSG_MOVENPC:		//NPC�ƶ�
		{
  			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.m_vNpc[num]->MoveTo(m_stMsg[Id].Param1*CellWidth, 
				m_stMsg[Id].Param2*CellHeight);
		}
		break;

	case MSG_SETNPCPOS:		//����npcλ��
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
			{
				if( m_stMsg[Id].Param3 != ERROR_DATA )
					map.Block[map.Width*map.m_vNpc[num]->State.Y+
						map.m_vNpc[num]->State.X]=0;

				map.m_vNpc[num]->State.x=map.m_vNpc[num]->State.oldx=
					m_stMsg[Id].Param1*CellWidth+CellWidth/2;

				map.m_vNpc[num]->State.y=map.m_vNpc[num]->State.oldy=
					m_stMsg[Id].Param2*CellHeight+CellHeight/2;
				
				//ȡ��������
				map.m_vNpc[num]->State.X=map.m_vNpc[num]->State.oldX=
					_grid_x(map.m_vNpc[num]->State.x);

				map.m_vNpc[num]->State.Y=map.m_vNpc[num]->State.oldY=
					_grid_y(map.m_vNpc[num]->State.y);
				//�赲
				map.Block[map.Width*map.m_vNpc[num]->State.Y
					+map.m_vNpc[num]->State.X]=1;
			}
		}
		break;

	case MSG_SETNPCDIR:		//����npc����
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
				map.m_vNpc[num]->State.CurrentDir=m_stMsg[Id].Param1;
		}
		break;

	case MSG_SETNPCSTEP:	//����npc����
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
				map.m_vNpc[num]->State.CurrentStep=m_stMsg[Id].Param1;
		}
		break;

	case MSG_ADDNPC:		//����NPC
		{
			bool *BackCell=NULL;
			int x=m_stMsg[Id].Param1;
			int y=m_stMsg[Id].Param2;

			//�������ݣ���ͼ�赲��ϵ
			if( x != ERROR_DATA && y != ERROR_DATA )
			{
				BackCell=new bool[map.Width*map.Height];
				for(int i=0; i<map.Width*map.Height; i++)
				{
					BackCell[i]=map.Block[i];
				}
			}
			//���
			int num=map.AddNpc(m_stMsg[Id].String1, m_stMsg[Id].String2);

			if( num != ERROR_DATA )		//λ��
				if( x != ERROR_DATA && y != ERROR_DATA )
				{
					//����TILE����
					int n=map.m_vNpc[num]->State.X+map.m_vNpc[num]->State.Y*map.Width;
					//�ָ��赲
					map.Block[n]=BackCell[n];
					//�ŵ���������
					map.m_vNpc[num]->State.x=StartX+x*CellWidth+CellWidth/2;
					map.m_vNpc[num]->State.y=StartY+y*CellHeight+CellHeight/2;
					//����λ�õ��赲
					map.Block[map.Width*y+x]=1;
					//��ͼλ��
					map.m_vNpc[num]->State.X = map.m_vNpc[num]->State.oldX = x;
					map.m_vNpc[num]->State.Y = map.m_vNpc[num]->State.oldY = y;
				}
			
			_DELETE( BackCell );		//�ͷ�
		}
		break;

	case MSG_DELNPC:		//ɾ��NPC
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.DelNpc(num);
		}
		break;

	case MSG_SETNPCACTIVE:	//����NPC����
		if( m_stMsg[Id].Param3 == 1 )	//Param2�Ǳ��
		{
			map.m_vNpc[m_stMsg[Id].Param2]->State.bActive=m_stMsg[Id].Param1;
		}
		else	//ͨ�����ֻ�ȡ
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
			{
				map.m_vNpc[num]->State.bActive=m_stMsg[Id].Param1;
			}
		}
		break;

	case MSG_SETNPCSHOW:	//����NPC����
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.m_vNpc[num]->State.bShow=m_stMsg[Id].Param1;
		}
		break;

	case MSG_SETNPCSCRIPT:	//����NPC�ű��ļ�
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				strcpy(map.m_vNpc[num]->ScriptFile, m_stMsg[Id].String2);;
		}
		break;

//------------------------TOOLS-----------------------------------

	case MSG_DELAY:			//��ʱ
		bControl=false;		//���ܿ���
		m_bMessageRunning=true;		//�ȴ�����
		m_stMsg[Id].Param2=timeGetTime();	//���浱ǰʱ�䣬�ȵ�BackMessageʱʹ��
		break;

	case MSG_FADEIN:		//����
		{
			int time=1;
			WORD color;

			//��ɫ
			if( m_stMsg[Id].Param2 == ERROR_DATA )
				color=0;
			else
				color=RGB16(m_stMsg[Id].Param2);

			//��ʱ
			if( m_stMsg[Id].Param3 == ERROR_DATA )
			{
				if( m_stMsg[Id].Param1 == 1 ) 
					time=20;

				else 
					if( m_stMsg[Id].Param1 == 2 ) 
						time=50;
			}
			else
				time=m_stMsg[Id].Param3;

			map.FadeIn(m_stMsg[Id].Param1, time, color);
		}
		break;

	case MSG_FADEOUT:		//����
		{
			int time=1;

			//��ʱ
			if( m_stMsg[Id].Param2 == ERROR_DATA )
			{
				if( m_stMsg[Id].Param1 == 1 ) 
					time=20;
				else 
					if( m_stMsg[Id].Param1 == 2 ) 
						time=50;
			}
			else
				time=m_stMsg[Id].Param2;

			map.FadeOut(m_stMsg[Id].Param1, time);
		}
		break;

	case MSG_PLAYMUSIC:		//���ű�������
		{
			Music.Stop();
			WCHAR* str = WChar(m_stMsg[Id].String1);
			Music.LoadMusic(str);
			_DELETE( str );
			Music.Play();
		}
		break;

	case MSG_STOPMUSIC:		//ֹͣ���ű�������
		Music.Stop();
		break;

	case MSG_PAUSEMUSIC:	//��ͣ����
		Music.Pause();
		break;

	case MSG_RESUMEMUSIC:	//�ָ�����
		Music.Resume();
		break;

	case MSG_PLAYSOUND:		//������Ч
		Sound.LoadSoundFX( hWnd, m_stMsg[Id].String1 );
		Sound.Play();
		break;

	case MSG_STOPSOUND:		//ֹͣ����
		Sound.Stop();
		break;

	case MSG_GAMEMESSAGE:	//��ʾһ����Ϸ��Ϣ
		GameMessage.DoMenu(lpDDSBack, m_stMsg[Id].String1);
		break;

	case MSG_SHOWMESSAGE:	//��ʾһ����Ϣ
		ShowMessage(m_stMsg[Id].String1, m_stMsg[Id].Param1, 
				m_stMsg[Id].Param2, m_stMsg[Id].Param3, 
				m_stMsg[Id].String2, m_stMsg[Id].String3);
		break;

	case MSG_SETCPTYPE:
		map.SetCPType(m_stMsg[Id].Param1,m_stMsg[Id].Param2,m_stMsg[Id].Param3);
		break;
	}
	return true;
}

//��Ϣ��������
bool CMessage::BackMessage()
{
	switch( m_stMsg[m_nStartId].msg )
	{
	case MSG_SCROLLMAPTO:	//����ͼ
		if( g_ePlayState==PS_MAIN )		//������˻����ó�PS_MAIN���ο�Map.ScrollLoop()
		{
			bControl=true;				//��ҿ��Կ���
			m_bMessageRunning=false;	//����ִ��������Ϣ
			DeleteMessage();			//ɾ������Ϣ
		}
		break;
	
	case MSG_MOVE:			//������Ϣ
	case MSG_MOVEDIRECTTO:
		if( role[0].bMoving==false )	//�ƶ���λ	
		{
			bControl=true;
			m_bMessageRunning=false;
			DeleteMessage();
			if(role[0].bCancelTalk)
			{
				if(m_stMsg[m_nStartId].msg == MSG_TALKDIR)
					DeleteMessage();
			}
		}
		break;

	case MSG_MOVENPCTO:		//NPC������Ϣ
		//�ƶ���λ��
		//m_stMsg[m_nStartId].Param1������NPC��ID���ο�RunMessage�Ĵ���
		if( map.m_vNpc[m_stMsg[m_nStartId].Param1]->bMoving==false )
		{
			bControl=true;
			m_bMessageRunning=false;
			DeleteMessage();
		}
		break;

	case MSG_DELAY:			//��ʱ
		//m_stMsg[m_nStartId].Param2������DELAY��ʼʱ��ʱ�䣬�ο�RunMessage�Ĵ���
		if( timeGetTime() - m_stMsg[m_nStartId].Param2 >= 
				(unsigned int)m_stMsg[m_nStartId].Param1 )
		{
			bControl=true;
			m_bMessageRunning=false;
			DeleteMessage();
		}
		break;

	default:				//ȱʡɾ��������Ϣ
		DeleteMessage();
	}
	return true;
}
