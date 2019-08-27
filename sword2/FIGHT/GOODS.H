//********************************************
//	物品 相关函数
//  创建于2000年7月07日
//********************************************

class CRole;

//物品种类
enum GOODSCLASS{
	GC_ARMS=0,			//武器
	GC_CORSELET,		//护具
	GC_GOODS,			//物品
	GC_OHTER,			//其他
};

typedef struct{
	int Id;					//编号
	char Name[32];			//名字
	char About[128];		//功能
	int Class;				//类型
	int Money;				//价值
	
	int Exp;		//经验
	int Hp;			//生命
	int MaxHp;		//生命最大值
	int Mp;			//魔法
	int MaxMp;		//魔法最大值

	int Attack;		//攻击
	int Defend;		//防御
	int MagicAct;	//魔法攻击
	int MagicDef;	//魔法防御

	int Stamina;	//耐力
	int Luck;		//幸运
	int Speed;		//敏捷
	int Bp;			//体力
	int Ip;			//精神

	char PicFileName[32];		//图片文件名
	LPDIRECTDRAWSURFACE Pic;	//图片
}stGoods;

//********物品类*********
class CGoods : public CListWindow{
public:
	static int MAX_GOODS;			//物品数量
	static stGoods *GOODS;		//物品列表
	static RECT GoodsRect;			//物品图片大小

	int PicWidth, PicHeight;		//物品图片的大小
	CWindow AboutWindow;			//说明窗口

public:
	CGoods();
	~CGoods();

	void LoadWindowIni(char *, char *);		//读入窗口的配置
	void LoadGoodsIni(char *);				//读入物品列表

	void ShowGoodsListWindow(LPDIRECTDRAWSURFACE, bool bShowWindow=true, int left=-1, int top=-1);	//显示
	void ShowAboutWindow(LPDIRECTDRAWSURFACE, bool);	//显示物品说明

	void Init(LPDIRECTDRAWSURFACE);						//初始化数据
	void DoMenu(LPDIRECTDRAWSURFACE , bool =true);		//菜单循环

	static int GetGoodsNum(char *name);			//获取物体编号
	void UseGoods(int);							//使用一个物品
	int GoodEffect(int GoodNum, CRole *Role);	//使用物品的结果
};
