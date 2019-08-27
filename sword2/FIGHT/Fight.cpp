//   ******************************
//   *      战斗系统类函数
//   *     创建于 5月29日
//   ******************************
//  ************************************
//  *     战斗系统的主循环函数文件 
//  *   创建于  5月29日   
//  ************************************

#include "stdio.h"
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\menu\menu_inc.h"
#include "..\main.h"
#include "FightGoods.h"
#include "Fight.h"

//主循环体
int CFight::Loop(int mouse,POINT point)
{
	static ret=0;			//战斗的返回值
	static POINT old_point; //上次鼠标的位置
	int i;
	static int old;			//上次选择的按纽
	static int oldjoy=0;		//上次joy选的按钮
	static bool bUseMagic = false;  //是否使用魔法
	static bool bShowMagicPic = false;//是否显示魔法图片
	bool bButton[5]={0,0,0,0,0};	//当前选择的按纽标志数组

	RECT Rstatus_pic={0,0,170,120};	//人物状态图的矩形
	RECT Rtemp={0,0,50,50};			//头像图矩形

	//背景图象显示
	Blt(lpDDSBack,0,0,background[Number-1],RectScreen,false);

	//--------------------以上为公共代码--------------------

	//-------------------------------------------------------------------
	// 控制部分
	//-------------------------------------------------------------------
	switch( main_count )
	{
	/////////////////////////////////////////////////////////////////////
	// 进行游戏菜单选择-------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case MAIN:
		{
			// 显示战场人物
			Show_Role_Enemy();
			if(mouse == RB_UP || B_UP )		// 取消操作
			{
				if(command_count > 0)
					command_count --;		//减少命令记数
			}

			// 选人光标的显示
			cursor_role.ShowAni(lpDDSBack, FightRole[command_count].x+30, FightRole[command_count].y-30);

			// 键盘控制 ----------------------------------------
			switch(key)
			{
			case DIK_A:				// 攻击
				{
					bButton[1] = 1;
				}
				break;
			case DIK_E:				// 魔法
				{
					bButton[2] = 1;
				}
				break;
			case DIK_G:				// 道具
				{
					bButton[3] = 1;
				}
				break;
			case DIK_Q:				// 逃跑
				{
					bButton[4] = 1;
				}
				break;
			case DIK_R:				// 重复
				{
					bButton[0] = 1;
				}
				break;

			case DIK_F11:		// test exit
				main_count=OVER;
				temp_swap=1;
				key = 0;
				break;

			case DIK_H:			// hidden key
				{
					for(int i=0; i<RoleNum;i++)
					{
						role[i].Propertiy.Hp = role[i].Propertiy.MaxHp;
						role[i].Propertiy.Mp = role[i].Propertiy.MaxMp;
					}
				}
				break;

			default:
				break;
			}
			//手柄，3号键确定
			if(joystick == DIJ_3)
				bButton[oldjoy] = 1;
			if(joystick == DIJ_4)	//4号键选择
			{
				oldjoy++;
				if(oldjoy==5)
					oldjoy = 0;
			}
			// ----------------------------------------------
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 使用物品的选择角色
	/////////////////////////////////////////////////////////////////////
	case USER_WHO:
		{
			if(mouse == RB_UP || B_UP )		// 取消操作
				main_count = MAIN;			// 回到主循环状态

			nNonce = Test_Role(nNonce);

			if(old_point.x == point.x && old_point.y == point.y)	//鼠标位置没有变化
			{
			}
			else
			{
				old_point.x = point.x;		//保存鼠标位置
				old_point.y = point.y;
				//得到鼠标所指向的主角
				//范围：向上15向下70向做右15，向右65的矩形
				for(i=0;i<3;i++)			
					if(point.x>FightRole[i].x+15&&point.x<FightRole[i].x+65
						&& point.y>FightRole[i].y+15&&point.y<FightRole[i].y+70 
						&& FightRole[i].use==1)
					{
						nNonce = i;				//保存该主角
						break;
					}
			}

			// 键盘控制
			if(DOWN_DOWN)					//向下，直到找到主角为止
			{
				if(nNonce < 3)
				{
					if(FightRole[nNonce+1].use == 1)
						nNonce ++;
					else if(FightRole[nNonce+2].use == 1)
						nNonce += 2;
					else if(FightRole[nNonce+3].use == 1)
						nNonce += 3;
				}
				else
					nNonce = 3;
			}
			if(UP_DOWN)							//向上，直到找到主角为止
			{
				if(nNonce > 0)
				{
					if(FightRole[nNonce-1].use == 1)
						nNonce --;
					else if(FightRole[nNonce-2].use == 1)
						nNonce -= 2;
					else if(FightRole[nNonce-3].use == 1)
						nNonce -= 3;
				}
				else
					nNonce = 0;
			}
			
			//显示主角和敌人
			//会有轮廓
			Show_Role_Enemy();
			//选人鼠标
			cursor_enemy.ShowAni(lpDDSBack, FightRole[nNonce].x+30, FightRole[nNonce].y-15);

			if( mouse == LB_UP || A_UP )	//选定主角
			{
				i = nNonce;
				{
					Sound_Click.Play();	//音效
					if(!bUseMagic)
					{
						Command[command_count].lCommand = TOOLS;	//该次是使用物品
						Command[command_count].lAccessory = m_fightgoods.GetGoodsID();//物品类型
						Command[command_count].kill_who=i;			//使用对象
						Command[command_count].Class=1;				//对自己的
						main_count=MAIN;	//进入主循环
						command_count++;	//加入该条指令
					}
					else
					{
						Command[command_count].lCommand = MAGIC;	//该次是使用魔法
						Command[command_count].lAccessory = m_fightmagic.GetMagicID();//魔法类型
						Command[command_count].kill_who=i;			//使用对象
						Command[command_count].Class=1;
						main_count=MAIN;	//进入主循环
						command_count++;	//加入该条指令
						bUseMagic = false;
					}
					if(command_count>GetLastRoleNum())	//所有的主角都已经使用了
					{
						command_count=0;				//归0
						main_count = ROLE_KILL_ENEMY;	//命令发布完毕，开始攻击
						for(i=0; i<3; i++)
							if(FightRole[i].use==1  )
							{
								_FREE(Role_Temp);
								//下一个数据
								if(Command[i].lCommand == TOOLS)
									Role_Temp=Role_Tool[i].ReadText("设置","continue");
								else if(Command[i].lCommand == MAGIC)	 //用对自己人的魔法，
								{
									Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("设置","continue");
									//设置图片的起始坐标
									if(Role_Magic[Command[i].lAccessory].ReadInt("设置","相对"))
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[i].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[i].y;
									}
									else
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[Command[i].kill_who].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[Command[i].kill_who].y;
									}
									//加载魔法声音
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[i].lAccessory].SoundFileName);
								}
								else		//攻击
									Role_Temp=Role_Move[i].ReadText("设置","continue");
								kill_count=0;			//从第一个主角开始
								break;
							}
						Account_Attack();	//计算攻击力
					}
					else if(FightRole[command_count].use==0)		//轮到下一个主角
					{
						command_count++;
						if(FightRole[command_count].use==0)
							command_count=0;
						main_count = MAIN;		//主循环
					}
					break;
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 选择要攻击的对象--------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case KILL_WHO:
		{
			if(mouse == RB_UP || B_UP )		// 取消操作
				main_count = MAIN;
			
			nNonce = Test_Enemy(nNonce);
			// 用来显示光标的判断
			//和USER_WHO差不多
			if(old_point.x == point.x && old_point.y == point.y)
			{
			}
			else
			{
				old_point.x = point.x;
				old_point.y = point.y;
				for(i=0;i<4;i++)
				if(point.x>EnemyRole[i].x+15
					&&point.x<EnemyRole[i].x+65 && point.y>EnemyRole[i].y+15
					&&point.y<EnemyRole[i].y+70 && EnemyRole[i].use==1)
				{
					nNonce = i;
					break;
				}
			}

			// 键盘的控制
			if(DOWN_DOWN)
			{
				if(nNonce < 3)
				{
					if(EnemyRole[nNonce+1].use == 1)
						nNonce ++;
					else if(EnemyRole[nNonce+2].use == 1)
						nNonce += 2;
					else if(EnemyRole[nNonce+3].use == 1)
						nNonce += 3;
				}
				else
					nNonce = 3;
			}
			if(UP_DOWN)
			{
				if(nNonce > 0)
				{
					if(EnemyRole[nNonce-1].use == 1)
						nNonce --;
					else if(EnemyRole[nNonce-2].use == 1)
						nNonce -= 2;
					else if(EnemyRole[nNonce-3].use == 1)
						nNonce -= 3;
				}
				else
					nNonce = 0;
			}
			
			//显示敌人和我方的人
			Show_Role_Enemy();		
			cursor_enemy.ShowAni(lpDDSBack, EnemyRole[nNonce].x+24, EnemyRole[nNonce].y-15);

			//选定
			if( mouse == LB_UP || A_UP )
			{
				i = nNonce;
				{
					Sound_Click.Play();	//音效
					if(!bUseMagic)
					{
						Command[command_count].lCommand = ATTACK;
						Command[command_count].kill_who=i;
						main_count=MAIN;
						command_count++;
					}
					else
					{
						Command[command_count].lCommand = MAGIC;	//该次是使用魔法
						Command[command_count].lAccessory = m_fightmagic.GetMagicID();//魔法类型
						Command[command_count].kill_who=i;			//使用对象
						Command[command_count].Class=0;
						main_count=MAIN;	//进入主循环
						command_count++;	//加入该条指令
						bUseMagic = false;
					}
					if(command_count>GetLastRoleNum())
					{
						command_count=0;
						main_count = ROLE_KILL_ENEMY;	//命令发布完毕，开始攻击
						for(i=0; i<3; i++)
							if(FightRole[i].use==1 )
							{
								_FREE(Role_Temp);
								if(Command[i].lCommand == TOOLS)
									Role_Temp=Role_Tool[i].ReadText("设置","continue");
								else if(Command[i].lCommand == MAGIC)		//对敌人的魔法
								{
									Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("设置","continue");
									if(Role_Magic[Command[i].lAccessory].ReadInt("设置","相对"))
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[i].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[i].y;
									}
									else
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[i].kill_who].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[i].kill_who].y;
									}
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[i].lAccessory].SoundFileName);
								}
								else 
									Role_Temp=Role_Move[i].ReadText("设置","continue");
								kill_count=0;			//从第一个主角开始
								break;
							}
						Account_Attack();
					}
					else if(FightRole[command_count].use==0)
					{
						command_count++;
						if(FightRole[command_count].use==0)
							command_count=0;
					}
					break;
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 进行攻击的动作------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case ROLE_KILL_ENEMY:
		{
			for(i=0; i<3; i++)		//遍历主角，执行战斗指令
			{
			if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == ATTACK)		//轮到该主角攻击了
			{
				bShowMagicPic = false;
				// 测试要攻击的对象是否存在，如果不存在则换人
				if(Command[kill_count].kill_who!=Test_Enemy(Command[kill_count].kill_who))	
				{
					Command[kill_count].kill_who=Test_Enemy(Command[kill_count].kill_who);
					Account_Attack();
				}
				//进攻动作
				FightRole[kill_count].stop_temp=10;
				Sleep(Role_Move[i].ReadInt("设置","延时"));
				//读取位图的大小数据
				RMove_Pic.left=Role_Move[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Role_Move[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Role_Move[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Role_Move[i].ReadInt(Role_Temp,"bottom");
				//循环动作
				//从开始攻击到敌人受伤到主角复位
				if(Role_Move[i].ReadInt(Role_Temp,"对比")==1)
				{
					//得到现在应该出现的位置
					FightRole[kill_count].x1=FightRole[kill_count].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=FightRole[kill_count].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//下一次的数据
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"对比")==2)	//移动
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"对比")==3)	//敌人受伤
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//敌人动作变成受伤的动作
					EnemyRole[Command[kill_count].kill_who].stop_temp=11;
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"对比")==4)	//攻击
				{
					FightRole[kill_count].sound_Fight.Play();	//攻击声
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//敌人动作变成后退动作
					EnemyRole[Command[kill_count].kill_who].stop_temp=10;
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"对比")==5)	//敌人惨叫
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//敌人动作复位成受伤动作
					//---------
					//整个过程是
					//敌人先被攻击
					//然后后退
					//然后复位到被攻击的动作
					//-------------
					EnemyRole[Command[kill_count].kill_who].stop_temp=11;
					EnemyRole[Command[kill_count].kill_who].sound_Defend.Play();	//叫喊声
				}
				else
				{
					//主角回到原位置
					FightRole[kill_count].stop_temp=1;
					//如果胜利了，就直接跳出进攻部分
					if( Account_Loss()==1 ) 
						goto _Next;
					//计算攻击力
					Account_Attack();

					for(int j=i+1; j<3; j++)
						if(FightRole[j].use==1)		// 用来判断下一个角色是哪个角色
						{
							char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
							Role_Move[j].Open(str);
							free(str);
							_FREE(Role_Temp);
							Role_Temp=Role_Move[j].ReadText("设置","continue");
							kill_count=j;			//切换到下一个主角
							goto _Next;
						}
					//没有下个主角了，就换敌人攻击
					Enemy_AI();			//为敌人攻击做好准备
					main_count=ENEMY_KILL_ROLE;		//进入敌人进攻部分
					m_nRoundCount ++;			//增加回合计数
					Account_Attack();
				}
