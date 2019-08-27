//********************************************
//	������ ��غ���
//	������2000��7��06��
//********************************************

#ifndef _SCROLL_H_
#define _SCROLL_H_

enum SCROLLSTYLE{
	SS_VAR=0,			//��ֱ����
	SS_HOR,				//ˮƽ����
};

//��������
class CScroll
{
public:
	int Id;				//���
	int Style;			//���
	int x, y;			//����
	int Width;			//���
	int Height;			//����

	char PicFileName[32];			//λͼ�ļ���
	LPDIRECTDRAWSURFACE Pic;		//λͼ

	RECT Rect;						//������λ��
	RECT Sub_Rect, Sub2_Rect;
	RECT Add_Rect, Add2_Rect;		//��ťλ��
	RECT Bar_Rect, Space_Rect;		//��λ�úͿհ�λ��

	RECT RectSub, RectAdd;
	RECT RectPageSub, RectPageAdd;
	RECT RectBar, RectSpace;

	float StepSize;					//ÿ���ߵľ���

	int MinNum, MaxNum, CurNum;		//��С����󡢵�ǰֵ
	int PageNum;					//ÿҳ������

	bool bDrop;						//�Ƿ��϶�
	int bCheck;						//�Ƿ���
	unsigned int lastcount;

public:
	CScroll();				//����
	~CScroll();				//����
	bool LoadIni(char *,char *);	//��ini�ļ�����
	void Show();			//��ʾ
	bool CheckLoop();		//���ѭ��

	int GetStyle()			{return Style;}		//���÷��
	int SetStyle(int style)	{Style=style;MakeRect();return Style;}

	int GetHeight()			{return Height;}	//���ó���
	int SetHeight(int h)	{Height=h;MakeRect();return Height;}

	int GetCurNum()			{return CurNum;}
	int GetMinNum()			{return MinNum;}
	int GetMaxNum()			{return MaxNum;}
	int GetPageNum()		{return PageNum;}

	int SetCurNum(int n);	//����
	int SetMinNum(int n);
	int SetMaxNum(int n);
	int SetPageNum(int n);

	bool InRect(RECT *, int, int);	//�������Ƿ���һ��������
	void CountNewRect();	//��������������λ��
	void MakeRect();		//���ɾ���
};

#endif