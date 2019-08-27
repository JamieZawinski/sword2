//   ******************************
//   *      ս��ϵͳ�ຯ��
//   *     ������ 5��29��
//   ******************************
//  ************************************
//  *     ս��ϵͳ����ѭ�������ļ� 
//  *   ������  5��29��   
//  ************************************

#include "stdio.h"
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\menu\menu_inc.h"
#include "..\main.h"
#include "FightGoods.h"
#include "Fight.h"

//��ѭ����
int CFight::Loop(int mouse,POINT point)
{
	static ret=0;			//ս���ķ���ֵ
	static POINT old_point; //�ϴ�����λ��
	int i;
	static int old;			//�ϴ�ѡ��İ�Ŧ
	static int oldjoy=0;		//�ϴ�joyѡ�İ�ť
	static bool bUseMagic = false;  //�Ƿ�ʹ��ħ��
	static bool bShowMagicPic = false;//�Ƿ���ʾħ��ͼƬ
	bool bButton[5]={0,0,0,0,0};	//��ǰѡ��İ�Ŧ��־����

	RECT Rstatus_pic={0,0,170,120};	//����״̬ͼ�ľ���
	RECT Rtemp={0,0,50,50};			//ͷ��ͼ����

	//����ͼ����ʾ
	Blt(lpDDSBack,0,0,background[Number-1],RectScreen,false);

	//--------------------����Ϊ��������--------------------

	//-------------------------------------------------------------------
	// ���Ʋ���
	//-------------------------------------------------------------------
	switch( main_count )
	{
	/////////////////////////////////////////////////////////////////////
	// ������Ϸ�˵�ѡ��-------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case MAIN:
		{
			// ��ʾս������
			Show_Role_Enemy();
			if(mouse == RB_UP || B_UP )		// ȡ������
			{
				if(command_count > 0)
					command_count --;		//�����������
			}

			// ѡ�˹�����ʾ
			cursor_role.ShowAni(lpDDSBack, FightRole[command_count].x+30, FightRole[command_count].y-30);

			// ���̿��� ----------------------------------------
			switch(key)
			{
			case DIK_A:				// ����
				{
					bButton[1] = 1;
				}
				break;
			case DIK_E:				// ħ��
				{
					bButton[2] = 1;
				}
				break;
			case DIK_G:				// ����
				{
					bButton[3] = 1;
				}
				break;
			case DIK_Q:				// ����
				{
					bButton[4] = 1;
				}
				break;
			case DIK_R:				// �ظ�
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
			//�ֱ���3�ż�ȷ��
			if(joystick == DIJ_3)
				bButton[oldjoy] = 1;
			if(joystick == DIJ_4)	//4�ż�ѡ��
			{
				oldjoy++;
				if(oldjoy==5)
					oldjoy = 0;
			}
			// ----------------------------------------------
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ʹ����Ʒ��ѡ���ɫ
	/////////////////////////////////////////////////////////////////////
	case USER_WHO:
		{
			if(mouse == RB_UP || B_UP )		// ȡ������
				main_count = MAIN;			// �ص���ѭ��״̬

			nNonce = Test_Role(nNonce);

			if(old_point.x == point.x && old_point.y == point.y)	//���λ��û�б仯
			{
			}
			else
			{
				old_point.x = point.x;		//�������λ��
				old_point.y = point.y;
				//�õ������ָ�������
				//��Χ������15����70������15������65�ľ���
				for(i=0;i<3;i++)			
					if(point.x>FightRole[i].x+15&&point.x<FightRole[i].x+65
						&& point.y>FightRole[i].y+15&&point.y<FightRole[i].y+70 
						&& FightRole[i].use==1)
					{
						nNonce = i;				//���������
						break;
					}
			}

			// ���̿���
			if(DOWN_DOWN)					//���£�ֱ���ҵ�����Ϊֹ
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
			if(UP_DOWN)							//���ϣ�ֱ���ҵ�����Ϊֹ
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
			
			//��ʾ���Ǻ͵���
			//��������
			Show_Role_Enemy();
			//ѡ�����
			cursor_enemy.ShowAni(lpDDSBack, FightRole[nNonce].x+30, FightRole[nNonce].y-15);

			if( mouse == LB_UP || A_UP )	//ѡ������
			{
				i = nNonce;
				{
					Sound_Click.Play();	//��Ч
					if(!bUseMagic)
					{
						Command[command_count].lCommand = TOOLS;	//�ô���ʹ����Ʒ
						Command[command_count].lAccessory = m_fightgoods.GetGoodsID();//��Ʒ����
						Command[command_count].kill_who=i;			//ʹ�ö���
						Command[command_count].Class=1;				//���Լ���
						main_count=MAIN;	//������ѭ��
						command_count++;	//�������ָ��
					}
					else
					{
						Command[command_count].lCommand = MAGIC;	//�ô���ʹ��ħ��
						Command[command_count].lAccessory = m_fightmagic.GetMagicID();//ħ������
						Command[command_count].kill_who=i;			//ʹ�ö���
						Command[command_count].Class=1;
						main_count=MAIN;	//������ѭ��
						command_count++;	//�������ָ��
						bUseMagic = false;
					}
					if(command_count>GetLastRoleNum())	//���е����Ƕ��Ѿ�ʹ����
					{
						command_count=0;				//��0
						main_count = ROLE_KILL_ENEMY;	//�������ϣ���ʼ����
						for(i=0; i<3; i++)
							if(FightRole[i].use==1  )
							{
								_FREE(Role_Temp);
								//��һ������
								if(Command[i].lCommand == TOOLS)
									Role_Temp=Role_Tool[i].ReadText("����","continue");
								else if(Command[i].lCommand == MAGIC)	 //�ö��Լ��˵�ħ����
								{
									Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("����","continue");
									//����ͼƬ����ʼ����
									if(Role_Magic[Command[i].lAccessory].ReadInt("����","���"))
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[i].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[i].y;
									}
									else
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[Command[i].kill_who].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[Command[i].kill_who].y;
									}
									//����ħ������
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[i].lAccessory].SoundFileName);
								}
								else		//����
									Role_Temp=Role_Move[i].ReadText("����","continue");
								kill_count=0;			//�ӵ�һ�����ǿ�ʼ
								break;
							}
						Account_Attack();	//���㹥����
					}
					else if(FightRole[command_count].use==0)		//�ֵ���һ������
					{
						command_count++;
						if(FightRole[command_count].use==0)
							command_count=0;
						main_count = MAIN;		//��ѭ��
					}
					break;
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ѡ��Ҫ�����Ķ���--------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case KILL_WHO:
		{
			if(mouse == RB_UP || B_UP )		// ȡ������
				main_count = MAIN;
			
			nNonce = Test_Enemy(nNonce);
			// ������ʾ�����ж�
			//��USER_WHO���
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

			// ���̵Ŀ���
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
			
			//��ʾ���˺��ҷ�����
			Show_Role_Enemy();		
			cursor_enemy.ShowAni(lpDDSBack, EnemyRole[nNonce].x+24, EnemyRole[nNonce].y-15);

			//ѡ��
			if( mouse == LB_UP || A_UP )
			{
				i = nNonce;
				{
					Sound_Click.Play();	//��Ч
					if(!bUseMagic)
					{
						Command[command_count].lCommand = ATTACK;
						Command[command_count].kill_who=i;
						main_count=MAIN;
						command_count++;
					}
					else
					{
						Command[command_count].lCommand = MAGIC;	//�ô���ʹ��ħ��
						Command[command_count].lAccessory = m_fightmagic.GetMagicID();//ħ������
						Command[command_count].kill_who=i;			//ʹ�ö���
						Command[command_count].Class=0;
						main_count=MAIN;	//������ѭ��
						command_count++;	//�������ָ��
						bUseMagic = false;
					}
					if(command_count>GetLastRoleNum())
					{
						command_count=0;
						main_count = ROLE_KILL_ENEMY;	//�������ϣ���ʼ����
						for(i=0; i<3; i++)
							if(FightRole[i].use==1 )
							{
								_FREE(Role_Temp);
								if(Command[i].lCommand == TOOLS)
									Role_Temp=Role_Tool[i].ReadText("����","continue");
								else if(Command[i].lCommand == MAGIC)		//�Ե��˵�ħ��
								{
									Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("����","continue");
									if(Role_Magic[Command[i].lAccessory].ReadInt("����","���"))
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[i].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[i].y;
									}
									else
									{
										fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+EnemyRole[Command[i].kill_who].x;
										fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+EnemyRole[Command[i].kill_who].y;
									}
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[i].lAccessory].SoundFileName);
								}
								else 
									Role_Temp=Role_Move[i].ReadText("����","continue");
								kill_count=0;			//�ӵ�һ�����ǿ�ʼ
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
	// ���й����Ķ���------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case ROLE_KILL_ENEMY:
		{
			for(i=0; i<3; i++)		//�������ǣ�ִ��ս��ָ��
			{
			if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == ATTACK)		//�ֵ������ǹ�����
			{
				bShowMagicPic = false;
				// ����Ҫ�����Ķ����Ƿ���ڣ��������������
				if(Command[kill_count].kill_who!=Test_Enemy(Command[kill_count].kill_who))	
				{
					Command[kill_count].kill_who=Test_Enemy(Command[kill_count].kill_who);
					Account_Attack();
				}
				//��������
				FightRole[kill_count].stop_temp=10;
				Sleep(Role_Move[i].ReadInt("����","��ʱ"));
				//��ȡλͼ�Ĵ�С����
				RMove_Pic.left=Role_Move[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Role_Move[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Role_Move[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Role_Move[i].ReadInt(Role_Temp,"bottom");
				//ѭ������
				//�ӿ�ʼ�������������˵����Ǹ�λ
				if(Role_Move[i].ReadInt(Role_Temp,"�Ա�")==1)
				{
					//�õ�����Ӧ�ó��ֵ�λ��
					FightRole[kill_count].x1=FightRole[kill_count].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=FightRole[kill_count].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//��һ�ε�����
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"�Ա�")==2)	//�ƶ�
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"�Ա�")==3)	//��������
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//���˶���������˵Ķ���
					EnemyRole[Command[kill_count].kill_who].stop_temp=11;
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"�Ա�")==4)	//����
				{
					FightRole[kill_count].sound_Fight.Play();	//������
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//���˶�����ɺ��˶���
					EnemyRole[Command[kill_count].kill_who].stop_temp=10;
				}
				else if(Role_Move[i].ReadInt(Role_Temp,"�Ա�")==5)	//���˲ҽ�
				{
					FightRole[kill_count].x1=EnemyRole[Command[kill_count].kill_who].x+Role_Move[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=EnemyRole[Command[kill_count].kill_who].y+Role_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Role_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//���˶�����λ�����˶���
					//---------
					//����������
					//�����ȱ�����
					//Ȼ�����
					//Ȼ��λ���������Ķ���
					//-------------
					EnemyRole[Command[kill_count].kill_who].stop_temp=11;
					EnemyRole[Command[kill_count].kill_who].sound_Defend.Play();	//�к���
				}
				else
				{
					//���ǻص�ԭλ��
					FightRole[kill_count].stop_temp=1;
					//���ʤ���ˣ���ֱ��������������
					if( Account_Loss()==1 ) 
						goto _Next;
					//���㹥����
					Account_Attack();

					for(int j=i+1; j<3; j++)
						if(FightRole[j].use==1)		// �����ж���һ����ɫ���ĸ���ɫ
						{
							char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
							Role_Move[j].Open(str);
							free(str);
							_FREE(Role_Temp);
							Role_Temp=Role_Move[j].ReadText("����","continue");
							kill_count=j;			//�л�����һ������
							goto _Next;
						}
					//û���¸������ˣ��ͻ����˹���
					Enemy_AI();			//Ϊ���˹�������׼��
					main_count=ENEMY_KILL_ROLE;		//������˽�������
					m_nRoundCount ++;			//���ӻغϼ���
					Account_Attack();
				}
_Next:;
			}
			//ʹ����Ʒ
			else if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == TOOLS)
			{
				bShowMagicPic = false;
				//��λ�ò����ڣ�����
				if(Command[kill_count].kill_who!=Test_Role(Command[kill_count].kill_who))	// ����Ҫ�����Ķ����Ƿ���ڣ��������������
				{
					Command[kill_count].kill_who=Test_Role(Command[kill_count].kill_who);
					Account_Attack();
				}
				//��������Ʒ����ʹ�ã��ͽ���ʹ��ģʽ
				if(m_fightgoods.IsCanUse((int)Command[kill_count].lAccessory))
					FightRole[kill_count].stop_temp=10;
				//����ʹ�õĻ����ͱ��ֳ�̬
				else 
					FightRole[kill_count].stop_temp = 1;

				Sleep(Role_Tool[i].ReadInt("����","��ʱ"));
				//��ȡλͼ�Ĵ�С����
				RMove_Pic.left=Role_Tool[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Role_Tool[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Role_Tool[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Role_Tool[i].ReadInt(Role_Temp,"bottom");
				if(Role_Tool[i].ReadInt(Role_Temp,"�Ա�")==1)
				{
					//λ������
					FightRole[kill_count].x1=FightRole[kill_count].x+Role_Tool[i].ReadInt(Role_Temp,"x");
					FightRole[kill_count].y1=FightRole[kill_count].y+Role_Tool[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//��һ������
					Role_Temp=Role_Tool[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else
				{
					//�ָ�ԭ״̬
					FightRole[kill_count].stop_temp=1;
					//ʹ����Ʒ
					if(FightRole[kill_count].use==1)
					{
						if(m_fightgoods.IsCanUse((int)Command[kill_count].lAccessory))
						{
							//����Ч��
							m_fightgoods.GoodEffect((int)Command[kill_count].lAccessory, Hero[Command[kill_count].kill_who],Command[kill_count].kill_who);
							//����Ѫ������
							FightRole[Command[kill_count].kill_who].BroodLong=Hero[Command[kill_count].kill_who]->Hp * 100 / Hero[Command[kill_count].kill_who]->MaxHp;
						}
						else
						{
							//����ʹ�õ�ʱ����ʾ��ʾ
							GameMessage.DoMenu(lpDDSBack, "��Ʒʹ�ù��ˣ�");
						}
					}

					for(int j=i+1; j<3; j++)
						if(FightRole[j].use==1)		// �����ж���һ����ɫ���ĸ���ɫ
						{
							if(Command[j].lCommand == ATTACK)
							{
								char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
								Role_Move[j].Open(str);
								free(str);
								_FREE(Role_Temp);
								Role_Temp=Role_Move[j].ReadText("����","continue");
							}
							else if(Command[j].lCommand == TOOLS)
							{
								char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
								Role_Tool[j].Open(str);
								free(str);
								_FREE(Role_Temp);
								Role_Temp=Role_Tool[j].ReadText("����","continue");
							}
							kill_count=j;
							goto _GoodsNext;
						}
					//û����һ�������˾ͻ����˹���
					Enemy_AI();
					main_count=ENEMY_KILL_ROLE;
					m_nRoundCount ++;			//���ӻغϼ���
					Account_Attack();
				}
_GoodsNext:;
			}			//ʹ��ħ��
			else if(kill_count==i && FightRole[kill_count].use==1 
				&& Command[kill_count].lCommand == MAGIC)
			{
				if(Command[kill_count].Class==1)		//���Լ���
				{
					if(Command[kill_count].kill_who!=Test_Role(Command[kill_count].kill_who))	// ����Ҫ�����Ķ����Ƿ���ڣ��������������
					{
						//���¼���
						Command[kill_count].kill_who=Test_Role(Command[kill_count].kill_who);
						Account_Attack();
						if(Role_Magic[Command[kill_count].lAccessory].ReadInt("����","���"))
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","startx")+FightRole[kill_count].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","starty")+FightRole[kill_count].y;
						}
						else
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","startx")+FightRole[Command[kill_count].kill_who].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","starty")+FightRole[Command[kill_count].kill_who].y;
						}
					}
					//������ħ������ʹ�ã��ͽ���ʹ��ģʽ
					if(m_fightmagic.IsCanUse((int)Command[kill_count].lAccessory))
					{
						//����ʹ��ģʽ
						FightRole[kill_count].stop_temp=10;
						//��ʾħ��Ч��ͼƬ
						bShowMagicPic = true;
						int temp = Command[kill_count].lAccessory;
						//����
						Sleep(Role_Magic[temp].ReadInt("����","��ʱ"));
						//��ȡ���ﶯ��λͼ������
						RMove_Pic.left=Role_Magic[temp].ReadInt(Role_Temp,"left");
						RMove_Pic.top=Role_Magic[temp].ReadInt(Role_Temp,"top");
						RMove_Pic.right=Role_Magic[temp].ReadInt(Role_Temp,"right");
						RMove_Pic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"bottom");
						//ħ��ͼƬ������
						fightmagic.RMagicPic.left = Role_Magic[temp].ReadInt(Role_Temp,"picleft");
						fightmagic.RMagicPic.top = Role_Magic[temp].ReadInt(Role_Temp,"pictop");
						fightmagic.RMagicPic.right=Role_Magic[temp].ReadInt(Role_Temp,"picright");
						fightmagic.RMagicPic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"picbottom");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==1)	//ħ����ʼ
						{
							//����ħ��Ч������
							char * soundtemp;
							soundtemp=Role_Magic[temp].ReadText(Role_Temp,"SoundFileName");
							if(soundtemp[0])
							{
								sound_Magic.Stop();
								sound_Magic.LoadSoundFX(hWnd,soundtemp);
							}
							sound_Magic.Play();
							//λ������
							int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
							int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
							if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
							if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
							//��һ������
							Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
							free(str);
						}
						else if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==2)	//��ͨ״̬
						{
							//λ������
							int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
							int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
							if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
							if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
							//��һ������
							Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
							free(str);
						}
						else		//����ħ��
						{
							//����ʾͼƬ
							bShowMagicPic = false;
							//�ָ�ԭ״̬
							FightRole[kill_count].stop_temp=1;
							//ʹ��ħ��
							if(FightRole[kill_count].use==1)
							{
								//����Ч��
								//ʹ������ħ���������߱仯ħ���趨��ֵ
								m_fightmagic.MagicEffect((int)Command[kill_count].lAccessory,Hero[kill_count], Hero[Command[kill_count].kill_who]);
								//����ħ��������
								FightRole[kill_count].MagicLong=Hero[kill_count]->Mp * 100 / Hero[kill_count]->MaxMp;
								//����Ѫ������
								FightRole[Command[kill_count].kill_who].BroodLong=Hero[Command[kill_count].kill_who]->Hp * 100 / Hero[Command[kill_count].kill_who]->MaxHp;
								//ֹͣ����
								sound_Magic.Stop();
								//Ҫ��ʾ��Ѫ
								m_nLifeupCountLife = 3;
								m_nLifeupCountMove = 0;
								FightRole[Command[kill_count].kill_who].Lifeup = CMagic::MAGIC[Command[kill_count].lAccessory].Hp;
							}
							for(int j=i+1; j<3; j++)
								if(FightRole[j].use==1)		// �����ж���һ����ɫ���ĸ���ɫ
								{
									if(Command[j].lCommand == ATTACK)
									{
										char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
										Role_Move[j].Open(str);
										free(str);
										_FREE(Role_Temp);
										Role_Temp=Role_Move[j].ReadText("����","continue");
									}
									else if(Command[j].lCommand == TOOLS)
									{
										char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
										Role_Tool[j].Open(str);
										free(str);
										_FREE(Role_Temp);
										Role_Temp=Role_Tool[j].ReadText("����","continue");
									}
									else
									{
										_FREE(Role_Temp);
										Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("����","continue");
										sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
										if(Command[j].Class==1)			//���Լ��˵�
										{
											if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
											}
											else
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[Command[j].kill_who].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[Command[j].kill_who].y;
											}
										}
										else
										{
											if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
											}
											else
											{
												fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+EnemyRole[Command[j].kill_who].x;
												fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+EnemyRole[Command[j].kill_who].y;
											}
										}
									}
									kill_count=j;			//�л�����һ������
									goto _MagicNext;
								}
								//û����һ�������˾ͻ����˹���
								Enemy_AI();
								main_count=ENEMY_KILL_ROLE;
								m_nRoundCount ++;			//���ӻغϼ���
								Account_Attack();
						}
					}
					//����ʹ�õĻ����ͱ��ֳ�̬
					else 
					{
						FightRole[kill_count].stop_temp = 1;
						bShowMagicPic = false;
						//����ʹ�õ�ʱ����ʾ��ʾ
						GameMessage.DoMenu(lpDDSBack, "ħ�����㣬����ʹ��");
						for(int j=i+1; j<3; j++)
							if(FightRole[j].use==1)		// �����ж���һ����ɫ���ĸ���ɫ
							{
								if(Command[j].lCommand == ATTACK)
								{
									char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
									Role_Move[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Move[j].ReadText("����","continue");
								}
								else if(Command[j].lCommand == TOOLS)
								{
									char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
									Role_Tool[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Tool[j].ReadText("����","continue");
								}
								else
								{
									_FREE(Role_Temp);
									Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("����","continue");
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
									if(Command[j].Class==1)			//���Լ��˵�
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[Command[j].kill_who].y;
										}
									}
									else
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+EnemyRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+EnemyRole[Command[j].kill_who].y;
										}
									}
								}
								kill_count=j;
								goto _MagicNext;
							}
							//û����һ�������˾ͻ����˹���
							Enemy_AI();
							main_count=ENEMY_KILL_ROLE;
							m_nRoundCount ++;			//���ӻغϼ���
							Account_Attack();
					}
				}
				else			//�Ե��˵�
				{
					if(Command[kill_count].kill_who!=Test_Enemy(Command[kill_count].kill_who))	// ����Ҫ�����Ķ����Ƿ���ڣ��������������
					{
						//���¼���
						Command[kill_count].kill_who=Test_Enemy(Command[kill_count].kill_who);
						Account_Attack();
						if(Role_Magic[Command[kill_count].lAccessory].ReadInt("����","���"))
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","startx")+FightRole[kill_count].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","starty")+FightRole[kill_count].y;
						}
						else
						{
							fightmagic.x = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","startx")+EnemyRole[Command[kill_count].kill_who].x;
							fightmagic.y = Role_Magic[Command[kill_count].lAccessory].ReadInt("����","starty")+EnemyRole[Command[kill_count].kill_who].y;
						}
					}
					int temp = Command[kill_count].lAccessory;
					//�ܹ�ʹ��ħ��
					//m_fightmagic.IsCanUse(temp)
					
					//��ʾͼƬ
					bShowMagicPic = true;
					//����ʹ��״̬
					FightRole[kill_count].stop_temp=10;
					Sleep(Role_Magic[temp].ReadInt("����","��ʱ"));
					
					//��ȡ����λͼ������
					RMove_Pic.left=Role_Magic[temp].ReadInt(Role_Temp,"left");
					RMove_Pic.top=Role_Magic[temp].ReadInt(Role_Temp,"top");
					RMove_Pic.right=Role_Magic[temp].ReadInt(Role_Temp,"right");
					RMove_Pic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"bottom");
					//��ȡħ��Ч��ͼƬ������
					fightmagic.RMagicPic.left = Role_Magic[temp].ReadInt(Role_Temp,"picleft");
					fightmagic.RMagicPic.top = Role_Magic[temp].ReadInt(Role_Temp,"pictop");
					fightmagic.RMagicPic.right=Role_Magic[temp].ReadInt(Role_Temp,"picright");
					fightmagic.RMagicPic.bottom=Role_Magic[temp].ReadInt(Role_Temp,"picbottom");
					//ѭ������
					//�ӿ�ʼ�������������˵����Ǹ�λ
					if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==1)	//��ʼ
					{
						//����Ч������
						char * soundtemp;
						soundtemp=Role_Magic[temp].ReadText(Role_Temp,"SoundFileName");
						if(soundtemp[0])
						{
							sound_Magic.Stop();
							sound_Magic.LoadSoundFX(hWnd,soundtemp);
						}
						sound_Magic.Play();
						//�õ�����Ӧ�ó��ֵ�λ��
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
						if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
						//��һ�ε�����
						Role_Temp=Role_Magic[temp].ReadText(Role_Temp,"continue");
						free(str);
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==2)	//�ƶ�
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
						if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
					else if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==3)	//��������
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
						if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
						
						//���˶���������˵Ķ���
						EnemyRole[Command[kill_count].kill_who].stop_temp=11;
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==4)	//����
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
						if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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
						
						//���˶�����ɺ��˶���
						EnemyRole[Command[kill_count].kill_who].stop_temp=10;
					}
					else if(Role_Magic[temp].ReadInt(Role_Temp,"�Ա�")==5)	//���˲ҽ�
					{
						int rolex=Role_Magic[temp].ReadInt(Role_Temp,"x");
						int roley=Role_Magic[temp].ReadInt(Role_Temp,"y");
						if(Role_Magic[temp].ReadInt(Role_Temp,"�������"))
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
						if(Role_Magic[temp].ReadInt(Role_Temp,"ͼƬ���"))
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

						//����ħ��Ч��
						//�ͷ�����ħ��ֵ�������߸���ħ������
						MagicEffect(temp, Hero[kill_count],&EnemyRole[Command[kill_count].kill_who],Command[kill_count].kill_who);
						//���˶�����λ�����˶���
						//---------
						//����������
						//�����ȱ�����
						//Ȼ�����
						//Ȼ��λ���������Ķ���
						//-------------
						EnemyRole[Command[kill_count].kill_who].stop_temp=11;
						EnemyRole[Command[kill_count].kill_who].sound_Defend.Play();	//�к���
					}
					else
					{
						//���ǻص�ԭλ�ã�����ʾħ��ͼƬ
						bShowMagicPic = false;
						FightRole[kill_count].stop_temp=1;
						
						//���ʤ���ˣ���ֱ��������������
						if( Account_Loss()==1 ) 
							goto _MagicNext;
						//���㹥����
						Account_Attack();
						//����ħ��������
						FightRole[kill_count].MagicLong=Hero[kill_count]->Mp * 100 / Hero[kill_count]->MaxMp;
						//ֹͣ����
						sound_Magic.Stop();
						for(int j=i+1; j<3; j++)
							if(FightRole[j].use==1)		// �����ж���һ����ɫ���ĸ���ɫ
							{
								if(Command[j].lCommand == ATTACK)
								{
									char* str = FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Move_Ini");
									Role_Move[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Move[j].ReadText("����","continue");
								}
								else if(Command[j].lCommand == TOOLS)
								{
									char* str =  FightRoleInit.ReadText(Hero[j]->Fight_Ini_Name,"Tool_Ini");
									Role_Tool[j].Open(str);
									free(str);
									_FREE(Role_Temp);
									Role_Temp=Role_Tool[j].ReadText("����","continue");
								}
								else
								{
									_FREE(Role_Temp);
									Role_Temp=Role_Magic[Command[j].lAccessory].ReadText("����","continue");
									sound_Magic.LoadSoundFX(hWnd,CMagic::MAGIC[Command[j].lAccessory].SoundFileName);
									if(Command[j].Class==1)			//���Լ��˵�
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[Command[j].kill_who].y;
										}
									}
									else
									{
										if(Role_Magic[Command[j].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+FightRole[j].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+FightRole[j].y;
										}
										else
										{
											fightmagic.x = Role_Magic[Command[j].lAccessory].ReadInt("����","startx")+EnemyRole[Command[j].kill_who].x;
											fightmagic.y = Role_Magic[Command[j].lAccessory].ReadInt("����","starty")+EnemyRole[Command[j].kill_who].y;
										}
									}
								}
								kill_count=j;
								goto _MagicNext;
							}
							//û���¸������ˣ��ͻ����˹���
							Enemy_AI();			//Ϊ���˹�������׼��
							main_count=ENEMY_KILL_ROLE;		//������˽�������
							m_nRoundCount ++;			//���ӻغϼ���
							Account_Attack();
					}
				}
			}
		}
			//��ʾ����
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
			//����Ҫ�Ǳ������Ϳ�ʼ��Ѫ��ʾ����
			for(i=0;i<4;i++)
			{
				if(EnemyRole[i].use==1)
				{
					if(EnemyRole[i].stop_temp==11)
					{
						m_nAttackCountLife = 10;
					}
					EnemyRole[i].ready = 1;//����û�н���
				}
			}
			//��ʾ����
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

					FightRole[i].ready = 0;	//�����ǽ���״̬
				}
			}
			//��ʾħ����ͼƬ
			if(bShowMagicPic)
				Blt(lpDDSBack,fightmagic.x,fightmagic.y,
						CMagic::MAGIC[Command[kill_count].lAccessory].EffectPic,
						fightmagic.RMagicPic,true);
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ���˽��й����Ķ���-------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case ENEMY_KILL_ROLE:			//��ROLE_KILL_ENEMY������ͬ
		{
			for(i=0; i<4; i++)
			if(enemy_count==i && EnemyRole[enemy_count].use==1 )
			{
				//���������Ƿ���ڣ����ھͻ���
				if(Enemy_Command[enemy_count].kill_who!=Test_Role(Enemy_Command[enemy_count].kill_who))	// ����Ҫ�����Ķ����Ƿ���ڣ��������������
				{
					Enemy_Command[enemy_count].kill_who=Test_Role(Enemy_Command[enemy_count].kill_who);
					Account_Attack();
				}
				//��������
				EnemyRole[enemy_count].stop_temp=10;
				Sleep(Enemy_Move[i].ReadInt("����","��ʱ"));

				//��ȡλͼ�Ĵ�С����
				RMove_Pic.left=Enemy_Move[i].ReadInt(Role_Temp,"left");
				RMove_Pic.top=Enemy_Move[i].ReadInt(Role_Temp,"top");
				RMove_Pic.right=Enemy_Move[i].ReadInt(Role_Temp,"right");
				RMove_Pic.bottom=Enemy_Move[i].ReadInt(Role_Temp,"bottom");

				if(Enemy_Move[i].ReadInt(Role_Temp,"�Ա�")==1)
				{
					//λ������
					EnemyRole[enemy_count].x1=EnemyRole[enemy_count].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=EnemyRole[enemy_count].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					//�¸����ݵ�λ��
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"�Ա�")==2)	//�ƶ�
				{
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"�Ա�")==3)	//��������
				{
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					//���Ƕ����л����̺�ROLE_KILL_ENEMY�еĵ��˶����л���ͬ
					FightRole[Enemy_Command[enemy_count].kill_who].stop_temp=11;
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"�Ա�")==4)	//����
				{
					EnemyRole[enemy_count].sound_Fight.Play();
					EnemyRole[enemy_count].x1=FightRole[Enemy_Command[enemy_count].kill_who].x+Enemy_Move[i].ReadInt(Role_Temp,"x");
					EnemyRole[enemy_count].y1=FightRole[Enemy_Command[enemy_count].kill_who].y+Enemy_Move[i].ReadInt(Role_Temp,"y");
					char *str = Role_Temp;
					Role_Temp=Enemy_Move[i].ReadText(Role_Temp,"continue");
					free(str);
					FightRole[Enemy_Command[enemy_count].kill_who].stop_temp=10;
				}
				else if(Enemy_Move[i].ReadInt(Role_Temp,"�Ա�")==5)	//���ǲҽ�
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
					//���˸�λ
					EnemyRole[enemy_count].stop_temp=1;
					//����ȫ���������������˹���ѭ��
					if( Account_Loss()== 2 ) 
						goto _Next_Enemy;
					//���㹥����
					Account_Attack();

					for(int j=i+1; j<4; j++)
					if(EnemyRole[j].use==1)			//�ж���һ������
					{
						char* str = EnemyInit.ReadText(EnemyRole[j].name,"Move_Ini");
						Enemy_Move[j].Open(str);
						free(str);
						_FREE(Role_Temp);
						Role_Temp=Enemy_Move[j].ReadText("����","continue");
						enemy_count=j;				//�ֵ��¸����˹���
						goto _Next_Enemy;
					}
					//����ѭ����ɣ�������ѭ��
					main_count=MAIN;
					//command_count�ó��ŵ���ǰ�������
					command_count=Test_Role(0);
				}
