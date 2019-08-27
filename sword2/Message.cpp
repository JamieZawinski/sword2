//********************************************
//	消息处理 相关函数
//  创建于2000年5月8日
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

//初始化
CMessage::CMessage()
{
	ClearAllMessage();				//清空
	m_bMessageRunning=false;		//没有消息在运行
}

//清除所有消息
void CMessage::ClearAllMessage()
{
	m_nStartId=m_nEndId=0;			//头尾指针
}

//消息队列是否为空
bool CMessage::IsEmpty()
{
	if( m_nStartId==m_nEndId )
		return true;
	else 
		return false;
}

//加入一条消息
void CMessage::PutMessage(stMessage Message)
{
	m_stMsg[m_nEndId]=Message;		//加入到数组中
	m_nEndId++;						//尾指针加1
	if( m_nEndId >= MAX_MSG )
		m_nEndId=0;					//循环
}

//加入一条消息
void CMessage::PutMessage(enum MESSAGE Class, int p1, int p2, int p3, char *str1, char *str2, char *str3)
{
	//设置结构
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

//获得一条消息类型
enum MESSAGE CMessage::GetMessage(int Id)
{
	if( Id==-1 ) 
		Id=m_nStartId;	//缺省消息

	if( Id < 0 || Id >= MAX_MSG ) 
		return MSG_NULL;

	if( m_nEndId==Id ) 
		return MSG_NULL;	//消息队列为空

	return m_stMsg[Id].msg;
}

//删除一条消息
void CMessage::DeleteMessage(int Id)
{
	if( Id==-1 ) 
		Id=m_nStartId;	//缺省消息

	if( Id < 0 || Id >= MAX_MSG ) 
		return;

	if( m_nEndId==Id ) 
		return;		//消息队列为空

	//释放字符串变量
	_FREE( m_stMsg[Id].String1 );
	_FREE( m_stMsg[Id].String2 );
	_FREE( m_stMsg[Id].String3 );

	m_nStartId++;				//头指针向后移动一个
	if( m_nStartId>=MAX_MSG )
		m_nStartId=0;
}

//执行
void CMessage::Run()
{
	//-----------------------///////////////////////
	//Message.RunMessage();	//   处理当前消息	 //
	//---------------------///////////////////////

	//-----------------------//////////////////////
	//Message.BackMessage();// 上条消息结束处理 //
	//---------------------//////////////////////

	// 如果SetPlayState(0)就连续执行消息，一直执行到SetPlayState(1)为止
	do
	{
		if( m_nStartId == m_nEndId ) 
			return;	//消息队列为空

		RunMessage();		//执行一个消息
		BackMessage();		//结束一个消息
	}
	while( g_ePlayState == PS_Null && m_nStartId != m_nEndId );		//连续执行
}

//执行一条消息
bool CMessage::RunMessage(int Id)
{
	if( m_bMessageRunning ) 
		return false;			//有消息正在执行

	if( Id==-1 ) 
		Id=m_nStartId;						//缺省消息

	if( Id < 0 || Id >= MAX_MSG ) 
		return false;		//消息越界

	switch( m_stMsg[Id].msg )
	{
	case MSG_BREAK:
		return true;
	case MSG_BREAKSCRIPT:	//继续执行脚本
		{
			for(int i=Script.RecursionLevel; i>=0; i--)
			{
				//接着上次暂停的地方执行，如果有遇到暂停或者脚本完成就退出
				if( Script.Run( Script.RecursionFile[i], "",
						Script.RecursionPoint[i] ) == CScript::SR_RECURSION )
					break;
			}
		}
		break;

	case MSG_SETVARIABLE:	//设置脚本变量的值
		Script.SetValue( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_SETPLAYSTATE:	//设置运行状态
		g_ePlayState=(enum ePLAYSTATE)m_stMsg[Id].Param1;
		break;

	case MSG_SETDISPLAYSTATE:	//设置显示状态
		g_eDisplayState=(enum eDISPLAYSTATE)m_stMsg[Id].Param1;
		break;

	case MSG_BACKTOTITLE:	//回游戏主菜单
		Title.DoMenu(lpDDSBack);
		break;

	case MSG_TEXTTITLE:		//显示字幕
		Events.TextTitle( m_stMsg[Id].String1, m_stMsg[Id].String2 );
		break;

	case MSG_FIGHT:			//战斗
		{
			FadeIn(2, 20, RGB16(255,255,255));		//屏幕变白
			//敌人名字
			if( strcmp(m_stMsg[Id].String1, "")==0 ) 
				m_stMsg[Id].String1=NULL;
			if( strcmp(m_stMsg[Id].String2, "")==0 ) 
				m_stMsg[Id].String2=NULL;
			if( strcmp(m_stMsg[Id].String3, "")==0 ) 
				m_stMsg[Id].String3=NULL;

			//主角数据
			stRolePropertiy *rps[3];
			if( m_stMsg[Id].Param1 == ERROR_DATA )	//没有指定主角，就全部上
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
			{										//只上指定的主角
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

			Fight.bLeave=false;			//不能逃跑
			//只支持三个敌人，2位置不会有人
			Fight.Start("ini\\fight\\background1.ini", rps[0], rps[1], rps[2], 
				m_stMsg[Id].String1, NULL, m_stMsg[Id].String2, m_stMsg[Id].String3);

			FadeOut(2);
		}
		break;

	case MSG_ENABLEFIGHT:	//是否踩地雷
		Fight.bRandomFight=m_stMsg[Id].Param1;
		break;

	case MSG_BOSSFIGHT:	//是否踩地雷
		Fight.SetBossFight(m_stMsg[Id].Param1);
		break;

	//-----------------------------------------------------------

	case MSG_LOADMAP:		//读入地图
		map.LoadMap( m_stMsg[Id].String1 );
		break;

	case MSG_SAVEMAP:		//存储地图
		map.SaveMap( m_stMsg[Id].String1 );
		break;

	case MSG_ADDMAPHOOK:	//添加地图陷阱
		map.AddHook( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].String1);
		break;

	case MSG_SETMAPHOOK:	//设置地图陷阱
		map.SetHook( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3);
		break;

	case MSG_SETMAPCELL:	//设置地图某格
		{
			int n1=(int)m_stMsg[Id].String1;
			int n2=(int)m_stMsg[Id].String2;
			map.SetCell( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3, n1, n2);
			m_stMsg[Id].String1=NULL;
			m_stMsg[Id].String2=NULL;
		}
		break;

	case MSG_SETMAPBLOCK:	//设置地图阻挡
		map.SetBlock( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3 );
		break;

	case MSG_SETMAPLEVEL:	//设置地图层次
		{
			int n1=(int)m_stMsg[Id].String1;
			map.SetLevel( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3, n1 );
			m_stMsg[Id].String1=NULL;
		}
		break;

	case MSG_CENTERMAP:		//设置地图中心点
		map.CenterMap( m_stMsg[Id].Param1, m_stMsg[Id].Param2 );
		break;

	case MSG_SCROLLMAPTO:	//卷动地图到某点
		bControl=false;
		m_bMessageRunning=true;		//等到地图卷动到位了才执行下一条消息

		if( m_stMsg[Id].Param3==ERROR_DATA ) //没有指定节奏
			m_stMsg[Id].Param3=8;

		map.ScrollTo( m_stMsg[Id].Param1, m_stMsg[Id].Param2, m_stMsg[Id].Param3 );
		break;
	
	case MSG_LOADNPC:		//读取地图的NPC
		map.LoadIniNpcList( m_stMsg[Id].String1 );
		break;

//------------------------ROLE------------------------------------

	case MSG_TALK:			//对话消息
		talk.Talk(lpDDSBackGround, m_stMsg[Id].String1);	//对话
		break;

	case MSG_SAY:			//说一句话
		talk.Say(lpDDSBackGround, m_stMsg[Id].String1, m_stMsg[Id].String2, 
					m_stMsg[Id].Param1);
		break;

	case MSG_TALKDIR:		//调整主角和npc对话时的方向
		{
			int px=role[0].State.X;
			int py=role[0].State.Y;
			int nx=map.m_vNpc[m_stMsg[Id].Param1]->State.X;
			int ny=map.m_vNpc[m_stMsg[Id].Param1]->State.Y;
			if( abs(px-nx) >= abs(py-ny) )	//横向
			{
				if( px < nx )	//主角在左边
				{
					role[0].State.CurrentDir=3;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=1;
				}
				else			//主角在右边
				{
					role[0].State.CurrentDir=1;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=3;
				}
			}
			else	//纵向
			{
				if( py < ny )	//主角在上边
				{
					role[0].State.CurrentDir=0;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=2;
				}
				else			//主角在下边
				{
					role[0].State.CurrentDir=2;
					map.m_vNpc[m_stMsg[Id].Param1]->State.CurrentDir=0;
				}
			}		//运行脚本
			Script.Run(m_stMsg[Id].String1);
			Message.PutMessage( MSG_SETNPCACTIVE, m_stMsg[Id].Param2,m_stMsg[Id].Param1, 1);		//npc恢复活动
		}
		break;
	
	case MSG_MOVE:			//行走消息
		bControl=false;
		m_bMessageRunning=true;			//主角到位才接着执行
		if(m_stMsg[Id].Param3)
		{
			map.Cell[m_stMsg[Id].Param1+m_stMsg[Id].Param2*map.Width].Block =1;
			role[0].MoveTo(m_stMsg[Id].Param1*CellWidth, m_stMsg[Id].Param2*CellHeight);
			map.Cell[m_stMsg[Id].Param1+m_stMsg[Id].Param2*map.Width].Block =0;
		}
		else
			role[0].MoveTo(m_stMsg[Id].Param1*CellWidth, m_stMsg[Id].Param2*CellHeight);
		break;

	case MSG_MOVEDIRECTTO:		//朝一个方向移动
		{
			bControl = false;
			m_bMessageRunning=true;
			int x = role[0].State.X*CellWidth;
			int y = role[0].State.Y*CellHeight;
			switch(m_stMsg[Id].Param1)		//根据方向算出目的点
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
	case MSG_SETDIR:		//设置人物方向
		role[0].State.CurrentDir=m_stMsg[Id].Param1;
		break;

	case MSG_SETPOSITION:	//设置人物位置
		if( m_stMsg[Id].Param3 != ERROR_DATA )		//清除原位置的阻挡关系
			map.Block[map.Width*role[0].State.Y+role[0].State.X]=0;
		role[0].bMoving = false;
		role[0].State.x=role[0].State.oldx=m_stMsg[Id].Param1*CellWidth+CellWidth/2;
		role[0].State.y=role[0].State.oldy=m_stMsg[Id].Param2*CellHeight+CellHeight/2;
		//取主角的格子坐标
		role[0].State.X=role[0].State.oldX=_grid_x(role[0].State.x);
		role[0].State.Y=role[0].State.oldY=_grid_y(role[0].State.y);
		//设置新的当前步状态
		role[0].State.CurrentStep = 0;
		//设置新的阻挡关系
		map.Block[map.Width*role[0].State.Y+role[0].State.X]=1;
		break;

	case MSG_CENTERPLAYER:	//以主角为地图中心
		map.CenterMap( role[0].State.X, role[0].State.Y );
		break;

	case MSG_SETPLAYERSHOW:	//设置主角显示与否
		role[0].State.bShow=m_stMsg[Id].Param1;
		break;

	case MSG_LOADPLAYER:	//从ini文件读入主角
		role[m_stMsg[Id].Param1].LoadRoleIni(m_stMsg[Id].String1, m_stMsg[Id].String2);
		break;

	case MSG_SWITCHPLAYER:	//交换主角
		{
			int r1=m_stMsg[Id].Param1;
			int r2=m_stMsg[Id].Param2;
			//先保存
			role[r1].SaveRole("save\\temp1.rol");
			role[r2].SaveRole("save\\temp2.rol");
			//交换加载
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
			//继承过来
			role[r2].Propertiy.Money=role[r1].Propertiy.Money;	//金钱
			role[r2].Propertiy.CurGoodsNum=role[r1].Propertiy.CurGoodsNum;	//物品总数
			//物品（最多可带64种）
			memcpy(role[r2].Propertiy.Goods, role[r1].Propertiy.Goods, MAXGOODS*2);
			//每种物品的数量
			memcpy(role[r2].Propertiy.GoodsNum, role[r1].Propertiy.GoodsNum, MAXGOODS*2);	
			role[r2].Propertiy.CurArmNum=role[r1].Propertiy.CurArmNum;	//装备总数
			//装备（最多可带32种）
			memcpy(role[r2].Propertiy.Arm, role[r2].Propertiy.Arm, MAXARM*2);				
			//每种装备的数量
			memcpy(role[r2].Propertiy.ArmNum, role[r2].Propertiy.ArmNum, MAXARM*2);			
		}
		break;

	case MSG_SETPLAYERNUM:	//设置主角个数
		RoleNum=m_stMsg[Id].Param1;
		break;

	case MSG_ADDMONEY:		//改变金钱
		role[0].Propertiy.Money+=m_stMsg[Id].Param1;
		break;

	case MSG_ADDOBJECT:		//添加物体
		if( m_stMsg[Id].Param1==ERROR_DATA ) //数目
			m_stMsg[Id].Param1=1;

		role[0].AddObject( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_DELOBJECT:		//删除物体
		if( m_stMsg[Id].Param1==ERROR_DATA ) //数目
			m_stMsg[Id].Param1=1;

		role[0].DelObject( m_stMsg[Id].String1, m_stMsg[Id].Param1 );
		break;

	case MSG_ADDMAGIC:		//添加魔法
		if(m_stMsg[Id].Param1 == ERROR_DATA)
			m_stMsg[Id].Param1 = 0;		//缺省是第一个角色
		role[m_stMsg[Id].Param1].AddMagic(m_stMsg[Id].String1);

		break;

	case MSG_DELMAGIC:		//添加魔法
		if(m_stMsg[Id].Param1 == ERROR_DATA)
			m_stMsg[Id].Param1 = 0;		//缺省是第一个角色
		role[m_stMsg[Id].Param1].DelMagic(m_stMsg[Id].String1);

		break;
//------------------------NPC------------------------------------

	case MSG_MOVENPCTO:		//NPC移动
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);	//得到NPC的编号
			if( num != -1 )
			{
				map.m_vNpc[num]->MoveTo(m_stMsg[Id].Param1*CellWidth, 
					m_stMsg[Id].Param2*CellHeight);

				m_stMsg[Id].Param1=num;					//保存，到BackMessage时使用
				bControl=false;							//不能控制
				m_bMessageRunning=true;					//等到NPC移动到位
			}
		}
		break;

	case MSG_MOVENPC:		//NPC移动
		{
  			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.m_vNpc[num]->MoveTo(m_stMsg[Id].Param1*CellWidth, 
				m_stMsg[Id].Param2*CellHeight);
		}
		break;

	case MSG_SETNPCPOS:		//设置npc位置
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
				
				//取格子坐标
				map.m_vNpc[num]->State.X=map.m_vNpc[num]->State.oldX=
					_grid_x(map.m_vNpc[num]->State.x);

				map.m_vNpc[num]->State.Y=map.m_vNpc[num]->State.oldY=
					_grid_y(map.m_vNpc[num]->State.y);
				//阻挡
				map.Block[map.Width*map.m_vNpc[num]->State.Y
					+map.m_vNpc[num]->State.X]=1;
			}
		}
		break;

	case MSG_SETNPCDIR:		//设置npc方向
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
				map.m_vNpc[num]->State.CurrentDir=m_stMsg[Id].Param1;
		}
		break;

	case MSG_SETNPCSTEP:	//设置npc步伐
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
				map.m_vNpc[num]->State.CurrentStep=m_stMsg[Id].Param1;
		}
		break;

	case MSG_ADDNPC:		//加入NPC
		{
			bool *BackCell=NULL;
			int x=m_stMsg[Id].Param1;
			int y=m_stMsg[Id].Param2;

			//备份数据，地图阻挡关系
			if( x != ERROR_DATA && y != ERROR_DATA )
			{
				BackCell=new bool[map.Width*map.Height];
				for(int i=0; i<map.Width*map.Height; i++)
				{
					BackCell[i]=map.Block[i];
				}
			}
			//添加
			int num=map.AddNpc(m_stMsg[Id].String1, m_stMsg[Id].String2);

			if( num != ERROR_DATA )		//位置
				if( x != ERROR_DATA && y != ERROR_DATA )
				{
					//计算TILE坐标
					int n=map.m_vNpc[num]->State.X+map.m_vNpc[num]->State.Y*map.Width;
					//恢复阻挡
					map.Block[n]=BackCell[n];
					//脚底中心坐标
					map.m_vNpc[num]->State.x=StartX+x*CellWidth+CellWidth/2;
					map.m_vNpc[num]->State.y=StartY+y*CellHeight+CellHeight/2;
					//现在位置的阻挡
					map.Block[map.Width*y+x]=1;
					//地图位置
					map.m_vNpc[num]->State.X = map.m_vNpc[num]->State.oldX = x;
					map.m_vNpc[num]->State.Y = map.m_vNpc[num]->State.oldY = y;
				}
			
			_DELETE( BackCell );		//释放
		}
		break;

	case MSG_DELNPC:		//删除NPC
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.DelNpc(num);
		}
		break;

	case MSG_SETNPCACTIVE:	//设置NPC活动与否
		if( m_stMsg[Id].Param3 == 1 )	//Param2是编号
		{
			map.m_vNpc[m_stMsg[Id].Param2]->State.bActive=m_stMsg[Id].Param1;
		}
		else	//通过名字获取
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);
			if( num != -1 )
			{
				map.m_vNpc[num]->State.bActive=m_stMsg[Id].Param1;
			}
		}
		break;

	case MSG_SETNPCSHOW:	//设置NPC活动与否
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				map.m_vNpc[num]->State.bShow=m_stMsg[Id].Param1;
		}
		break;

	case MSG_SETNPCSCRIPT:	//设置NPC脚本文件
		{
			int num=map.GetNpcId(m_stMsg[Id].String1);

			if( num != -1 )
				strcpy(map.m_vNpc[num]->ScriptFile, m_stMsg[Id].String2);;
		}
		break;

