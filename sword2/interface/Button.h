//********************************************
//	��ť ��غ���
//	������2000��6��19��
//********************************************

#ifndef _BUTTON_H_
#define _BUTTON_H_

//��ť��
class CCButton
{
public:
	enum BUTTONSTYLE
	{
		BS_NORMAL=0,	//��ͨ
	};

	int Id;				//���
	char Name[32];		//����
	int Style;			//���
	int x, y;			//����
	int Width, Height;	//��С
	char PicFileName[32];			//λͼ�ļ���
	LPDIRECTDRAWSURFACE Pic;		//λͼ
	RECT Normal_Rect, Over_Rect, Down_Rect;		//��ťλ��
	bool* m_bTest;								//��ť����
public:
	CCButton();						//����
	~CCButton();					//����
	bool LoadIni(char *,char *);	//��ini�ļ�����
	void Show(LPDIRECTDRAWSURFACE, int bHighLight=-1);			//��ʾ
	BOOL CheckLoop(LPDIRECTDRAWSURFACE, int bHighLight=-1);		//���ѭ��
protected:
	bool CreateRgn();				//������ť����
private:
	
	BOOL CheckIn();					//�����Ƿ�����Ч��������
};

#endif