//********************************************
//	����ѡ�� ��غ���
//  ������2000��7��09��
//********************************************

#ifndef _ROLESEL_H_
#define _ROLESEL_H_

//********״̬��*********
class CRoleSel : public CWindow{
public:
	int ButtonNum;			//��ť����
	CCButton *Role;			//��ť
	LPDIRECTDRAWSURFACE *PhotoPic;	//ͷ��

	int SelectRole;			//��ǰѡ�������

public:
	CRoleSel();
	~CRoleSel();

	void LoadRoleSelIni(char *, char *);		//��������
	void ShowRoleSelWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ

	void Init(LPDIRECTDRAWSURFACE, bool=true);				//��ʼ������
	int DoMenu(LPDIRECTDRAWSURFACE& surf, bool=true);		//�˵�ѭ��
};

#endif