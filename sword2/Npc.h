//********************************************
//	Npc角色 相关函数
//  创建于2000年4月7日
//********************************************

#ifndef _NPC_H_
#define _NPC_H_

class CRole;

//Npc的参数结构
struct stNpcPropertiy{
	int Level;		//等级
	int Exp;		//经验

	int Hp;			//生命
	int MaxHp;		//生命最大值
	int Mp;			//魔法
	int MaxMp;		//魔法最大值

	int Attack;		//攻击
	int Defend;		//防御
	int MagicAct;	//魔法攻击
	int MagicDef;	//魔法防御
	
	int Stamina;	//耐力
	int Luck;		//幸运
	int Speed;		//敏捷
	int Bp;			//体力0%-100%
	int Ip;			//精神0%-100%

	int Money;		//金钱
	WORD Arms;		//武器
	WORD Corselet;	//护具

	int Act;			//动作数量
	char surf_name[64];	//图片文件名
};

class CNpc : public CRole
{
public:
	stNpcPropertiy Propertiy;

public:
	bool LoadNpcIni(char *, char *);	//读取角色设定
	void Move(int, int);			//移动
	void ShowNpcPic(int, int, int);	//显示
};

#endif