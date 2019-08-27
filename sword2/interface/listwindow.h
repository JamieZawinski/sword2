//********************************************
//	ͼ���б�� ��غ���
//  ������2000��7��07��
//********************************************

#ifndef _LISTBOX_H_
#define _LISTBOX_H_

#include "window.h"
#include "scroll.h"

class CScroll;
class CGpFont;

//�б����
class CListWindow : public CWindow
{
public:
	//����ṹ
	struct stColumn{
		int x;			//��ʾλ��
		int Align;		//���뷽ʽ 0=left 1=right 2=center
		int Width;		//���
		char *Name;		//����
	};

	int Id;
	int x, y, Width, Height;
	int LineHeight;					//�и�

	int ColumnHeight;				//����߶�
	WORD ColumnColor;				//��ɫ
	CGpFont ColumnFont;				//��������

	int ColumnNum;					//��������
	stColumn *Column;				//����

	CScroll ScrollBar;				//������

	int Select;						//��ǰѡ��
	WORD SelectColor;				//��ѡ��Ԫ�ĵ�ɫ

public:
	CListWindow();
	~CListWindow();

	bool LoadListWindowIni(char *, char *);	//�����б���
	void ShowListWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ�Ի���
};

#endif
