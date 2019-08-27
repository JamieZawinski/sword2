//********************************************
//	物品 相关函数
//  创建于2000年7月07日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

int CGoods::MAX_GOODS=0;
stGoods *CGoods::GOODS=NULL;
RECT CGoods::GoodsRect;

CGoods::CGoods()
{
	Select=0;
	PicWidth=PicHeight=32;
}

CGoods::~CGoods()
{
	if( GOODS != NULL && MAX_GOODS > 0 ) 
	{
		_FREE(GOODS);
		MAX_GOODS=0;
	}
}

//读取窗口
void CGoods::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);

	//物品介绍窗口
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "AboutWindowFile");
	char* strIndex = Ini.ReadText(index, "AboutWindowIndex");
	AboutWindow.LoadWindowIni(strFile, strIndex);
	free(strFile);
	free(strIndex);
	AboutWindow.dialogx-=32;
	AboutWindow.dialogy-=32;
	AboutWindow.dialogw+=64;
	AboutWindow.dialogh+=64;
}

//读取物体列表
void CGoods::LoadGoodsIni(char *filename)
{
	char *index;
	char *temp;

	CIniSet Ini(filename);
	MAX_GOODS=Ini.ReadInt( "物品", "MAX_GOODS" );
	PicWidth=Ini.ReadInt( "物品", "Width" );
	PicHeight=Ini.ReadInt( "物品", "Height" );
	GoodsRect=GetRect(0,0,PicWidth, PicHeight);
	GOODS=(stGoods *)realloc(GOODS, sizeof(stGoods)*MAX_GOODS);

	for(int i=0; i<MAX_GOODS; i++)
	{
		index=Ini.ReadText("物品列表", i);

		GOODS[i].Id=Ini.ReadInt(index, "Id");		//获得物品Id
		
		//获得物品名字
		temp=Ini.ReadText(index, "Name");
		strcpy( GOODS[i].Name, temp);
		free(temp);
		
		//获得物品介绍
		temp=Ini.ReadText(index, "About");
		strcpy( GOODS[i].About, temp);
		free(temp);

		GOODS[i].Class=Ini.ReadInt(index, "Class");

		GOODS[i].Money=Ini.ReadInt(index, "Money");			//价值
		GOODS[i].Exp=Ini.ReadInt(index, "Exp");				//经验
		GOODS[i].Hp=Ini.ReadInt(index, "Hp");				//生命
		GOODS[i].MaxHp=Ini.ReadInt(index, "MaxHp");			//生命最大值
		GOODS[i].Mp=Ini.ReadInt(index, "Mp");				//魔法
		GOODS[i].MaxMp=Ini.ReadInt(index, "MaxMp");			//魔法最大值

		GOODS[i].Attack=Ini.ReadInt(index, "Attack");		//攻击
		GOODS[i].Defend=Ini.ReadInt(index, "Defend");		//防御
		GOODS[i].MagicAct=Ini.ReadInt(index, "MagicAct");	//魔法攻击
		GOODS[i].MagicDef=Ini.ReadInt(index, "MagicDef");	//魔法防御

		GOODS[i].Stamina=Ini.ReadInt(index, "Stamina");		//耐力
		GOODS[i].Luck=Ini.ReadInt(index, "Luck");			//幸运
		GOODS[i].Speed=Ini.ReadInt(index, "Speed");			//敏捷
		GOODS[i].Bp=Ini.ReadInt(index, "Bp");				//体力
		GOODS[i].Ip=Ini.ReadInt(index, "Ip");				//精神

		//读入物品的图片
		temp=Ini.ReadText(index, "PicFileName");
		strcpy( GOODS[i].PicFileName, temp);
		CreateBitmap( GOODS[i].Pic, PicWidth, PicHeight, temp);
		DDSetColorKey( GOODS[i].Pic, ColorKey );
		free(temp);

		free(index);
	}
}

