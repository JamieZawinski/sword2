//********************************************
//	顶级菜单 相关函数
//  创建于2000年7月09日
//********************************************

#ifndef _MENUTOP_H_
#define _MENUTOP_H_

//********物品类*********
class CMenuTop : public CWindow{
public:
	int ButtonNum;			//按钮数量
	CCButton *Button;		//按钮

	int MoneyX, MoneyY;		//钱的位置

public:
	CMenuTop();
	~CMenuTop();

	void LoadMenuTopIni(char *, char *);						//读入配置
	void ShowMenuTop(LPDIRECTDRAWSURFACE, bool=true);	//显示
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);	//菜单循环
};

#endif