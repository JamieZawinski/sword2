//  ************************************
//  *     战斗系统的类头文件 
//  *   创建于  5月29日   
//  ************************************

#ifndef _FIGHT_H_
#define _FIGHT_H_

#include "..\role.h"
#include "..\npc.h"
#include "..\interface\button.h"
#include "FightGoods.h"
#include "Fightmagic.h"

#define START			81
#define MAIN			102
#define USER_GOODS		103
#define USER_WHO		104
#define KILL_WHO		105
#define ROLE_KILL_ENEMY	106
#define ENEMY_KILL_ROLE	107
#define USER_MAGIC		108
#define OVER			153
#define ENEMY_DIE		123
#define ROLE_DIE		124
#define DIE				125

#define COLOR_BROOD_NULL	63488	//RGB16(255, 0, 0)
#define COLOR_BROOD			65504	//RGB16(255, 255, 0)
#define COLOR_MAGIC_NULL	28		//RGB16(0, 0, 225)
#define COLOR_MAGIC			2016	//RGB16(0, 255, 0)

class CCButton;

class CFight
{
	// 战斗人物的状态
	typedef struct{
		bool bLive;					// 是否存在
		char name[16];				// 人物名称
		int use;					// 人物是否使用
		int status;					// 人物状态 （-1：死 0：正常 1：中毒 2：麻痹）
		int ready;					// 进攻状态标示 （1：未进攻 0：相反）
		int x,y;					// 人物在屏幕上的坐标
		int x1,y1;					// 人物在移动时的相对坐标
		
		LPDIRECTDRAWSURFACE header;	//人物头像的图象
		LPDIRECTDRAWSURFACE surf;	//人物行动的图侦
		RECT Rstop[4];
		RECT Rbruise;				// 人物受到攻击的位图
		int stop_temp;				// 人物静止时的动作序列
		int UnAttack;				// 被攻击所降的HP
		int Lifeup;					// 魔法或者是物品提升的HP
		int BroodLong;				// 血槽长度
		int MagicLong;				// 气槽长度

		CSound sound_Fight;			//攻击声音
		CSound sound_Defend;		//被击声
		CSound sound_Dead;			//死亡声
	}stFightRole;

	//战斗的魔法图片数据
	typedef struct{
		int x;			//显示的x坐标
		int y;			//......y....
		RECT RMagicPic;	//在整个图片的位置矩形
	}stFightMagic;

	// 战斗系统的指令结构
	typedef struct{
		long lCommand;
		long lAccessory;
		int player;		// 主角是否选择攻击或选择了什么攻击
		int kill_who;	// 主角这次攻击的对象是谁
		int Class;		// 配合魔法的，表示对敌人还是自己  1:自己；0：敌人
	}stFightCommand;
public:
	enum
	{
		ATTACK,
		DEFEND,
		REPEAT,
		TOOLS,
		MAGIC,
		FLEE
	};
private:
	CGpFont font;						//字体对象

	int temp_swap;						// 用在程序的临时变量
	int main_count;						// 主控制循环 
	int kill_count;						// 主角的战斗控制循环变量
	int enemy_count;					// 敌人的战斗控制循环变量
	int command_count;					// 目前控制的角色
	CGpFont FightFont;					// 战斗字体
	int nNonce;							// 要攻击的对象

	stFightCommand Command[3];			// 战斗指令的数据
	stFightCommand Enemy_Command[4];	// 敌人的战斗指令的数据

	stRolePropertiy *Hero[3];			// 人物属性数据结构
	stFightRole FightRole[3];			// 主角在战斗中的数据
	CIniSet Role_Move[3];				// 人物移动文件
	CIniSet Role_Tool[3];				// 人物使用物品文件
	CIniSet Role_Magic[10];				// 人物施放法术文件 

	stNpcPropertiy Enemy[4];			// 敌人的属性数据
	stFightRole EnemyRole[4];			// 敌人在战斗中的数据
	CIniSet Enemy_Move[4];				// 敌人移动文件

	CIniSet FightRoleInit;
	CIniSet EnemyInit;

