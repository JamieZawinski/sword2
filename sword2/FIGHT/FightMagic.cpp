
//--------------------------------------------------------------------	
//	战斗魔法 相关函数
//  创建于2002年4月25日
//--------------------------------------------------------------------

#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "..\menu\menu_inc.h"
#include "FightMagic.h"


CFightMagic::CFightMagic()
{
	Select =-1;
}

CFightMagic::~CFightMagic()
{

}
// 初始化窗口
void CFightMagic::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);	//初始化列表窗口

	//魔法介绍窗口
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
// 显示魔法列表
int CFightMagic::ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel, int left, int top)
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
			if( Select >= role[rol_sel].Propertiy.MagicsNum ) 
				Select=role[rol_sel].Propertiy.MagicsNum-1;			//超出，处理成最后一个
		}
	}

	//显示个数
	int shownum=(ScrollBar.PageNum<role[rol_sel].Propertiy.MagicsNum)?
			ScrollBar.PageNum:role[rol_sel].Propertiy.MagicsNum;

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
		Blt(surf, x, y+LineHeight*i, CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Pic, 
			GetRect(0, 0, CMagic::PicWidth, LineHeight), true);

		//阴影显示文字
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, 
						CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Name);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, 
					CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Name);
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
				number = role[rol_sel].Propertiy.Magics[sel];
				Select =-1;
				return 1;
			}
		}
	}
	// 取消
	if(mouse == RB_UP||	B_UP)
	{
		Select =-1;
		return 2;
	}

	//使用
	if(	A_UP )
	{
		if( role[rol_sel].Propertiy.MagicsNum > 0 &&Select!=-1)
		{
			CMenu::Sound_Click.Play();
			number = role[rol_sel].Propertiy.Magics[Select];
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

		if( Select >= ScrollBar.MinNum + role[0].Propertiy.MagicsNum )
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
// about窗口
void CFightMagic::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel)
{
	AboutWindow.ShowWindow(surf, bShowWindow);//显示about窗口
	//显示说明文字
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, 
					CMagic::MAGIC[role[rol_sel].Propertiy.Magics[Select]].About);

}
// 初始化
void CFightMagic::Init(LPDIRECTDRAWSURFACE surf)
{
	ScrollBar.MinNum=0;
	ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;

	GetGameSurface(lpDDSBackGround);
	FillSurface(lpDDSBack, RGB16(0));
	ABlt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, 0, 24);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
	AboutWindow.ShowWindow(surf);
}

int CFightMagic::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow,int rol_sel)
{
	ShowAboutWindow(lpDDSBack, bShow,rol_sel);
	return ShowMagicListWindow(lpDDSBack, bShow,rol_sel);
}
// 能否用这个魔法
bool CFightMagic::IsCanUse(int n,int rol_sel)
{	
	//检查消耗魔法值和人物魔法值
	return CMagic::MAGIC[n].Mp
					<=role[rol_sel].Propertiy.Mp;
}
// 得到魔法的id
int CFightMagic::GetMagicID()
{
	return number;
}
// 魔法效果
int CFightMagic::MagicEffect(int Num, stRolePropertiy * Propertiy,stRolePropertiy * Propertiy2)
{
	//类型要是对自己使用的，然后魔法值要足够
	if( CMagic::MAGIC[Num].Class==MC_SELF&&CMagic::MAGIC[Num].Mp<=Propertiy->Mp)
	{
		Propertiy2->MaxHp+=CMagic::MAGIC[Num].MaxHp;	//生命最大值
		Propertiy2->Hp+=CMagic::MAGIC[Num].Hp;			//生命
		if( Propertiy2->Hp > Propertiy2->MaxHp )
			Propertiy2->Hp = Propertiy2->MaxHp;

		Propertiy2->Attack+=CMagic::MAGIC[Num].Attack;		//攻击
		Propertiy2->Defend+=CMagic::MAGIC[Num].Defend;		//防御
		Propertiy2->Magic+=CMagic::MAGIC[Num].Magic;		//魔力

		Propertiy2->Stamina+=CMagic::MAGIC[Num].Stamina;	//耐力
		Propertiy2->Luck+=CMagic::MAGIC[Num].Luck;			//幸运
		Propertiy2->Speed+=CMagic::MAGIC[Num].Speed;		//敏捷
		Propertiy2->Bp+=CMagic::MAGIC[Num].Bp;				//体力
		Propertiy2->Ip+=CMagic::MAGIC[Num].Ip;				//精神

		Propertiy->Mp-=CMagic::MAGIC[Num].Mp;				
		return 1;
	}
	return 0;
}

