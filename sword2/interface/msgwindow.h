//********************************************
//	��Ϣ���� ��غ���
//  ������2000��10��08��
//********************************************

#ifndef _MSGWINDOW_H_
#define _MSGWINDOW_H_

class CGpFont;

//��Ϣ����
class CMsgWindow : public CWindow
{
public:
	int Id;
	int x, y, Width, Height;

public:
	CMsgWindow();
	~CMsgWindow();

	bool LoadMsgWindowIni(char *, char *);	//���봰��
	void ShowMsgWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ�Ի���
	void DoMenu(LPDIRECTDRAWSURFACE&, char *);		//�˵�ѭ��
};

#endif
