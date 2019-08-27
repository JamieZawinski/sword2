
//--------------------------------------------------------------------
//	魔法 相关函数
//  创建于2002年4月25日
//--------------------------------------------------------------------

#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\mapedit\mapedit.h"
#include "..\map.h"
#include "..\role.h"
#include "..\script.h"
#include "menu_inc.h"

int CMagic::MAX_MAGIC=0;
stMagic *CMagic::MAGIC=NULL;
RECT CMagic::MagicRect;
int CMagic::PicWidth=24;
int CMagic::PicHeight=24;	//魔法图片的大小

CMagic::CMagic()
{
	Select = -1;
}

CMagic::~CMagic()
{
	if( MAGIC != NULL && MAX_MAGIC > 0 ) 
	{
		_FREE(MAGIC);
		MAX_MAGIC=0;
	}
}

//初始化数据
void CMagic::Init(LPDIRECTDRAWSURFACE surf)						
{	
	ScrollBar.MinNum=0;
	ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;

	GetGameSurface(surf);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
	AboutWindow.ShowWindow(surf);
}

//初始化窗口数据
void CMagic::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);

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

void CMagic::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	ShowAboutWindow(surf, bShow);
	ShowMagicListWindow(surf, bShow);
}

void CMagic::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	AboutWindow.ShowWindow(surf, bShowWindow);
	//显示说明文字
	int sel_role=CMenu::RoleSel.SelectRole;			//选择的主角
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, 
					MAGIC[role[sel_role].Propertiy.Magics[Select]].About);

}

