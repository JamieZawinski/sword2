
//--------------------------------------------------------------------	
//	ս��ħ�� ��غ���
//  ������2002��4��25��
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
// ��ʼ������
void CFightMagic::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);	//��ʼ���б���

	//ħ�����ܴ���
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
// ��ʾħ���б�
int CFightMagic::ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel, int left, int top)
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
			if( Select >= role[rol_sel].Propertiy.MagicsNum ) 
				Select=role[rol_sel].Propertiy.MagicsNum-1;			//��������������һ��
		}
	}

	//��ʾ����
	int shownum=(ScrollBar.PageNum<role[rol_sel].Propertiy.MagicsNum)?
			ScrollBar.PageNum:role[rol_sel].Propertiy.MagicsNum;

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
		Blt(surf, x, y+LineHeight*i, CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Pic, 
			GetRect(0, 0, CMagic::PicWidth, LineHeight), true);

		//��Ӱ��ʾ����
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, 
						CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Name);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, 
					CMagic::MAGIC[role[rol_sel].Propertiy.Magics[n]].Name);
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
				number = role[rol_sel].Propertiy.Magics[sel];
				Select =-1;
				return 1;
			}
		}
	}
	// ȡ��
	if(mouse == RB_UP||	B_UP)
	{
		Select =-1;
		return 2;
	}

	//ʹ��
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

		if( Select >= ScrollBar.MinNum + role[0].Propertiy.MagicsNum )
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
// about����
void CFightMagic::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow,int rol_sel)
{
	AboutWindow.ShowWindow(surf, bShowWindow);//��ʾabout����
	//��ʾ˵������
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, 
					CMagic::MAGIC[role[rol_sel].Propertiy.Magics[Select]].About);

}
// ��ʼ��
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
// �ܷ������ħ��
bool CFightMagic::IsCanUse(int n,int rol_sel)
{	
	//�������ħ��ֵ������ħ��ֵ
	return CMagic::MAGIC[n].Mp
					<=role[rol_sel].Propertiy.Mp;
}
// �õ�ħ����id
int CFightMagic::GetMagicID()
{
	return number;
}
// ħ��Ч��
int CFightMagic::MagicEffect(int Num, stRolePropertiy * Propertiy,stRolePropertiy * Propertiy2)
{
	//����Ҫ�Ƕ��Լ�ʹ�õģ�Ȼ��ħ��ֵҪ�㹻
	if( CMagic::MAGIC[Num].Class==MC_SELF&&CMagic::MAGIC[Num].Mp<=Propertiy->Mp)
	{
		Propertiy2->MaxHp+=CMagic::MAGIC[Num].MaxHp;	//�������ֵ
		Propertiy2->Hp+=CMagic::MAGIC[Num].Hp;			//����
		if( Propertiy2->Hp > Propertiy2->MaxHp )
			Propertiy2->Hp = Propertiy2->MaxHp;

		Propertiy2->Attack+=CMagic::MAGIC[Num].Attack;		//����
		Propertiy2->Defend+=CMagic::MAGIC[Num].Defend;		//����
		Propertiy2->Magic+=CMagic::MAGIC[Num].Magic;		//ħ��

		Propertiy2->Stamina+=CMagic::MAGIC[Num].Stamina;	//����
		Propertiy2->Luck+=CMagic::MAGIC[Num].Luck;			//����
		Propertiy2->Speed+=CMagic::MAGIC[Num].Speed;		//����
		Propertiy2->Bp+=CMagic::MAGIC[Num].Bp;				//����
		Propertiy2->Ip+=CMagic::MAGIC[Num].Ip;				//����

		Propertiy->Mp-=CMagic::MAGIC[Num].Mp;				
		return 1;
	}
	return 0;
}

