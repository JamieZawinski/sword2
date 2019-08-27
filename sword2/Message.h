//********************************************
//	消息处理 相关函数
//  创建于2000年5月8日
//********************************************

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define MAX_MSG	1024		//最大消息数

//消息定义
enum MESSAGE{
		MSG_NULL=0,			//空消息
		MSG_BREAKSCRIPT,	//暂时挂起脚本

		MSG_SETVARIABLE,	//设置脚本变量的值
		MSG_SETPLAYSTATE,	//设置运行状态
		MSG_SETDISPLAYSTATE,//设置显示模式
		MSG_BACKTOTITLE,	//回游戏主菜单
		MSG_TEXTTITLE,		//显示字幕 (s1=filename, s2=index)
		MSG_FIGHT,			//战斗 (s1/s2/s3=敌人 p1/p2/p3=我方)
		MSG_ENABLEFIGHT,	//是否踩地雷 (p1=ture/false)
		MSG_BOSSFIGHT,		//是否是boss战(p1=ture/false)

		MSG_LOADMAP,		//读入地图
		MSG_SAVEMAP,		//存储地图
		MSG_ADDMAPHOOK,		//添加地图陷阱 (p1=x, p2=y, s1=sptfile)
		MSG_SETMAPHOOK,		//设置地图陷阱 (p1=x, p2=y, p3=Hook)
		MSG_SETMAPCELL,		//设置地图某格 (p1=Level, p2=x, p3=y, s1=num s2=page)
		MSG_SETMAPBLOCK,	//设置地图阻挡 (p1=x, p2=y, p2=block)
		MSG_SETMAPLEVEL,	//设置地图层次 (p1=x, p2=y, p3=level1, s1=level2)
		MSG_CENTERMAP,		//设置地图中心点( p1=x, p2=y )
		MSG_SCROLLMAPTO,	//卷动地图到某点( p1=x, p2=y )

		MSG_LOADNPC,		//读取地图的NPC

		MSG_TALK,			//对话			(s1=对话标志)
		MSG_SAY,			//说话			(s1=一句话的内容)
		MSG_TALKDIR,		//调整主角和NPC的方向使其面对 (p1=NpcId,p2=Npc原来的活动属性,
							//								s1=对话脚本)

		MSG_MOVE,			//行走			(p1=x, p2=y)
		MSG_SETDIR,			//设置人物方向	(p1=方向)
		MSG_SETPOSITION,	//设置人物位置	(p1=x, p2=y)
		MSG_CENTERPLAYER,	//以主角为地图中心
		MSG_SETPLAYERSHOW,	//设置主角显示与否 (p1=bShow)
		MSG_LOADPLAYER,		//从ini文件读入主角(p1=rolenum s1=filename s2=index)
		MSG_SWITCHPLAYER,	//交换主角位置
		MSG_SETPLAYERNUM,	//设置主角个数

		MSG_ADDMONEY,		//改变金钱		(p1=money)
		MSG_ADDOBJECT,		//加入物体		(s1=物体名 p1=num)
		MSG_DELOBJECT,		//删除物体		(s1=物体名 p1=num)
		MSG_ADDMAGIC,		//加入魔法		(s1=魔法名)
		MSG_DELMAGIC,		//删除魔法      (s1=魔法名)

		MSG_MOVENPCTO,		//NPC移动到		(s1=Npc名字, p1=x, p2=y)
		MSG_MOVENPC,		//NPC移动		(s1=Npc名字, p1=x, p2=y)
		MSG_SETNPCPOS,		//设置NPC位置	(p1=x, p2=y)
		MSG_SETNPCDIR,		//设置NPC方向	(p1=方向);
		MSG_SETNPCSTEP,		//设置NPC步伐	(p1=步伐);
		MSG_ADDNPC,			//加入NPC		(s1=filename p1=id, p2=x, p3=y)
		MSG_DELNPC,			//删除NPC		(s1=Npc名字)
		MSG_SETNPCACTIVE,	//设置NPC活动与否	(s1=Npc名字, p1=bActive)
		MSG_SETNPCSHOW,		//设置NPC显示与否	(s1=Npc名字, p1=bShow)
		MSG_SETNPCSCRIPT,	//设置NPC脚本		(s1=Npc名字, s2=脚本文件)

		MSG_DELAY,			//延时			(p1=time)
		MSG_FADEIN,			//淡入			(p1=style, p2=color)
		MSG_FADEOUT,		//淡出			(p1=style)
		MSG_PLAYMUSIC,		//播放音乐		(s1=File)
		MSG_STOPMUSIC,		//停止播放
		MSG_PAUSEMUSIC,		//暂停音乐
		MSG_RESUMEMUSIC,	//恢复音乐
		MSG_PLAYSOUND,		//播放音效		(s1=File)
		MSG_STOPSOUND,		//停止勃发
		MSG_GAMEMESSAGE,	//显示一条游戏消息	(s1=消息)
		MSG_SHOWMESSAGE,	//显示一条消息	(s1=消息)
		MSG_BREAK,			//本次不执行
		MSG_SETCPTYPE,		//设置陷阱类型
		MSG_MOVEDIRECTTO,	//向某个方向移动一个距离
};

//消息处理类
class CMessage
{
	//消息传递数据
	typedef struct{
		enum MESSAGE msg;					//消息类型
		int Param1, Param2, Param3;			//参数
		char *String1, *String2, *String3, *String4;	//字符指针参数
	}stMessage;

	bool m_bMessageRunning;					//是否有消息正在执行
	int m_nStartId, m_nEndId;				//队列开头和结束指针
	stMessage m_stMsg[MAX_MSG];				//消息队列

public:
	CMessage();								//初始化
	void ClearAllMessage();					//清除全部消息
	bool IsEmpty();							//消息队列是否为空
	void DeleteMessage(int Id=-1);			//删除一条消息
	void PutMessage(stMessage);				//加入一条消息
	void PutMessage(enum MESSAGE, int p1=0, int p2=0 ,int p3=0, char *str1=NULL, char *str2=NULL, char *str3=NULL);//加入一条消息
	enum MESSAGE GetMessage(int Id=-1);		//获取一条消息（缺省＝当前消息）
	bool RunMessage(int Id=-1);				//执行一条消息（缺省＝当前消息）
	bool BackMessage();						//消息结束处理
	void Run();								//执行（可能是一条消息，也可能是多条消息）
};

#endif