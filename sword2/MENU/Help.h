//********************************************
//	关于 相关函数
//  创建于2000年10月22日
//********************************************

#ifndef _HELP_H_
#define _HELP_H_

//********关于类*********
class CHelp : public CWindow{
public:
	int page;			//页数

	CHelp();
	~CHelp();

	void LoadHelpIni(char *, char *);				//读入窗口的配置
	void Init(LPDIRECTDRAWSURFACE);					//初始化数据
	void ShowHelpWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示
	void DoMenu(LPDIRECTDRAWSURFACE, bool=true);	//菜单循环
};

#endif