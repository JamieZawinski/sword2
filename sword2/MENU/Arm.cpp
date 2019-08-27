//********************************************
//	װ�� ��غ���
//  ������2000��9��27��
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

CArm::CArm()
{
	ArmList.Select =-1;
}

CArm::~CArm()
{
}

//��ȡ����
void CArm::LoadArmIni(char *filename, char *index)
{
	//����
	CIniSet Ini(filename);
	char* strFile= Ini.ReadText(index, "ArmFile");
	char* strIndex = Ini.ReadText(index, "ArmIndex");
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

	//װ���б��
	strFile = Ini.ReadText(index, "ListWindowFile");
	strIndex = Ini.ReadText(index, "ListWindowIndex");
	ArmList.LoadListWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
}

//��ʼ������
void CArm::Init(LPDIRECTDRAWSURFACE surf)
{
	ArmList.ScrollBar.MinNum=0;
	ArmList.ScrollBar.MaxNum=role[0].Propertiy.CurArmNum;

	GetGameSurface(surf);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
}

//��ʾ
void CArm::ShowArmWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int sel_role=CMenu::RoleSel.SelectRole;

	//ͷ��
	Blt(surf, dialogx+60, dialogy+42, CMenu::RoleSel.PhotoPic[role[sel_role].Id], PhotoRect, true);

	//����
	Bar(surf, dialogx+32+5, dialogy+102, dialogx+32+dialogw-74, dialogy+103, RGB16(240,211,123));
	Bar(surf, dialogx+32+5, dialogy+103, dialogx+32+dialogw-74, dialogy+104, RGB16(156,119,5));


	DWORD c=Font.GetColor();

	Font.SetColor(c);
	Font.PrintText(surf, dialogx+200, dialogy+42,  "<c4>%6s</c>  Lv:<c4>%d</c>", role[sel_role].Name, role[sel_role].Propertiy.Level);
	Font.PrintText(surf, dialogx+200, dialogy+61, "HP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Hp, role[sel_role].Propertiy.MaxHp );
	Font.PrintText(surf, dialogx+200, dialogy+79, "MP: <c4>%6d</c>/<c4>%d</c>", role[sel_role].Propertiy.Mp, role[sel_role].Propertiy.MaxMp );
	Font.PrintText(surf, dialogx+50, dialogy+120, "����:<c4>%5d", role[sel_role].Propertiy.Attack );
	Font.PrintText(surf, dialogx+50, dialogy+145, "����:<c4>%5d", role[sel_role].Propertiy.Defend );
	Font.PrintText(surf, dialogx+50, dialogy+170, "ħ��:<c4>%5d", role[sel_role].Propertiy.Magic );
	Font.PrintText(surf, dialogx+50, dialogy+220, "����:<c4>%5d", role[sel_role].Propertiy.Stamina );
	Font.PrintText(surf, dialogx+50, dialogy+245, "����:<c4>%5d", role[sel_role].Propertiy.Luck );
	Font.PrintText(surf, dialogx+50, dialogy+270, "����:<c4>%5d", role[sel_role].Propertiy.Speed );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+39, "����:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Arms].Name );
	Font.PrintText(surf, Window_Setup.dialogx+36, Window_Setup.dialogy+69, "����:<c4>%s</c>", CGoods::GOODS[role[sel_role].Propertiy.Corselet].Name );
	
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+34, CGoods::GOODS[role[sel_role].Propertiy.Arms].Pic, CGoods::GoodsRect, true);
	Blt(surf, Window_Setup.dialogx+145, Window_Setup.dialogy+64, CGoods::GOODS[role[sel_role].Propertiy.Corselet].Pic, CGoods::GoodsRect, true);
}

