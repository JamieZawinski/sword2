//  ************************************
//  *     ս��ϵͳ����ͷ�ļ� 
//  *   ������  5��29��   
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
	// ս�������״̬
	typedef struct{
		bool bLive;					// �Ƿ����
		char name[16];				// ��������
		int use;					// �����Ƿ�ʹ��
		int status;					// ����״̬ ��-1���� 0������ 1���ж� 2����ԣ�
		int ready;					// ����״̬��ʾ ��1��δ���� 0���෴��
		int x,y;					// ��������Ļ�ϵ�����
		int x1,y1;					// �������ƶ�ʱ���������
		
		LPDIRECTDRAWSURFACE header;	//����ͷ���ͼ��
		LPDIRECTDRAWSURFACE surf;	//�����ж���ͼ��
		RECT Rstop[4];
		RECT Rbruise;				// �����ܵ�������λͼ
		int stop_temp;				// ���ﾲֹʱ�Ķ�������
		int UnAttack;				// ������������HP
		int Lifeup;					// ħ����������Ʒ������HP
		int BroodLong;				// Ѫ�۳���
		int MagicLong;				// ���۳���

		CSound sound_Fight;			//��������
		CSound sound_Defend;		//������
		CSound sound_Dead;			//������
	}stFightRole;

	//ս����ħ��ͼƬ����
	typedef struct{
		int x;			//��ʾ��x����
		int y;			//......y....
		RECT RMagicPic;	//������ͼƬ��λ�þ���
	}stFightMagic;

	// ս��ϵͳ��ָ��ṹ
	typedef struct{
		long lCommand;
		long lAccessory;
		int player;		// �����Ƿ�ѡ�񹥻���ѡ����ʲô����
		int kill_who;	// ������ι����Ķ�����˭
		int Class;		// ���ħ���ģ���ʾ�Ե��˻����Լ�  1:�Լ���0������
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
	CGpFont font;						//�������

	int temp_swap;						// ���ڳ������ʱ����
	int main_count;						// ������ѭ�� 
	int kill_count;						// ���ǵ�ս������ѭ������
	int enemy_count;					// ���˵�ս������ѭ������
	int command_count;					// Ŀǰ���ƵĽ�ɫ
	CGpFont FightFont;					// ս������
	int nNonce;							// Ҫ�����Ķ���

	stFightCommand Command[3];			// ս��ָ�������
	stFightCommand Enemy_Command[4];	// ���˵�ս��ָ�������

	stRolePropertiy *Hero[3];			// �����������ݽṹ
	stFightRole FightRole[3];			// ������ս���е�����
	CIniSet Role_Move[3];				// �����ƶ��ļ�
	CIniSet Role_Tool[3];				// ����ʹ����Ʒ�ļ�
	CIniSet Role_Magic[10];				// ����ʩ�ŷ����ļ� 

	stNpcPropertiy Enemy[4];			// ���˵���������
	stFightRole EnemyRole[4];			// ������ս���е�����
	CIniSet Enemy_Move[4];				// �����ƶ��ļ�

	CIniSet FightRoleInit;
	CIniSet EnemyInit;

	int Number;							// ս��ʱ������ҳ����
	LPDIRECTDRAWSURFACE *background;	// ս��ʱ����ͼƬ����
	LPDIRECTDRAWSURFACE* background_build;		// ս��ʱ�ı����ڵ�
	LPDIRECTDRAWSURFACE	status_pic;		// ״̬��ͼƬ
	CCButton Button[5];					// ս�����水ť
	CAni cursor_role, cursor_enemy;		// ѡ������ʱ�Ĺ��

	RECT RMove_Pic;			// ��ɫ�ƶ�����Ծ���
	char * Role_Temp;		// ��ɫ�ƶ�ʱ�õ�����ʱ����

	CMusic FMusic;			// ս������
	CSound Sound_Click;		// ��ť��
	CSound Sound_High;		// ������ť��
	CSound sound_Magic;		// ħ������Ч��
	stFightMagic fightmagic;	//ħ������


public:
	void UseGoods();
	BOOL bRandomFight;		// �Ƿ���Բȵ���
	BOOL bLeave;			// �Ƿ��������

	CFight();
	~CFight();
	void OnTimer(void);		// ʱ��
	void Init(void);		// ��ʼ��

	// background:��ս�۵ı���ͼƬ�Լ����ݵ������ļ���
	// enemy_1��enemy_4���ĸ����˵�����
	// ���enemy1~4��ֵΪNULL���Ǹ�λ��û�е���
	Start(char *bg_name,
		  stRolePropertiy * role_1,
		  stRolePropertiy * role_2,
		  stRolePropertiy * role_3,
		  char *enemy_1,
		  char *enemy_2,
		  char *enemy_3,
		  char *enemy_4);

	int RandomStart(int nFreq=0, bool bMouse=false);		//�ȵ���ս��

	void Enemy_AI(void);		// ����ս��ʱ��ѡ�񹥻������AI
	void Clear(void);			// ������ѭ��ʱ�����ݽṹ����ո�λ
	void Account_Attack(void);	// ���㹥��
	int  Account_Loss(void);	// ������ʧ
	void Print_Attack(void);	// ��ʾ��������
	int Test_Enemy(int a);		// ���Ե��˽�ɫ�Ƿ����
	int Test_Role(int a);		// �����ҷ���ɫ�Ƿ����
	void UpdateLevel();			// �ӷ�����
	void GetFightGameSurface(LPDIRECTDRAWSURFACE surf);	//ս����Ϸ����

	int GetEnemyNum();
	int GetLastEnemyNum();
	int GetRoleNum();
	int GetLastRoleNum();
	void Show_Role_Enemy();		//��ʾ����
	void Show_Role_Enemy_Alpha();		//��ʾ����(Alpha)

	int Loop(int mouse,POINT point);	//ս��ѭ��

	CFightGoods m_fightgoods;
	CFightMagic m_fightmagic;

// 2001��3��11�� �޸�
	int m_nAttackCountMove;				// ������ֵ��ƫ����
	int m_nAttackCountLife;				// ������ֵ����ʾʱ��
	int m_nLifeupCountMove;				// ������ֵ��ƫ����
	int m_nLifeupCountLife;				// ������ֵ����ʾʱ��

///////////////////////////////////////////////////////////////////
// �����б�
///////////////////////////////////////////////////////////////////

	struct stEnemyList
	{
		char strName[16];
	};

private:
	int m_nRoundCount;						// �غϼ���
	bool m_bHidden;							// ���ع���ģʽ
	BOOL m_bBoss;							// bossս
	int m_nMagicCount;						// bossս����ʧmp����
	static int m_nEnemyNum;					// �������͵�����
	static stEnemyList* m_listEnemy;		// ���������б�

public:
	void SetBossFight(BOOL flags);			// bossս
	void Print_Lifeup();					// ������
	void MagicEffect(int Num, stRolePropertiy * Propertiy,stFightRole* enemy,int num);	// ħ��Ч��
	BOOL IsPlayingMusic();					//ս������
	void ResumeMusic();
	void PauseMusic();
	static void LoadEnemyList(char* file, char* index);		// ��������б�
	static int GetEnemyListNum()		{ return m_nEnemyNum; }
	static char* GetEnemyName(int n)	{ return m_listEnemy[n].strName; }

};

#endif