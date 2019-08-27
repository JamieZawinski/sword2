//********************************************
//	地图编辑 相关函数
//  创建于2000年4月10日
//********************************************

#include "..\map.h"

//编辑器当前状态
enum CURRENTLEVEL{
	CL_GROUND=0,		//地面
	CL_OBJECT,			//物体
	CL_OBJECT2,			//物体第二层
	CL_PIC_SEL,			//选择静态图素
	CL_ANI_SEL,			//选择动态图素
};

///////////////////////////////////
//场景编辑器专用
///////////////////////////////////
extern int cx2, cy2;			//临时鼠标格子

extern bool bShowBackGround;	//显示背景
extern bool bShowCell;			//显示表格
extern bool bShowObject;		//显示物体
extern bool bShowBlock;			//显示阻挡关系
extern bool bShowCurrent;		//显示当前物体
extern bool bShowEdit;			//显示编辑提示（物体、切换点、方格）
extern bool bShowOnlyObject;	//显示物体
extern bool bShowHardCursor;	//是否显示硬件光标
extern bool bShowFps;			//是否显示FPS

extern int Max_GroundPic;		//地图地面页面数
extern int Cur_GroundPic;		//当前选中地面的编号
extern int Cur_Ground;			//当前选中的地面
extern int Cur_MouseType;		//当前选择的鼠标样式

extern int Max_Level;			//总层数
extern int Cur_Level;			//当前编辑的层(0=地面 1=物体)

extern char *Level[];

extern CMap::stCell *UndoMap;	//反悔地图
typedef CMap::stCell Cell_Struct;
///////////////////////////////////////////////////
//函数列表
///////////////////////////////////////////////////
void EditLoop();					//编辑器主循环
void EditGround(char);				//编辑地面层
void EditObj(char);					//编辑物体层
void SelectGround(char);			//选择静态图素
void SelectAni(char);				//选择动态图素
void ClearEditData();				//清空编辑数据
void ShowCell(LPDIRECTDRAWSURFACE);	//显示网格
void PopupMenu();					//弹出菜单

void MapEditProc(long msg);			//处理编辑器右键菜单消息