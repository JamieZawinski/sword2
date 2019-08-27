//********************************************
//	״̬ ��غ���
//  ������2000��7��09��
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

//��ȡ����
void CState::LoadStateIni(char *filename, char *index)
{
	//����
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

	//ħ������
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

	//װ������
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

//��ʼ������
void CState::Init(LPDIRECTDRAWSURFACE surf)
{
	GetGameSurface(surf);		//�õ�һ����Ϸ����
	PopWindow(surf, PopWindowStyle, PopWindowStep);		//��������
}

//��ʾ
void CState::ShowStateWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int sel_role=CMenu::RoleSel.SelectRole;			//ѡ�������

	//ͷ��
	Blt(surf, dialogx+60, dialogy+42, CMenu::RoleSel.PhotoPic[role[sel_role].Id], PhotoRect, true);

	//����
	Bar(surf, dialogx+32+5, dialogy+102, dialogx+32+dialogw-74, dialogy+103, RGB16(240,211,123));
	Bar(surf, dialogx+32+5, dialogy+103, dialogx+32+dialogw-74, dialogy+104, RGB16(156,119,5));
	Bar(surf, dialogx+32+5, dialogy+330, dialogx+32+dialogw-74, dialogy+331, RGB16(240,211,123));
	Bar(surf, dialogx+32+5, dialogy+331, dialogx+32+dialogw-74, dialogy+332, RGB16(156,119,5));
	//��Ӱ��ʾ
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	dialogx++;
	dialogy++;

	Font.PrintText(surf, dialogx+200, dialogy+42,  "%6s  Lv:%d", role[sel_role].Name, role[sel_role].Propertiy.Level);
	Font.PrintText(surf, dialogx+200, dialogy+61, "HP: %6d/%d", role[sel_role].Propertiy.Hp, role[sel_role].Propertiy.MaxHp );
	Font.PrintText(surf, dialogx+200, dialogy+79, "MP: %6d/%d", role[sel_role].Propertiy.Mp, role[sel_role].Propertiy.MaxMp );
	Font.PrintText(surf, dialogx+50, dialogy+120, "����:%8d", role[sel_role].Propertiy.Attack );
	Font.PrintText(surf, dialogx+50, dialogy+145, "����:%8d", role[sel_role].Propertiy.Defend );
	Font.PrintText(surf, dialogx+50, dialogy+170, "ħ��:%8d", role[sel_role].Propertiy.Magic );
	Font.PrintText(surf, dialogx+50, dialogy+220, "����:%8d", role[sel_role].Propertiy.Stamina );
	Font.PrintText(surf, dialogx+50, dialogy+245, "����:%8d", role[sel_role].Propertiy.Luck );
	Font.PrintText(surf, dialogx+50, dialogy+270, "����:%8d", role[sel_role].Propertiy.Speed );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+39, "����:%s", CGoods::GOODS[role[sel_role].Propertiy.Arms].Name );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+69, "����:%s", CGoods::GOODS[role[sel_role].Propertiy.Corselet].Name );
	Font.PrintText(surf, dialogx+50, dialogy+340,  "�������飺%6d/%d", role[sel_role].Propertiy.Exp, role[sel_role].Propertiy.MaxExp );

	Font.SetColor(c);
	dialogx--;
	dialogy--;

	Font.PrintText(surf, dialogx+200, dialogy+42,  "<c4>%6s</c>  Lv:<c4>%d</c>", role[sel_role].Name, role[sel_role].Propertiy.Level);
	Font.PrintText(surf, dialogx+200, dialogy+61, "HP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Hp, role[sel_role].Propertiy.MaxHp );
	Font.PrintText(surf, dialogx+200, dialogy+79, "MP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Mp, role[sel_role].Propertiy.MaxMp );
	Font.PrintText(surf, dialogx+50, dialogy+120, "����:<c4>%8d", role[sel_role].Propertiy.Attack );
	Font.PrintText(surf, dialogx+50, dialogy+145, "����:<c4>%8d", role[sel_role].Propertiy.Defend );
	Font.PrintText(surf, dialogx+50, dialogy+170, "ħ��:<c4>%8d", role[sel_role].Propertiy.Magic );
	Font.PrintText(surf, dialogx+50, dialogy+220, "����:<c4>%8d", role[sel_role].Propertiy.Stamina );
	Font.PrintText(surf, dialogx+50, dialogy+245, "����:<c4>%8d", role[sel_role].Propertiy.Luck );
	Font.PrintText(surf, dialogx+50, dialogy+270, "����:<c4>%8d", role[sel_role].Propertiy.Speed );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+39, "����:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Arms].Name );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+69, "����:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Corselet].Name );
	Font.PrintText(surf, dialogx+50, dialogy+340,  "�������飺<c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Exp, role[sel_role].Propertiy.MaxExp );
	//��ʾװ�����ڵ�����
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+34, CGoods::GOODS[role[sel_role].Propertiy.Arms].Pic, CGoods::GoodsRect, true);
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+64, CGoods::GOODS[role[sel_role].Propertiy.Corselet].Pic, CGoods::GoodsRect, true);
	//ħ�����ڵ�����
	for(int i=0;i<role[sel_role].Propertiy.MagicsNum/5+1;i++)
		for(int j=0;j<role[sel_role].Propertiy.MagicsNum%5;j++)
			Blt(surf,Window_Magic.dialogx+36+j*30,Window_Magic.dialogy+39+i*30,
				CMagic::MAGIC[role[sel_role].Propertiy.Magics[i*4+j]].Pic,CMagic::MagicRect,true);
}

//ѭ��
void CState::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	//�л���ť
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