_Next:;
			}
			//使用物品
			else if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == TOOLS)
			{
				bShowMagicPic = false;
				//该位置不存在，换人
				if(Command[kill_count].kill_who!=Test_Role(Command[kill_count].kill_who))	// 测试要攻击的对象是否存在，如果不存在则换人
				{
					Command[kill_count].kill_who=Test_Role(Command[kill_count].kill_who);
					Account_Attack();
				}
				//如果这个物品可以使用，就进入使用模式
				if(m_fightgoods.IsCanUse((int)Command[kill_count].lAccessory))
					FightRole[kill_count].stop_temp=10;
				//不能使用的话，就保持常态
				else 
					FightRole[kill_count].stop_temp = 1;

				Sleep(Role_Tool[i].ReadInt("设置","延时"));
				//读取位图的大小数据
				RMove_Pic.left=Role_Tool[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Role_Tool[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Role_Tool[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Role_Tool[i].ReadInt(Role_Temp,"bottom");
				if(Role_Tool[i].ReadInt(Role_Temp,"对比")==1)
				{
					//位置数据
					FightRole[kill_count].x1=FightRole[kill_count].x+Role_Tool[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=FightRole[kill_count].y+Role_Tool[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//下一个数据
					Role_Temp=Role_Tool[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else
				{
					//恢复原状态
					FightRole[kill_count].stop_temp=1;
					//使用物品
					if(FightRole[kill_count].use==1)
					{
						if(m_fightgoods.IsCanUse((int)Command[kill_count].lAccessory))
						{
							//产生效果
							m_fightgoods.GoodEffect((int)Command[kill_count].lAccessory, Hero[Command[kill_count].kill_who],Command[kill_count].kill_who);
							//更新血槽数据
							FightRole[Command[kill_count].kill_who].BroodLong=Hero[Command[kill_count].kill_who]->Hp * 100 / Hero[Command[kill_count].kill_who]->MaxHp;
						}
						else
						{
							//不能使用的时候，显示提示
							GameMessage.DoMenu(lpDDSBack, "物品使用光了！");
						}
					}

					for(int j=i+1; j<3; j++)
						if(FightRole[j].use==1)		// 用来判断下一个角色是哪个角色
						{
							if(Command[j].lCommand == ATTACK)
							{
								char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
								Role_Move[j].Open(str);
								free(str);
								_FREE(Role_Temp);
								Role_Temp=Role_Move[j].ReadText("设置","continue");
							}
							else if(Command[j].lCommand == TOOLS)
							{
								char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
								Role_Tool[j].Open(str);
								free(str);
								_FREE(Role_Temp);
								Role_Temp=Role_Tool[j].ReadText("设置","continue");
							}
							kill_count=j;
							goto _GoodsNext;
						}
					//没有下一个主角了就换敌人攻击
					Enemy_AI();
					main_count=ENEMY_KILL_ROLE;
					m_nRoundCount ++;			//增加回合计数
					Account_Attack();
				}
_GoodsNext:;
			}			//使用魔法
			else if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == MAGIC)
			{
				if(Command[kill_count].Class==1)		//对自己的
				{
					if(Command[kill_count].kill_who!=Test_Role(Command[kill_count].kill_who))	// 测试要攻击的对象是否存在，如果不存在则换人
					{
						//重新计算
						Command[kill_count].kill_who=Test_Role(Command[kill_count].kill_who);
						Account_Attack();
						if(Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","相对"))
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","startx")+FightRole[kill_count].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","starty")+FightRole[kill_count].y;
						}
						else
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","startx")+FightRole[Command[kill_count].kill_who].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","starty")+FightRole[Command[kill_count].kill_who].y;
						}
					}
					//如果这个魔法可以使用，就进入使用模式
					if(m_fightmagic.IsCanUse((int)Command[kill_count].lAccessory))
					{
						//进入使用模式
						FightRole[kill_count].stop_temp=10;
						//显示魔法效果图片
						bShowMagicPic = true;
						int temp = Command[kill_count].lAccessory;
						//休眠
						Sleep(Role_Magic[temp].ReadInt("设置","延时"));
						//读取人物动作位图的数据
						RMove_Pic.left=Role_Magic[temp].ReadInt(Role_Temp,"left");
						RMove_Pic.top=Role_Magic[temp].ReadInt(Role_Temp,"top");
						RMove_Pic.right=Role_Magic[temp].ReadInt(Role_Temp,"right");
						RMove_Pic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"bottom");
						//魔法图片的数据
						fightmagic.RMagicPic.left = Role_Magic[temp].ReadInt(Role_Temp,"picleft");
						fightmagic.RMagicPic.top = Role_Magic[temp].ReadInt(Role_Temp,"pictop");
						fightmagic.RMagicPic.right=Role_Magic[temp].ReadInt(Role_Temp,"picright");
						fightmagic.RMagicPic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"picbottom");
						if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==1)	//魔法开始
						{
							//播放魔法效果声音
							char * soundtemp;
							soundtemp=Role_Magic[temp].ReadText(Role_Temp,"SoundFileName");
							if(soundtemp[0])
							{
								sound_Magic.Stop();
								sound_Magic.LoadSoundFX(hWnd,soundtemp);
							}
							sound_Magic.Play();
							//位置数据
							int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
							int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
							if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
							{
								FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
								FightRole[kill_count].y1=FightRole[kill_count].y+roley;
							}
							else
							{
								FightRole[kill_count].x1=FightRole[Command[kill_count].kill_who].x+rolex;
								FightRole[kill_count].y1=FightRole[Command[kill_count].kill_who].y+roley;
							}
							int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
							int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
							if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
							{
								fightmagic.x = FightRole[kill_count].x + picx;
								fightmagic.y = FightRole[kill_count].y + picy;
							}
							else
							{
								fightmagic.x = FightRole[Command[kill_count].kill_who].x + picx;
								fightmagic.y = FightRole[Command[kill_count].kill_who].y + picy;
							}
							char *str = Role_Temp;
							//下一个数据
							Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
							free(str);
						}
						else if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==2)	//普通状态
						{
							//位置数据
							int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
							int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
							if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
							{
								FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
								FightRole[kill_count].y1=FightRole[kill_count].y+roley;
							}
							else
							{
								FightRole[kill_count].x1=FightRole[Command[kill_count].kill_who].x+rolex;
								FightRole[kill_count].y1=FightRole[Command[kill_count].kill_who].y+roley;
							}
							int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
							int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
							if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
							{
								fightmagic.x = FightRole[kill_count].x + picx;
								fightmagic.y = FightRole[kill_count].y + picy;
							}
							else
							{
								fightmagic.x = FightRole[Command[kill_count].kill_who].x + picx;
								fightmagic.y = FightRole[Command[kill_count].kill_who].y + picy;
							}
							char *str = Role_Temp;
							//下一个数据
							Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
							free(str);
						}
						else		//结束魔法
						{
							//不显示图片
							bShowMagicPic = false;
							//恢复原状态
							FightRole[kill_count].stop_temp=1;
							//使用魔法
							if(FightRole[kill_count].use==1)
							{
								//产生效果
								//使用者少魔法，接受者变化魔法设定的值
								m_fightmagic.MagicEffect((int)Command[kill_count].lAccessory,Hero[kill_count], Hero[Command[kill_count].kill_who]);
								//更新魔法槽数据
								FightRole[kill_count].MagicLong=Hero[kill_count]->Mp * 100 / Hero[kill_count]->MaxMp;
								//更新血槽数据
								FightRole[Command[kill_count].kill_who].BroodLong=Hero[Command[kill_count].kill_who]->Hp * 100 / Hero[Command[kill_count].kill_who]->MaxHp;
								//停止音乐
								sound_Magic.Stop();
								//要显示加血
								m_nLifeupCountLife = 3;
								m_nLifeupCountMove = 0;
								FightRole[Command[kill_count].kill_who].Lifeup = CMagic::MAGIC[Command[kill_count].lAccessory].Hp;
							}
							for(int j=i+1; j<3; j++)
								if(FightRole[j].use==1)		// 用来判断下一个角色是哪个角色
								{
									if(Command[j].lCommand == ATTACK)
									{
										char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
										Role_Move[j].Open(str);
										free(str);
										_FREE(Role_Temp);
										Role_Temp=Role_Move[j].ReadText("设置","continue");
									}
									else if(Command[j].lCommand == TOOLS)
									{
										char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
										Role_Tool[j].Open(str);
										free(str);
										_FREE(Role_Temp);
										Role_Temp=Role_Tool[j].ReadText("设置","continue");
									}
									else
									{
										_FREE(Role_Temp);
										Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("设置","continue");
										sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
										if(Command[j].Class==1)			//对自己人的
										{
											if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
											}
											else
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[Command[j].kill_who].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[Command[j].kill_who].y;
											}
										}
										else
										{
											if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
											}
											else
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[j].kill_who].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[j].kill_who].y;
											}
										}
									}
									kill_count=j;			//切换到下一个主角
									goto _MagicNext;
								}
								//没有下一个主角了就换敌人攻击
								Enemy_AI();
								main_count=ENEMY_KILL_ROLE;
								m_nRoundCount ++;			//增加回合计数
								Account_Attack();
						}
					}
					//不能使用的话，就保持常态
					else 
					{
						FightRole[kill_count].stop_temp = 1;
						bShowMagicPic = false;
						//不能使用的时候，显示提示
						GameMessage.DoMenu(lpDDSBack, "魔法不足，不能使用");
						for(int j=i+1; j<3; j++)
							if(FightRole[j].use==1)		// 用来判断下一个角色是哪个角色
							{
								if(Command[j].lCommand == ATTACK)
								{
									char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
									Role_Move[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Move[j].ReadText("设置","continue");
								}
								else if(Command[j].lCommand == TOOLS)
								{
									char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
									Role_Tool[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Tool[j].ReadText("设置","continue");
								}
								else
								{
									_FREE(Role_Temp);
									Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("设置","continue");
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
									if(Command[j].Class==1)			//对自己人的
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[Command[j].kill_who].y;
										}
									}
									else
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[j].kill_who].y;
										}
									}
								}
								kill_count=j;
								goto _MagicNext;
							}
							//没有下一个主角了就换敌人攻击
							Enemy_AI();
							main_count=ENEMY_KILL_ROLE;
							m_nRoundCount ++;			//增加回合计数
							Account_Attack();
					}
				}
				else			//对敌人的
				{
					if(Command[kill_count].kill_who!=Test_Enemy(Command[kill_count].kill_who))	// 测试要攻击的对象是否存在，如果不存在则换人
					{
						//重新计算
						Command[kill_count].kill_who=Test_Enemy(Command[kill_count].kill_who);
						Account_Attack();
						if(Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","相对"))
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","startx")+FightRole[kill_count].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","starty")+FightRole[kill_count].y;
						}
						else
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[kill_count].kill_who].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[kill_count].kill_who].y;
						}
					}
					int temp = Command[kill_count].lAccessory;
					//能够使用魔法
					//m_fightmagic.IsCanUse(temp)
					
					//显示图片
					bShowMagicPic = true;
					//进入使用状态
					FightRole[kill_count].stop_temp=10;
					Sleep(Role_Magic[temp].ReadInt("设置","延时"));
					
					//读取人物位图的数据
					RMove_Pic.left=Role_Magic[temp].ReadInt(Role_Temp,"left");
					RMove_Pic.top=Role_Magic[temp].ReadInt(Role_Temp,"top");
					RMove_Pic.right=Role_Magic[temp].ReadInt(Role_Temp,"right");
					RMove_Pic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"bottom");
					//读取魔法效果图片的数据
					fightmagic.RMagicPic.left = Role_Magic[temp].ReadInt(Role_Temp,"picleft");
					fightmagic.RMagicPic.top = Role_Magic[temp].ReadInt(Role_Temp,"pictop");
					fightmagic.RMagicPic.right=Role_Magic[temp].ReadInt(Role_Temp,"picright");
					fightmagic.RMagicPic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"picbottom");
					//循环动作
					//从开始攻击到敌人受伤到主角复位
					if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==1)	//开始
					{
						//播放效果声音
						char * soundtemp;
						soundtemp=Role_Magic[temp].ReadText(Role_Temp,"SoundFileName");
						if(soundtemp[0])
						{
							sound_Magic.Stop();
							sound_Magic.LoadSoundFX(hWnd,soundtemp);
						}
						sound_Magic.Play();
						//得到现在应该出现的位置
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
						{
							FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
							FightRole[kill_count].y1=FightRole[kill_count].y+roley;
						}
						else
						{
							FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+rolex;
							FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+roley;
						}
						int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
						int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
						if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
						{
							fightmagic.x = FightRole[kill_count].x + picx;
							fightmagic.y = FightRole[kill_count].y + picy;
						}
						else
						{
							fightmagic.x = EnemyRole[Command[kill_count].kill_who].x + picx;
							fightmagic.y = EnemyRole[Command[kill_count].kill_who].y + picy;
						}
						char *str = Role_Temp;
						//下一次的数据
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==2)	//移动
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
						{
							FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
							FightRole[kill_count].y1=FightRole[kill_count].y+roley;
						}
						else
						{
							FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+rolex;
							FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+roley;
						}
						int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
						int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
						if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
						{
							fightmagic.x = FightRole[kill_count].x + picx;
							fightmagic.y = FightRole[kill_count].y + picy;
						}
						else
						{
							fightmagic.x = EnemyRole[Command[kill_count].kill_who].x + picx;
							fightmagic.y = EnemyRole[Command[kill_count].kill_who].y + picy;
						}
						char *str = Role_Temp;
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==3)	//敌人受伤
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
						{
							FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
							FightRole[kill_count].y1=FightRole[kill_count].y+roley;
						}
						else
						{
							FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+rolex;
							FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+roley;
						}
						int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
						int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
						if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
						{
							fightmagic.x = FightRole[kill_count].x + picx;
							fightmagic.y = FightRole[kill_count].y + picy;
						}
						else
						{
							fightmagic.x = EnemyRole[Command[kill_count].kill_who].x + picx;
							fightmagic.y = EnemyRole[Command[kill_count].kill_who].y + picy;
						}
						char *str = Role_Temp;
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);
						
						//敌人动作变成受伤的动作
						EnemyRole[Command[kill_count].kill_who].stop_temp=11;
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==4)	//攻击
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
						{
							FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
							FightRole[kill_count].y1=FightRole[kill_count].y+roley;
						}
						else
						{
							FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+rolex;
							FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+roley;
						}
						int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
						int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
						if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
						{
							fightmagic.x = FightRole[kill_count].x + picx;
							fightmagic.y = FightRole[kill_count].y + picy;
						}
						else
						{
							fightmagic.x = EnemyRole[Command[kill_count].kill_who].x + picx;
							fightmagic.y = EnemyRole[Command[kill_count].kill_who].y + picy;
						}
						char *str = Role_Temp;
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);
						
						//敌人动作变成后退动作
						EnemyRole[Command[kill_count].kill_who].stop_temp=10;
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"对比")==5)	//敌人惨叫
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"人物相对"))
						{
							FightRole[kill_count].x1=FightRole[kill_count].x+rolex;
							FightRole[kill_count].y1=FightRole[kill_count].y+roley;
						}
						else
						{
							FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+rolex;
							FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+roley;
						}
						int picx = Role_Magic[temp].ReadInt(Role_Temp,"picx");
						int picy = Role_Magic[temp].ReadInt(Role_Temp,"picy");
						if(Role_Magic[temp].ReadInt(Role_Temp,"图片相对"))
						{
							fightmagic.x = FightRole[kill_count].x + picx;
							fightmagic.y = FightRole[kill_count].y + picy;
						}
						else
						{
							fightmagic.x = EnemyRole[Command[kill_count].kill_who].x + picx;
							fightmagic.y = EnemyRole[Command[kill_count].kill_who].y + picy;
						}
						char *str = Role_Temp;
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);

						//产生魔法效果
						//释放者少魔法值，承受者根据魔法决定
						MagicEffect(temp, Hero[kill_count],&EnemyRole[Command[kill_count].kill_who],Command[kill_count].kill_who);
						//敌人动作复位成受伤动作
						//---------
						//整个过程是
						//敌人先被攻击
						//然后后退
						//然后复位到被攻击的动作
						//-------------
						EnemyRole[Command[kill_count].kill_who].stop_temp=11;
						EnemyRole[Command[kill_count].kill_who].sound_Defend.Play();	//叫喊声
					}
					else
					{
						//主角回到原位置，不显示魔法图片
						bShowMagicPic = false;
						FightRole[kill_count].stop_temp=1;
						
						//如果胜利了，就直接跳出进攻部分
						if( Account_Loss()==1 ) 
							goto _MagicNext;
						//计算攻击力
						Account_Attack();
						//更新魔法槽数据
						FightRole[kill_count].MagicLong=Hero[kill_count]->Mp * 100 / Hero[kill_count]->MaxMp;
						//停止音乐
						sound_Magic.Stop();
						for(int j=i+1; j<3; j++)
							if(FightRole[j].use==1)		// 用来判断下一个角色是哪个角色
							{
								if(Command[j].lCommand == ATTACK)
								{
									char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
									Role_Move[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Move[j].ReadText("设置","continue");
								}
								else if(Command[j].lCommand == TOOLS)
								{
									char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
									Role_Tool[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Tool[j].ReadText("设置","continue");
								}
								else
								{
									_FREE(Role_Temp);
									Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("设置","continue");
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
									if(Command[j].Class==1)			//对自己人的
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[Command[j].kill_who].y;
										}
									}
									else
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[j].kill_who].y;
										}
									}
								}
								kill_count=j;
								goto _MagicNext;
							}
							//没有下个主角了，就换敌人攻击
							Enemy_AI();			//为敌人攻击做好准备
							main_count=ENEMY_KILL_ROLE;		//进入敌人进攻部分
							m_nRoundCount ++;			//增加回合计数
							Account_Attack();
					}
				}
			}
		}
			//显示敌人
