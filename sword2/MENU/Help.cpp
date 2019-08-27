//********************************************
//	���� ��غ���
//  ������2000��10��22��
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "menu_inc.h"

char *About[]={
"������ʥII������ƪ���ǽ��С�顰ʥ��ϵ�С���һ���⴫�����������ˡ�ʥII��˫�н������������أ�Ȼ������µĽ�ɫ�ͳ���������������һ���µĹ��¡�</n>\
���������ԡ�����ƪ����������ʥ��II���Ĺ��¡���ͬʱҲ���������е���Ϸ��Դ���Ժ���¾��ɴ���������ˣ�</n>\
����������Ҫ��л���һֱ�����ԡ�ʥ������֧�֡���л�������Ĺ���Ӣ�۾������͡�ʥII��˫�н����������ǣ������������ǵĴ��ڣ���������������������µ���ս��Ҳ��������������������Ĺ��������á�ʥ��ϵ�С�һ����һ��������Ӧ��˵��ʥ����������Ǵ�ҵ���Ѫ��\
</n>\
����ϣ����Ҽ�����ע��㣬����֧�֡�ʥ��Ӣ�۴�����лл��", \
"\
�������ǵ���ҳ�����ʱ�ա���</n>\
����http://www.gpgame.com</n>\
</n>\
    sleepd : sleepd@163.net</n>\
  endedman : ended-man@263.net</n>\
   sea_bug : lindehui@263.net</n>\
      soft : softboy@sina.com</n>\
      ���� : gundamy@telekbird.com.cn</n>\
      ���� : jimmy_dog@sina.com</n>\
     yahle : yahle@sina.com</n>\
     tiamo : tiamo@mail.bnu.edu.cn</n>\
    zrhspy : zrhspy@163.com</n>\
   skywind : skywindt@yeah.net</n>\
   amiyami : nzy830222@263.net</n>\
  �������� : johndragon@sohu.com</n>\
\
       ��㹤���顡��������"
};

CHelp::CHelp()
{
	page=0;
}

CHelp::~CHelp()
{
}

//��ȡ����
void CHelp::LoadHelpIni(char *filename, char *index)
{
	//����
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "HelpFile");
	char* strIndex = Ini.ReadText(index, "HelpIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;
}

//��ʼ������
void CHelp::Init(LPDIRECTDRAWSURFACE surf)
{
	GetGameSurface(lpDDSBackGround);
	FillSurface(lpDDSBack, RGB16(0));
	ABlt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, 0, 24);
	PopWindow(surf, PopWindowStyle, PopWindowStep);
}

//��ʾ
void CHelp::ShowHelpWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow, int left, int top)
{
	DWORD c=Font.GetColor();
	//��Ӱ��ʾ����
	Font.SetColor(0);

	Font.PrintText(surf, dialogx+40, dialogy+40,  About[page]);

	Font.SetColor(c);

	Font.PrintText(surf, dialogx+40, dialogy+40,  About[page]);
}

//ѭ��
void CHelp::DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow)
{
	if( mouse==LB_UP||A_UP )
	{
		page = 1 -page;	//��ҳ
		CMenu::ReInitMenu();
		ShowHelpWindow(lpDDSBack, false);
	}
}
