//********************************************
//	顶级菜单 相关函数
//  创建于2000年7月09日
//********************************************
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\map.h"
#include "..\role.h"
#include "..\message.h"
#include "menu_inc.h"

CSystem::CSystem()
{
	Button = NULL;
	m_lSelect = 0;
}

CSystem::~CSystem()
{
	_DELETE_ARRAY( Button );
}

//读取
void CSystem::LoadSystemIni(char *filename, char *index)
{
	CIniSet Ini(filename);
	char* strFile = Ini.ReadText(index, "WindowFile");
	char* strIndex = Ini.ReadText(index, "WindowIndex");
	LoadWindowIni(strFile, strIndex);
	_FREE(strFile);
	_FREE(strIndex);

	ButtonNum=Ini.ReadInt(index, "ButtonNum");
	Button=new CCButton[ButtonNum];

	//初始化按钮
	for(int i=0; i<ButtonNum; i++)
	{
		strFile = Ini.ReadText(index, i);
		strIndex = Ini.ReadData(index,i);
		Button[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
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
void CSystem::ShowSystem(LPDIRECTDRAWSURFACE surf, bool bShowWindow)
{
	ShowWindow(surf, bShowWindow);		//显示窗口

	for(int i=0; i<ButtonNum; i++)
	{
		Button[i].CheckLoop(surf);		//按键测试
	}
}

//菜单循环
int CSystem::DoMenu(LPDIRECTDRAWSURFACE& surf, bool bShowWindow)
{
	ShowWindow(surf, bShowWindow);		//显示窗口

	bool b[20];
	int bHigh=-1;
	for(int i=0; i<ButtonNum; i++)
	{
		// 高亮显示当前选择的按钮
		if( i == m_lSelect )
		{
			bHigh = 1;
		}
		else
		{
			bHigh = -1;
		}

		// 检测
		int r = Button[i].CheckLoop(surf, bHigh);
		if( r==1 )
		{
			b[i]=true;
		}
		else
		{
			b[i]=false;
		}

		// 高亮
		if( r==2 )
		{
			m_lSelect = i;
		}
	}

	if( b[0] || A_UP && m_lSelect == 0 )	//Load
	{
		CMenu::Sound_Click.Play();
		if( CMenu::LoadSave.DoMenu(surf, true, 180, 100) ) 
			return 1;
		else 
			CMenu::ReInitMenu();
	}

	if( b[1] || A_UP && m_lSelect == 1  )	//Save
	{
		CMenu::Sound_Click.Play();
		if( CMenu::LoadSave.DoMenu(surf, false, 180, 100) ) 
			return 2;
		CMenu::ReInitMenu();
	}

	if( b[2] || A_UP && m_lSelect == 2  )	//exit
	{
		CMenu::Sound_Click.Play();
		Message.ClearAllMessage();			//清空消息队列
		bControl = true;
		return 3;
	}

	// 下一个
	if( DOWN_DOWN )
	{
		m_lSelect++;
		CMenu::Sound_Change.Play();
		if( m_lSelect > 2 ) 
			m_lSelect = 0;			//循环
	}

	// 上一个
	if( UP_DOWN )
	{
		m_lSelect--;
		CMenu::Sound_Change.Play();
		if( m_lSelect < 0 ) 
			m_lSelect = 2;
	}

	return 0;
}