_MagicNext:;
			for(i=0;i<4;i++)
			{
				if(EnemyRole[i].use==1)
				{
					if(EnemyRole[i].stop_temp==1)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[0],true);
					else if(EnemyRole[i].stop_temp==2)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[1],true);
					else if(EnemyRole[i].stop_temp==3)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[2],true);
					else if(EnemyRole[i].stop_temp==4)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[3],true);
					else if(EnemyRole[i].stop_temp==10)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rbruise,true);
					else if(EnemyRole[i].stop_temp==11)
						Blt(lpDDSBack,EnemyRole[i].x-5,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rbruise,true);
				}
			}
			//敌人要是被攻击就开始减血显示记数
			for(i=0;i<4;i++)
			{
				if(EnemyRole[i].use==1)
				{
					if(EnemyRole[i].stop_temp==11)
					{
						m_nAttackCountLife = 10;
					}
					EnemyRole[i].ready = 1;//敌人没有进攻
				}
			}
			//显示主角
			for(i=0;i<3;i++)
			{
				if(FightRole[i].use==1)
				{
					if(FightRole[i].stop_temp==1)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[0],true);
					else if(FightRole[i].stop_temp==2)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[1],true);
					else if(FightRole[i].stop_temp==3)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[2],true);
					else if(FightRole[i].stop_temp==4)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[3],true);
					else if(FightRole[i].stop_temp==10)
						Blt(lpDDSBack,FightRole[i].x1,FightRole[i].y1,FightRole[i].surf,RMove_Pic,true);

					FightRole[i].ready = 0;	//主角是进攻状态
				}
			}
			//显示魔法的图片
			if(bShowMagicPic)
				Blt(lpDDSBack,fightmagic.x,fightmagic.y,
						CMagic::MAGIC[Command[kill_count].lAccessory].EffectPic,
						fightmagic.RMagicPic,true);
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 敌人进行攻击的动作-------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case ENEMY_KILL_ROLE:			//和ROLE_KILL_ENEMY基本相同
		{
			for(i=0; i<4; i++)
			if(enemy_count==i && EnemyRole[enemy_count].use==1 )
			{
				//测试主角是否存在，不在就换人
				if(Enemy_Command[enemy_count].kill_who!=Test_Role(Enemy_Command[enemy_count].kill_who))	// 测试要攻击的对象是否存在，如果不存在则换人
				{
					Enemy_Command[enemy_count].kill_who=Test_Role(Enemy_Command[enemy_count].kill_who);
					Account_Attack();
				}
				//进攻动作
				EnemyRole[enemy_count].stop_temp=10;
				Sleep(Enemy_Move[i].ReadInt("设置","延时"));

				//读取位图的大小数据
				RMove_Pic.left=Enemy_Move[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Enemy_Move[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Enemy_Move[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Enemy_Move[i].ReadInt(Role_Temp,"bottom");

				if(Enemy_Move[i].ReadInt(Role_Temp,"对比")==1)
				{
					//位置数据
					EnemyRole[enemy_count].x1=EnemyRole[enemy_count].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=EnemyRole[enemy_count].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//下个数据的位置
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"对比")==2)	//移动
				{
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"对比")==3)	//主角受伤
				{
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//主角动作切换过程和ROLE_KILL_ENEMY中的敌人动作切换相同
					FightRole[Enemy_Command[enemy_count].kill_who].stop_temp=11;
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"对比")==4)	//攻击
				{
					EnemyRole[enemy_count].sound_Fight.Play();
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					FightRole[Enemy_Command[enemy_count].kill_who].stop_temp=10;
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"对比")==5)	//主角惨叫
				{
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					FightRole[Enemy_Command[enemy_count].kill_who].stop_temp=11;
					FightRole[Enemy_Command[enemy_count].kill_who].sound_Defend.Play();
				}
				else
				{
					//敌人复位
					EnemyRole[enemy_count].stop_temp=1;
					//主角全部死亡，跳出敌人攻击循环
					if( Account_Loss()== 2 ) 
						goto _Next_Enemy;
					//计算攻击力
					Account_Attack();

					for(int j=i+1; j<4; j++)
					if(EnemyRole[j].use==1)			//判断下一个敌人
					{
						char* str = EnemyInit.ReadText(EnemyRole[j].name,"Move_Ini");
						Enemy_Move[j].Open(str);
						free(str);
						_FREE(Role_Temp);
						Role_Temp=Enemy_Move[j].ReadText("设置","continue");
						enemy_count=j;				//轮到下个敌人攻击
						goto _Next_Enemy;
					}
					//敌人循环完成，进入主循环
					main_count=MAIN;
					//command_count置成排到最前面的主角
					command_count=Test_Role(0);
				}
_Next_Enemy:;
			}
			//显示主角
			for(i=0;i<3;i++)
			{
				if(FightRole[i].use==1)
				{
					if(FightRole[i].stop_temp==1)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[0],true);
					else if(FightRole[i].stop_temp==2)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[1],true);
					else if(FightRole[i].stop_temp==3)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[2],true);
					else if(FightRole[i].stop_temp==4)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[3],true);
					else if(FightRole[i].stop_temp==10)
						Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rbruise,true);
					else if(FightRole[i].stop_temp==11)
						Blt(lpDDSBack,FightRole[i].x+5,FightRole[i].y,FightRole[i].surf,FightRole[i].Rbruise,true);
				}
			}
			//开始主角减血记数
			for(i=0;i<3;i++)
			{
				if(FightRole[i].use==1)
				{
					if(FightRole[i].stop_temp==11)
					{
						m_nAttackCountLife = 10;
					}
					FightRole[i].ready = 1;		//主角不是进攻状态
				}
			}
			//显示敌人
			for(i=0;i<4;i++)
			{
				if(EnemyRole[i].use==1)
				{
					if(EnemyRole[i].stop_temp==1)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[0],true);
					else if(EnemyRole[i].stop_temp==2)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[1],true);
					else if(EnemyRole[i].stop_temp==3)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[2],true);
					else if(EnemyRole[i].stop_temp==4)
						Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[3],true);
					else if(EnemyRole[i].stop_temp==10)
						Blt(lpDDSBack,EnemyRole[i].x1,EnemyRole[i].y1,EnemyRole[i].surf,RMove_Pic,true);
					//该敌人是进攻状态
					EnemyRole[i].ready = 0;
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 战斗结束-----------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case OVER:
		{
			if(temp_swap==1)		//逃跑成功
			{
				Sleep(1000);
				temp_swap=0;
				ret=1;
			}
			else if(temp_swap==2)	//胜利
			{
				temp_swap=0;
				ret=2;
			}
			else if(temp_swap==3)	//失败
			{
				temp_swap=0;
				ret=3;
			}
			else if( temp_swap==4 )		//逃跑失败
			{
				Sleep(800);
				Enemy_AI();				//换敌人攻击
				main_count=ENEMY_KILL_ROLE;
				Account_Attack();
				ret=4;
			}
			else
			{
				Clear();
				return ret;
			}
		}
		break;
	}

	//-------------------------------------------------------------------
	// 显示部分
	//-------------------------------------------------------------------
	//显示敌人的HP

	//if( bShowFps )
	{
		if(Enemy[0].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20,"敌人上：%d",Enemy[0].Hp);
		if(Enemy[1].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size,"敌人右：%d",Enemy[1].Hp);
		if(Enemy[2].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*2,"敌人左：%d",Enemy[2].Hp);
		if(Enemy[3].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*3,"敌人下：%d",Enemy[3].Hp);
		FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*4,"回合计数：%d",m_nRoundCount);
	}

	//背景遮挡贴图图象显示
	RECT RBuild={0, 0, 640, 239};
	Blt(lpDDSBack, 0, 260, background_build[0], RBuild, true);

	//透明处理
	Show_Role_Enemy_Alpha();

	//显示魔法的图片
	if(bShowMagicPic)
		ABlt(lpDDSBack,fightmagic.x,fightmagic.y,
					CMagic::MAGIC[Command[kill_count].lAccessory].EffectPic,
					fightmagic.RMagicPic,ColorKey16,16);

	//人物头像与状态的显示部分
	if(FightRole[0].status==1)
	{
		// HP血槽的长短，暂定
		Bar(lpDDSBack, 58, 434, 158, 439, COLOR_BROOD_NULL);
		FightRole[0].BroodLong = Hero[0]->Hp*100/Hero[0]->MaxHp;
		Bar(lpDDSBack, 58, 434, 58+FightRole[0].BroodLong, 439, COLOR_BROOD);
		// MP血槽的长短，暂定
		Bar(lpDDSBack, 58, 450, 158, 455, COLOR_MAGIC_NULL);
		FightRole[0].MagicLong = Hero[0]->Mp*100/Hero[0]->MaxMp;
		Bar(lpDDSBack, 58, 450, 58+FightRole[0].MagicLong, 455, COLOR_MAGIC);
		
		Blt(lpDDSBack,5,354,status_pic,Rstatus_pic,true);
		Blt(lpDDSBack,11,360,FightRole[0].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(lpDDSBack, 5+100, 354+31, "%d", Hero[0]->Ip);
		font.PrintText(lpDDSBack, 5+100, 354+50, "%d", Hero[0]->Bp);
		font.PrintText(lpDDSBack, 5+125, 434-11, "%d",Hero[0]->Hp);
		font.PrintText(lpDDSBack, 5+125, 450-11, "%d",Hero[0]->Mp);
	}
	if(FightRole[1].status==1)
	{
		// HP血槽的长短，暂定
		Bar(lpDDSBack, 236, 434, 336, 439, COLOR_BROOD_NULL);
		FightRole[1].BroodLong = Hero[1]->Hp*100/Hero[1]->MaxHp;
		Bar(lpDDSBack, 236, 434, 236+FightRole[1].BroodLong, 439, COLOR_BROOD);
		// MP血槽的长短，暂定
		Bar(lpDDSBack, 236, 450, 336, 455, COLOR_MAGIC_NULL);
		FightRole[1].MagicLong = Hero[1]->Mp*100/Hero[1]->MaxMp;
		Bar(lpDDSBack, 236, 450, 236+FightRole[1].MagicLong, 455, COLOR_MAGIC);

		Blt(lpDDSBack,183,354,status_pic,Rstatus_pic,true);
		Blt(lpDDSBack,189,360,FightRole[1].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(lpDDSBack, 183+100, 354+31, "%d", Hero[1]->Ip);
		font.PrintText(lpDDSBack, 183+100, 354+50, "%d", Hero[1]->Bp);
		font.PrintText(lpDDSBack, 183+125, 434-11, "%d",Hero[1]->Hp);
		font.PrintText(lpDDSBack, 183+125, 450-11, "%d",Hero[1]->Mp);
	}
	if(FightRole[2].status==1)
	{
		// HP血槽的长短，暂定
		Bar(lpDDSBack, 416, 434, 516, 439, COLOR_BROOD_NULL);
		FightRole[2].MagicLong = Hero[2]->Mp*100/Hero[2]->MaxMp;
		Bar(lpDDSBack, 416, 434, 416+FightRole[2].BroodLong, 439, COLOR_BROOD);	
		// MP血槽的长短，暂定
		Bar(lpDDSBack, 416, 450, 516, 455, COLOR_MAGIC_NULL);
		FightRole[2].MagicLong = Hero[2]->Mp*100/Hero[2]->MaxMp;
		Bar(lpDDSBack, 416, 450, 416+FightRole[2].MagicLong, 455, COLOR_MAGIC);
		
		Blt(lpDDSBack,363,354,status_pic,Rstatus_pic,true);
		Blt(lpDDSBack,369,360,FightRole[2].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(lpDDSBack, 363+100, 354+31, "%d", Hero[2]->Ip);
		font.PrintText(lpDDSBack, 363+100, 354+50, "%d", Hero[2]->Bp);
		font.PrintText(lpDDSBack, 363+125, 434-11, "%d",Hero[2]->Hp);
		font.PrintText(lpDDSBack, 363+125, 450-11, "%d",Hero[2]->Mp);
	}
	font.font_Color=RGB(255,255,255);

	// 各个状态的显示
	switch( main_count )
	{
	case MAIN:
		{
			// 菜单选项的显示
			int bMouse = 0;
			if(!bButton[0]&&!bButton[1]&&!bButton[2]&&!bButton[3]&&!bButton[4])
			{
				for(i=0; i<5; i++)
				{
					switch( Button[i].CheckLoop(lpDDSBack) )
					{
					case 1:	// 点击
						Sound_Click.Play();	//音效
						bButton[i]=1;
						break;
						
					case 3:			//按住不放
						bMouse=1;
						break;
						
					case 2:	// 移动在上面
						if( old != i )		//上次不是这个按纽
						{
							Sound_High.Play();	//音效
							old=i;			//保存
						}
						bButton[i]=0;		//清0
						bMouse = 1;
						// 操作说明文字的显示
						switch(i)
						{
						case 0:
							font.PrintText(lpDDSBack, 495, 450, "重复上次的操作(R)");
							break;
						case 1:
							font.PrintText(lpDDSBack, 510, 450, "普通攻击(A)");
							break;
						case 2:
							font.PrintText(lpDDSBack, 510, 450, "使用魔法(E)");
							break;
						case 3:
							font.PrintText(lpDDSBack, 510, 450, "使用道具(G)");
							break;
						case 4:
							font.PrintText(lpDDSBack, 530, 450, "逃跑(Q)");
							break;
						default:
							break;
						}
						break;
						default:
							break;
					}
				}
			
				if(!bMouse)
				{
					switch(oldjoy)
					{
					case 0:
						font.PrintText(lpDDSBack, 495, 450, "重复上次的操作(R)");
						break;
					case 1:
						font.PrintText(lpDDSBack, 510, 450, "普通攻击(A)");
						break;
					case 2:
						font.PrintText(lpDDSBack, 510, 450, "使用魔法(E)");
						break;
					case 3:
						font.PrintText(lpDDSBack, 510, 450, "使用道具(G)");
						break;
					case 4:
						font.PrintText(lpDDSBack, 530, 450, "逃跑(Q)");
						break;
					default:
						break;
					}
				}
			}
			if( bButton[0] )	//重复
			{
				bButton[0] = 0;	//清0
				//转入攻击状态
				//主角的攻击指令和上次的一样
				main_count=ROLE_KILL_ENEMY;
				for(i=0; i<3; i++)
					if(FightRole[i].use==1)
					{
						if(Command[i].lCommand == ATTACK)
						{
							char* str = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Move_Ini");
							Role_Move[i].Open(str);
							free(str);
							_FREE(Role_Temp);
							Role_Temp=Role_Move[i].ReadText("设置","continue");
						}
						else if(Command[i].lCommand == TOOLS)
						{
							for(int j=i;j<3;j++)
								if(FightRole[j].use ==1)
								{
									if(!m_fightgoods.IsCanUse(Command[j].lAccessory))
									{
										GameMessage.DoMenu(lpDDSBack,"该物品不能使用");
										main_count = MAIN;
										return 0;
									}
								}
							char* str =  FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Tool_Ini");
							Role_Tool[i].Open(str);
							free(str);
							_FREE(Role_Temp);
							Role_Temp=Role_Tool[i].ReadText("设置","continue");
						}
						else
						{
							for(int j=i;j<3;j++)
								if(FightRole[j].use ==1)
								{
									if(!m_fightmagic.IsCanUse(Command[j].lAccessory,j))
									{
										GameMessage.DoMenu(lpDDSBack,"该魔法不能使用");
										main_count = MAIN;
										return 0;
									}
								}
							_FREE(Role_Temp);
							Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("设置","continue");
							if(Command[i].Class==1)			//对自己人的
							{
								if(Role_Magic[Command[i].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[i].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[i].y;
								}
								else
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[Command[i].kill_who].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[Command[i].kill_who].y;
								}
							}
							else
							{
								if(Role_Magic[Command[i].lAccessory].ReadInt("设置","相对"))	//根据魔法的对象决定魔法图片显示的初位置
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+FightRole[i].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+FightRole[i].y;
								}
								else
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("设置","startx")+EnemyRole[Command[i].kill_who].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("设置","starty")+EnemyRole[Command[i].kill_who].y;
								}
							}
						}
						kill_count=i;
						break;
					}
				//计算攻击力
				Account_Attack();
			}
			else if( bButton[1] )	//攻击
			{
				bButton[1] = 0;
				if( GetEnemyNum()==1 )	//只有一个敌人时
				{
					Command[command_count].lCommand = ATTACK;
					Command[command_count].kill_who=GetLastEnemyNum();
					main_count=MAIN;
					command_count++;
					if( command_count > GetLastRoleNum() )	//开始攻击
					{
						main_count=ROLE_KILL_ENEMY;
						for(i=0; i<3; i++)
							if(FightRole[i].use==1)
							{
								_FREE(Role_Temp);
								Role_Temp=Role_Move[i].ReadText("设置","continue");
								kill_count=i;
								break;
							}
						Account_Attack();
					}
				}
				else
				{
					//不止一个敌人
					//进入选择敌人状态
					Command[command_count].player=1;
					nNonce = 0;
					main_count=KILL_WHO;
				}
			}
			else if( bButton[2] )	//魔法
			{
				bButton[2] = 0;
				main_count = USER_MAGIC;
			}
			else if( bButton[3] )	//道具
			{
				bButton[3] = 0;
				//进入选择物品状态
				main_count = USER_GOODS;
			}
			else if( bButton[4] )	//逃跑
			{
				bButton[4] = 0;
				if( bLeave )		//可以逃跑
				{
					if( random(100) > 50 )
					{
						font.PrintText(lpDDSBack, 300, 220, "逃跑成功！");
						main_count=OVER;
						temp_swap=1;
					}
					else
					{
						font.PrintText(lpDDSBack, 300, 220, "逃跑失败！");
						main_count=OVER;
						temp_swap=4;
					}
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// 选择物品
	/////////////////////////////////////////////////////////////////////
	case USER_GOODS:
		{
			int ret = m_fightgoods.DoMenu(lpDDSBack, true);
			if( ret== 1)
			{
				//选好了一个物品
				//进入选择使用对象模式
				main_count = USER_WHO;
				nNonce = 0;
			}
			else if(ret == 2)//没有选择物品
				main_count = MAIN;
		}
		break;

	case USER_MAGIC:
		{
			int ret = m_fightmagic.DoMenu(lpDDSBack, true,command_count);
			if( ret== 1)
			{
				//选好了一个魔法，根据魔法的类型
				//进入不同的选择模式
				bUseMagic = true;
				if(CMagic::MAGIC[m_fightmagic.GetMagicID()].Class==0)
					main_count = USER_WHO;
				else
					main_count = KILL_WHO;
				nNonce = 0;
				//能不能使用这个魔法
				if(!m_fightmagic.IsCanUse(m_fightmagic.GetMagicID(),command_count))
				{
					GameMessage.DoMenu(lpDDSBack, "魔法不足，不能使用");
					main_count = MAIN;		//回到选择模式
					bUseMagic = false;
				}
			}
			else if(ret == 2)//没有选择物品
				main_count = MAIN;
		}
		break;
	}
	//显示少血
	Print_Attack();
	//显示加血
	Print_Lifeup();
	return 0;
}

// 计算攻击力
void CFight::Account_Attack(void)
{
	if(main_count==ROLE_KILL_ENEMY)				//主角攻击部分，按照攻击者的攻击力和被攻击
	{											//者的防御力进行一定的计算，其中加入了移动
		int who=Command[kill_count].kill_who;	//因素。who是被攻击者，计算出来的temp将是
		int temp;								//被攻击者减少的HP。

		if(kill_count==0)
		{
			temp=Hero[0]->Attack*(1+Hero[0]->Bp)/100-Enemy[who].Defend;
			if( temp < 500)
			{
				temp=(random(100)+400) * ( 200- Enemy[who].Bp ) / 100;
			}
			else
				temp=(temp+random(temp/5))*(200-Enemy[who].Bp) / 100;
			EnemyRole[who].UnAttack=temp;
		}
		else if(kill_count==1)
		{
			temp=Hero[1]->Attack*(1+Hero[1]->Bp)/100-Enemy[who].Defend;
			if( temp < 500 )
			{
				temp=(random(100)+400) * ( 200- Enemy[who].Bp ) / 100;
			}
			else
				temp=(temp+random(temp/5))*(200-Enemy[who].Bp) / 100;
			EnemyRole[who].UnAttack=temp;
		}
		else if(kill_count==2)
		{
			temp=Hero[2]->Attack*(1+Hero[2]->Bp)/100-Enemy[who].Defend;
			if( temp < 500 )
			{
				temp=(random(100)+400) * ( 200- Enemy[who].Bp ) / 100;
			}
			else
				temp=(temp+random(temp/5))*(200-Enemy[who].Bp) / 100;
			EnemyRole[who].UnAttack=temp;
		}
	}

	else if(main_count==ENEMY_KILL_ROLE)		//敌人攻击
	{
		int who=Enemy_Command[enemy_count].kill_who;
		int temp;

		if( who==0 )
		{
			temp=Enemy[enemy_count].Attack*(1+Enemy[enemy_count].Bp)/100 - Hero[0]->Defend;
			if( temp < 100)
			{
				temp =(40+ random(60))*(200-Hero[0]->Bp)/100 ;//+ Enemy[enemy_count].Attack/100;
			}
			else
				temp = (temp + random(temp/10))*(200-Hero[0]->Bp)/100;
			FightRole[who].UnAttack=temp;
		}
		else if( who==1 )
		{
			temp=Enemy[enemy_count].Attack*(1+Enemy[enemy_count].Bp)/100 - Hero[1]->Defend;
			if( temp < 100 )
			{
				temp = (40 + random(60))*(200-Hero[0]->Bp)/100;// + Enemy[enemy_count].Attack/100;
			}
			else
				temp = (temp + random(temp/10))*(200-Hero[0]->Bp)/100;
			FightRole[who].UnAttack=temp;
		}
		else if( who==2 )
		{
			temp=Enemy[enemy_count].Attack*(1+Enemy[enemy_count].Bp)/100 - Hero[2]->Defend;
			if( temp < 100 )
			{
				temp = (40+random(60))*(200-Hero[0]->Bp)/100 ;//+ Enemy[enemy_count].Attack/100;
			}
			else
				temp = temp + random(temp/10)*(200-Hero[0]->Bp)/100;
			FightRole[who].UnAttack=temp;
		}
	}

	m_nAttackCountMove = 0;
	m_nAttackCountLife = 0;
}

// 参数，被攻击的敌人，返回确定后的敌人
int CFight::Test_Enemy(int a)			//如果选择的敌人存在，就返回那个敌人的位置
{
	if(EnemyRole[a].use==0)				//如果不存在，就递归查找下一个敌人。
	{
		if(a<3)							//最多4个敌人
			return Test_Enemy(a+1);
		else 
			return 0;					//仍然不存在，就返回0
	}
	else
		return a;
}

// 参数，被攻击的主角，返回确定后的主角
int CFight::Test_Role(int a)			//和上一个函数一样的算法
{
	if(FightRole[a].use==0)
	{
		if(a<2)
			return Test_Role(a+1);
		else 
			return 0;
	}
	else
		return a;
}

// 计算损失以及人物死亡的显示
int CFight::Account_Loss(void)
{
	//敌人减血
	if(main_count==ROLE_KILL_ENEMY)
	{
		//当前被攻击的敌人少血
		//kill_count是当前出手的主角
		Enemy[Command[kill_count].kill_who].Hp-=EnemyRole[Command[kill_count].kill_who].UnAttack;
		//敌人被打死了
		if(Enemy[Command[kill_count].kill_who].Hp<=0)
		{
			EnemyRole[Command[kill_count].kill_who].sound_Dead.Play();	//死亡声
			Enemy[Command[kill_count].kill_who].Hp=0;
			EnemyRole[Command[kill_count].kill_who].use=0;
		}
		
		//胜利
		//杀死了所有的敌人
		if( EnemyRole[0].use==0 && EnemyRole[1].use==0 && EnemyRole[2].use==0 && EnemyRole[3].use==0 )
		{
			UpdateLevel();		//加分升级
			main_count=OVER;
			temp_swap=2;		//胜利
			return 1;
		}
	}

	//我方减血
	else if(main_count==ENEMY_KILL_ROLE)
	{
		for( int i=0; i<3; i++)
		if( Enemy_Command[enemy_count].kill_who==i )
		{
			Hero[i]->Hp-=FightRole[i].UnAttack;
			if(Hero[i]->Hp<=0)
			{
				FightRole[i].sound_Dead.Play();	//死亡声
				FightRole[i].use=0;
				Hero[i]->Hp=0;
			}
			//更新血槽
			FightRole[i].BroodLong=Hero[i]->Hp * 100 / Hero[i]->MaxHp;
			if(m_bBoss)
			{
				if(m_nMagicCount)
				{
					Hero[i]->Mp =5;
					m_nMagicCount =0;
				}
				else
				{
					Hero[i]->Mp -= FightRole[i].UnAttack/3;
					if(Hero[i]->Mp<0)
						Hero[i]->Mp =0;
				}
			}
		}
		//失败
		//全部的主角都死了
		if(FightRole[0].use==0 && FightRole[1].use==0 && FightRole[2].use==0 )
		{
			main_count=OVER;
			temp_swap=3;		//失败
			return 2;
		}
	}
	return 0;
}

//显示攻击
void CFight::Print_Attack(void)
{
	m_nAttackCountMove ++;			//显示数值向上移动
	if(m_nAttackCountLife > 0)		//仍然需要显示
	{
		if(main_count==ROLE_KILL_ENEMY)		//显示敌人少血
		{
			DWORD color = font.GetColor();
			font.SetColor(RGB(255,0,0));
			font.PrintText(lpDDSBack, EnemyRole[Command[kill_count].kill_who].x+20,
				EnemyRole[Command[kill_count].kill_who].y-20-m_nAttackCountMove,"- %d",EnemyRole[Command[kill_count].kill_who].UnAttack);
			font.SetColor(color);
		}
		else if(main_count==ENEMY_KILL_ROLE)
		{
			DWORD color = font.GetColor();
			font.SetColor(RGB(255,0,0));
			font.PrintText(lpDDSBack, FightRole[Enemy_Command[enemy_count].kill_who].x+17,
				FightRole[Enemy_Command[enemy_count].kill_who].y-20-m_nAttackCountMove,"- %d",FightRole[Enemy_Command[enemy_count].kill_who].UnAttack);
			font.SetColor(color);
		}
		m_nAttackCountLife --;		//显示时间减少
	}
}

//敌人攻击AI
void CFight::Enemy_AI(void)
{
	char* str=NULL;

	// 敌人进攻的设定
	//先读移动数据，然后用个临时的变量保存下一个动作的数据所在文件里的位置
	if(EnemyRole[0].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[0].name,"Move_Ini");
		Enemy_Move[0].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[0].ReadText("设置","continue");
		enemy_count=0;
	}
	else if(EnemyRole[1].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[1].name,"Move_Ini");
		Enemy_Move[1].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[1].ReadText("设置","continue");
		enemy_count=1;
	}
	else if(EnemyRole[2].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[2].name,"Move_Ini");
		Enemy_Move[2].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[2].ReadText("设置","continue");
		enemy_count=2;
	}
	else if(EnemyRole[3].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[3].name,"Move_Ini");
		Enemy_Move[3].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[3].ReadText("设置","continue");
		enemy_count=3;
	}
	//全是攻击
	Enemy_Command[0].player=1;
	Enemy_Command[1].player=1;
	Enemy_Command[2].player=1;
	Enemy_Command[3].player=1;

	int i,j;
	//选择攻击哪个角色
	Enemy_Command[0].kill_who=random(3);
	Enemy_Command[1].kill_who=random(3);
	Enemy_Command[2].kill_who=random(3);
	Enemy_Command[3].kill_who=random(3);
	/*for( j=0;j<10;j++)	// 进行多次的循环取随机数，确保不会出现攻击错误
	for( i=0;i<4;i++)
	{
		if(FightRole[Enemy_Command[i].kill_who].use==0)			//该位置没有人就换个人
			Enemy_Command[i].kill_who=random(3);
	}*/
	for(i=0;i<4;i++)
		for(j=0;j<3;j++)
		{
			if(FightRole[Enemy_Command[i].kill_who].use==0)
			{
				Enemy_Command[i].kill_who = Test_Role(j);
			}
		}
	_FREE(str);
	m_nAttackCountMove =0;
}

