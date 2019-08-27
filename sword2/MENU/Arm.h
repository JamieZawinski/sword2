//********************************************
//	װ�� ��غ���
//  ������2000��9��27��
//********************************************

#ifndef _ARM_H_
#define _ARM_H_

//********װ����*********
class CArm : public CWindow{
public:
	int PhotoWidth, PhotoHeight;
	RECT PhotoRect;

	CCButton Button_Left, Button_Right;	//���һ��˰�ť
	CWindow Window_Setup;				//װ������

	CListWindow ArmList;				//װ���б��

public:
	CArm();
	~CArm();

	void LoadArmIni(char *, char *);	//���봰�ڵ�����
	void Init(LPDIRECTDRAWSURFACE);		//��ʼ������
	void ShowArmWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ
	void ShowArmListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top);	//��ʾװ���б��
	void ShowChangeNum(LPDIRECTDRAWSURFACE, int, int);	//װ��������ֵԤ��
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);		//�˵�ѭ��
};

#endif