//ѭ��
void CArm::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	//�л���ť
	if( Button_Left.CheckLoop(surf)==1 || LEFT_DOWN )
	{
		CMenu::Sound_Click.Play();
		CMenu::RoleSel.SelectRole--;
		if( CMenu::RoleSel.SelectRole<0 )
			CMenu::RoleSel.SelectRole=RoleNum-1;
		CMenu::ReInitMenu();
	}

	if( Button_Right.CheckLoop(surf)==1 || RIGHT_DOWN )
	{
		CMenu::Sound_Click.Play();
		CMenu::RoleSel.SelectRole++;
		if( CMenu::RoleSel.SelectRole>=RoleNum )
			CMenu::RoleSel.SelectRole=0;
		CMenu::ReInitMenu();
	}

	//��ʾ�б��
	ShowArmListWindow(surf, bShow, -1, -1);

	//Ԥ��װ��Ч��
	if( point.x > ArmList.dialogx+32 && point.x < ArmList.dialogx+ArmList.dialogw-32 && point.y>ArmList.dialogy+32 && point.y<ArmList.dialogy+ 32 + ArmList.LineHeight*role[0].Propertiy.CurArmNum )
	if( role[0].Propertiy.CurArmNum>0 && ArmList.Select<role[0].Propertiy.CurArmNum )
	{
		int sel_arm=role[0].Propertiy.Arm[ArmList.Select];	//ѡ����Ʒ
		int sel_role=CMenu::RoleSel.SelectRole;				//ѡ������
		int chg_arm;		//Ҫ������װ��
		if( CMenu::Goods.GOODS[sel_arm].Class==GC_ARMS )
		{
			chg_arm=role[sel_role].Propertiy.Arms;	//������
		}
		else if( CMenu::Goods.GOODS[sel_arm].Class==GC_CORSELET )
		{
			chg_arm=role[sel_role].Propertiy.Corselet;	//�ɻ���
		}

		ShowChangeNum(surf, sel_arm, chg_arm);
	}
}

//װ��������ֵԤ��
void CArm::ShowChangeNum(LPDIRECTDRAWSURFACE surf, int sel_arm, int chg_arm)
{
	DWORD c=Font.GetColor();
	Font.SetColor(RGB(255,0,0));
	dialogx+=92;
	
	int sel_role=CMenu::RoleSel.SelectRole;

	int num = (int)(role[sel_role].oldAttack+CMenu::Goods.GOODS[sel_arm].Attack)*(100.0+
							CMenu::Goods.GOODS[sel_arm].AttackPer)/100.0f-
							(role[sel_role].oldAttack+CMenu::Goods.GOODS[chg_arm].Attack)*(100.0+
							CMenu::Goods.GOODS[chg_arm].AttackPer)/100.0f;
	if( num>0 )	Font.PrintText(surf, dialogx+50, dialogy+120, "<c2>(+%d)</c>", num );
	else if( num<0 )Font.PrintText(surf, dialogx+50, dialogy+120, "(%d)", num );

	num = (int)(role[sel_role].oldDefend+CMenu::Goods.GOODS[sel_arm].Defend)*(100.0+
							CMenu::Goods.GOODS[sel_arm].DefendPer)/100.0f-
							(role[sel_role].oldDefend+CMenu::Goods.GOODS[chg_arm].Defend)*(100.0+
							CMenu::Goods.GOODS[chg_arm].DefendPer)/100.0f;

	if( num>0 )	Font.PrintText(surf, dialogx+50, dialogy+145, "<c2>(+%d)</c>", num );
	else if( num<0 ) Font.PrintText(surf, dialogx+50, dialogy+145, "(%d)", num );
	
	num=CMenu::Goods.GOODS[sel_arm].Magic - CMenu::Goods.GOODS[chg_arm].Magic;
	if( num>0 )	Font.PrintText(surf, dialogx+50, dialogy+170, "<c2>(+%d)</c>", num );
	else if( num<0 ) Font.PrintText(surf, dialogx+50, dialogy+170, "(%d)", num );

	num=CMenu::Goods.GOODS[sel_arm].Stamina - CMenu::Goods.GOODS[chg_arm].Stamina;
	if( num>0 ) Font.PrintText(surf, dialogx+50, dialogy+220, "<c2>(+%d)</c>", num );
	else if( num<0 ) Font.PrintText(surf, dialogx+50, dialogy+220, "(%d)", num );

	num=CMenu::Goods.GOODS[sel_arm].Luck - CMenu::Goods.GOODS[chg_arm].Luck;
	if( num>0 ) Font.PrintText(surf, dialogx+50, dialogy+245, "<c2>(+%d)</c>", num);
	else if( num<0 ) Font.PrintText(surf, dialogx+50, dialogy+245, "(%d)", num);

	num=CMenu::Goods.GOODS[sel_arm].Speed - CMenu::Goods.GOODS[chg_arm].Speed;
	if( num>0 ) Font.PrintText(surf, dialogx+50, dialogy+270, "<c2>(+%d)</c>", num);
	else if( num<0 ) Font.PrintText(surf, dialogx+50, dialogy+270, "(%d)", num);

	dialogx-=92;
	Font.SetColor(c);
}


