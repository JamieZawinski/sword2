//********************************************
//	菜单 相关函数
//  创建于2000年7月09日
//********************************************

#ifndef _MENU_H_
#define _MENU_H_

//********物品类*********
class CMenu{
public:
	static int SelectItem;				//当前处的菜单项
	static bool bMenuRun;				//是否在菜单循环

	static CMenuTop MenuTop;			//顶级菜单
	static CRoleSel RoleSel;			//人物选择
	static CState State;				//（1）属性
	static CGoods Goods;				//（2）物品项
	static CArm Arm;					//（3）装备
	static CSystem System;				//（4）系统
	static CMagic Magic;				//魔法窗口
	static CLoadSave LoadSave;			//进度窗口
	static CHelp Help;					//关于窗口

	static CSound Sound_TileClick;		//开头
	static CSound Sound_HighLight;		//鼠标划过
	static CSound Sound_Click;			//按钮声音
	static CSound Sound_Change;			//切换菜单
	static CSound Sound_Pop;			//菜单弹出
	static CSound Sound_Cancel;			//菜单取消

public:
	CMenu();
	~CMenu();

	void LoadMenuIni(char *, char *);		//读入配置
	
	static void InitMenu();					//菜单初始化
	static void ReInitMenu();				//重置菜单
	void DoMenuLoop();						//菜单循环
};


#endif
