//********************************************
//	状态 相关函数
//  创建于2000年7月09日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

CState::CState()
{
}

CState::~CState()
{
}

//读取窗口
void CState::LoadStateIni(char *filename, char *index)
{
	//窗口
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "StateFile");
	char* strIndex = Ini.ReadText(index, "StateIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;

	PhotoWidth=Ini.ReadInt(index, "PhotoWidth");
	PhotoHeight=Ini.ReadInt(index, "PhotoHeight");
	PhotoRect=GetRect(0, 0, PhotoWidth, PhotoHeight);

	Button_Left.LoadIni(filename, "left");
	Button_Left.x+=dialogx;
	Button_Left.y+=dialogy;

	Button_Right.LoadIni(filename, "right");
	Button_Right.x+=dialogx;
	Button_Right.y+=dialogy;

	//魔法窗口
	strFile = Ini.ReadText(index, "MagicWindowFile");
	strIndex = Ini.ReadText(index, "MagicWindowIndex");
	Window_Magic.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	Window_Magic.dialogx+=dialogx;
	Window_Magic.dialogy+=dialogy;
	Window_Magic.dialogx-=32;
	Window_Magic.dialogy-=32;
	Window_Magic.dialogw+=64;
	Window_Magic.dialogh+=64;

	//装备窗口
	strFile = Ini.ReadText(index, "SetupWindowFile");
	strIndex = Ini.ReadText(index, "SetupWindowIndex");
	Window_Setup.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	Window_Setup.dialogx+=dialogx;
	Window_Setup.dialogy+=dialogy;
	Window_Setup.dialogx-=32;
	Window_Setup.dialogy-=32;
	Window_Setup.dialogw+=64;
	Window_Setup.dialogh+=64;
}

//初始化数据
void CState::Init(LPDIRECTDRAWSURFACE surf)
{
	GetGameSurface(surf);		//得到一个游戏画面
	PopWindow(surf, PopWindowStyle, PopWindowStep);		//弹出窗口
}

