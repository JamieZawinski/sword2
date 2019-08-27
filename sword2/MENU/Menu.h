//********************************************
//	�˵� ��غ���
//  ������2000��7��09��
//********************************************

#ifndef _MENU_H_
#define _MENU_H_

//********��Ʒ��*********
class CMenu{
public:
	static int SelectItem;				//��ǰ���Ĳ˵���
	static bool bMenuRun;				//�Ƿ��ڲ˵�ѭ��

	static CMenuTop MenuTop;			//�����˵�
	static CRoleSel RoleSel;			//����ѡ��
	static CState State;				//��1������
	static CGoods Goods;				//��2����Ʒ��
	static CArm Arm;					//��3��װ��
	static CSystem System;				//��4��ϵͳ
	static CMagic Magic;				//ħ������
	static CLoadSave LoadSave;			//���ȴ���
	static CHelp Help;					//���ڴ���

	static CSound Sound_TileClick;		//��ͷ
	static CSound Sound_HighLight;		//��껮��
	static CSound Sound_Click;			//��ť����
	static CSound Sound_Change;			//�л��˵�
	static CSound Sound_Pop;			//�˵�����
	static CSound Sound_Cancel;			//�˵�ȡ��

public:
	CMenu();
	~CMenu();

	void LoadMenuIni(char *, char *);		//��������
	
	static void InitMenu();					//�˵���ʼ��
	static void ReInitMenu();				//���ò˵�
	void DoMenuLoop();						//�˵�ѭ��
};


#endif
