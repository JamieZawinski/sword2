//********************************************
//	���� ��غ���
//  ������2000��6��21��
//********************************************

#ifndef _WINDOW_H_
#define _WINDOW_H_

//���ڵ������
enum POPWINDOWSTYLE{
	PWS_NULL=0,		//��Ч��
	PWS_HOR,		//����
	PWS_VER,		//����
	PWS_BOTH,		//�Ŵ�
	PWS_RANDOM=-1,	//���
};

class CGpFont;

class CWindow
{
public:
	int dialogx, dialogy;	//�Ի���λ�ã���λ�����أ�
	int dialogw, dialogh;	//�Ի����С����λ�����أ�
	int Alpha;				//�Ի����Alphaֵ
	enum POPWINDOWSTYLE PopWindowStyle;		//�Ի��򵯳����
	int PopWindowStep;						//�Ի��򵯳��ٶ�

	CGpFont Font;			//�Ի�����
	char PicFileName[64];	//ͼƬ�ļ���

	LPDIRECTDRAWSURFACE lpPic;	//�Ի���ͼƬ

public:
	CWindow();
	~CWindow();
	bool LoadWindowIni(char *, char *);	//���봰��

	void ShowWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ�Ի���
	void PopWindow(LPDIRECTDRAWSURFACE& surf, enum POPWINDOWSTYLE=PWS_NULL, int Num=8);	//�����Ի���
	int PressAnyKey();				//�����⽡����
};

#endif