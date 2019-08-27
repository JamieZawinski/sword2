//********************************************
//	窗口 相关函数
//  创建于2000年6月21日
//********************************************

#ifndef _WINDOW_H_
#define _WINDOW_H_

//窗口弹出风格
enum POPWINDOWSTYLE{
	PWS_NULL=0,		//无效果
	PWS_HOR,		//横向
	PWS_VER,		//纵向
	PWS_BOTH,		//放大
	PWS_RANDOM=-1,	//随机
};

class CGpFont;

class CWindow
{
public:
	int dialogx, dialogy;	//对话框位置（单位：像素）
	int dialogw, dialogh;	//对话框大小（单位：像素）
	int Alpha;				//对话框的Alpha值
	enum POPWINDOWSTYLE PopWindowStyle;		//对话框弹出风格
	int PopWindowStep;						//对话框弹出速度

	CGpFont Font;			//对话字体
	char PicFileName[64];	//图片文件名

	LPDIRECTDRAWSURFACE lpPic;	//对话框图片

public:
	CWindow();
	~CWindow();
	bool LoadWindowIni(char *, char *);	//读入窗口

	void ShowWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow=true, int left=-1, int top=-1);	//显示对话框
	void PopWindow(LPDIRECTDRAWSURFACE& surf, enum POPWINDOWSTYLE=PWS_NULL, int Num=8);	//弹出对话框
	int PressAnyKey();				//按任意健继续
};

#endif