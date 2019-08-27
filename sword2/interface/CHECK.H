//********************************************
//	复选按钮 相关函数
//	创建于2000年7月10日
//********************************************

#ifndef _CHECK_H_
#define _CHECK_H_

//复选按钮类
class CCheck
{
public:
	int Id;				//编号
	char Name[32];		//文字
	int x, y;			//坐标
	int Width, Height;	//大小

	char PicFileName[32];			//位图文件名
	LPDIRECTDRAWSURFACE Pic;		//位图
	RECT Normal_Rect, Over_Rect;	//按钮位置

	CGpFont Font;					//文体
	bool bChecked;					//是否选中

public:
	CCheck();				//构造
	~CCheck();				//析构
	bool LoadIni(char *,char *);			//从ini文件读入
	void Show(LPDIRECTDRAWSURFACE);			//显示
	bool CheckLoop(LPDIRECTDRAWSURFACE);	//检测循环
};

#endif