// 定时器函数 更新动画和声音
void CFight::OnTimer(void)
{
	int i;
	for(i=0;i<3;i++)						//循环动作序列
	{
		if(FightRole[i].stop_temp==1)
			FightRole[i].stop_temp=2;
		else if(FightRole[i].stop_temp==2)
			FightRole[i].stop_temp=3;
		else if(FightRole[i].stop_temp==3)
			FightRole[i].stop_temp=4;
		else if(FightRole[i].stop_temp==4)
			FightRole[i].stop_temp=1;
		else if(FightRole[i].stop_temp==10)	//处理被攻击后的恢复工作
			FightRole[i].stop_temp=1;
		else if(FightRole[i].stop_temp==11)
			FightRole[i].stop_temp=1;
	}

	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].stop_temp==1)
			EnemyRole[i].stop_temp=2;
		else if(EnemyRole[i].stop_temp==2)
			EnemyRole[i].stop_temp=3;
		else if(EnemyRole[i].stop_temp==3)
			EnemyRole[i].stop_temp=4;
		else if(EnemyRole[i].stop_temp==4)
			EnemyRole[i].stop_temp=1;
		else if(EnemyRole[i].stop_temp==10)		// 处理被攻击后的恢复工作
			EnemyRole[i].stop_temp=1;
		else if(EnemyRole[i].stop_temp==11)		// 处理被攻击后的恢复工作
			EnemyRole[i].stop_temp=1;
	}

	if( CMusic::bPlay && !FMusic.IsPlay() )		//音乐停止了，就再次播放
	{
		FMusic.Play();
	}
}

