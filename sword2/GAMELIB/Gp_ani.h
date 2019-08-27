//********************************************
//	���� ��غ���
//  ������2000��4��16��
//********************************************
#ifndef _GP_ANI_
#define _GP_ANI_

//������
class CAni
{
	int x,y;			//������ʾλ��
	int tx, ty;			//�ȵ�ƫ����
	int Width, Height;	//������С

	int Max_Pics;		//ͼƬ��Ŀ
	int Cur_Pic;		//��ʾ�ĵ�ǰ��
	int ShowCount;		//��ʾ������( 0:����ʾ -1:һֱ��ʾ  >0:ÿ�μ�1 )
	RECT Cur_Rect;		//��ǰ����
	
	LPDIRECTDRAWSURFACE Pics;	//ͼƬ�б�
	char FileName[32];			//ͼƬ�ļ���
	int WLines, HLines;			//ͼƬ�ĺ������и���
	BOOL bArrayH;				//��������

	BOOL bColorKey;		//�Ƿ�͸��
	BOOL bActive;		//�Ƿ�
	BOOL bShow;			//�Ƿ���ʾ
	
	unsigned int OldTick, Delay;	//��һ�ε�ʱ�䣬��֡��ļ��ʱ��

public:
	CAni();		//��ʼ��
	~CAni();	//�ͷ�
	
	bool LoadAni(char *, char *);			//����һ������
	void ShowAni(LPDIRECTDRAWSURFACE, int, int, int Cur_Picture=-1);		//��ʾ��ǰ֡��Ȼ��ѵ�ǰ���һ
	void ShowAni(LPDIRECTDRAWSURFACE);		//��ʾ��ǰ֡��Ȼ��ѵ�ǰ���һ
	
	int Getx()					{return x;}
	int Gety()					{return y;}
	void SetPos(int x1, int y1)	{x=x1;	y=y1;}			//����x,y
	void SetCurPic(int num)		{Cur_Pic=num%Max_Pics;}	//���õ�ǰ��
	int GetCurPic()				{return Cur_Pic;}		//��ȡ��ǰ��
	void SetShowCount(int num)	{ShowCount=num;}		//������ʾ������
	int GetShowCount()	{return ShowCount;}				//��ȡ���ʱ��
	void SetDelay(int num)		{Delay=num;}			//���ü��ʱ��
	int GetDelay()				{return Delay;}			//��ȡ���ʱ��
	void SetActiveFlag(BOOL boo){bActive=boo;}			//���û��־
	BOOL GetActiveFlag()		{return bActive;}		//��ȡ���־
	void SetShowFlag(BOOL boo)	{bShow=boo;}			//������ʾ��־
	BOOL GetShowFlag()			{return bShow;}			//��ȡ��ʾ��־
};

#endif