//��ʾװ���б��
void CArm::ShowArmListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static MouseY=-1;
	ArmList.ShowListWindow(surf, bShowWindow, left, top);

	//����ƶ�ѡ��
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > ArmList.x && point.x < ArmList.x+ArmList.Width-ArmList.ScrollBar.Width &&
			point.y > ArmList.y && point.y < ArmList.y+ArmList.LineHeight*role[0].Propertiy.CurArmNum )
		{
			ArmList.Select=(point.y-ArmList.y)/ArmList.LineHeight + ArmList.ScrollBar.CurNum;
			if( ArmList.Select >= role[0].Propertiy.CurArmNum ) 
				ArmList.Select=role[0].Propertiy.CurArmNum-1;
		}
	}

	//��ʾ
	int shownum=(ArmList.ScrollBar.PageNum<role[0].Propertiy.CurArmNum)?ArmList.ScrollBar.PageNum:role[0].Propertiy.CurArmNum;
	for(int i=0; i<shownum; i++)
	{		
		n=ArmList.ScrollBar.CurNum+i;

		if( n==ArmList.Select )
		{
			if( ArmList.ScrollBar.PageNum < ArmList.ScrollBar.MaxNum-ArmList.ScrollBar.MinNum )
				Bar(surf, ArmList.x, ArmList.y+ArmList.LineHeight*i+2, ArmList.x+ArmList.Width-ArmList.ScrollBar.Width, ArmList.y+ArmList.LineHeight*(i+1)-2, ArmList.SelectColor);
			else
				Bar(surf, ArmList.x, ArmList.y+ArmList.LineHeight*i+2, ArmList.x+ArmList.Width-1, ArmList.y+ArmList.LineHeight*(i+1)-2, ArmList.SelectColor);
		}
		
		Blt(surf, ArmList.x, ArmList.y+ArmList.LineHeight*i, CMenu::Goods.GOODS[role[0].Propertiy.Arm[n]].Pic, GetRect(0, 0, CMenu::Goods.PicWidth, ArmList.LineHeight), true);
		
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, ArmList.x+ArmList.Column[1].x+1, ArmList.y+4+ArmList.LineHeight*i+1, CMenu::Goods.GOODS[role[0].Propertiy.Arm[n]].Name);
		Font.PrintText(surf, ArmList.x+ArmList.Column[3].x+1, ArmList.y+4+ArmList.LineHeight*i+1, "%2d", role[0].Propertiy.ArmNum[n]);
		Font.SetColor(c);
		Font.PrintText(surf, ArmList.x+ArmList.Column[1].x, ArmList.y+4+ArmList.LineHeight*i, CMenu::Goods.GOODS[role[0].Propertiy.Arm[n]].Name);
		Font.PrintText(surf, ArmList.x+ArmList.Column[3].x, ArmList.y+4+ArmList.LineHeight*i, "%2d", role[0].Propertiy.ArmNum[n]);
	}

	//����װ��
	if( mouse == LB_UP && point.x > ArmList.x && point.x < ArmList.x+ArmList.Width-ArmList.ScrollBar.Width && point.y > ArmList.y && point.y < ArmList.y+ArmList.LineHeight*role[0].Propertiy.CurArmNum )
	{
		int sel =(point.y-ArmList.y)/ArmList.LineHeight + ArmList.ScrollBar.CurNum;	//ʹ��װ��
		//if(sel!=-1)
		{
			CMenu::Sound_Click.Play();	//��Ч
			int SelR=CMenu::RoleSel.SelectRole;
			int arm=role[0].Propertiy.Arm[sel];
			if( CMenu::Goods.GOODS[arm].Class == 0 )	//����
			{
				int rolearm=role[SelR].Propertiy.Arms;	//������
				if( rolearm != 0 )	
					role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[0].DelArm(CMenu::Goods.GOODS[arm].Name);
				role[SelR].Propertiy.Arms=arm;
				//������ֵ
				role[SelR].UnSetupArm(rolearm);
				role[SelR].SetupArm(arm);
			}
			else if( CMenu::Goods.GOODS[arm].Class == 1 )	//����
			{
				int rolearm=role[SelR].Propertiy.Corselet;	//�ɻ���
				if( rolearm != 0 )	
					role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[0].DelArm(CMenu::Goods.GOODS[arm].Name);
				role[SelR].Propertiy.Corselet=arm;					
				
				//������ֵ
				role[SelR].UnSetupArm(rolearm);
				role[SelR].SetupArm(arm);
			}
			Menu.ReInitMenu();	//������Ļ
			if(role[SelR].Propertiy.CurArmNum>0)
			{
				if(ArmList.Select>=role[SelR].Propertiy.CurArmNum)
					ArmList.Select = role[SelR].Propertiy.CurArmNum-1;
			}
			else
				ArmList.Select = -1;
		}
	}
	if(A_UP)
	{
		int sel = ArmList.Select;	//ʹ��װ��
		if(sel!=-1)
		{
			CMenu::Sound_Click.Play();	//��Ч
			int SelR=CMenu::RoleSel.SelectRole;
			int arm=role[0].Propertiy.Arm[sel];
			if( CMenu::Goods.GOODS[arm].Class == 0 )	//����
			{
				int rolearm=role[SelR].Propertiy.Arms;	//������
				if( rolearm != 0 )	
					role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[0].DelArm(CMenu::Goods.GOODS[arm].Name);
				role[SelR].Propertiy.Arms=arm;
				//������ֵ
				role[SelR].UnSetupArm(rolearm);
				role[SelR].SetupArm(arm);
			}
			else if( CMenu::Goods.GOODS[arm].Class == 1 )	//����
			{
				int rolearm=role[SelR].Propertiy.Corselet;	//�ɻ���
				if( rolearm != 0 )	
					role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[0].DelArm(CMenu::Goods.GOODS[arm].Name);
				role[SelR].Propertiy.Corselet=arm;					
				
				//������ֵ
				role[SelR].UnSetupArm(rolearm);
				role[SelR].SetupArm(arm);
			}
			Menu.ReInitMenu();	//������Ļ
			if(role[SelR].Propertiy.CurArmNum>0)
			{
				if(ArmList.Select>=role[SelR].Propertiy.CurArmNum)
					ArmList.Select = role[SelR].Propertiy.CurArmNum-1;
			}
			else
				ArmList.Select = -1;
		}
	}

	//ж��װ��
	if( point.x > Window_Setup.dialogx+32 && point.x < Window_Setup.dialogx+Window_Setup.dialogw-32 )
	{
		if( point.y > Window_Setup.dialogy+32 && point.y < Window_Setup.dialogy+64 )
		{
			int SelR=CMenu::RoleSel.SelectRole;
			ShowChangeNum(surf, 0, role[SelR].Propertiy.Arms);
			Rectangle(surf, Window_Setup.dialogx+32, Window_Setup.dialogy+32, Window_Setup.dialogx+Window_Setup.dialogw-34, Window_Setup.dialogy+62, RGB16(0,255,255));
			if( mouse == LB_UP )
			{
				CMenu::Sound_Click.Play();	//��Ч
				int rolearm=role[SelR].Propertiy.Arms;	//������
				if( rolearm != 0 )	
					role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[SelR].Propertiy.Arms=0;
				role[SelR].UnSetupArm(rolearm);
				Menu.ReInitMenu();	//������Ļ
			}
		}
		else if( point.y > Window_Setup.dialogy+64 && point.y < Window_Setup.dialogy+Window_Setup.dialogh-32 )
		{
			int SelR=CMenu::RoleSel.SelectRole;
			ShowChangeNum(surf, 0, role[SelR].Propertiy.Corselet);
			Rectangle(surf, Window_Setup.dialogx+32, Window_Setup.dialogy+63, Window_Setup.dialogx+Window_Setup.dialogw-34, Window_Setup.dialogy+Window_Setup.dialogh-34, RGB16(0,255,255));
			if( mouse == LB_UP )
			{
				CMenu::Sound_Click.Play();	//��Ч
				int rolearm=role[SelR].Propertiy.Corselet;	//�ɻ���
				if( rolearm != 0 )	role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
				role[SelR].Propertiy.Corselet=0;
				role[SelR].UnSetupArm(rolearm);
				Menu.ReInitMenu();	//������Ļ
			}
		}
	}

	//ж��װ��
	if( Key.KeyDown( DIK_A ) )
	{
		int SelR=CMenu::RoleSel.SelectRole;
		ShowChangeNum(surf, 0, role[SelR].Propertiy.Arms);
		Rectangle(surf, Window_Setup.dialogx+32, Window_Setup.dialogy+32, Window_Setup.dialogx+Window_Setup.dialogw-34, Window_Setup.dialogy+62, RGB16(0,255,255));
	}

	if( key == DIK_A + 128 || joystick == DIJ_L2 )
	{
		int SelR=CMenu::RoleSel.SelectRole;
		CMenu::Sound_Click.Play();	//��Ч
		int rolearm=role[SelR].Propertiy.Arms;	//������
		if( rolearm != 0 )	role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
		role[SelR].Propertiy.Arms=0;
		role[SelR].UnSetupArm(rolearm);
		Menu.ReInitMenu();	//������Ļ
		if(point.x > ArmList.x && point.x < ArmList.x+ArmList.Width-ArmList.ScrollBar.Width && point.y > ArmList.y && point.y < ArmList.y+ArmList.LineHeight*role[0].Propertiy.CurArmNum)
			ArmList.Select = (point.y-ArmList.y)/ArmList.LineHeight + ArmList.ScrollBar.CurNum;	
		else
			ArmList.Select = 0;
	}

	if( Key.KeyDown( DIK_Z ) )
	{
		int SelR=CMenu::RoleSel.SelectRole;
		ShowChangeNum(surf, 0, role[SelR].Propertiy.Corselet);
		Rectangle(surf, Window_Setup.dialogx+32, Window_Setup.dialogy+63, Window_Setup.dialogx+Window_Setup.dialogw-34, Window_Setup.dialogy+Window_Setup.dialogh-34, RGB16(0,255,255));
	}

	if( key == DIK_Z + 128 || joystick == DIJ_L1 )
	{
		int SelR=CMenu::RoleSel.SelectRole;
		CMenu::Sound_Click.Play();	//��Ч
		int rolearm=role[SelR].Propertiy.Corselet;	//�ɻ���
		if( rolearm != 0 )	role[0].AddArm(CMenu::Goods.GOODS[rolearm].Name);
		role[SelR].Propertiy.Corselet=0;
		role[SelR].UnSetupArm(rolearm);
		Menu.ReInitMenu();	//������Ļ
		if(point.x > ArmList.x && point.x < ArmList.x+ArmList.Width-ArmList.ScrollBar.Width && point.y > ArmList.y && point.y < ArmList.y+ArmList.LineHeight*role[0].Propertiy.CurArmNum)
			ArmList.Select = (point.y-ArmList.y)/ArmList.LineHeight + ArmList.ScrollBar.CurNum;	
		else
			ArmList.Select = 0;

	}
	
	//��һ��
	if( DOWN_DOWN && ArmList.Select < ArmList.ScrollBar.MaxNum-1 )
	{
		CMenu::Sound_Change.Play();
		ArmList.Select++;
		if( ArmList.Select >= ArmList.ScrollBar.CurNum + ArmList.ScrollBar.PageNum )
			ArmList.ScrollBar.SetCurNum(ArmList.ScrollBar.CurNum+1);
	}

	//��һ��
	if( UP_DOWN && ArmList.Select > 0 )
	{
		CMenu::Sound_Change.Play();
		ArmList.Select--;
		if( ArmList.Select < ArmList.ScrollBar.CurNum )
			ArmList.ScrollBar.SetCurNum(ArmList.ScrollBar.CurNum-1);
	}

}