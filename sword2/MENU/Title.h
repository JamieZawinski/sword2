//********************************************
//	���� ��غ���
//  ������2000��9��29��
//********************************************

#ifndef _TITLE_H_
#define _TITLE_H_

//********������*********
class CTitle{
public:
	int ButtonNum;			//��ť����
	CCButton *Button;		//��ť

	int Select;				//��ǰѡ��
	char *BackGroundFile;	//����ͼƬ�ļ�

public:
	CGpFont ShowVersionFont;
	CTitle();
	~CTitle();

	void LoadTitleIni(char *, char *);					//��������
	void ShowTitle(LPDIRECTDRAWSURFACE, bool bShowWindow=true);		//��ʾ
	void DoMenu(LPDIRECTDRAWSURFACE, bool bShowWindow=true);		//����ѭ��
private:
	char m_szVersion[10];
};

#endif