//显示物品列表框
void CGoods::ShowGoodsListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static MouseY=-1;
	ShowListWindow(surf, bShowWindow, left, top);

	//鼠标移动选择
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( Select >= role[0].Propertiy.CurGoodsNum ) 
				Select=role[0].Propertiy.CurGoodsNum-1;
		}
	}

	//显示
	int shownum=(ScrollBar.PageNum<role[0].Propertiy.CurGoodsNum)?ScrollBar.PageNum:role[0].Propertiy.CurGoodsNum;
	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;

		if( n==Select )
		{
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, SelectColor);
		}
		
		Blt(surf, x, y+LineHeight*i, GOODS[role[0].Propertiy.Goods[n]].Pic, GetRect(0, 0, PicWidth, LineHeight), true);
		
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x+1, y+4+LineHeight*i+1, "%5d", role[0].Propertiy.GoodsNum[n]);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x, y+4+LineHeight*i, "%5d", role[0].Propertiy.GoodsNum[n]);
	}

	//选择
	if( mouse == LB_UP )
	{
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( sel == Select )	//使用
			{
				UseGoods(sel);
				Menu.ReInitMenu();	//更新屏幕
			}
		}
	}

	//使用
	if( A_UP )
	{
		UseGoods(Select);
		Menu.ReInitMenu();	//更新屏幕
	}
	
	//下一个
	if( DOWN_DOWN && Select < ScrollBar.MaxNum-1 )
	{
		CMenu::Sound_Change.Play();
		Select++;
		if( Select >= ScrollBar.CurNum + ScrollBar.PageNum )
			ScrollBar.SetCurNum(ScrollBar.CurNum+1);
	}

	//上一个
	if( UP_DOWN && Select > 0 )
	{
		CMenu::Sound_Change.Play();
		Select--;
		if( Select < ScrollBar.CurNum )
			ScrollBar.SetCurNum(ScrollBar.CurNum-1);
	}

}

//显示物品说明
void CGoods::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	AboutWindow.ShowWindow(surf, bShowWindow);
	Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, GOODS[role[0].Propertiy.Goods[Select]].About);
}

//------------------------------------

//初始化数据
void CGoods::Init(LPDIRECTDRAWSURFACE surf)
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
void CGoods::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	ShowAboutWindow(surf, bShow);
	ShowGoodsListWindow(surf, bShow);
}

//使用一个物品
void CGoods::UseGoods(int sel)
{
	int goods=role[0].Propertiy.Goods[sel];
	if( role[0].Propertiy.CurGoodsNum > 0 )
	
	Blt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, false);

	int num=Menu.RoleSel.DoMenu(lpDDSBack);
	if( num != -1 )
	{
		//产生效果
		GoodEffect(goods, &role[num]);

		role[0].DelObject(GOODS[goods].Name, 1);
		ScrollBar.MinNum=0;
		ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
		
		//滚动条修正
		if( ScrollBar.MaxNum < ScrollBar.PageNum )
			ScrollBar.MaxNum = ScrollBar.PageNum;

		//当前位置修正
		if( ScrollBar.CurNum > ScrollBar.MaxNum - ScrollBar.PageNum )
		{
			ScrollBar.CurNum = ScrollBar.MaxNum - ScrollBar.PageNum;
		}
		
		//当前位置修正
		if( ScrollBar.MaxNum - ScrollBar.CurNum < ScrollBar.PageNum )
		{
			ScrollBar.CurNum = ScrollBar.MaxNum - ScrollBar.PageNum;
		}

		ScrollBar.CountNewRect();
	}
}

//使用物品的结果
int CGoods::GoodEffect(int GoodNum, CRole *Role)
{
	//if( GOODS[GoodNum].Class==GC_GOODS )//类型
	{
		Role->Propertiy.Exp+=GOODS[GoodNum].Exp;		//经验

		Role->Propertiy.MaxHp+=GOODS[GoodNum].MaxHp;	//生命最大值
		Role->Propertiy.Hp+=GOODS[GoodNum].Hp;			//生命
		if( Role->Propertiy.Hp > Role->Propertiy.MaxHp )
			Role->Propertiy.Hp = Role->Propertiy.MaxHp;

		Role->Propertiy.MaxMp+=GOODS[GoodNum].MaxMp;	//魔法最大值
		Role->Propertiy.Mp+=GOODS[GoodNum].Mp;			//魔法
		if( Role->Propertiy.Mp > Role->Propertiy.MaxMp )
			Role->Propertiy.Mp = Role->Propertiy.MaxMp;		

		Role->Propertiy.Attack+=GOODS[GoodNum].Attack;		//攻击
		Role->Propertiy.Defend+=GOODS[GoodNum].Defend;		//防御
		Role->Propertiy.MagicAct+=GOODS[GoodNum].MagicAct;	//魔法攻击
		Role->Propertiy.MagicDef+=GOODS[GoodNum].MagicDef;	//魔法防御

		Role->Propertiy.Stamina+=GOODS[GoodNum].Stamina;	//耐力
		Role->Propertiy.Luck+=GOODS[GoodNum].Luck;			//幸运
		Role->Propertiy.Speed+=GOODS[GoodNum].Speed;		//敏捷
		Role->Propertiy.Bp+=GOODS[GoodNum].Bp;				//体力
		Role->Propertiy.Ip+=GOODS[GoodNum].Ip;				//精神
	}
	
	return 1;	
}

//获取物体编号
int CGoods::GetGoodsNum(char *name)
{
	for(int i=0; i<MAX_GOODS; i++)
	{
		if( strcmp(name, GOODS[i].Name)==0 )
			return i;
	}
	return -1;
}
