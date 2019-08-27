
//--------------------------------------------------------------------	
//	战斗魔法 相关函数
//  创建于2002年4月25日
//--------------------------------------------------------------------

#ifndef _FIGHTMAGIC_H_
#define _FIGHTMAGIC_H_

#include "..\interface\listwindow.h"

class CFightMagic  : public CListWindow
{
public:
	int MagicEffect(int num,stRolePropertiy* Propertiy,stRolePropertiy * Propertiy2);	//魔法效果
	int GetMagicID();					//根据魔法名字得到ID
	bool IsCanUse(int MagicNum,int rol_sel=0);	//让不让使用改魔法
	int DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow,int rol_sel=0);
	void Init(LPDIRECTDRAWSURFACE surf);
	void ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel=0);
	int ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow =true,int rol_sel =0, int left=-1, int top=-1);
	void LoadWindowIni(char *filename, char *index);
	CFightMagic();
	virtual ~CFightMagic();
private:
	CWindow AboutWindow;			//说明窗口
	int number;						//魔法编号

};

#endif 
