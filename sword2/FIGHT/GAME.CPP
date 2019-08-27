//********************************************
//	游戏主程序段     相关函数
//  创建于2000年5月29日
//********************************************

#include <windows.h>
#include <stdio.h>

#include "..\gamelib\goldpoint2.h"
#include "fight.h"

//////////////////////////////////
//通用变量
//////////////////////////////////
LPDIRECTDRAWSURFACE *pic;
int Loopstatus;								//主循环的判断
//------------- 游戏变量数据 ---------------------------

stRolePropertiy Propertiy_lib1;   //  角色状态数据存放的内存空间
stRolePropertiy * Propertiy1;   //  指向角色数据的指针(用来传递数据)

stRolePropertiy Propertiy_lib2;   //第二个角色的数据
stRolePropertiy * Propertiy2;   
Fight fight;


/*   我自定义的DrawSurface表面   */
//游戏初始化
BOOL InitGame()
{	
	Propertiy1=&Propertiy_lib1;
	Propertiy_lib1.Hp=102;
	Propertiy2=&Propertiy_lib2;
	Propertiy_lib2.Hp=105;
//	fight.Init(&Propertiy_lib1); //  初始化/
	fight.Init();
	//初始化一个后台表面   读取文件

	Loopstatus=1;
	CreateBitmap(lpDDSBackGround, 0, 0, "Image\\background.bmp");

//	DDSetColorKey(pic,RGB(255,0,255));

	/*自定义表面的数据读取 */

	//初始化字体
	font.InitFont(16, "宋体");

	Propertiy_lib1.Hp=100;
	Propertiy_lib2.Hp=100;
	Propertiy_lib1.Mp=100;
	Propertiy_lib2.Mp=100;
	Propertiy_lib1.Power=100;
	Propertiy_lib2.Power=100;
	Propertiy_lib1.Heart=100;
	Propertiy_lib2.Heart=100;
	Propertiy_lib1.Attack=10;
	Propertiy_lib2.Attack=13;
	Propertiy_lib1.Defend=5;
	Propertiy_lib2.Defend=5;

	CIniSet FightRoleInit("ini\\FightRoleInit.ini");
	Propertiy_lib1.Fight_Ini_Name="克莉丝";
	Propertiy_lib2.Fight_Ini_Name="兰特";
// 以上是我预先帮你读取的头像数据

	
	return true;
}

//--------------------------------------------------

//主循环
void MainLoop()
{
	//延时控速
	NowTick=timeGetTime();
	if( NowTick < OldTick+DelayTick ) return;
	OldTick=NowTick;

	//获取鼠标当前的坐标
	GetCursorPos(&point);
	if( WindowMode != 0 )	//窗口
	{
		point.x-=RectWindow.left;
		point.y-=RectWindow.top;
	}
	
	Mouse.GetMouseState();		//获取鼠标立即信息
	mouse=Mouse.GetMouseData();	//获取鼠标缓冲信息
	Key.GetKeyData();			//获取键盘缓冲数据(附带即时数据)
	key=Key.GetKey();			//获取键盘数据

	//切换全屏与窗口
	if( (Key.GetState( DIK_LALT ) || Key.GetState( DIK_RALT )) && Key.GetState( DIK_RETURN ) )
	{
		if ( bSwitch && bActive )
		{
			if( WindowMode == 0 )
				lpDD->RestoreDisplayMode();
			GetWindowRect(hWnd, &RectWindow);
			WindowMode = 1-WindowMode;
			ChangeCoopLevel(hWnd);
		}
	}

//   Loopstatus为判断使用哪个循环
	if(Loopstatus==1)		//  游戏进行的主体的循环
	{
		DelayTick=0;
			//按下鼠标左健
		if( mouse == LB_UP )
		{
			if(point.x>=0&&point.x<=72&&point.y>=0&&point.y<=72)	//方式1
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					NULL,&Propertiy_lib2,NULL,
					"兰色怪","兰色怪","兰色怪","兰色怪");
			}
			else if(point.x>=72&&point.x<=144&&point.y>=0&&point.y<=72)	//方式2
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					NULL,&Propertiy_lib2,NULL,
					"兰色怪","兰色怪",NULL,"兰色怪");
			}
			else if(point.x>=144&&point.x<=216&&point.y>=0&&point.y<=72)	//方式3
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					NULL,&Propertiy_lib2,NULL,
					"兰色怪",NULL,NULL,"兰色怪");
			}
			else if(point.x>=216&&point.x<=288&&point.y>=0&&point.y<=72)	//方式4
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					NULL,&Propertiy_lib2,NULL,
					NULL,"兰色怪",NULL,NULL);
			}

			else if(point.x>=0&&point.x<=72&&point.y>=72&&point.y<=144)	//方式5
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib2,NULL,&Propertiy_lib1,
					"兰色怪","兰色怪","兰色怪","兰色怪");
			}
			else if(point.x>=72&&point.x<=144&&point.y>=72&&point.y<=144)	//方式6
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib2,NULL,&Propertiy_lib1,
					"兰色怪","兰色怪",NULL,"兰色怪");
			}
			else if(point.x>=144&&point.x<=216&&point.y>=72&&point.y<=144)	//方式7
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib2,NULL,&Propertiy_lib1,
					"兰色怪",NULL,NULL,"兰色怪");
			}
			else if(point.x>=216&&point.x<=288&&point.y>=72&&point.y<=144)	//方式8
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib2,NULL,&Propertiy_lib1,
					NULL,"兰色怪",NULL,NULL);
			}
			else if(point.x>=0&&point.x<=72&&point.y>=144&&point.y<=216)	//方式9
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib1,&Propertiy_lib2,&Propertiy_lib1,
					"兰色怪","兰色怪","兰色怪","兰色怪");
			}
			else if(point.x>=72&&point.x<=144&&point.y>=144&&point.y<=216)	//方式10
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib1,&Propertiy_lib2,&Propertiy_lib1,
					"兰色怪","兰色怪",NULL,"兰色怪");
			}
			else if(point.x>=144&&point.x<=216&&point.y>=144&&point.y<=216)	//方式11
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib1,&Propertiy_lib2,&Propertiy_lib1,
					"兰色怪",NULL,NULL,"兰色怪");
			}
			else if(point.x>=216&&point.x<=288&&point.y>=144&&point.y<=216)	//方式12
			{
				Loopstatus=2;
				fight.Start("ini\\background1.ini",
					&Propertiy_lib1,&Propertiy_lib2,&Propertiy_lib1,
					NULL,"兰色怪",NULL,NULL);
			}

/*				fight.Start("ini\\background1.ini",
					NULL,&Propertiy_lib2,&Propertiy_lib1,
					"兰色怪","兰色怪",NULL,"兰色怪");*/
		}
		Blt(lpDDSBack,0,0,lpDDSBackGround,RectScreen,false);
		//Blt(lpDDSBack,0,0,fight.background,RectScreen,false);
		font.PrintText(lpDDSBack, 100, 250,"这里列出常用的几种方式");
		font.PrintText(lpDDSBack, 100, 300,"现在处于游戏正常的进程，sb完成的界面!");
		font.PrintText(lpDDSBack, 100, 350,"ESC键退出");
	}
	else if(Loopstatus==2)	//  战斗系统的主循环
	{
		Loopstatus=fight.Loop(mouse,point);
	}

	//更新屏幕
	//(注：游戏中的所有东西最好都先Blt到lpDDSBack,然后一起更新到屏幕)
	UpdateScreen();
}
