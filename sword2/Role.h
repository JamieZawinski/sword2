//********************************************
//	角色 相关函数
//  创建于2000年4月14日
//********************************************

#ifndef _ROLE_H_
#define _ROLE_H_

#define CenterX 304
#define CenterY 240
#define MAXGOODS 64
#define MAXARM 32

//人物的参数结构
struct stRolePropertiy{
	int Level;		//等级
	int Exp;		//经验
	int MaxExp;		//到下一级所需的经验

	int Hp;			//生命
	int MaxHp;		//生命最大值
	int Mp;			//魔法
	int MaxMp;		//魔法最大值

	int Attack;		//攻击
	int Defend;		//防御

	int Magic;		//魔力
	int Stamina;	//耐力
	int Luck;		//幸运
	int Speed;		//敏捷
	int Bp;			//体力0%-100%
	int Ip;			//精神0%-100%

	int Money;		//金钱
	WORD Arms;		//武器
	WORD Corselet;	//护具

	int CurGoodsNum;			//物品总数
	short Goods[MAXGOODS];		//物品（最多可带64种）
	short GoodsNum[MAXGOODS];	//每种物品的数量

	int CurArmNum;				//装备总数
	short Arm[MAXARM];			//装备（最多可带32种）
	short ArmNum[MAXARM];		//每种装备的数量

	int MagicsNum;				//当前魔法总数
	short Magics[10];			//装备的魔法

	char Fight_Ini_Name[64];	// 战斗中角色图片数据的ini文件
	int use;					// 用来判断此角色是否使用
};

//人物状态
struct stRoleState{
	//------状态数据----------
	int x, y;			//当前坐标 脚底中心点
	int oldx,oldy;		//上一步的坐标
	int X, Y;			//所在格子坐标
	int oldX, oldY;		//上一步
	byte CurrentStep;	//当前步数
	byte CurrentDir;	//当前方向
	byte LastDir;		//上一步的方向
	BOOL bActive;		//是否活动
	BOOL bShow;			//是否可见
	
	LPDIRECTDRAWSURFACE Surface;			//行走的页面
	LPDIRECTDRAWSURFACE lpShadow;			//影子页面
	int SW, SH;			//影子大小
	char SurfaceFileName[32];	//对应文件
};

//   2
// 1   3
//   0

//角色类
class CRole
{
public:
	//------通用数据----------
	int Id;				//角色编号
	char Name[32];		//名字
	int Width, Height;	//单张尺寸
	byte Max_Step;		//每个方向行走的步数
	byte Max_Dir;		//方向数
	byte StepList[4];	//行走图片变换列表
	byte DirList[4];	//行走方向列表
	POINT FootRect[4];	//脚底碰撞矩阵
	int StepSize;		//每步走的距离
	int StepSize2;		//斜向走的距离

	char ScriptFile[64];				//对应脚本
	stRolePropertiy Propertiy;		//人物属性
	stRoleState State;				//人物状态

	//------移动数据-----------
	POINT *Path;		//行走路线
	int PathSteps;		//总步数
	int PathCurrentStep;//当前步数
	int DesX, DesY;		//行走目标点
	bool bMoving;		//是否正在移动标志
	int StepStep;		//步伐判断
	BOOL bCuted;		//路径是否被减切
	int MoveX, MoveY;	//当前步的坐标变化
	bool bShowed;		//是否已经显示过一次
	int oldAttack;
	int oldDefend;

public:
	void ReCalcPro();	//重新计算攻击和防御
	void DelMagic(char *name);	//删除魔法
	void AddMagic(char* name);	//添加一个魔法
	BOOL bCancelTalk;	//是否取消和NPC的对话
	void AdjustPosition();
	CRole();			//构造
	~CRole();			//析构
	void Release();		//释放
	bool InitRole();	//初始化一个角色
	bool LoadRoleIni(char *, char *);	//读取角色设定
	bool CreateShadow();				//生产人物阴影

	bool LoadRole(char *);	//读取一个角色
	bool SaveRole(char *);	//保存一个角色
	void ShowRole();		//显示一个角色
	void ShowRole(int);		//alpha显示一个角色
	void Move(int,int);		//处理行走
	void Show(int,int);		//显示-处理碰撞检测等
	void ShowRole(int, int, int, int);	//在指定位置显示一个指定的角色

	bool MoveTo(int, int);	//移动到某点
	void ChangeScene();		//场景切换
	void RunScript();		//触发开关

	void Goto(int, int);	//运动到某点（循环）

	//物体相关
	void AddObject(char *, int num=1);			//加入一个物体
	void DelObject(char *, int num=1);			//删除一个物体
	int CheckObject(char *);				//检测一个物品的数量

	//装备相关
	void AddArm(char *, int num =1);			//加入一个装备
	void DelArm(char *, int num=1);			//删除一个装备
	int CheckArm(char *);				//检测一个装备的数量
	void SetupArm(char *);				//装备
	void SetupArm(int );				//装备
	void UnSetupArm(char *);			//卸除装备
	void UnSetupArm(int );			    //卸除装备
};

#endif