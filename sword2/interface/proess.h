//********************************************
//	进度条 相关函数
//	创建于2001年5月12日
//********************************************

#ifndef _PROESS_H_
#define _PROESS_H_

//进度条类
class CProess
{
public:
	int Id;				//编号
	char Name[32];		//文字
	int x, y;			//坐标
	int Width, Height;	//大小

	char PicFileName[32];			//位图文件名
	LPDIRECTDRAWSURFACE Pic;		//位图
	RECT Normal_Rect, Over_Rect;	//位置
	RECT DestRect;					//目标位置

	char strDest[32];				//描述
	float fCurCount;				//当前值

public:
	CProess();				//构造
	~CProess();				//析构

	bool LoadIni(char *,char *);			//从ini文件读入
	void Show(LPDIRECTDRAWSURFACE);			//显示
	bool CheckLoop(LPDIRECTDRAWSURFACE);	//检测循环

	float GetCurCount()			{ return fCurCount; }
	void SetCurCount(float f)	{ fCurCount = f; }
	void AddCurCount(float f)	{ fCurCount += f; if(fCurCount > 1) fCurCount=1; }
	char* GetDest()	{ return strDest; }
};

#endif