// 加分升级
void CFight::UpdateLevel()
{
	int Exp=0;
	for(int i=0; i<4; i++)
	{
		if( EnemyRole[i].bLive )		//得到所有敌人带的经验值的总和
			Exp+=Enemy[i].Exp;
	}

	char x[256];
	sprintf(x, "战斗胜利，获得%d点经验值！", Exp);
	GameMessage.DoMenu(lpDDSBack, x);	//显示胜利	

	for(i=0; i<3; i++)
	{
		if( FightRole[i].bLive )
		{
			Hero[i]->Exp+=Exp;				//角色的经验值增加
			int nTemp = Hero[i]->Level;		//当前级数，用来判断是否升级
			while( Hero[i]->Exp >= Hero[i]->MaxExp )	//大于升级所需要的经验值就升级
			{											//因为可能一次升几级，所以循环
				Hero[i]->Level++;						
				//算下升一级所需要的经验值
				Hero[i]->MaxExp += 10*(Hero[i]->Level+1)*(Hero[i]->Level)+30;
				Hero[i]->MaxHp+=200;						//HP加20
				Hero[i]->Hp=Hero[i]->MaxHp;				//把HP加满
				Hero[i]->MaxMp+=60;
				Hero[i]->Mp=Hero[i]->MaxMp;

				role[i].oldAttack+=80;
				role[i].oldDefend+=40;
				Hero[i]->Magic+=3;
				
				Hero[i]->Stamina+=3;
				Hero[i]->Luck+=3;
				Hero[i]->Speed+=3;
				Hero[i]->Bp=100;
				Hero[i]->Ip=100;
				role[i].ReCalcPro();
			}
			if(Hero[i]->Level>nTemp)		//显示
			{
				sprintf(x, "<c2>%s</c> 等级升为 <c2>%d</c> ！", Hero[i]->Fight_Ini_Name, Hero[i]->Level );
				//FadeIn(2, 20, RGB16(0,255,255));
				GameMessage.DoMenu(lpDDSBack, x);
			}
		}
	}
}

