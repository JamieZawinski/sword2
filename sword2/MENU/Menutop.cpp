//********************************************
//	顶级菜单 相关函数
//  创建于2000年7月09日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

CMenuTop::CMenuTop()
{
	Button = NULL;
}

CMenuTop::~CMenuTop()
{
	_DELETE_ARRAY( Button );
}

//读取
void CMenuTop::LoadMenuTopIni(char *filename, char *index)
{
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "WindowFile");
	char* strIndex = Ini.ReadText(index, "WindowIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	MoneyX=Ini.ReadInt(index, "MoneyX")+dialogx;
	MoneyY=Ini.ReadInt(index, "MoneyY")+dialogy;

	ButtonNum=Ini.ReadInt(index, "ButtonNum");
	Button=new CCButton[ButtonNum];

	//初始化按钮
	char* strTemp;
	for(int i=0; i<ButtonNum; i++)
	{
		strTemp = Ini.ReadText(index, i);
		char * str = Ini.ReadData(index,i);
		Button[i].LoadIni(strTemp, str);
		_FREE(strTemp);
		_FREE(str);
		Button[i].x+=dialogx;
		Button[i].y+=dialogy;
	}

	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;
}

//------------------------------------

//显示
void CMenuTop::ShowMenuTop(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	for(int i=0; i<ButtonNum; i++)
	{
		if( i==CMenu::SelectItem )
			Button[i].CheckLoop(surf, 2);
		else
			Button[i].CheckLoop(surf);
	}
	//阴影显示文字
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	Font.PrintText(surf, dialogx+MoneyX+1, dialogy+MoneyY+1, "钱:%6d", role[0].Propertiy.Money);
	Font.SetColor(c);
	Font.PrintText(surf, dialogx+MoneyX, dialogy+MoneyY, "钱:<c4>%6d</c>", role[0].Propertiy.Money);
}

//菜单循环
void CMenuTop::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	bool b[20];		//选择状态数组
	int bHigh;
	for(int i=0; i<ButtonNum; i++)
	{
		if( i==CMenu::SelectItem ) 
			bHigh=2;
		else 
			bHigh=-1;

		if( Button[i].CheckLoop(surf, bHigh)==1 )
		{
			CMenu::Sound_Click.Play();	//音效
			b[i]=true;
		}
		else
			b[i]=false;
	}
	//阴影显示
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	Font.PrintText(surf, dialogx+MoneyX+1, dialogy+MoneyY+1, "钱:%6d", role[0].Propertiy.Money);
	Font.SetColor(c);
	Font.PrintText(surf, dialogx+MoneyX, dialogy+MoneyY, "钱:<c4>%6d</c>", role[0].Propertiy.Money);

	for(i=0; i<ButtonNum; i++)
		if( b[i] || key==(unsigned int)DIK_1+i )		//键盘数字键控制
		{
			CMenu::Sound_Click.Play();	//音效
			CMenu::SelectItem=i;
			CMenu::ReInitMenu();
			return;
		}

	if( Key.KeyDown( DIK_LCONTROL ) && LEFT_DOWN )		//方向键控制
	{
		CMenu::Sound_Click.Play();	//音效
		CMenu::SelectItem--;
		if( CMenu::SelectItem<0 ) 
			CMenu::SelectItem=ButtonNum-1;	//循环

		CMenu::ReInitMenu();
	}
	
	if( Key.KeyDown( DIK_LCONTROL ) && RIGHT_DOWN )
	{
		CMenu::Sound_Click.Play();	//音效
		CMenu::SelectItem++;
		if( CMenu::SelectItem>ButtonNum-1 ) 
			CMenu::SelectItem=0;

		CMenu::ReInitMenu();
	}
	
	if( key==DIK_TAB || joystick== DIJ_4  )
	{
		CMenu::Sound_Click.Play();	//音效
		CMenu::SelectItem++;
		if( CMenu::SelectItem>ButtonNum-1 ) 
			CMenu::SelectItem=0;
		CMenu::ReInitMenu();
	}

}
