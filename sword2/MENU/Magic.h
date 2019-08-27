
//--------------------------------------------------------------------
//	魔法 相关函数
//  创建于2002年4月25日
//--------------------------------------------------------------------

#ifndef _MAGIC_H_
#define _MAGIC_H_
//魔法种类
enum MAGICCLASS{
	MC_SELF=0,			//自己
	MC_ENEMY,			//对敌人
	MC_OHTER,			//其他
};
struct stMagic{
	int Id;					//编号
	char Name[32];			//名字
	char About[128];		//功能
	int Class;				//类型

	int Hp;			//生命
	int MaxHp;		//生命最大值
	int Mp;			//魔法

	int Attack;		//攻击
	int Defend;		//防御

	int Magic;		//魔力
	int Stamina;	//耐力
	int Luck;		//幸运
	int Speed;		//敏捷
	int Bp;			//体力
	int Ip;			//精神
	char PicFileName[64];		//图片文件名
	LPDIRECTDRAWSURFACE Pic;	//图片
	char EffectPicFileName[64];		//图片文件名
	LPDIRECTDRAWSURFACE EffectPic;	//效果图片
	char MagicFileName[64];		//战斗时候数据文件名
	char SoundFileName[64];		//战斗时候声音数据文件名
};

//-----------魔法---------------------
class CMagic : public CListWindow 
{
public:
	static int GetMagicNum(char * name);	// 得到魔法id
	int MagicEffect(int Num, CRole *Role,CRole *RoleReceive);	// 魔法效果
	void UseMagic(int sel,int sel_role);	// 使用魔法
	void ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow = true, int left =-1, int top=-1);	// 显示魔法窗口
	void LoadMagicIni(char * filename);		// 初始化魔法数据
	void ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow);	// about窗口
	void DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow);	
	static int MAX_MAGIC;			//魔法数量
	static stMagic *MAGIC;			//魔法列表
	static RECT MagicRect;			//魔法图片大小
	static int PicWidth, PicHeight;	//魔法图片的大小
	CWindow AboutWindow;			//说明窗口

	CMagic();
	void Init(LPDIRECTDRAWSURFACE);						//初始化数据
	void LoadWindowIni(char *filename, char *index);	// 初始化窗口
	virtual ~CMagic();
};

#endif 