//显示
void CState::ShowStateWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int sel_role=CMenu::RoleSel.SelectRole;			//选择的主角

	//头像
	Blt(surf, dialogx+60, dialogy+42, CMenu::RoleSel.PhotoPic[role[sel_role].Id], PhotoRect, true);

	//横线
	Bar(surf, dialogx+32+5, dialogy+102, dialogx+32+dialogw-74, dialogy+103, RGB16(240,211,123));
	Bar(surf, dialogx+32+5, dialogy+103, dialogx+32+dialogw-74, dialogy+104, RGB16(156,119,5));
	Bar(surf, dialogx+32+5, dialogy+330, dialogx+32+dialogw-74, dialogy+331, RGB16(240,211,123));
	Bar(surf, dialogx+32+5, dialogy+331, dialogx+32+dialogw-74, dialogy+332, RGB16(156,119,5));
	//阴影显示
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	dialogx++;
	dialogy++;

	Font.PrintText(surf, dialogx+200, dialogy+42,  "%6s  Lv:%d", role[sel_role].Name, role[sel_role].Propertiy.Level);
	Font.PrintText(surf, dialogx+200, dialogy+61, "HP: %6d/%d", role[sel_role].Propertiy.Hp, role[sel_role].Propertiy.MaxHp );
	Font.PrintText(surf, dialogx+200, dialogy+79, "MP: %6d/%d", role[sel_role].Propertiy.Mp, role[sel_role].Propertiy.MaxMp );
	Font.PrintText(surf, dialogx+50, dialogy+120, "攻击:%8d", role[sel_role].Propertiy.Attack );
	Font.PrintText(surf, dialogx+50, dialogy+145, "防御:%8d", role[sel_role].Propertiy.Defend );
	Font.PrintText(surf, dialogx+50, dialogy+170, "魔法:%8d", role[sel_role].Propertiy.Magic );
	Font.PrintText(surf, dialogx+50, dialogy+220, "耐力:%8d", role[sel_role].Propertiy.Stamina );
	Font.PrintText(surf, dialogx+50, dialogy+245, "幸运:%8d", role[sel_role].Propertiy.Luck );
	Font.PrintText(surf, dialogx+50, dialogy+270, "敏捷:%8d", role[sel_role].Propertiy.Speed );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+39, "武器:%s", CGoods::GOODS[role[sel_role].Propertiy.Arms].Name );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+69, "护具:%s", CGoods::GOODS[role[sel_role].Propertiy.Corselet].Name );
	Font.PrintText(surf, dialogx+50, dialogy+340,  "经　　验：%6d/%d", role[sel_role].Propertiy.Exp, role[sel_role].Propertiy.MaxExp );

	Font.SetColor(c);
	dialogx--;
	dialogy--;

	Font.PrintText(surf, dialogx+200, dialogy+42,  "<c4>%6s</c>  Lv:<c4>%d</c>", role[sel_role].Name, role[sel_role].Propertiy.Level);
	Font.PrintText(surf, dialogx+200, dialogy+61, "HP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Hp, role[sel_role].Propertiy.MaxHp );
	Font.PrintText(surf, dialogx+200, dialogy+79, "MP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Mp, role[sel_role].Propertiy.MaxMp );
	Font.PrintText(surf, dialogx+50, dialogy+120, "攻击:<c4>%8d", role[sel_role].Propertiy.Attack );
	Font.PrintText(surf, dialogx+50, dialogy+145, "防御:<c4>%8d", role[sel_role].Propertiy.Defend );
	Font.PrintText(surf, dialogx+50, dialogy+170, "魔法:<c4>%8d", role[sel_role].Propertiy.Magic );
	Font.PrintText(surf, dialogx+50, dialogy+220, "耐力:<c4>%8d", role[sel_role].Propertiy.Stamina );
	Font.PrintText(surf, dialogx+50, dialogy+245, "幸运:<c4>%8d", role[sel_role].Propertiy.Luck );
	Font.PrintText(surf, dialogx+50, dialogy+270, "敏捷:<c4>%8d", role[sel_role].Propertiy.Speed );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+39, "武器:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Arms].Name );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+69, "护具:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Corselet].Name );
	Font.PrintText(surf, dialogx+50, dialogy+340,  "经　　验：<c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Exp, role[sel_role].Propertiy.MaxExp );
	//显示装备窗口的内容
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+34, CGoods::GOODS[role[sel_role].Propertiy.Arms].Pic, CGoods::GoodsRect, true);
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+64, CGoods::GOODS[role[sel_role].Propertiy.Corselet].Pic, CGoods::GoodsRect, true);
	//魔法窗口的内容
	for(int i=0;i<role[sel_role].Propertiy.MagicsNum/5+1;i++)
		for(int j=0;j<role[sel_role].Propertiy.MagicsNum%5;j++)
			Blt(surf,Window_Magic.dialogx+36+j*30,Window_Magic.dialogy+39+i*30,
				CMagic::MAGIC[role[sel_role].Propertiy.Magics[i*4+j]].Pic,CMagic::MagicRect,true);
}

//循环
void CState::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	//切换按钮
	if( Button_Right.CheckLoop(surf)==1 || LEFT_DOWN )
	{
		CMenu::Sound_Click.Play();
		CMenu::RoleSel.SelectRole++;
		if( CMenu::RoleSel.SelectRole>=RoleNum )
			CMenu::RoleSel.SelectRole=0;
		CMenu::ReInitMenu();
	}

	if( Button_Left.CheckLoop(surf)==1 || RIGHT_DOWN )
	{
		CMenu::Sound_Click.Play();
		CMenu::RoleSel.SelectRole--;
		if( CMenu::RoleSel.SelectRole<0 )
			CMenu::RoleSel.SelectRole=RoleNum-1;
		CMenu::ReInitMenu();
	}

}