//敌人数量
int CFight::GetEnemyNum()
{
	int count=0;
	for(int i=0; i<4; i++)
	{
		if( EnemyRole[i].use )			//得到敌人的数目
			count++;
	}
	return count;
}

//最后一个敌人编号
int CFight::GetLastEnemyNum()
{
	int count=0;
	for(int i=0; i<4; i++)
	{
		if( EnemyRole[i].use )
			count=i;
	}
	return count;
}

//最后一个我方编号
int CFight::GetLastRoleNum()
{
	int count=0;
	for(int i=0; i<3; i++)
	{
		if( FightRole[i].use )
			count=i;
	}
	return count;
}

//显示敌人和我方
void CFight::Show_Role_Enemy()
{
	static int c=100, n=4;	//计算显示轮廓的green数值，做成动画形式
	
	c+=n;
	if( c >= 250 )
	{
		n=-4;
	}
	if( c <= 128 )
	{
		n=4;
	}

	// 角色的显示
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)			//该位置有敌人
		{
			Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],true);

			// 显示轮廓
			if( main_count == USER_WHO && nNonce == i )
				Edge_Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ], ColorKey16, RGB16(0,c,0));
		}
	}

	//敌人显示
	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].use==1)
		{
			Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], true);

			// 显示轮廓
			if( main_count == KILL_WHO && nNonce == i )
				Edge_Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, RGB16(c,0,0));
		}
	}

}

