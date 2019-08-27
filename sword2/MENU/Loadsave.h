//********************************************
//	进度 相关函数
//  创建于2000年10月09日
//********************************************

#ifndef _LOADSAVE_H_
#define _LOADSAVE_H_

#define MAX_RECORD 10

//********物品类*********
class CLoadSave : public CListWindow
{
	//存档结构
	struct stRecord{
		char Name[8];		//名称
		char About[32];		//说明
	};

public:
	int PicWidth, PicHeight;			//物品图片的大小
	stRecord Record[MAX_RECORD];	//存档

public:
	CLoadSave();
	~CLoadSave();

	void LoadWindowIni(char *, char *);		//读入窗口的配置
	void LoadRecord(char *);				//读入记录列表
	void SaveRecord(char *);				//写入记录列表

	void LoadGame(int);						//读入进度
	void SaveGame(int);						//存储进度
	void GetGameTime(char *);				//获取进度时间

	void ShowLoadSaveWindow(LPDIRECTDRAWSURFACE surf, bool bLoad=true, int left=-1, int top=-1);	//显示

	bool DoMenu(LPDIRECTDRAWSURFACE& , bool, int, int);	//菜单循环
};

#endif