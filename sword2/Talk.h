//********************************************
//	对话 相关函数
//  创建于2000年5月01日
//********************************************

#ifndef _TALK_H_
#define _TALK_H_

#include "interface\interface.h"

class CIniSet;
class CAni;

class CTalk : public CWindow{
public:
	char FileName[MAX_PATH];		//对话文件
	CIniSet Dialog;			//对话内容
	char Index[64];			//对话标志
	int CurrentLine;		//当前行
	
	int photox, photoy;		//头像位置（单位：像素）
	int photox2,photoy2;	//头像位置二
	int photow, photoh;		//头像大小（单位：像素）
	int PhotoPlace;			//头像位置(0,1)

	char PhotoFilePath[128];	//对话头像图片目录

	LPDIRECTDRAWSURFACE lpBack;	//保存当前屏幕
	LPDIRECTDRAWSURFACE lpPhoto;//头像

	bool bPhoto;			//是否显示头像
	CAni Continue;			//闪烁的小箭头
	BOOL bFullDialog;		//是否使用整图对话框

	unsigned long NextTick;	//文字出现时的延时

	//----------------------------------------
	LPDIRECTDRAWSURFACE lpTalk;	//对话框底图
	RECT TalkRect;				//范围
	int TalkX, TalkY;	//位置
	WORD TalkAlphaColor;		//半透明的颜色

public:

	CTalk();
	~CTalk();
	bool Init(char *, char *);			//初始化对话框
	void OpenFile(char *);				//打开文件
	void CreatePhoto(char *filename);	//生成头像
	void ShowPhoto(LPDIRECTDRAWSURFACE, int bPhotoInleft);	//显示头像
	void ShowText(LPDIRECTDRAWSURFACE, char *);	//显示文本
	void Talk(LPDIRECTDRAWSURFACE&, char *, int bPhotoInLeft=1);	//对话
	void Say(LPDIRECTDRAWSURFACE&, char *, char *, int bPhotoInLeft=1);	//说一句话
};

#endif