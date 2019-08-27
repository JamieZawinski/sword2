//********************************************
//	��Ʒ ��غ���
//  ������2000��7��07��
//********************************************

#ifndef _FIGHTGOODS_H_
#define _FIGHTGOODS_H_

#include "..\interface\listwindow.h"

class CRole;
struct stRolePropertiy;
struct stGoods;

//********ս����Ʒ��*********
class CFightGoods : public CListWindow
{
private:
	CWindow AboutWindow;			//˵������
	int number;						//��Ʒ���

public:
	int GetGoodsID();
	
	CFightGoods();
	~CFightGoods();

	void LoadWindowIni(char *, char *);		//���봰�ڵ�����

	int ShowGoodsListWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//��ʾ
	void ShowAboutWindow(LPDIRECTDRAWSURFACE, bool);	//��ʾ��Ʒ˵��

	void Init(LPDIRECTDRAWSURFACE);						//��ʼ������
	int DoMenu(LPDIRECTDRAWSURFACE , bool b=true);		//�˵�ѭ��

	int GoodEffect(int GoodNum, stRolePropertiy* Propertiy,int rolesel =0);	//ʹ����Ʒ�Ľ��

	int GetGoodsNumber(void);
	bool IsCanUse(int GoodNum);
};

#endif