//显示带ＡＬＰＨＡ的敌人和我方
void CFight::Show_Role_Enemy_Alpha()
{
	static int c=100, n=4;		//和上个函数一样的用处
	
	c+=n;
	if( c >= 250 )
	{
		n=-4;
	}
	if( c <= 128 )
	{
		n=4;
	}

	// 角色的显示
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)			//有角色
		{
			if(!FightRole[i].ready)		//进攻的时候，位置、图片的序列请参考LOOP函数
			{
				if(FightRole[i].stop_temp==10)
					ABlt(lpDDSBack,FightRole[i].x1,FightRole[i].y1,FightRole[i].surf,RMove_Pic,ColorKey16, 16);
				else
					ABlt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],ColorKey16, 16);
			}
			else						//未进攻的时候
			{
				if(FightRole[i].stop_temp == 10)
					ABlt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rbruise,ColorKey16, 16);
				else if(FightRole[i].stop_temp ==11)
					ABlt(lpDDSBack,FightRole[i].x+5,FightRole[i].y,FightRole[i].surf,FightRole[i].Rbruise,ColorKey16, 16);
				else
					ABlt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],ColorKey16, 16);
			}
		}
	}

	//敌人显示
	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].use==1)
		{
			if(EnemyRole[i].ready)
			{
				if(EnemyRole[i].stop_temp == 11)
					ABlt(lpDDSBack,EnemyRole[i].x-5,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rbruise, ColorKey16, 16);
				else if(EnemyRole[i].stop_temp == 10)
					ABlt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rbruise, ColorKey16, 16);
				else 
					ABlt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, 16);
			}
			else
			{
				if(EnemyRole[i].stop_temp == 10)
					ABlt(lpDDSBack,EnemyRole[i].x1,EnemyRole[i].y1,EnemyRole[i].surf,RMove_Pic, ColorKey16, 16);
				else 
					ABlt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, 16);
			}
		}
	}

}

