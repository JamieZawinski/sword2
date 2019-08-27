//********************************************
//	Npc��ɫ ��غ���
//  ������2000��4��7��
//********************************************

#ifndef _NPC_H_
#define _NPC_H_

class CRole;

//Npc�Ĳ����ṹ
struct stNpcPropertiy{
	int Level;		//�ȼ�
	int Exp;		//����

	int Hp;			//����
	int MaxHp;		//�������ֵ
	int Mp;			//ħ��
	int MaxMp;		//ħ�����ֵ

	int Attack;		//����
	int Defend;		//����
	int MagicAct;	//ħ������
	int MagicDef;	//ħ������
	
	int Stamina;	//����
	int Luck;		//����
	int Speed;		//����
	int Bp;			//����0%-100%
	int Ip;			//����0%-100%

	int Money;		//��Ǯ
	WORD Arms;		//����
	WORD Corselet;	//����

	int Act;			//��������
	char surf_name[64];	//ͼƬ�ļ���
};

class CNpc : public CRole
{
public:
	stNpcPropertiy Propertiy;

public:
	bool LoadNpcIni(char *, char *);	//��ȡ��ɫ�趨
	void Move(int, int);			//�ƶ�
	void ShowNpcPic(int, int, int);	//��ʾ
};

#endif