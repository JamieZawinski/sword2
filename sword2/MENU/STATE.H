//********************************************
//	状态 相关函数
//  创建于2000年7月09日
//********************************************

#ifndef _STATE_H_
#define _STATE_H_

//********状态类*********
class CState : public CWindow{
public:
	int PhotoWidth, PhotoHeight;
	RECT PhotoRect;

	CCButton Button_Left, Button_Right;	//左右换人按钮
	CWindow Window_Magic;				//魔法窗口
	CWindow Window_Setup;				//装备窗口

public:
	CState();
	~CState();

	void LoadStateIni(char *, char *);				//读入窗口的配置
	void Init(LPDIRECTDRAWSURFACE);					//初始化数据
	void ShowStateWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);	//菜单循环
};

#endif