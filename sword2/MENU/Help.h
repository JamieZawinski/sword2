//********************************************
//	���� ��غ���
//  ������2000��10��22��
//********************************************

#ifndef _HELP_H_
#define _HELP_H_

//********������*********
class CHelp : public CWindow{
public:
	int page;			//ҳ��

	CHelp();
	~CHelp();

	void LoadHelpIni(char *, char *);				//���봰�ڵ�����
	void Init(LPDIRECTDRAWSURFACE);					//��ʼ������
	void ShowHelpWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);	//�˵�ѭ��
};

#endif