	int Number;							// 战斗时背景的页面数
	LPDIRECTDRAWSURFACE *background;	// 战斗时背景图片缓存
	LPDIRECTDRAWSURFACE* background_build;		// 战斗时的背景遮挡
	LPDIRECTDRAWSURFACE	status_pic;		// 状态栏图片
	CCButton Button[5];					// 战斗界面按钮
	CAni cursor_role, cursor_enemy;		// 选择人物时的光标

	RECT RMove_Pic;			// 角色移动的相对矩形
	char * Role_Temp;		// 角色移动时用到的临时变量

	CMusic FMusic;			// 战斗音乐
	CSound Sound_Click;		// 按钮声
	CSound Sound_High;		// 划过按钮声
	CSound sound_Magic;		// 魔法声音效果
	stFightMagic fightmagic;	//魔法数据


public:
	void UseGoods();
	BOOL bRandomFight;		// 是否可以踩地雷
	BOOL bLeave;			// 是否可以逃走

	CFight();
	~CFight();
	void OnTimer(void);		// 时钟
	void Init(void);		// 初始化

	// background:本战役的背景图片以及数据的数据文件名
	// enemy_1～enemy_4是四个敌人的名字
	// 如果enemy1~4的值为NULL则那个位置没有敌人
	Start(char *bg_name,
		  stRolePropertiy * role_1,
		  stRolePropertiy * role_2,
		  stRolePropertiy * role_3,
		  char *enemy_1,
		  char *enemy_2,
		  char *enemy_3,
		  char *enemy_4);

	int RandomStart(int nFreq=0, bool bMouse=false);		//踩地雷战斗

	void Enemy_AI(void);		// 敌人战斗时的选择攻击对象的AI
	void Clear(void);			// 返回主循环时对数据结构的清空复位
	void Account_Attack(void);	// 计算攻击
	int  Account_Loss(void);	// 计算损失
	void Print_Attack(void);	// 显示攻击数字
	int Test_Enemy(int a);		// 测试敌人角色是否存在
	int Test_Role(int a);		// 测试我方角色是否存在
	void UpdateLevel();			// 加分升级
	void GetFightGameSurface(LPDIRECTDRAWSURFACE surf);	//战斗游戏背景

	int GetEnemyNum();
	int GetLastEnemyNum();
	int GetRoleNum();
	int GetLastRoleNum();
	void Show_Role_Enemy();		//显示人物
	void Show_Role_Enemy_Alpha();		//显示人物(Alpha)

	int Loop(int mouse,POINT point);	//战斗循环

	CFightGoods m_fightgoods;
	CFightMagic m_fightmagic;

// 2001年3月11日 修改
	int m_nAttackCountMove;				// 攻击数值的偏移量
	int m_nAttackCountLife;				// 攻击数值的显示时间
	int m_nLifeupCountMove;				// 攻击数值的偏移量
	int m_nLifeupCountLife;				// 攻击数值的显示时间

///////////////////////////////////////////////////////////////////
// 敌人列表
///////////////////////////////////////////////////////////////////

	struct stEnemyList
	{
		char strName[16];
	};

private:
	int m_nRoundCount;						// 回合计数
	bool m_bHidden;							// 隐藏攻击模式
	BOOL m_bBoss;							// boss战
	int m_nMagicCount;						// boss战的损失mp计数
	static int m_nEnemyNum;					// 敌人类型的数量
	static stEnemyList* m_listEnemy;		// 敌人名称列表

public:
	void SetBossFight(BOOL flags);			// boss战
	void Print_Lifeup();					// 加生命
	void MagicEffect(int Num, stRolePropertiy * Propertiy,stFightRole* enemy,int num);	// 魔法效果
	BOOL IsPlayingMusic();					//战斗音乐
	void ResumeMusic();
	void PauseMusic();
	static void LoadEnemyList(char* file, char* index);		// 读入敌人列表
	static int GetEnemyListNum()		{ return m_nEnemyNum; }
	static char* GetEnemyName(int n)	{ return m_listEnemy[n].strName; }

};

#endif