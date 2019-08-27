//********************************************
//	按钮 相关函数
//	创建于2000年6月19日
//********************************************

#ifndef _BUTTON_H_
#define _BUTTON_H_

//按钮类
class CCButton
{
public:
	enum BUTTONSTYLE
	{
		BS_NORMAL=0,	//普通
	};

	int Id;				//编号
	char Name[32];		//文字
	int Style;			//风格
	int x, y;			//坐标
	int Width, Height;	//大小
	char PicFileName[32];			//位图文件名
	LPDIRECTDRAWSURFACE Pic;		//位图
	RECT Normal_Rect, Over_Rect, Down_Rect;		//按钮位置
	bool* m_bTest;								//按钮区域
public:
	CCButton();						//构造
	~CCButton();					//析构
	bool LoadIni(char *,char *);	//从ini文件读入
	void Show(LPDIRECTDRAWSURFACE, int bHighLight=-1);			//显示
	BOOL CheckLoop(LPDIRECTDRAWSURFACE, int bHighLight=-1);		//检测循环
protected:
	bool CreateRgn();				//创建按钮区域
private:
	
	BOOL CheckIn();					//测试是否在有效区域里面
};

#endif