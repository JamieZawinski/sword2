
//--------------------------------------------------------------------
//	ħ�� ��غ���
//  ������2002��4��25��
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
int CMagic::PicHeight=24;	//ħ��ͼƬ�Ĵ�С

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

//��ʼ������
void CMagic::Init(LPDIRECTDRAWSURFACE surf)						
{	
	ScrollBar.MinNum=0;
	ScrollBar.MaxNum=role[0].Propertiy.MagicsNum;

	GetGameSurface(surf);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
	AboutWindow.ShowWindow(surf);
}

//��ʼ����������
void CMagic::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);

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

void CMagic::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	ShowAboutWindow(surf, bShow);
	ShowMagicListWindow(surf, bShow);
}

void CMagic::ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	AboutWindow.ShowWindow(surf, bShowWindow);
	//��ʾ˵������
	int sel_role=CMenu::RoleSel.SelectRole;			//ѡ�������
	if(Select>-1)
		Font.PrintText(surf, AboutWindow.dialogx+34, AboutWindow.dialogy+34, 
					MAGIC[role[sel_role].Propertiy.Magics[Select]].About);

}

void CMagic::LoadMagicIni(char *filename)
{
	char *index;
	char *temp;

	CIniSet Ini(filename);
	PicWidth=Ini.ReadInt( "ħ��", "Width" );
	PicHeight=Ini.ReadInt( "ħ��", "Height" );
	MagicRect=GetRect(0,0,PicWidth, PicHeight);

	MAX_MAGIC = Ini.GetContinueDataNum("ħ���б�");
	MAGIC=(stMagic *)malloc(sizeof(stMagic)*MAX_MAGIC);

	for(int i=0; i<MAX_MAGIC; i++)
	{
		index=Ini.ReadText("ħ���б�", i);

		MAGIC[i].Id=Ini.ReadInt(index, "Id");		//�����ƷId
		
		//�����Ʒ����
		temp=Ini.ReadText(index, "Name");
		strcpy( MAGIC[i].Name, temp);
		_FREE(temp);
		
		//�����Ʒ����
		temp=Ini.ReadText(index, "About");
		strcpy(MAGIC[i].About, temp);
		_FREE(temp);

		MAGIC[i].Class=Ini.ReadInt(index, "Class");
		MAGIC[i].Hp=Ini.ReadInt(index, "Hp");				//����
		MAGIC[i].MaxHp=Ini.ReadInt(index, "MaxHp");			//�������ֵ
		MAGIC[i].Mp=Ini.ReadInt(index, "Mp");				//ħ��

		MAGIC[i].Attack=Ini.ReadInt(index, "Attack");		//����
		MAGIC[i].Defend=Ini.ReadInt(index, "Defend");		//����

		MAGIC[i].Magic=Ini.ReadInt(index, "Magic");			//ħ��
		MAGIC[i].Stamina=Ini.ReadInt(index, "Stamina");		//����
		MAGIC[i].Luck=Ini.ReadInt(index, "Luck");			//����
		MAGIC[i].Speed=Ini.ReadInt(index, "Speed");			//����
		MAGIC[i].Bp=Ini.ReadInt(index, "Bp");				//����
		MAGIC[i].Ip=Ini.ReadInt(index, "Ip");				//����

		//����ħ����ͼƬ
		temp=Ini.ReadText(index, "PicFileName");
		strcpy( MAGIC[i].PicFileName, temp);
		CreateBitmap( MAGIC[i].Pic, PicWidth, PicHeight, temp);
		DDSetColorKey( MAGIC[i].Pic, ColorKey );
		_FREE(temp);
	
		//����ħ����Ч��ͼƬ
		temp=Ini.ReadText(index, "EffectPicFileName");
		strcpy( MAGIC[i].EffectPicFileName, temp);
		CreateBitmap( MAGIC[i].EffectPic, 0, 0, temp);
		DDSetColorKey( MAGIC[i].EffectPic, ColorKey );
		_FREE(temp);
		
		//����ս��ʱ�ͷ�ħ�����ƶ�����
		temp=Ini.ReadText(index, "MagicFileName");
		strcpy( MAGIC[i].MagicFileName, temp);
		_FREE(temp);
		//��������
		temp=Ini.ReadText(index,"SoundFileName");
		strcpy(MAGIC[i].SoundFileName,temp);
		_FREE(temp);

		_FREE(index);
	}

}
// ��ʾħ������
void CMagic::ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	int n;
	static int MouseY = -1;
	ShowListWindow(surf, bShowWindow, left, top);

	int sel_role=CMenu::RoleSel.SelectRole;			//ѡ�������

	//����ƶ�ѡ��
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;		//��ǰѡ��
			if( Select >= role[sel_role].Propertiy.MagicsNum ) 
				Select=role[sel_role].Propertiy.MagicsNum-1;				//������������һ��
		}
	}

	//��ʾ��Ŀ����һҳ�����ʾһҳ��������ʾȫ��
	
	int shownum=(ScrollBar.PageNum<role[sel_role].Propertiy.MagicsNum)?
			ScrollBar.PageNum:role[sel_role].Propertiy.MagicsNum;

	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//������ʾ
		if( n==Select )
		{
			//���²�����ʾ������
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, 
											y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, 
											SelectColor);
		}
		//��ʾʾ��ͼƬ
		Blt(surf, x, y+LineHeight*i, MAGIC[role[sel_role].Propertiy.Magics[n]].Pic, 
				GetRect(0, 0, PicWidth, LineHeight), true);

		//��Ӱ��ʾ����
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, 
						MAGIC[role[sel_role].Propertiy.Magics[n]].Name);

		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, 
						MAGIC[role[sel_role].Propertiy.Magics[n]].Name);

	}

	//ѡ��
	if( mouse == LB_UP )
	{
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( sel == Select )	//ʹ��
			{
				CMenu::Sound_Click.Play();
				UseMagic(sel,sel_role);
				Menu.ReInitMenu();	//������Ļ
				if(Select!=(point.y-y)/LineHeight + ScrollBar.CurNum)
					Select =-1;
				return;
			}
		}
	}

	//ʹ��
	if( A_UP &&Select>-1)
	{
		if( role[sel_role].Propertiy.MagicsNum > 0 )
		{
			CMenu::Sound_Click.Play();
			UseMagic(Select,sel_role);
			Menu.ReInitMenu();	//������Ļ
			if(Select!=(point.y-y)/LineHeight + ScrollBar.CurNum)
				Select =-1;
			return;
		}
	}
	
	//��һ��
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

	//��һ��
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
// ʹ��ħ��
void CMagic::UseMagic(int sel,int sel_role)
{
	int use_role=Menu.RoleSel.DoMenu(lpDDSBack);	//ѡ�������
	int magic=role[sel_role].Propertiy.Magics[sel];	//ѡ���ħ��
	if(!(use_role!=-1))
		return;
	//����Ч��
	if(!MagicEffect(magic, &role[sel_role],&role[use_role]))
	{
		char x[256];
		sprintf(x, "��ħ������ʹ��");
		GameMessage.DoMenu(lpDDSBack, x);
		return;
	}
	ScrollBar.MinNum=0;
	ScrollBar.MaxNum=role[0].Propertiy.CurGoodsNum;
	
	//����������
	if( ScrollBar.MaxNum < ScrollBar.PageNum )
		ScrollBar.MaxNum = ScrollBar.PageNum;
	
	//��ǰλ������
	if( ScrollBar.CurNum > ScrollBar.MaxNum - ScrollBar.PageNum )
	{
		ScrollBar.CurNum = ScrollBar.MaxNum - ScrollBar.PageNum;
	}
	
	//��ǰλ������
	if( ScrollBar.MaxNum - ScrollBar.CurNum < ScrollBar.PageNum )
	{
		ScrollBar.CurNum = ScrollBar.MaxNum - ScrollBar.PageNum;
	}
	
	ScrollBar.CountNewRect();
}
// ħ��Ч��
int CMagic::MagicEffect(int Num, CRole *Role, CRole *RoleReceive)
{
	//����Ҫ�Ƕ��Լ�ʹ�õģ�Ȼ��ħ��ֵҪ�㹻
	if( CMagic::MAGIC[Num].Class==MC_SELF&&CMagic::MAGIC[Num].Mp<=Role->Propertiy.Mp)
	{
		RoleReceive->Propertiy.MaxHp+=CMagic::MAGIC[Num].MaxHp;	//�������ֵ
		RoleReceive->Propertiy.Hp+=CMagic::MAGIC[Num].Hp;			//����
		if( RoleReceive->Propertiy.Hp > RoleReceive->Propertiy.MaxHp )
			RoleReceive->Propertiy.Hp = RoleReceive->Propertiy.MaxHp;

		RoleReceive->Propertiy.Attack+=CMagic::MAGIC[Num].Attack;		//����
		RoleReceive->Propertiy.Defend+=CMagic::MAGIC[Num].Defend;		//����
		RoleReceive->Propertiy.Magic+=CMagic::MAGIC[Num].Magic;		//ħ��

		RoleReceive->Propertiy.Stamina+=CMagic::MAGIC[Num].Stamina;	//����
		RoleReceive->Propertiy.Luck+=CMagic::MAGIC[Num].Luck;			//����
		RoleReceive->Propertiy.Speed+=CMagic::MAGIC[Num].Speed;		//����
		RoleReceive->Propertiy.Bp+=CMagic::MAGIC[Num].Bp;				//����
		RoleReceive->Propertiy.Ip+=CMagic::MAGIC[Num].Ip;				//����

		Role->Propertiy.Mp-=CMagic::MAGIC[Num].Mp;
		return 1;
	}
	
	return 0;	
}
// �õ�ħ��id
int CMagic::GetMagicNum(char *name)
{
	for(int i=0; i<MAX_MAGIC; i++)
	{
		if( strcmp(name, MAGIC[i].Name)==0 )
			return i;
	}
	return -1;
}


