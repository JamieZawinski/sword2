//********************************************
//	封面 相关函数
//  创建于2000年9月29日
//********************************************

#ifndef _TITLE_H_
#define _TITLE_H_

//********封面类*********
class CTitle{
public:
	int ButtonNum;			//按钮数量
	CCButton *Button;		//按钮

	int Select;				//当前选择
	char *BackGroundFile;	//背景图片文件

public:
	CGpFont ShowVersionFont;
	CTitle();
	~CTitle();

	void LoadTitleIni(char *, char *);					//读入配置
	void ShowTitle(LPDIRECTDRAWSURFACE, bool bShowWindow=true);		//显示
	void DoMenu(LPDIRECTDRAWSURFACE, bool bShowWindow=true);		//封面循环
private:
	char m_szVersion[10];
};

#endif