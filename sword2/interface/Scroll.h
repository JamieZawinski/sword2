//********************************************
//	滚动条 相关函数
//	创建于2000年7月06日
//********************************************

#ifndef _SCROLL_H_
#define _SCROLL_H_

enum SCROLLSTYLE{
	SS_VAR=0,			//垂直排列
	SS_HOR,				//水平排列
};

//滚动条类
class CScroll
{
public:
	int Id;				//编号
	int Style;			//风格
	int x, y;			//坐标
	int Width;			//宽度
	int Height;			//长度

	char PicFileName[32];			//位图文件名
	LPDIRECTDRAWSURFACE Pic;		//位图

	RECT Rect;						//滚动条位置
	RECT Sub_Rect, Sub2_Rect;
	RECT Add_Rect, Add2_Rect;		//按钮位置
	RECT Bar_Rect, Space_Rect;		//条位置和空白位置

	RECT RectSub, RectAdd;
	RECT RectPageSub, RectPageAdd;
	RECT RectBar, RectSpace;

	float StepSize;					//每步走的距离

	int MinNum, MaxNum, CurNum;		//最小、最大、当前值
	int PageNum;					//每页的数量

	bool bDrop;						//是否拖动
	int bCheck;						//是否按下
	unsigned int lastcount;

public:
	CScroll();				//构造
	~CScroll();				//析构
	bool LoadIni(char *,char *);	//从ini文件读入
	void Show();			//显示
	bool CheckLoop();		//检测循环

	int GetStyle()			{return Style;}		//设置风格
	int SetStyle(int style)	{Style=style;MakeRect();return Style;}

	int GetHeight()			{return Height;}	//设置长度
	int SetHeight(int h)	{Height=h;MakeRect();return Height;}

	int GetCurNum()			{return CurNum;}
	int GetMinNum()			{return MinNum;}
	int GetMaxNum()			{return MaxNum;}
	int GetPageNum()		{return PageNum;}

	int SetCurNum(int n);	//设置
	int SetMinNum(int n);
	int SetMaxNum(int n);
	int SetPageNum(int n);

	bool InRect(RECT *, int, int);	//检测鼠标是否在一个矩形内
	void CountNewRect();	//计算三个部件的位置
	void MakeRect();		//生成矩形
};

#endif