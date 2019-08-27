//********************************************
//	消息窗口 相关函数
//  创建于2000年10月08日
//********************************************

#ifndef _MSGWINDOW_H_
#define _MSGWINDOW_H_

class CGpFont;

//消息窗口
class CMsgWindow : public CWindow
{
public:
	int Id;
	int x, y, Width, Height;

public:
	CMsgWindow();
	~CMsgWindow();

	bool LoadMsgWindowIni(char *, char *);	//读入窗口
	void ShowMsgWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示对话框
	void DoMenu(LPDIRECTDRAWSURFACE&, char *);		//菜单循环
};

#endif
