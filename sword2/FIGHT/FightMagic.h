
//--------------------------------------------------------------------	
//	ս��ħ�� ��غ���
//  ������2002��4��25��
//--------------------------------------------------------------------

#ifndef _FIGHTMAGIC_H_
#define _FIGHTMAGIC_H_

#include "..\interface\listwindow.h"

class CFightMagic  : public CListWindow
{
public:
	int MagicEffect(int num,stRolePropertiy* Propertiy,stRolePropertiy * Propertiy2);	//ħ��Ч��
	int GetMagicID();					//����ħ�����ֵõ�ID
	bool IsCanUse(int MagicNum,int rol_sel=0);	//�ò���ʹ�ø�ħ��
	int DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow,int rol_sel=0);
	void Init(LPDIRECTDRAWSURFACE surf);
	void ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel=0);
	int ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow =true,int rol_sel =0, int left=-1, int top=-1);
	void LoadWindowIni(char *filename, char *index);
	CFightMagic();
	virtual ~CFightMagic();
private:
	CWindow AboutWindow;			//˵������
	int number;						//ħ�����

};

#endif 