//------------------------TOOLS-----------------------------------

	case MSG_DELAY:			//延时
		bControl=false;		//不能控制
		m_bMessageRunning=true;		//等待结束
		m_stMsg[Id].Param2=timeGetTime();	//保存当前时间，等到BackMessage时使用
		break;

	case MSG_FADEIN:		//淡入
		{
			int time=1;
			WORD color;

			//颜色
			if( m_stMsg[Id].Param2 == ERROR_DATA )
				color=0;
			else
				color=RGB16(m_stMsg[Id].Param2);

			//延时
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

	case MSG_FADEOUT:		//淡出
		{
			int time=1;

			//延时
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

	case MSG_PLAYMUSIC:		//播放背景音乐
		{
			Music.Stop();
			WCHAR* str = WChar(m_stMsg[Id].String1);
			Music.LoadMusic(str);
			_DELETE( str );
			Music.Play();
		}
		break;

	case MSG_STOPMUSIC:		//停止播放背景音乐
		Music.Stop();
		break;

	case MSG_PAUSEMUSIC:	//暂停音乐
		Music.Pause();
		break;

	case MSG_RESUMEMUSIC:	//恢复音乐
		Music.Resume();
		break;

	case MSG_PLAYSOUND:		//播放音效
		Sound.LoadSoundFX( hWnd, m_stMsg[Id].String1 );
		Sound.Play();
		break;

	case MSG_STOPSOUND:		//停止播放
		Sound.Stop();
		break;

	case MSG_GAMEMESSAGE:	//显示一条游戏消息
		GameMessage.DoMenu(lpDDSBack, m_stMsg[Id].String1);
		break;

	case MSG_SHOWMESSAGE:	//显示一条消息
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

//消息结束处理
bool CMessage::BackMessage()
{
	switch( m_stMsg[m_nStartId].msg )
	{
	case MSG_SCROLLMAPTO:	//卷动地图
		if( g_ePlayState==PS_MAIN )		//卷动完成了会设置成PS_MAIN，参看Map.ScrollLoop()
		{
			bControl=true;				//玩家可以控制
			m_bMessageRunning=false;	//可以执行下条消息
			DeleteMessage();			//删除本消息
		}
		break;
	
	case MSG_MOVE:			//行走消息
	case MSG_MOVEDIRECTTO:
		if( role[0].bMoving==false )	//移动到位	
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

	case MSG_MOVENPCTO:		//NPC行走消息
		//移动到位了
		//m_stMsg[m_nStartId].Param1保存了NPC的ID，参看RunMessage的处理
		if( map.m_vNpc[m_stMsg[m_nStartId].Param1]->bMoving==false )
		{
			bControl=true;
			m_bMessageRunning=false;
			DeleteMessage();
		}
		break;

	case MSG_DELAY:			//延时
		//m_stMsg[m_nStartId].Param2保存了DELAY开始时的时间，参看RunMessage的处理
		if( timeGetTime() - m_stMsg[m_nStartId].Param2 >= 
				(unsigned int)m_stMsg[m_nStartId].Param1 )
		{
			bControl=true;
			m_bMessageRunning=false;
			DeleteMessage();
		}
		break;

	default:				//缺省删除本条消息
		DeleteMessage();
	}
	return true;
}