_Next_Enemy:;
			}
			//��ʾ����
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
			//��ʼ���Ǽ�Ѫ����
			for(i=0;i<3;i++)
			{
				if(FightRole[i].use==1)
				{
					if(FightRole[i].stop_temp==11)
					{
						m_nAttackCountLife = 10;
					}
					FightRole[i].ready = 1;		//���ǲ��ǽ���״̬
				}
			}
			//��ʾ����
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
					//�õ����ǽ���״̬
					EnemyRole[i].ready = 0;
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ս������-----------------------------------------------------
	/////////////////////////////////////////////////////////////////////
	case OVER:
		{
			if(temp_swap==1)		//���ܳɹ�
			{
				Sleep(1000);
				temp_swap=0;
				ret=1;
			}
			else if(temp_swap==2)	//ʤ��
			{
				temp_swap=0;
				ret=2;
			}
			else if(temp_swap==3)	//ʧ��
			{
				temp_swap=0;
				ret=3;
			}
			else if( temp_swap==4 )		//����ʧ��
			{
				Sleep(800);
				Enemy_AI();				//�����˹���
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
	// ��ʾ����
	//-------------------------------------------------------------------
	//��ʾ���˵�HP

	//if( bShowFps )
	{
		if(Enemy[0].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20,"�����ϣ�%d",Enemy[0].Hp);
		if(Enemy[1].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size,"�����ң�%d",Enemy[1].Hp);
		if(Enemy[2].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*2,"������%d",Enemy[2].Hp);
		if(Enemy[3].Hp)
			FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*3,"�����£�%d",Enemy[3].Hp);
		FightFont.PrintText(lpDDSBack, 20, 20+FightFont.font_Size*4,"�غϼ�����%d",m_nRoundCount);
	}

	//�����ڵ���ͼͼ����ʾ
	RECT RBuild={0, 0, 640, 239};
	Blt(lpDDSBack, 0, 260, background_build[0], RBuild, true);

	//͸������
	Show_Role_Enemy_Alpha();

	//��ʾħ����ͼƬ
	if(bShowMagicPic)
		ABlt(lpDDSBack,fightmagic.x,fightmagic.y,
					CMagic::MAGIC[Command[kill_count].lAccessory].EffectPic,
					fightmagic.RMagicPic,ColorKey16,16);

	//����ͷ����״̬����ʾ����
	if(FightRole[0].status==1)
	{
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(lpDDSBack, 58, 434, 158, 439, COLOR_BROOD_NULL);
		FightRole[0].BroodLong = Hero[0]->Hp*100/Hero[0]->MaxHp;
		Bar(lpDDSBack, 58, 434, 58+FightRole[0].BroodLong, 439, COLOR_BROOD);
		// MPѪ�۵ĳ��̣��ݶ�
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
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(lpDDSBack, 236, 434, 336, 439, COLOR_BROOD_NULL);
		FightRole[1].BroodLong = Hero[1]->Hp*100/Hero[1]->MaxHp;
		Bar(lpDDSBack, 236, 434, 236+FightRole[1].BroodLong, 439, COLOR_BROOD);
		// MPѪ�۵ĳ��̣��ݶ�
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
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(lpDDSBack, 416, 434, 516, 439, COLOR_BROOD_NULL);
		FightRole[2].MagicLong = Hero[2]->Mp*100/Hero[2]->MaxMp;
		Bar(lpDDSBack, 416, 434, 416+FightRole[2].BroodLong, 439, COLOR_BROOD);	
		// MPѪ�۵ĳ��̣��ݶ�
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

	// ����״̬����ʾ
	switch( main_count )
	{
	case MAIN:
		{
			// �˵�ѡ�����ʾ
			int bMouse = 0;
			if(!bButton[0]&&!bButton[1]&&!bButton[2]&&!bButton[3]&&!bButton[4])
			{
				for(i=0; i<5; i++)
				{
					switch( Button[i].CheckLoop(lpDDSBack) )
					{
					case 1:	// ���
						Sound_Click.Play();	//��Ч
						bButton[i]=1;
						break;
						
					case 3:			//��ס����
						bMouse=1;
						break;
						
					case 2:	// �ƶ�������
						if( old != i )		//�ϴβ��������Ŧ
						{
							Sound_High.Play();	//��Ч
							old=i;			//����
						}
						bButton[i]=0;		//��0
						bMouse = 1;
						// ����˵�����ֵ���ʾ
						switch(i)
						{
						case 0:
							font.PrintText(lpDDSBack, 495, 450, "�ظ��ϴεĲ���(R)");
							break;
						case 1:
							font.PrintText(lpDDSBack, 510, 450, "��ͨ����(A)");
							break;
						case 2:
							font.PrintText(lpDDSBack, 510, 450, "ʹ��ħ��(E)");
							break;
						case 3:
							font.PrintText(lpDDSBack, 510, 450, "ʹ�õ���(G)");
							break;
						case 4:
							font.PrintText(lpDDSBack, 530, 450, "����(Q)");
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
						font.PrintText(lpDDSBack, 495, 450, "�ظ��ϴεĲ���(R)");
						break;
					case 1:
						font.PrintText(lpDDSBack, 510, 450, "��ͨ����(A)");
						break;
					case 2:
						font.PrintText(lpDDSBack, 510, 450, "ʹ��ħ��(E)");
						break;
					case 3:
						font.PrintText(lpDDSBack, 510, 450, "ʹ�õ���(G)");
						break;
					case 4:
						font.PrintText(lpDDSBack, 530, 450, "����(Q)");
						break;
					default:
						break;
					}
				}
			}
			if( bButton[0] )	//�ظ�
			{
				bButton[0] = 0;	//��0
				//ת�빥��״̬
				//���ǵĹ���ָ����ϴε�һ��
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
							Role_Temp=Role_Move[i].ReadText("����","continue");
						}
						else if(Command[i].lCommand == TOOLS)
						{
							for(int j=i;j<3;j++)
								if(FightRole[j].use ==1)
								{
									if(!m_fightgoods.IsCanUse(Command[j].lAccessory))
									{
										GameMessage.DoMenu(lpDDSBack,"����Ʒ����ʹ��");
										main_count = MAIN;
										return 0;
									}
								}
							char* str =  FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Tool_Ini");
							Role_Tool[i].Open(str);
							free(str);
							_FREE(Role_Temp);
							Role_Temp=Role_Tool[i].ReadText("����","continue");
						}
						else
						{
							for(int j=i;j<3;j++)
								if(FightRole[j].use ==1)
								{
									if(!m_fightmagic.IsCanUse(Command[j].lAccessory,j))
									{
										GameMessage.DoMenu(lpDDSBack,"��ħ������ʹ��");
										main_count = MAIN;
										return 0;
									}
								}
							_FREE(Role_Temp);
							Role_Temp=Role_Magic[Command[i].lAccessory].ReadText("����","continue");
							if(Command[i].Class==1)			//���Լ��˵�
							{
								if(Role_Magic[Command[i].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[i].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[i].y;
								}
								else
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[Command[i].kill_who].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[Command[i].kill_who].y;
								}
							}
							else
							{
								if(Role_Magic[Command[i].lAccessory].ReadInt("����","���"))	//����ħ���Ķ������ħ��ͼƬ��ʾ�ĳ�λ��
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+FightRole[i].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+FightRole[i].y;
								}
								else
								{
									fightmagic.x = Role_Magic[Command[i].lAccessory].ReadInt("����","startx")+EnemyRole[Command[i].kill_who].x;
									fightmagic.y = Role_Magic[Command[i].lAccessory].ReadInt("����","starty")+EnemyRole[Command[i].kill_who].y;
								}
							}
						}
						kill_count=i;
						break;
					}
				//���㹥����
				Account_Attack();
			}
			else if( bButton[1] )	//����
			{
				bButton[1] = 0;
				if( GetEnemyNum()==1 )	//ֻ��һ������ʱ
				{
					Command[command_count].lCommand = ATTACK;
					Command[command_count].kill_who=GetLastEnemyNum();
					main_count=MAIN;
					command_count++;
					if( command_count > GetLastRoleNum() )	//��ʼ����
					{
						main_count=ROLE_KILL_ENEMY;
						for(i=0; i<3; i++)
							if(FightRole[i].use==1)
							{
								_FREE(Role_Temp);
								Role_Temp=Role_Move[i].ReadText("����","continue");
								kill_count=i;
								break;
							}
						Account_Attack();
					}
				}
				else
				{
					//��ֹһ������
					//����ѡ�����״̬
					Command[command_count].player=1;
					nNonce = 0;
					main_count=KILL_WHO;
				}
			}
			else if( bButton[2] )	//ħ��
			{
				bButton[2] = 0;
				main_count = USER_MAGIC;
			}
			else if( bButton[3] )	//����
			{
				bButton[3] = 0;
				//����ѡ����Ʒ״̬
				main_count = USER_GOODS;
			}
			else if( bButton[4] )	//����
			{
				bButton[4] = 0;
				if( bLeave )		//��������
				{
					if( random(100) > 50 )
					{
						font.PrintText(lpDDSBack, 300, 220, "���ܳɹ���");
						main_count=OVER;
						temp_swap=1;
					}
					else
					{
						font.PrintText(lpDDSBack, 300, 220, "����ʧ�ܣ�");
						main_count=OVER;
						temp_swap=4;
					}
				}
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ѡ����Ʒ
	/////////////////////////////////////////////////////////////////////
	case USER_GOODS:
		{
			int ret = m_fightgoods.DoMenu(lpDDSBack, true);
			if( ret== 1)
			{
				//ѡ����һ����Ʒ
				//����ѡ��ʹ�ö���ģʽ
				main_count = USER_WHO;
				nNonce = 0;
			}
			else if(ret == 2)//û��ѡ����Ʒ
				main_count = MAIN;
		}
		break;

	case USER_MAGIC:
		{
			int ret = m_fightmagic.DoMenu(lpDDSBack, true,command_count);
			if( ret== 1)
			{
				//ѡ����һ��ħ��������ħ��������
				//���벻ͬ��ѡ��ģʽ
				bUseMagic = true;
				if(CMagic::MAGIC[m_fightmagic.GetMagicID()].Class==0)
					main_count = USER_WHO;
				else
					main_count = KILL_WHO;
				nNonce = 0;
				//�ܲ���ʹ�����ħ��
				if(!m_fightmagic.IsCanUse(m_fightmagic.GetMagicID(),command_count))
				{
					GameMessage.DoMenu(lpDDSBack, "ħ�����㣬����ʹ��");
					main_count = MAIN;		//�ص�ѡ��ģʽ
					bUseMagic = false;
				}
			}
			else if(ret == 2)//û��ѡ����Ʒ
				main_count = MAIN;
		}
		break;
	}
	//��ʾ��Ѫ
	Print_Attack();
	//��ʾ��Ѫ
	Print_Lifeup();
	return 0;
}

// ���㹥����
void CFight::Account_Attack(void)
{
	if(main_count==ROLE_KILL_ENEMY)				//���ǹ������֣����չ����ߵĹ������ͱ�����
	{											//�ߵķ���������һ���ļ��㣬���м������ƶ�
		int who=Command[kill_count].kill_who;	//���ء�who�Ǳ������ߣ����������temp����
		int temp;								//�������߼��ٵ�HP��

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

	else if(main_count==ENEMY_KILL_ROLE)		//���˹���
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

// �������������ĵ��ˣ�����ȷ����ĵ���
int CFight::Test_Enemy(int a)			//���ѡ��ĵ��˴��ڣ��ͷ����Ǹ����˵�λ��
{
	if(EnemyRole[a].use==0)				//��������ڣ��͵ݹ������һ�����ˡ�
	{
		if(a<3)							//���4������
			return Test_Enemy(a+1);
		else 
			return 0;					//��Ȼ�����ڣ��ͷ���0
	}
	else
		return a;
}

// �����������������ǣ�����ȷ���������
int CFight::Test_Role(int a)			//����һ������һ�����㷨
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

// ������ʧ�Լ�������������ʾ
int CFight::Account_Loss(void)
{
	//���˼�Ѫ
	if(main_count==ROLE_KILL_ENEMY)
	{
		//��ǰ�������ĵ�����Ѫ
		//kill_count�ǵ�ǰ���ֵ�����
		Enemy[Command[kill_count].kill_who].Hp-=EnemyRole[Command[kill_count].kill_who].UnAttack;
		//���˱�������
		if(Enemy[Command[kill_count].kill_who].Hp<=0)
		{
			EnemyRole[Command[kill_count].kill_who].sound_Dead.Play();	//������
			Enemy[Command[kill_count].kill_who].Hp=0;
			EnemyRole[Command[kill_count].kill_who].use=0;
		}
		
		//ʤ��
		//ɱ�������еĵ���
		if( EnemyRole[0].use==0 && EnemyRole[1].use==0 && EnemyRole[2].use==0 && EnemyRole[3].use==0 )
		{
			UpdateLevel();		//�ӷ�����
			main_count=OVER;
			temp_swap=2;		//ʤ��
			return 1;
		}
	}

	//�ҷ���Ѫ
	else if(main_count==ENEMY_KILL_ROLE)
	{
		for( int i=0; i<3; i++)
		if( Enemy_Command[enemy_count].kill_who==i )
		{
			Hero[i]->Hp-=FightRole[i].UnAttack;
			if(Hero[i]->Hp<=0)
			{
				FightRole[i].sound_Dead.Play();	//������
				FightRole[i].use=0;
				Hero[i]->Hp=0;
			}
			//����Ѫ��
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
		//ʧ��
		//ȫ�������Ƕ�����
		if(FightRole[0].use==0 && FightRole[1].use==0 && FightRole[2].use==0 )
		{
			main_count=OVER;
			temp_swap=3;		//ʧ��
			return 2;
		}
	}
	return 0;
}

//��ʾ����
void CFight::Print_Attack(void)
{
	m_nAttackCountMove ++;			//��ʾ��ֵ�����ƶ�
	if(m_nAttackCountLife > 0)		//��Ȼ��Ҫ��ʾ
	{
		if(main_count==ROLE_KILL_ENEMY)		//��ʾ������Ѫ
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
		m_nAttackCountLife --;		//��ʾʱ�����
	}
}

//���˹���AI
void CFight::Enemy_AI(void)
{
	char* str=NULL;

	// ���˽������趨
	//�ȶ��ƶ����ݣ�Ȼ���ø���ʱ�ı���������һ�����������������ļ����λ��
	if(EnemyRole[0].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[0].name,"Move_Ini");
		Enemy_Move[0].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[0].ReadText("����","continue");
		enemy_count=0;
	}
	else if(EnemyRole[1].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[1].name,"Move_Ini");
		Enemy_Move[1].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[1].ReadText("����","continue");
		enemy_count=1;
	}
	else if(EnemyRole[2].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[2].name,"Move_Ini");
		Enemy_Move[2].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[2].ReadText("����","continue");
		enemy_count=2;
	}
	else if(EnemyRole[3].use==1)
	{
		str = EnemyInit.ReadText(EnemyRole[3].name,"Move_Ini");
		Enemy_Move[3].Open(str);
		_FREE(str);
		_FREE(Role_Temp);
		Role_Temp=Enemy_Move[3].ReadText("����","continue");
		enemy_count=3;
	}
	//ȫ�ǹ���
	Enemy_Command[0].player=1;
	Enemy_Command[1].player=1;
	Enemy_Command[2].player=1;
	Enemy_Command[3].player=1;

	int i,j;
	//ѡ�񹥻��ĸ���ɫ
	Enemy_Command[0].kill_who=random(3);
	Enemy_Command[1].kill_who=random(3);
	Enemy_Command[2].kill_who=random(3);
	Enemy_Command[3].kill_who=random(3);
	/*for( j=0;j<10;j++)	// ���ж�ε�ѭ��ȡ�������ȷ��������ֹ�������
	for( i=0;i<4;i++)
	{
		if(FightRole[Enemy_Command[i].kill_who].use==0)			//��λ��û���˾ͻ�����
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

// ��ʱ������ ���¶���������
void CFight::OnTimer(void)
{
	int i;
	for(i=0;i<3;i++)						//ѭ����������
	{
		if(FightRole[i].stop_temp==1)
			FightRole[i].stop_temp=2;
		else if(FightRole[i].stop_temp==2)
			FightRole[i].stop_temp=3;
		else if(FightRole[i].stop_temp==3)
			FightRole[i].stop_temp=4;
		else if(FightRole[i].stop_temp==4)
			FightRole[i].stop_temp=1;
		else if(FightRole[i].stop_temp==10)	//����������Ļָ�����
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
		else if(EnemyRole[i].stop_temp==10)		// ����������Ļָ�����
			EnemyRole[i].stop_temp=1;
		else if(EnemyRole[i].stop_temp==11)		// ����������Ļָ�����
			EnemyRole[i].stop_temp=1;
	}

	if( CMusic::bPlay && !FMusic.IsPlay() )		//����ֹͣ�ˣ����ٴβ���
	{
		FMusic.Play();
	}
}

// �ӷ�����
void CFight::UpdateLevel()
{
	int Exp=0;
	for(int i=0; i<4; i++)
	{
		if( EnemyRole[i].bLive )		//�õ����е��˴��ľ���ֵ���ܺ�
			Exp+=Enemy[i].Exp;
	}

	char x[256];
	sprintf(x, "ս��ʤ�������%d�㾭��ֵ��", Exp);
	GameMessage.DoMenu(lpDDSBack, x);	//��ʾʤ��	

	for(i=0; i<3; i++)
	{
		if( FightRole[i].bLive )
		{
			Hero[i]->Exp+=Exp;				//��ɫ�ľ���ֵ����
			int nTemp = Hero[i]->Level;		//��ǰ�����������ж��Ƿ�����
			while( Hero[i]->Exp >= Hero[i]->MaxExp )	//������������Ҫ�ľ���ֵ������
			{											//��Ϊ����һ��������������ѭ��
				Hero[i]->Level++;						
				//������һ������Ҫ�ľ���ֵ
				Hero[i]->MaxExp += 10*(Hero[i]->Level+1)*(Hero[i]->Level)+30;
				Hero[i]->MaxHp+=200;						//HP��20
				Hero[i]->Hp=Hero[i]->MaxHp;				//��HP����
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
			if(Hero[i]->Level>nTemp)		//��ʾ
			{
				sprintf(x, "<c2>%s</c> �ȼ���Ϊ <c2>%d</c> ��", Hero[i]->Fight_Ini_Name, Hero[i]->Level );
				//FadeIn(2, 20, RGB16(0,255,255));
				GameMessage.DoMenu(lpDDSBack, x);
			}
		}
	}
}

//��������
int CFight::GetEnemyNum()
{
	int count=0;
	for(int i=0; i<4; i++)
	{
		if( EnemyRole[i].use )			//�õ����˵���Ŀ
			count++;
	}
	return count;
}

//���һ�����˱��
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

//���һ���ҷ����
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

//��ʾ���˺��ҷ�
void CFight::Show_Role_Enemy()
{
	static int c=100, n=4;	//������ʾ������green��ֵ�����ɶ�����ʽ
	
	c+=n;
	if( c >= 250 )
	{
		n=-4;
	}
	if( c <= 128 )
	{
		n=4;
	}

	// ��ɫ����ʾ
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)			//��λ���е���
		{
			Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],true);

			// ��ʾ����
			if( main_count == USER_WHO && nNonce == i )
				Edge_Blt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ], ColorKey16, RGB16(0,c,0));
		}
	}

	//������ʾ
	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].use==1)
		{
			Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], true);

			// ��ʾ����
			if( main_count == KILL_WHO && nNonce == i )
				Edge_Blt(lpDDSBack,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, RGB16(c,0,0));
		}
	}

}

//��ʾ�����̣Уȣ��ĵ��˺��ҷ�
void CFight::Show_Role_Enemy_Alpha()
{
	static int c=100, n=4;		//���ϸ�����һ�����ô�
	
	c+=n;
	if( c >= 250 )
	{
		n=-4;
	}
	if( c <= 128 )
	{
		n=4;
	}

	// ��ɫ����ʾ
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)			//�н�ɫ
		{
			if(!FightRole[i].ready)		//������ʱ��λ�á�ͼƬ��������ο�LOOP����
			{
				if(FightRole[i].stop_temp==10)
					ABlt(lpDDSBack,FightRole[i].x1,FightRole[i].y1,FightRole[i].surf,RMove_Pic,ColorKey16, 16);
				else
					ABlt(lpDDSBack,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],ColorKey16, 16);
			}
			else						//δ������ʱ��
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

	//������ʾ
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
//�õ���ʱ��Ļ��棬û������FPS
void CFight::GetFightGameSurface(LPDIRECTDRAWSURFACE surf)
{
	//������ͼƬ
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

	// ��ɫ����ʾ
	for(int i=0;i<3;i++)
	{
		if(FightRole[i].use==1)
		{
			Blt(surf,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ],true);

			// ��ʾ����
			if( main_count == USER_WHO && nNonce == i )
				Edge_Blt(surf,FightRole[i].x,FightRole[i].y,FightRole[i].surf,FightRole[i].Rstop[ FightRole[i].stop_temp-1 ], ColorKey16, RGB16(0,c,0));
		}
	}

	//������ʾ
	for(i=0;i<4;i++)
	{
		if(EnemyRole[i].use==1)
		{
			Blt(surf,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], true);

			// ��ʾ����
			if( main_count == KILL_WHO && nNonce == i )
				Edge_Blt(surf,EnemyRole[i].x,EnemyRole[i].y,EnemyRole[i].surf,EnemyRole[i].Rstop[ EnemyRole[i].stop_temp-1 ], ColorKey16, RGB16(c,0,0));
		}
	}

	
	//�����ڵ���ͼͼ����ʾ
	RECT RBuild={0, 0, 640, 239};

	Blt(surf, 0, 260, background_build[0], RBuild, true);

	//����ͷ����״̬����ʾ����
	RECT Rstatus_pic={0,0,170,120};
	RECT Rtemp={0,0,50,50};

	if(FightRole[0].status==1)
	{
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(surf, 58, 434, 158, 439, COLOR_BROOD_NULL);
		Bar(surf, 58, 434, 58+FightRole[0].BroodLong, 439, COLOR_BROOD);
		// MPѪ�۵ĳ��̣��ݶ�
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
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(surf, 236, 434, 336, 439, COLOR_BROOD_NULL);
		Bar(surf, 236, 434, 236+FightRole[1].BroodLong, 439, COLOR_BROOD);
		// MPѪ�۵ĳ��̣��ݶ�
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
		// HPѪ�۵ĳ��̣��ݶ�
		Bar(surf, 416, 434, 516, 439, COLOR_BROOD_NULL);
		Bar(surf, 416, 434, 416+FightRole[2].BroodLong, 439, COLOR_BROOD);	
		// MPѪ�۵ĳ��̣��ݶ�
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

//��ͣ����
void CFight::PauseMusic()
{
	FMusic.Pause();
}
//��������
void CFight::ResumeMusic()
{
	FMusic.Resume();
}
//�Ƿ��ڲ�������
BOOL CFight::IsPlayingMusic()
{
	return FMusic.IsPlay();
}
//ʹ��ħ��
void CFight::MagicEffect(int Num, stRolePropertiy * Propertiy,stFightRole * enemy,int num)
{
	//����Ҫ�ǶԵ���ʹ�õģ�Ȼ��ħ��ֵҪ�㹻
	if(CMagic::MAGIC[Num].Class==MC_ENEMY&&CMagic::MAGIC[Num].Mp<=Propertiy->Mp)
	{
		if(CMagic::MAGIC[Num].Attack<0)	//�����ʾ�ǰٷֱȣ������Լ���������
			enemy->UnAttack = (-CMagic::MAGIC[Num].Attack)*enemy->UnAttack/100;
		else
			enemy->UnAttack += CMagic::MAGIC[Num].Attack;
		if(!m_bBoss||!m_bHidden)		//���Ƕ�bossս�����߲��ǽ���������ģʽ
			//����ħ���ֿ�
		{
			enemy->UnAttack = (int)(enemy->UnAttack*(100-Enemy[num].MagicDef)/100.0f);
			m_bHidden = true;
		}

		Propertiy->Mp-=CMagic::MAGIC[Num].Mp;
	}
}
// ��ʾ��Ѫ
void CFight::Print_Lifeup()
{
	m_nLifeupCountMove ++;			//��ʾ��ֵ�����ƶ�
	if(m_nLifeupCountLife > 0)		//��Ȼ��Ҫ��ʾ
	{
		//��ʾ�Լ���Ѫ
		DWORD color = font.GetColor();
		font.SetColor(RGB(255,0,0));
		font.PrintText(lpDDSBack, FightRole[Command[kill_count].kill_who].x+20,
			FightRole[Command[kill_count].kill_who].y-20-m_nLifeupCountMove,"+ %d",FightRole[Command[kill_count].kill_who].Lifeup);
		font.SetColor(color);
		m_nLifeupCountLife --;		//��ʾʱ�����
	}
}
// �Ƿ���bossս
void CFight::SetBossFight(BOOL flags)
{
	m_bBoss = flags;
}
