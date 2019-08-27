//********************************************
//	������ ��غ���
//	������2001��5��12��
//********************************************

#ifndef _PROESS_H_
#define _PROESS_H_

//��������
class CProess
{
public:
	int Id;				//���
	char Name[32];		//����
	int x, y;			//����
	int Width, Height;	//��С

	char PicFileName[32];			//λͼ�ļ���
	LPDIRECTDRAWSURFACE Pic;		//λͼ
	RECT Normal_Rect, Over_Rect;	//λ��
	RECT DestRect;					//Ŀ��λ��

	char strDest[32];				//����
	float fCurCount;				//��ǰֵ

public:
	CProess();				//����
	~CProess();				//����

	bool LoadIni(char *,char *);			//��ini�ļ�����
	void Show(LPDIRECTDRAWSURFACE);			//��ʾ
	bool CheckLoop(LPDIRECTDRAWSURFACE);	//���ѭ��

	float GetCurCount()			{ return fCurCount; }
	void SetCurCount(float f)	{ fCurCount = f; }
	void AddCurCount(float f)	{ fCurCount += f; if(fCurCount > 1) fCurCount=1; }
	char* GetDest()	{ return strDest; }
};

#endif