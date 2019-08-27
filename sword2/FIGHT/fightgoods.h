//********************************************
//	物品 相关函数
//  创建于2000年7月07日
//********************************************

#ifndef _FIGHTGOODS_H_
#define _FIGHTGOODS_H_

#include "..\interface\listwindow.h"

class CRole;
struct stRolePropertiy;
struct stGoods;

//********战斗物品类*********
class CFightGoods : public CListWindow
{
private:
	CWindow AboutWindow;			//说明窗口
	int number;						//物品编号

public:
	int GetGoodsID();
	
	CFightGoods();
	~CFightGoods();

	void LoadWindowIni(char *, char *);		//读入窗口的配置

	int ShowGoodsListWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示
	void ShowAboutWindow(LPDIRECTDRAWSURFACE, bool);	//显示物品说明

	void Init(LPDIRECTDRAWSURFACE);						//初始化数据
	int DoMenu(LPDIRECTDRAWSURFACE , bool b=true);		//菜单循环

	int GoodEffect(int GoodNum, stRolePropertiy* Propertiy,int rolesel =0);	//使用物品的结果

	int GetGoodsNumber(void);
	bool IsCanUse(int GoodNum);
};

#endif