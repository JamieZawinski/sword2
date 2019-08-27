//********************************************
//	物品 相关函数
//  创建于2000年7月07日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "..\menu\menu_inc.h"
#include "FightGoods.h"

CFightGoods::CFightGoods()
{
	Select=-1;		//当前选择，-1：没有选择。
}

CFightGoods::~CFightGoods()
{
}

//读取窗口
void CFightGoods::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);	//初始化列表窗口

	//物品介绍窗口
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "AboutWindowFile");
	char* strIndex = Ini.ReadText(index, "AboutWindowIndex");
	AboutWindow.LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	AboutWindow.dialogx-=32;
	AboutWindow.dialogy-=32;
	AboutWindow.dialogw+=64;
	AboutWindow.dialogh+=64;
}

//显示物品列表框
//return 1：想要使用一个物品，2：取消，0：其他
int CFightGoods::ShowGoodsListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static int MouseY=-1;
	ShowListWindow(surf, bShowWindow, left, top);	//显示列表窗口

	//鼠标移动选择
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;	//得到当前选择的元素
			if( Select >= role[0].Propertiy.CurGoodsNum ) 
				Select=role[0].Propertiy.CurGoodsNum-1;			//超出，处理成最后一个
		}
	}

	//显示个数
	int shownum=(ScrollBar.PageNum<role[0].Propertiy.CurGoodsNum)?
			ScrollBar.PageNum:role[0].Propertiy.CurGoodsNum;

	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//高亮显示
		if( n==Select )
		{
			//留出部分画滚动条
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, 
							y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, 
							SelectColor);
		}
		//画物品的示意图
		Blt(surf, x, y+LineHeight*i, CGoods::GOODS[role[0].Propertiy.Goods[n]].Pic, 
			GetRect(0, 0, CGoods::PicWidth, LineHeight), true);

		//阴影显示文字
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, CGoods::GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x+1, y+4+LineHeight*i+1, "%5d", role[0].Propertiy.GoodsNum[n]);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, CGoods::GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x, y+4+LineHeight*i, "%5d", role[0].Propertiy.GoodsNum[n]);
	}

	//选择
	if( mouse == LB_UP )
	{
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			//点鼠标的时候的选择
			int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( sel == Select )	//使用
			{
				number = role[0].Propertiy.Goods[sel];
				Select =-1;
				return 1;
			}
		}
	}
	if(mouse == RB_UP||	B_UP)
	{
		Select =-1;
		return 2;
	}

	//使用
	if(	A_UP )
	{
		if( role[0].Propertiy.CurGoodsNum > 0 &&Select!=-1)
		{
			CMenu::Sound_Click.Play();
			number = role[0].Propertiy.Goods[Select];
			Select = -1;
			return 1;
		}
	}
	
	//下一个
	
	if( DOWN_DOWN && Select < ScrollBar.MaxNum-1 )
	{
		CMenu::Sound_Change.Play();
		Select++;
		if( Select >= ScrollBar.CurNum + ScrollBar.PageNum )
		{
			Select = ScrollBar.CurNum+1;
			ScrollBar.SetCurNum(Select);
		}

		if( Select >= ScrollBar.MinNum + role[0].Propertiy.CurGoodsNum )
		{
			Select = ScrollBar.MinNum;
			ScrollBar.SetCurNum(Select);
		}
	}

	//上一个
	if( UP_DOWN && Select > 0 )
	{
		CMenu::Sound_Change.Play();
		Select--;
		if( Select < ScrollBar.CurNum )
			ScrollBar.SetCurNum(ScrollBar.CurNum-1);
	}
	return 0;
}

//显示物品说明
void CFightGoods::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	AboutWindow.ShowWindow(surf, bShowWindow);//显示about窗口
	//显示说明文字
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, CGoods::GOODS[role[0].Propertiy.Goods[Select]].About);
}

//------------------------------------

//初始化数据
void CFightGoods::Init(LPDIRECTDRAWSURFACE surf)
{
	ScrollBar.MinNum=0;
	ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;

	GetGameSurface(lpDDSBackGround);
	FillSurface(lpDDSBack, RGB16(0));
	ABlt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, 0, 24);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
	AboutWindow.ShowWindow(surf);
}

//菜单循环
int CFightGoods::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	ShowAboutWindow(lpDDSBack, bShow);
	return ShowGoodsListWindow(lpDDSBack, bShow);
}

//使用物品的结果
int CFightGoods::GoodEffect(int GoodNum, stRolePropertiy * Propertiy,int rolesel)
{
	if( CGoods::GOODS[GoodNum].Class==GC_GOODS )//类型，能够使用
	{
		Propertiy->Exp+=CGoods::GOODS[GoodNum].Exp;			//经验

		Propertiy->MaxHp+=CGoods::GOODS[GoodNum].MaxHp;		//生命最大值
		Propertiy->Hp+=CGoods::GOODS[GoodNum].Hp;			//生命
		if( Propertiy->Hp > Propertiy->MaxHp )
			Propertiy->Hp = Propertiy->MaxHp;

		Propertiy->MaxMp+=CGoods::GOODS[GoodNum].MaxMp;		//魔法最大值
		Propertiy->Mp+=CGoods::GOODS[GoodNum].Mp;			//魔法
		if( Propertiy->Mp > Propertiy->MaxMp )
			Propertiy->Mp = Propertiy->MaxMp;		

		role[rolesel].oldAttack+= CGoods::GOODS[GoodNum].Attack;		//攻击
		role[rolesel].oldAttack = (int)role[rolesel].oldAttack*(100.0+CGoods::GOODS[GoodNum].AttackPer)/100.0f;

		role[rolesel].oldDefend+= CGoods::GOODS[GoodNum].Defend;		//防御
		role[rolesel].oldDefend = (int)role[rolesel].oldDefend*(100.0+CGoods::GOODS[GoodNum].DefendPer)/100.0f;

		role[rolesel].ReCalcPro();

		Propertiy->Magic+=CGoods::GOODS[GoodNum].Magic;		//魔法

		Propertiy->Stamina+=CGoods::GOODS[GoodNum].Stamina;	//耐力
		Propertiy->Luck+=CGoods::GOODS[GoodNum].Luck;		//幸运
		Propertiy->Speed+=CGoods::GOODS[GoodNum].Speed;		//敏捷
		Propertiy->Bp+=CGoods::GOODS[GoodNum].Bp;			//体力
		Propertiy->Ip+=CGoods::GOODS[GoodNum].Ip;			//精神
		role[0].DelObject(CGoods::GOODS[GoodNum].Name, 1);	//删除该物品一个
	}
	return 1;
}
//该物品能不能使用
bool CFightGoods::IsCanUse(int GoodNum)
{
	//检察数目，小于等于0就不能用
	if(role[0].CheckObject(CGoods::GOODS[GoodNum].Name) <= 0)
		return false;

	return true;
}
//得到物品的编号
int CFightGoods::GetGoodsID()
{
	return number;
}
//得到一页的物品种数
int CFightGoods::GetGoodsNumber(void)
{
	return (ScrollBar.PageNum<role[0].Propertiy.CurGoodsNum)?ScrollBar.PageNum:role[0].Propertiy.CurGoodsNum;
}