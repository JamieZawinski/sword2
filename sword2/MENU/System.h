//********************************************
//	系统菜单 相关函数
//  创建于2000年10月09日
//********************************************

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//********物品类*********
class CSystem : public CWindow{
public:
	int m_lSelect;			//当前选择

	int ButtonNum;			//按钮数量
	CCButton *Button;		//按钮

public:
	CSystem();
	~CSystem();

	int GetSelect()			{ return m_lSelect; }
	void SetSelect(int n)	{ m_lSelect = n; }

	void LoadSystemIni(char *, char *);					//读入配置
	void ShowSystem(LPDIRECTDRAWSURFACE, bool=true);	//显示
	int DoMenu(LPDIRECTDRAWSURFACE&, bool=true);		//菜单循环
};

#endif