void CFight::UseGoods()
{

}
//得到打斗时候的画面，没有鼠标和FPS
void CFight::GetFightGameSurface(LPDIRECTDRAWSURFACE surf)
{
	//画背景图片
	Blt(surf,0,0,background[Number-1],RectScreen,false);
	static int c=100, n=4;
	
	c+=n;
	if( c >= 250 )
	{
		n=-4;
	}
	if( c <= 128 )
	{
		n=4;
	}

	// 角色的显示
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)
		{
			Blt(surf,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],true);

			// 显示轮廓
			if( main_count == USER_WHO && nNonce == i )
				Edge_Blt(surf,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ], ColorKey16, RGB16(0,c,0));
		}
	}

	//敌人显示
	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].use==1)
		{
			Blt(surf,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], true);

			// 显示轮廓
			if( main_count == KILL_WHO && nNonce == i )
				Edge_Blt(surf,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, RGB16(c,0,0));
		}
	}

	
	//背景遮挡贴图图象显示
	RECT RBuild={0, 0, 640, 239};

	Blt(surf, 0, 260, background_build[0], RBuild, true);

	//人物头像与状态的显示部分
	RECT Rstatus_pic={0,0,170,120};
	RECT Rtemp={0,0,50,50};

	if(FightRole[0].status==1)
	{
		// HP血槽的长短，暂定
		Bar(surf, 58, 434, 158, 439, COLOR_BROOD_NULL);
		Bar(surf, 58, 434, 58+FightRole[0].BroodLong, 439, COLOR_BROOD);
		// MP血槽的长短，暂定
		Bar(surf, 58, 450, 158, 455, COLOR_MAGIC_NULL);
		Bar(surf, 58, 450, 58+FightRole[0].MagicLong, 455, COLOR_MAGIC);
		
		Blt(surf,5,354,status_pic,Rstatus_pic,true);
		Blt(surf,11,360,FightRole[0].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(surf, 5+100, 354+31, "%d", Hero[0]->Ip);
		font.PrintText(surf, 5+100, 354+50, "%d", Hero[0]->Bp);
		font.PrintText(surf, 5+125, 434-11, "%d",Hero[0]->Hp);
		font.PrintText(surf, 5+125, 450-11, "%d",Hero[0]->Mp);
	}
	if(FightRole[1].status==1)
	{
		// HP血槽的长短，暂定
		Bar(surf, 236, 434, 336, 439, COLOR_BROOD_NULL);
		Bar(surf, 236, 434, 236+FightRole[1].BroodLong, 439, COLOR_BROOD);
		// MP血槽的长短，暂定
		Bar(surf, 236, 450, 336, 455, COLOR_MAGIC_NULL);
		Bar(surf, 236, 450, 236+FightRole[1].MagicLong, 455, COLOR_MAGIC);

		Blt(surf,183,354,status_pic,Rstatus_pic,true);
		Blt(surf,189,360,FightRole[1].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(surf, 183+100, 354+31, "%d", Hero[1]->Ip);
		font.PrintText(surf, 183+100, 354+50, "%d", Hero[1]->Bp);
		font.PrintText(surf, 183+125, 434-11, "%d",Hero[1]->Hp);
		font.PrintText(surf, 183+125, 450-11, "%d",Hero[1]->Mp);
	}
	if(FightRole[2].status==1)
	{
		// HP血槽的长短，暂定
		Bar(surf, 416, 434, 516, 439, COLOR_BROOD_NULL);
		Bar(surf, 416, 434, 416+FightRole[2].BroodLong, 439, COLOR_BROOD);	
		// MP血槽的长短，暂定
		Bar(surf, 416, 450, 516, 455, COLOR_MAGIC_NULL);
		Bar(surf, 416, 450, 416+FightRole[2].MagicLong, 455, COLOR_MAGIC);
		
		Blt(surf,363,354,status_pic,Rstatus_pic,true);
		Blt(surf,369,360,FightRole[2].header,Rtemp,false);
		font.font_Color=RGB(0,0,0);
		font.PrintText(surf, 363+100, 354+31, "%d", Hero[2]->Ip);
		font.PrintText(surf, 363+100, 354+50, "%d", Hero[2]->Bp);
		font.PrintText(surf, 363+125, 434-11, "%d",Hero[2]->Hp);
		font.PrintText(surf, 363+125, 450-11, "%d",Hero[2]->Mp);
	}
	font.font_Color=RGB(255,255,255);
}

//暂停音乐
void CFight::PauseMusic()
{
	FMusic.Pause();
}
//继续音乐
void CFight::ResumeMusic()
{
	FMusic.Resume();
}
//是否在播放音乐
BOOL CFight::IsPlayingMusic()
{
	return FMusic.IsPlay();
}
//使用魔法
void CFight::MagicEffect(int Num, stRolePropertiy * Propertiy,stFightRole * enemy,int num)
{
	//类型要是对敌人使用的，然后魔法值要足够
	if(CMagic::MAGIC[Num].Class==MC_ENEMY&&CMagic::MAGIC[Num].Mp<=Propertiy->Mp)
	{
		if(CMagic::MAGIC[Num].Attack<0)	//这个表示是百分比，提升自己攻击力的
			enemy->UnAttack = (-CMagic::MAGIC[Num].Attack)*enemy->UnAttack/100;
		else
			enemy->UnAttack += CMagic::MAGIC[Num].Attack;
		if(!m_bBoss||!m_bHidden)		//不是对boss战，或者不是进入了隐藏模式
			//算上魔法抵抗
		{
			enemy->UnAttack = (int)(enemy->UnAttack*(100-Enemy[num].MagicDef)/100.0f);
			m_bHidden = true;
		}

		Propertiy->Mp-=CMagic::MAGIC[Num].Mp;
	}
}
// 显示加血
void CFight::Print_Lifeup()
{
	m_nLifeupCountMove ++;			//显示数值向上移动
	if(m_nLifeupCountLife > 0)		//仍然需要显示
	{
		//显示自己加血
		DWORD color = font.GetColor();
		font.SetColor(RGB(255,0,0));
		font.PrintText(lpDDSBack, FightRole[Command[kill_count].kill_who].x+20,
			FightRole[Command[kill_count].kill_who].y-20-m_nLifeupCountMove,"+ %d",FightRole[Command[kill_count].kill_who].Lifeup);
		font.SetColor(color);
		m_nLifeupCountLife --;		//显示时间减少
	}
}
// 是否是boss战
void CFight::SetBossFight(BOOL flags)
{
	m_bBoss = flags;
}
