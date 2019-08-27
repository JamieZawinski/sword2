//********************************************
//	����ѡ�� ��غ���
//  ������2000��7��09��
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

CRoleSel::CRoleSel()
{
	SelectRole=0;
	Role = NULL;
	PhotoPic = NULL;
}

CRoleSel::~CRoleSel()
{
	_DELETE_ARRAY( Role );
	_DELETE( PhotoPic );
}

//��ȡ����
void CRoleSel::LoadRoleSelIni(char *filename, char *index)
{
	//����
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "RoleSelFile");
	char* strIndex = Ini.ReadText(index, "RoleSelIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;

	//��ʼ����ť
	ButtonNum=Ini.ReadInt(index, "ButtonNum");
	Role=new CCButton[ButtonNum];

	for(int i=0; i<ButtonNum; i++)
	{
		strFile = Ini.ReadText(index, i*2);
		strIndex = Ini.ReadData(index,i*2);
		Role[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
		Role[i].x=dialogx+45;
		Role[i].y=dialogy+45+70*i;
	}

	//��ʼ��ͷ��
	char *name;
	PhotoPic=new LPDIRECTDRAWSURFACE[ButtonNum];
	for(i=0; i<ButtonNum; i++)
	{
		name=Ini.ReadText(index, i*2+1);
		CreateBitmap(PhotoPic[i], 0,0, name);
		DDSetColorKey(PhotoPic[i], ColorKey);
		_FREE(name);
	}
}

//��ʼ����ʾ
void CRoleSel::Init(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	ShowWindow(surf, bShowWindow);
}

//��ʾ
void CRoleSel::ShowRoleSelWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	for(int i=0; i<RoleNum; i++)
	{
		if( i==SelectRole )
			Role[role[i].Id].CheckLoop(surf, 2);
		else
			Role[role[i].Id].CheckLoop(surf);
	}
}

//�˵�ѭ��
int CRoleSel::DoMenu(LPDIRECTDRAWSURFACE& surf, bool bShowWindow)
{
	int Sel=0;

	//��ʼ�����ں�ͷ��λ��
	dialogh=64+45+70*(RoleNum);
	dialogy=(ScreenHeight-dialogh)/2;
	for(int i=0; i<RoleNum; i++)
	{
		Role[role[i].Id].x=dialogx+45;
		Role[role[i].Id].y=dialogy+32+45+70*i;
	}

	ShowWindow(lpDDSBackGround, bShowWindow);
	
	//�˵�ѭ��
	MSG msg;
	while(bActive)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return -1;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//��ȡ������Ϣ
			GetInputData();

			Blt(surf, 0, 0, lpDDSBackGround, RectScreen, false);
			
			//����
			Bar(surf, dialogx+32+5, dialogy+64, dialogx+32+dialogw-74, dialogy+65, RGB16(240,211,123));
			Bar(surf, dialogx+32+5, dialogy+65, dialogx+32+dialogw-74, dialogy+66, RGB16(156,119,5));

			Font.SetColor(RGB(255,255,255));
			Font.PrintText(surf, dialogx+40, dialogy+40,  "��ѡ��ʹ�ö���");
			dialogy+=32;
			for(int i=0; i<RoleNum; i++)
			{
				if( Sel != i )
				{
					Font.SetColor(RGB(127,127,127));
					Font.PrintText(surf, dialogx+120, dialogy+42+70*i,  "%6s  Lv:%d", role[i].Name, role[i].Propertiy.Level);
					Font.PrintText(surf, dialogx+120, dialogy+61+70*i, "HP: %6d/%d", role[i].Propertiy.Hp, role[i].Propertiy.MaxHp );
					Font.PrintText(surf, dialogx+120, dialogy+79+70*i, "MP: %6d/%d", role[i].Propertiy.Mp, role[i].Propertiy.MaxMp );
				}
				else
				{
					Font.SetColor(RGB(255,255,255));
					Font.PrintText(surf, dialogx+120, dialogy+42+70*i,  "<c4>%6s</c>  Lv:<c4>%d</c>", role[i].Name, role[i].Propertiy.Level);
					Font.PrintText(surf, dialogx+120, dialogy+61+70*i, "HP: <c4>%6d</c>/<c4>%d</c>", role[i].Propertiy.Hp, role[i].Propertiy.MaxHp );
					Font.PrintText(surf, dialogx+120, dialogy+79+70*i, "MP: <c4>%6d</c>/<c4>%d</c>", role[i].Propertiy.Mp, role[i].Propertiy.MaxMp );
				}
			}
			dialogy-=32;

			//����ѡ��
			for(i=0; i<RoleNum; i++)
			{
				int temp = Role[role[i].Id].CheckLoop(surf, -1);
				if(temp==1)
				{
					SelectRole=i;
					mouse=key=0;
					return i;
				}
				if(temp==2)
				{
					Sel = i;
				}
			}
			
			ShowMouseCursor(surf,false);
			//��ʾFPS
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();

			if( DOWN_DOWN )		//��һ��
			{
				Sel++;
				if( Sel>=RoleNum ) Sel=0;
			}
			if( UP_DOWN )			//��һ��
			{
				Sel--;
				if( Sel<0 ) Sel=RoleNum-1;
			}
			if( A_UP )	//ȷ������
			{
				CMenu::Sound_Click.Play();
				SelectRole=Sel;
				mouse=key=0;
				return Sel;
			}

			if( B_UP || mouse == RB_UP )//ȡ������
			{
				CMenu::Sound_Cancel.Play();
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				return -1;
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
	return -1;
}
