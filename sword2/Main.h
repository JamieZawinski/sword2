//********************************************
//	主模块 相关函数
//  创建于2000年4月10日
//********************************************
#include <ddraw.h>

#define ScreenWidth	640		//游戏区宽度
#define ScreenHeight 480	//游戏区高度
#define CellWidth 32		//单个格子的宽度
#define CellHeight 32		//单个格子的高度

//#define _grid_x(x)	((x)/CellWidth)	//计算x点所处的格子
//#define _grid_y(y)	((y)/CellHeight)	//计算y点所处的格子
//#define _point_x(x)	((x)*CellWidth)	//计算X格子的绝对坐标
//#define _point_y(y) ((y)*CellHeight)	//计算Y格子的绝对坐标

#define _grid_x(x)	((x)>>5)	//计算x点所处的格子
#define _grid_y(y)	((y)>>5)	//计算y点所处的格子
#define _point_x(x)	((x)<<5)	//计算X格子的绝对坐标
#define _point_y(y) ((y)<<5)	//计算Y格子的绝对坐标

//游戏当前状态
enum ePLAYSTATE{
	PS_Null=0,		//空循环
	PS_MAIN=1,		//主循环
	PS_DIALOG,		//对话
	PS_SCROLLMAP,	//卷动地图
	PS_EDIT,		//编辑器主循环
	PS_FIGHT,		//战斗
};

//显示状
enum eDISPLAYSTATE{
	DS_NORMAL=0,	//正
	DS_NIGHT		//夜晚
};

/////////////////////////////////
//通用变量
/////////////////////////////////
class CMouse;
class CKey;

extern enum ePLAYSTATE g_ePlayState;		//当前状态
extern enum eDISPLAYSTATE g_eDisplayState;	//当前显示模式

extern LPDIRECTDRAWSURFACE lpDDSSour, lpDDSTemp;
extern LPDIRECTDRAWSURFACE lpDDSTools;		// 编辑器专用图素页面
extern LPDIRECTDRAWSURFACE lpDDSBackGround;	// 背景页面
extern LPDIRECTDRAWSURFACE *lpDDSMap;		// 地图地面页面

extern bool bShowFps;				//是否显示FPS
extern POINT point;					//鼠标坐标
extern int cx,cy;					//鼠标格子

extern int StartX,StartY;			//主区起始坐标
extern int EndX,EndY;				//主区结束坐标
extern int MainWidth,MainHeight;	//主区大小
extern int ShowCellW, ShowCellH;	//显示区域格子数

class CScript;
class CMessage;
class CEvents;
class CTitle;
class CMenu;
class CMap;
class CGpFont;
class CRole;
class CTalk;
class CEmote;
class CIniSet;
class CMsgWindow;
class CFight;

extern int RoleNum;				//主角数量（1-4）

extern CIniSet GameIni;			//游戏配置文件
extern CScript Script;			//游戏脚本机
extern CMessage Message;		//消息处理机
extern CEvents Events;			//特殊事件
extern CTitle Title;			//封面
extern CMenu Menu;				//菜单
extern CMap map;				//地图
extern CGpFont font;			//字体对象
extern CRole role[4];			//人物
extern CTalk talk;				//对话
extern CEmote emote;			//表情
extern CMsgWindow GameMessage;	//游戏消息框
extern CSound Sound;			//音效
extern CMusic Music;			//背景音乐
extern char CurrentPath[MAX_PATH];		//当前工作目录
extern unsigned mouse, key, joystick;	//鼠标、键盘和手柄的缓冲区数据接受
extern bool bControl;			//是否可控制
extern CFight Fight;			//战斗

extern int NowTick, OldTick, DelayTick;	//控速
extern int CurrentNpc;					//当前鼠标指向的NPC编号(-1=NULL)

///////////////////////////////////////////////////
//函数列表
///////////////////////////////////////////////////
void NewGame();					//新游戏初始化数据
void GetInputData();			//获取输入信息
void GameLoop();				//游戏主循环
void UpdateScreen_Dialog(LPDIRECTDRAWSURFACE);				//更新屏幕－对话
void ShowMouseCursor(LPDIRECTDRAWSURFACE lpSurf=lpDDSBack, bool Normal=true);	//画鼠标光标
void GetPrimarySurface(LPDIRECTDRAWSURFACE surf);			//把当前屏幕复制到后台表面
void GetGameSurface(LPDIRECTDRAWSURFACE surf);				//把游戏画面复制到一个表面上（不画鼠标和FPS等）
