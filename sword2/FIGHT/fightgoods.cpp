//********************************************
//	��Ʒ ��غ���
//  ������2000��7��07��
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
	Select=-1;		//��ǰѡ��-1��û��ѡ��
}

CFightGoods::~CFightGoods()
{
}

//��ȡ����
void CFightGoods::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);	//��ʼ���б���

	//��Ʒ���ܴ���
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

//��ʾ��Ʒ�б��
//return 1����Ҫʹ��һ����Ʒ��2��ȡ����0������
int CFightGoods::ShowGoodsListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static int MouseY=-1;
	ShowListWindow(surf, bShowWindow, left, top);	//��ʾ�б���

	//����ƶ�ѡ��
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;	//�õ���ǰѡ���Ԫ��
			if( Select >= role[0].Propertiy.CurGoodsNum ) 
				Select=role[0].Propertiy.CurGoodsNum-1;			//��������������һ��
		}
	}

	//��ʾ����
	int shownum=(ScrollBar.PageNum<role[0].Propertiy.CurGoodsNum)?
			ScrollBar.PageNum:role[0].Propertiy.CurGoodsNum;

	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//������ʾ
		if( n==Select )
		{
			//�������ֻ�������
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, 
							y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, 
							SelectColor);
		}
		//����Ʒ��ʾ��ͼ
		Blt(surf, x, y+LineHeight*i, CGoods::GOODS[role[0].Propertiy.Goods[n]].Pic, 
			GetRect(0, 0, CGoods::PicWidth, LineHeight), true);

		//��Ӱ��ʾ����
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, CGoods::GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x+1, y+4+LineHeight*i+1, "%5d", role[0].Propertiy.GoodsNum[n]);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, CGoods::GOODS[role[0].Propertiy.Goods[n]].Name);
		Font.PrintText(surf, x+Column[3].x, y+4+LineHeight*i, "%5d", role[0].Propertiy.GoodsNum[n]);
	}

	//ѡ��
	if( mouse == LB_UP )
	{
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			//������ʱ���ѡ��
			int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( sel == Select )	//ʹ��
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

	//ʹ��
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
	
	//��һ��
	
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

	//��һ��
	if( UP_DOWN && Select > 0 )
	{
		CMenu::Sound_Change.Play();
		Select--;
		if( Select < ScrollBar.CurNum )
			ScrollBar.SetCurNum(ScrollBar.CurNum-1);
	}
	return 0;
}

//��ʾ��Ʒ˵��
void CFightGoods::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	AboutWindow.ShowWindow(surf, bShowWindow);//��ʾabout����
	//��ʾ˵������
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, CGoods::GOODS[role[0].Propertiy.Goods[Select]].About);
}

//------------------------------------

//��ʼ������
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

//�˵�ѭ��
int CFightGoods::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	ShowAboutWindow(lpDDSBack, bShow);
	return ShowGoodsListWindow(lpDDSBack, bShow);
}

//ʹ����Ʒ�Ľ��
int CFightGoods::GoodEffect(int GoodNum, stRolePropertiy * Propertiy,int rolesel)
{
	if( CGoods::GOODS[GoodNum].Class==GC_GOODS )//���ͣ��ܹ�ʹ��
	{
		Propertiy->Exp+=CGoods::GOODS[GoodNum].Exp;			//����

		Propertiy->MaxHp+=CGoods::GOODS[GoodNum].MaxHp;		//�������ֵ
		Propertiy->Hp+=CGoods::GOODS[GoodNum].Hp;			//����
		if( Propertiy->Hp > Propertiy->MaxHp )
			Propertiy->Hp = Propertiy->MaxHp;

		Propertiy->MaxMp+=CGoods::GOODS[GoodNum].MaxMp;		//ħ�����ֵ
		Propertiy->Mp+=CGoods::GOODS[GoodNum].Mp;			//ħ��
		if( Propertiy->Mp > Propertiy->MaxMp )
			Propertiy->Mp = Propertiy->MaxMp;		

		role[rolesel].oldAttack+= CGoods::GOODS[GoodNum].Attack;		//����
		role[rolesel].oldAttack = (int)role[rolesel].oldAttack*(100.0+CGoods::GOODS[GoodNum].AttackPer)/100.0f;

		role[rolesel].oldDefend+= CGoods::GOODS[GoodNum].Defend;		//����
		role[rolesel].oldDefend = (int)role[rolesel].oldDefend*(100.0+CGoods::GOODS[GoodNum].DefendPer)/100.0f;

		role[rolesel].ReCalcPro();

		Propertiy->Magic+=CGoods::GOODS[GoodNum].Magic;		//ħ��

		Propertiy->Stamina+=CGoods::GOODS[GoodNum].Stamina;	//����
		Propertiy->Luck+=CGoods::GOODS[GoodNum].Luck;		//����
		Propertiy->Speed+=CGoods::GOODS[GoodNum].Speed;		//����
		Propertiy->Bp+=CGoods::GOODS[GoodNum].Bp;			//����
		Propertiy->Ip+=CGoods::GOODS[GoodNum].Ip;			//����
		role[0].DelObject(CGoods::GOODS[GoodNum].Name, 1);	//ɾ������Ʒһ��
	}
	return 1;
}
//����Ʒ�ܲ���ʹ��
bool CFightGoods::IsCanUse(int GoodNum)
{
	//�����Ŀ��С�ڵ���0�Ͳ�����
	if(role[0].CheckObject(CGoods::GOODS[GoodNum].Name) <= 0)
		return false;

	return true;
}
//�õ���Ʒ�ı��
int CFightGoods::GetGoodsID()
{
	return number;
}
//�õ�һҳ����Ʒ����
int CFightGoods::GetGoodsNumber(void)
{
	return (ScrollBar.PageNum<role[0].Propertiy.CurGoodsNum)?ScrollBar.PageNum:role[0].Propertiy.CurGoodsNum;
}