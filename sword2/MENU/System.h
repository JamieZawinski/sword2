//********************************************
//	ϵͳ�˵� ��غ���
//  ������2000��10��09��
//********************************************

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//********��Ʒ��*********
class CSystem : public CWindow{
public:
	int m_lSelect;			//��ǰѡ��

	int ButtonNum;			//��ť����
	CCButton *Button;		//��ť

public:
	CSystem();
	~CSystem();

	int GetSelect()			{ return m_lSelect; }
	void SetSelect(int n)	{ m_lSelect = n; }

	void LoadSystemIni(char *, char *);					//��������
	void ShowSystem(LPDIRECTDRAWSURFACE, bool=true);	//��ʾ
	int DoMenu(LPDIRECTDRAWSURFACE&, bool=true);		//�˵�ѭ��
};

#endif