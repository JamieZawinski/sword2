//********************************************
//	������ʾ��ش�����
//  softboy ������2000��1��25��
//********************************************
#ifndef _GP_FONT_
#define _GP_FONT_

#include <windows.h>

//������
class CGpFont{
public:
	static DWORD FONTCOLOR[10];		//�̶�������ɫ
	static DWORD DELAYTICK[10];		//���ֳ��ֵ���ʱ

	HFONT font_Font;	//�������
	int font_Size;		//��С
	char *font_Family;	//����
	int font_Weight;	//�߿�(0-1000)
	int font_Exp;		//�����С����ֵ(0-699=0 700-1000=1)
	DWORD font_Color;	//��ɫ
	DWORD font_BK;		//�ֵı����Ƿ��ο�
	DWORD font_BKColor;	//����ɫ
	int font_Width;		//һ�е������������ģ�width/2
	int font_LineHeight;//�м��
	bool bChangeColor;	//��һ���������Ƿ���Ա任��ɫ
	int DelayTick;		//���������γ���ʱ����ʱ

public:
	void GetFontInfo(LPDIRECTDRAWSURFACE surf,TEXTMETRIC* tm);
	CGpFont(bool=false);

	void InitFont(int, char *);		//��ʼ������
	void LoadFont(char *,char *);	//�������ļ�������
	void SetColor(DWORD);			//������ɫ
	DWORD GetColor();				//��ȡ��ɫ
	void SetBK(bool);				//�������屳���Ƿ��ο�
	void Restore_Default_Font();	//�ָ�ȱʡ����
	void SetChangeColor(bool);		//������һ���������Ƿ���Ա任��ɫ

	~CGpFont();
	void PrintText(LPDIRECTDRAWSURFACE,int,int,char *,...);					//������ʾ
	void PrintAlphaText(LPDIRECTDRAWSURFACE, int, int, int, char *,...);	//Alpha������ʾ
};

#endif