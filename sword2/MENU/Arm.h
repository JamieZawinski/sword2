//********************************************
//	装备 相关函数
//  创建于2000年9月27日
//********************************************

#ifndef _ARM_H_
#define _ARM_H_

//********装备类*********
class CArm : public CWindow{
public:
	int PhotoWidth, PhotoHeight;
	RECT PhotoRect;

	CCButton Button_Left, Button_Right;	//左右换人按钮
	CWindow Window_Setup;				//装备窗口

	CListWindow ArmList;				//装备列表框

public:
	CArm();
	~CArm();

	void LoadArmIni(char *, char *);	//读入窗口的配置
	void Init(LPDIRECTDRAWSURFACE);		//初始化数据
	void ShowArmWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示
	void ShowArmListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top);	//显示装备列表框
	void ShowChangeNum(LPDIRECTDRAWSURFACE, int, int);	//装备更换数值预显
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);		//菜单循环
};

#endif