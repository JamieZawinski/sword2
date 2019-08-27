//********************************************
//	人物选择 相关函数
//  创建于2000年7月09日
//********************************************

#ifndef _ROLESEL_H_
#define _ROLESEL_H_

//********状态类*********
class CRoleSel : public CWindow{
public:
	int ButtonNum;			//按钮数量
	CCButton *Role;			//按钮
	LPDIRECTDRAWSURFACE *PhotoPic;	//头像

	int SelectRole;			//当前选择的人物

public:
	CRoleSel();
	~CRoleSel();

	void LoadRoleSelIni(char *, char *);		//读入配置
	void ShowRoleSelWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示

	void Init(LPDIRECTDRAWSURFACE, bool=true);				//初始化数据
	int DoMenu(LPDIRECTDRAWSURFACE& surf, bool=true);		//菜单循环
};

#endif