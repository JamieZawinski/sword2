//********************************************
//	��Ϣ���� ��غ���
//  ������2000��5��8��
//********************************************

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define MAX_MSG	1024		//�����Ϣ��

//��Ϣ����
enum MESSAGE{
		MSG_NULL=0,			//����Ϣ
		MSG_BREAKSCRIPT,	//��ʱ����ű�

		MSG_SETVARIABLE,	//���ýű�������ֵ
		MSG_SETPLAYSTATE,	//��������״̬
		MSG_SETDISPLAYSTATE,//������ʾģʽ
		MSG_BACKTOTITLE,	//����Ϸ���˵�
		MSG_TEXTTITLE,		//��ʾ��Ļ (s1=filename, s2=index)
		MSG_FIGHT,			//ս�� (s1/s2/s3=���� p1/p2/p3=�ҷ�)
		MSG_ENABLEFIGHT,	//�Ƿ�ȵ��� (p1=ture/false)
		MSG_BOSSFIGHT,		//�Ƿ���bossս(p1=ture/false)

		MSG_LOADMAP,		//�����ͼ
		MSG_SAVEMAP,		//�洢��ͼ
		MSG_ADDMAPHOOK,		//��ӵ�ͼ���� (p1=x, p2=y, s1=sptfile)
		MSG_SETMAPHOOK,		//���õ�ͼ���� (p1=x, p2=y, p3=Hook)
		MSG_SETMAPCELL,		//���õ�ͼĳ�� (p1=Level, p2=x, p3=y, s1=num s2=page)
		MSG_SETMAPBLOCK,	//���õ�ͼ�赲 (p1=x, p2=y, p2=block)
		MSG_SETMAPLEVEL,	//���õ�ͼ��� (p1=x, p2=y, p3=level1, s1=level2)
		MSG_CENTERMAP,		//���õ�ͼ���ĵ�( p1=x, p2=y )
		MSG_SCROLLMAPTO,	//����ͼ��ĳ��( p1=x, p2=y )

		MSG_LOADNPC,		//��ȡ��ͼ��NPC

		MSG_TALK,			//�Ի�			(s1=�Ի���־)
		MSG_SAY,			//˵��			(s1=һ�仰������)
		MSG_TALKDIR,		//�������Ǻ�NPC�ķ���ʹ����� (p1=NpcId,p2=Npcԭ���Ļ����,
							//								s1=�Ի��ű�)

		MSG_MOVE,			//����			(p1=x, p2=y)
		MSG_SETDIR,			//�������﷽��	(p1=����)
		MSG_SETPOSITION,	//��������λ��	(p1=x, p2=y)
		MSG_CENTERPLAYER,	//������Ϊ��ͼ����
		MSG_SETPLAYERSHOW,	//����������ʾ��� (p1=bShow)
		MSG_LOADPLAYER,		//��ini�ļ���������(p1=rolenum s1=filename s2=index)
		MSG_SWITCHPLAYER,	//��������λ��
		MSG_SETPLAYERNUM,	//�������Ǹ���

		MSG_ADDMONEY,		//�ı��Ǯ		(p1=money)
		MSG_ADDOBJECT,		//��������		(s1=������ p1=num)
		MSG_DELOBJECT,		//ɾ������		(s1=������ p1=num)
		MSG_ADDMAGIC,		//����ħ��		(s1=ħ����)
		MSG_DELMAGIC,		//ɾ��ħ��      (s1=ħ����)

		MSG_MOVENPCTO,		//NPC�ƶ���		(s1=Npc����, p1=x, p2=y)
		MSG_MOVENPC,		//NPC�ƶ�		(s1=Npc����, p1=x, p2=y)
		MSG_SETNPCPOS,		//����NPCλ��	(p1=x, p2=y)
		MSG_SETNPCDIR,		//����NPC����	(p1=����);
		MSG_SETNPCSTEP,		//����NPC����	(p1=����);
		MSG_ADDNPC,			//����NPC		(s1=filename p1=id, p2=x, p3=y)
		MSG_DELNPC,			//ɾ��NPC		(s1=Npc����)
		MSG_SETNPCACTIVE,	//����NPC����	(s1=Npc����, p1=bActive)
		MSG_SETNPCSHOW,		//����NPC��ʾ���	(s1=Npc����, p1=bShow)
		MSG_SETNPCSCRIPT,	//����NPC�ű�		(s1=Npc����, s2=�ű��ļ�)

		MSG_DELAY,			//��ʱ			(p1=time)
		MSG_FADEIN,			//����			(p1=style, p2=color)
		MSG_FADEOUT,		//����			(p1=style)
		MSG_PLAYMUSIC,		//��������		(s1=File)
		MSG_STOPMUSIC,		//ֹͣ����
		MSG_PAUSEMUSIC,		//��ͣ����
		MSG_RESUMEMUSIC,	//�ָ�����
		MSG_PLAYSOUND,		//������Ч		(s1=File)
		MSG_STOPSOUND,		//ֹͣ����
		MSG_GAMEMESSAGE,	//��ʾһ����Ϸ��Ϣ	(s1=��Ϣ)
		MSG_SHOWMESSAGE,	//��ʾһ����Ϣ	(s1=��Ϣ)
		MSG_BREAK,			//���β�ִ��
		MSG_SETCPTYPE,		//������������
		MSG_MOVEDIRECTTO,	//��ĳ�������ƶ�һ������
};

//��Ϣ������
class CMessage
{
	//��Ϣ��������
	typedef struct{
		enum MESSAGE msg;					//��Ϣ����
		int Param1, Param2, Param3;			//����
		char *String1, *String2, *String3, *String4;	//�ַ�ָ�����
	}stMessage;

	bool m_bMessageRunning;					//�Ƿ�����Ϣ����ִ��
	int m_nStartId, m_nEndId;				//���п�ͷ�ͽ���ָ��
	stMessage m_stMsg[MAX_MSG];				//��Ϣ����

public:
	CMessage();								//��ʼ��
	void ClearAllMessage();					//���ȫ����Ϣ
	bool IsEmpty();							//��Ϣ�����Ƿ�Ϊ��
	void DeleteMessage(int Id=-1);			//ɾ��һ����Ϣ
	void PutMessage(stMessage);				//����һ����Ϣ
	void PutMessage(enum MESSAGE, int p1=0, int p2=0 ,int p3=0, char *str1=NULL, char *str2=NULL, char *str3=NULL);//����һ����Ϣ
	enum MESSAGE GetMessage(int Id=-1);		//��ȡһ����Ϣ��ȱʡ����ǰ��Ϣ��
	bool RunMessage(int Id=-1);				//ִ��һ����Ϣ��ȱʡ����ǰ��Ϣ��
	bool BackMessage();						//��Ϣ��������
	void Run();								//ִ�У�������һ����Ϣ��Ҳ�����Ƕ�����Ϣ��
};

#endif