void CMagic::LoadMagicIni(char *filename)
{
	char *index;
	char *temp;

	CIniSet Ini(filename);
	PicWidth=Ini.ReadInt( "魔法", "Width" );
	PicHeight=Ini.ReadInt( "魔法", "Height" );
	MagicRect=GetRect(0,0,PicWidth, PicHeight);

	MAX_MAGIC = Ini.GetContinueDataNum("魔法列表");
	MAGIC=(stMagic *)malloc(sizeof(stMagic)*MAX_MAGIC);

	for(int i=0; i<MAX_MAGIC; i++)
	{
		index=Ini.ReadText("魔法列表", i);

		MAGIC[i].Id=Ini.ReadInt(index, "Id");		//获得物品Id
		
		//获得物品名字
		temp=Ini.ReadText(index, "Name");
		strcpy( MAGIC[i].Name, temp);
		_FREE(temp);
		
		//获得物品介绍
		temp=Ini.ReadText(index, "About");
		strcpy(MAGIC[i].About, temp);
		_FREE(temp);

		MAGIC[i].Class=Ini.ReadInt(index, "Class");
		MAGIC[i].Hp=Ini.ReadInt(index, "Hp");				//生命
		MAGIC[i].MaxHp=Ini.ReadInt(index, "MaxHp");			//生命最大值
		MAGIC[i].Mp=Ini.ReadInt(index, "Mp");				//魔法

		MAGIC[i].Attack=Ini.ReadInt(index, "Attack");		//攻击
		MAGIC[i].Defend=Ini.ReadInt(index, "Defend");		//防御

		MAGIC[i].Magic=Ini.ReadInt(index, "Magic");			//魔力
		MAGIC[i].Stamina=Ini.ReadInt(index, "Stamina");		//耐力
		MAGIC[i].Luck=Ini.ReadInt(index, "Luck");			//幸运
		MAGIC[i].Speed=Ini.ReadInt(index, "Speed");			//敏捷
		MAGIC[i].Bp=Ini.ReadInt(index, "Bp");				//体力
		MAGIC[i].Ip=Ini.ReadInt(index, "Ip");				//精神

		//读入魔法的图片
		temp=Ini.ReadText(index, "PicFileName");
		strcpy( MAGIC[i].PicFileName, temp);
		CreateBitmap( MAGIC[i].Pic, PicWidth, PicHeight, temp);
		DDSetColorKey( MAGIC[i].Pic, ColorKey );
		_FREE(temp);
	
		//读入魔法的效果图片
		temp=Ini.ReadText(index, "EffectPicFileName");
		strcpy( MAGIC[i].EffectPicFileName, temp);
		CreateBitmap( MAGIC[i].EffectPic, 0, 0, temp);
		DDSetColorKey( MAGIC[i].EffectPic, ColorKey );
		_FREE(temp);
		
		//读入战斗时释放魔法的移动数据
		temp=Ini.ReadText(index, "MagicFileName");
		strcpy( MAGIC[i].MagicFileName, temp);
		_FREE(temp);
		//声音数据
		temp=Ini.ReadText(index,"SoundFileName");
		strcpy(MAGIC[i].SoundFileName,temp);
		_FREE(temp);

		_FREE(index);
	}

}
// 显示魔法窗口
void CMagic::ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static int MouseY = -1;
	ShowListWindow(surf, bShowWindow, left, top);

	int sel_role=CMenu::RoleSel.SelectRole;			//选择的主角

	//鼠标移动选择
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;		//当前选择
			if( Select >= role[sel_role].Propertiy.MagicsNum ) 
				Select=role[sel_role].Propertiy.MagicsNum-1;				//超出处理成最后一个
		}
	}

	//显示数目：比一页多就显示一页，否则显示全部
	
	int shownum=(ScrollBar.PageNum<role[sel_role].Propertiy.MagicsNum)?
			ScrollBar.PageNum:role[sel_role].Propertiy.MagicsNum;

	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//高亮显示
		if( n==Select )
		{
			//留下部分显示滚动条
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, 
											y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, 
											SelectColor);
		}
		//显示示意图片
		Blt(surf, x, y+LineHeight*i, MAGIC[role[sel_role].Propertiy.Magics[n]].Pic, 
				GetRect(0, 0, PicWidth, LineHeight), true);

		//阴影显示文字
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, 
						MAGIC[role[sel_role].Propertiy.Magics[n]].Name);

		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, 
						MAGIC[role[sel_role].Propertiy.Magics[n]].Name);

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
				CMenu::Sound_Click.Play();
				UseMagic(sel,sel_role);
				Menu.ReInitMenu();	//更新屏幕
				if(Select!=(point.y-y)/LineHeight + ScrollBar.CurNum)
					Select =-1;
				return;
			}
		}
	}

	//使用
	if( A_UP &&Select>-1)
	{
		if( role[sel_role].Propertiy.MagicsNum > 0 )
		{
			CMenu::Sound_Click.Play();
			UseMagic(Select,sel_role);
			Menu.ReInitMenu();	//更新屏幕
			if(Select!=(point.y-y)/LineHeight + ScrollBar.CurNum)
				Select =-1;
			return;
		}
	}
	
	//下一个
	if( DOWN_DOWN && Select < ScrollBar.MaxNum-1 )
	{
		Select++;
		CMenu::Sound_Change.Play();
		if( Select >= ScrollBar.CurNum + ScrollBar.PageNum )
		{
			Select = ScrollBar.CurNum+1;
			ScrollBar.SetCurNum(Select);
		}

		if( Select >= ScrollBar.MinNum + role[sel_role].Propertiy.MagicsNum )
		{
			Select = ScrollBar.MinNum;
			ScrollBar.SetCurNum(Select);
		}
	}

	//上一个
	if( UP_DOWN && Select > 0 )
	{
		Select--;
		CMenu::Sound_Change.Play();
		if( Select < ScrollBar.MinNum )
			Select = ScrollBar.MinNum;

		if( Select < ScrollBar.CurNum )
		{
			ScrollBar.SetCurNum(ScrollBar.CurNum-1);
		}
	}
}
// 使用魔法
void CMagic::UseMagic(int sel,int sel_role)
{
	int use_role=Menu.RoleSel.DoMenu(lpDDSBack);	//选择的主角
	int magic=role[sel_role].Propertiy.Magics[sel];	//选择的魔法
	if(!(use_role!=-1))
		return;
	//产生效果
	if(!MagicEffect(magic, &role[sel_role],&role[use_role]))
	{
		char x[256];
		sprintf(x, "该魔法不能使用");
		GameMessage.DoMenu(lpDDSBack, x);
		return;
	}
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
// 魔法效果
int CMagic::MagicEffect(int Num, CRole *Role, CRole *RoleReceive)
{
	//类型要是对自己使用的，然后魔法值要足够
	if( CMagic::MAGIC[Num].Class==MC_SELF&&CMagic::MAGIC[Num].Mp<=Role->Propertiy.Mp)
	{
		RoleReceive->Propertiy.MaxHp+=CMagic::MAGIC[Num].MaxHp;	//生命最大值
		RoleReceive->Propertiy.Hp+=CMagic::MAGIC[Num].Hp;			//生命
		if( RoleReceive->Propertiy.Hp > RoleReceive->Propertiy.MaxHp )
			RoleReceive->Propertiy.Hp = RoleReceive->Propertiy.MaxHp;

		RoleReceive->Propertiy.Attack+=CMagic::MAGIC[Num].Attack;		//攻击
		RoleReceive->Propertiy.Defend+=CMagic::MAGIC[Num].Defend;		//防御
		RoleReceive->Propertiy.Magic+=CMagic::MAGIC[Num].Magic;		//魔力

		RoleReceive->Propertiy.Stamina+=CMagic::MAGIC[Num].Stamina;	//耐力
		RoleReceive->Propertiy.Luck+=CMagic::MAGIC[Num].Luck;			//幸运
		RoleReceive->Propertiy.Speed+=CMagic::MAGIC[Num].Speed;		//敏捷
		RoleReceive->Propertiy.Bp+=CMagic::MAGIC[Num].Bp;				//体力
		RoleReceive->Propertiy.Ip+=CMagic::MAGIC[Num].Ip;				//精神

		Role->Propertiy.Mp-=CMagic::MAGIC[Num].Mp;
		return 1;
	}
	
	return 0;	
}
// 得到魔法id
int CMagic::GetMagicNum(char *name)
{
	for(int i=0; i<MAX_MAGIC; i++)
	{
		if( strcmp(name, MAGIC[i].Name)==0 )
			return i;
	}
	return -1;
}


