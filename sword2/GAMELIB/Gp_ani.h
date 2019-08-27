//********************************************
//	动画 相关函数
//  创建于2000年4月16日
//********************************************
#ifndef _GP_ANI_
#define _GP_ANI_

//动画类
class CAni
{
	int x,y;			//动画显示位置
	int tx, ty;			//热点偏移量
	int Width, Height;	//动画大小

	int Max_Pics;		//图片数目
	int Cur_Pic;		//显示的当前侦
	int ShowCount;		//显示记数器( 0:不显示 -1:一直显示  >0:每次减1 )
	RECT Cur_Rect;		//当前矩形
	
	LPDIRECTDRAWSURFACE Pics;	//图片列表
	char FileName[32];			//图片文件名
	int WLines, HLines;			//图片的横竖排列个数
	BOOL bArrayH;				//横向优先

	BOOL bColorKey;		//是否透明
	BOOL bActive;		//是否活动
	BOOL bShow;			//是否显示
	
	unsigned int OldTick, Delay;	//上一次的时间，两帧间的间隔时间

public:
	CAni();		//初始化
	~CAni();	//释放
	
	bool LoadAni(char *, char *);			//读入一个动画
	void ShowAni(LPDIRECTDRAWSURFACE, int, int, int Cur_Picture=-1);		//显示当前帧，然后把当前侦加一
	void ShowAni(LPDIRECTDRAWSURFACE);		//显示当前帧，然后把当前侦加一
	
	int Getx()					{return x;}
	int Gety()					{return y;}
	void SetPos(int x1, int y1)	{x=x1;	y=y1;}			//设置x,y
	void SetCurPic(int num)		{Cur_Pic=num%Max_Pics;}	//设置当前桢
	int GetCurPic()				{return Cur_Pic;}		//获取当前桢
	void SetShowCount(int num)	{ShowCount=num;}		//设置显示记数器
	int GetShowCount()	{return ShowCount;}				//获取间隔时间
	void SetDelay(int num)		{Delay=num;}			//设置间隔时间
	int GetDelay()				{return Delay;}			//获取间隔时间
	void SetActiveFlag(BOOL boo){bActive=boo;}			//设置活动标志
	BOOL GetActiveFlag()		{return bActive;}		//获取活动标志
	void SetShowFlag(BOOL boo)	{bShow=boo;}			//设置显示标志
	BOOL GetShowFlag()			{return bShow;}			//获取显示标志
};

#endif