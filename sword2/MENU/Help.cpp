//********************************************
//	关于 相关函数
//  创建于2000年10月22日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

char *About[]={
"　　《圣II－番外篇》是金点小组“圣剑系列”的一个外传，主角沿用了《圣II－双刃剑》的主角兰特，然后加入新的角色和场景，重新演绎了一个新的故事。</n>\
　　我们以《番外篇》来结束“圣剑II”的故事……同时也开放了所有的游戏资源，以后故事就由大家来演绎了！</n>\
　　在这里要感谢大家一直以来对“圣剑”的支持、感谢曾经关心过《英雄救美》和《圣II－双刃剑》的朋友们，正是由于你们的存在，才让我们有勇气来面对新的挑战；也正是由于你们曾经给予的鼓励，才让“圣剑系列”一代比一代进步，应该说“圣剑”凝结的是大家的心血。\
</n>\
　　希望大家继续关注金点，继续支持“圣剑英雄传”，谢谢！", \
"\
　　我们的主页“金点时空”：</n>\
　　http://www.gpgame.com</n>\
</n>\
    sleepd : sleepd@163.net</n>\
  endedman : ended-man@263.net</n>\
   sea_bug : lindehui@263.net</n>\
      soft : softboy@sina.com</n>\
      星仔 : gundamy@telekbird.com.cn</n>\
      狗狗 : jimmy_dog@sina.com</n>\
     yahle : yahle@sina.com</n>\
     tiamo : tiamo@mail.bnu.edu.cn</n>\
    zrhspy : zrhspy@163.com</n>\
   skywind : skywindt@yeah.net</n>\
   amiyami : nzy830222@263.net</n>\
  草稚旭日 : johndragon@sohu.com</n>\
\
       金点工作组　２００２"
};

CHelp::CHelp()
{
	page=0;
}

CHelp::~CHelp()
{
}

//读取窗口
void CHelp::LoadHelpIni(char *filename, char *index)
{
	//窗口
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "HelpFile");
	char* strIndex = Ini.ReadText(index, "HelpIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;
}

//初始化数据
void CHelp::Init(LPDIRECTDRAWSURFACE surf)
{
	GetGameSurface(lpDDSBackGround);
	FillSurface(lpDDSBack, RGB16(0));
	ABlt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, 0, 24);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
}

//显示
void CHelp::ShowHelpWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	DWORD c=Font.GetColor();
	//阴影显示文字
	Font.SetColor(0);

	Font.PrintText(surf, dialogx+40, dialogy+40,  About[page]);

	Font.SetColor(c);

	Font.PrintText(surf, dialogx+40, dialogy+40,  About[page]);
}

//循环
void CHelp::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	if( mouse==LB_UP||A_UP )
	{
		page = 1 -page;	//换页
		CMenu::ReInitMenu();
		ShowHelpWindow(lpDDSBack, false);
	}
}
