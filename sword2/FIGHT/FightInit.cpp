//   ******************************
//   *      ս��ϵͳ�ຯ��
//   *     ������ 5��29��
//   ******************************
//  ************************************
//  *     ս��ϵͳ�ĳ�ʼ�������ļ� 
//  *   ������  8��10��  
//  ************************************

#include <stdio.h>
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\menu\menu_inc.h"
#include "..\main.h"
#include "..\map.h"
#include "..\script.h"
#include "..\menu\title.h"
#include "..\Cursor.h"
#include "Fight.h"

int CFight::m_nEnemyNum = 0;				// �������͵�����
CFight::stEnemyList* CFight::m_listEnemy = NULL;	// ���������б�

// ��������б�
void CFight::LoadEnemyList(char* file, char* index)
{
	//��ʼ�����е����б��
	CIniSet Ini(file);
	m_nEnemyNum = Ini.GetContinueDataNum(index);
	if( m_nEnemyNum > 0  )
	{
		m_listEnemy = new stEnemyList[m_nEnemyNum];
		for(int i=0; i<m_nEnemyNum; i++)
		{
			char* str = Ini.ReadText("�����б�", i);
			strcpy( m_listEnemy[i].strName, str );
			_FREE(str);
		}
	}
}

CFight::CFight()
{
	bRandomFight=true;

	CIniSet FightSystemInit("ini\\fight\\FightSystemInit.ini");

	//��ʼ�����Ǻ͵�������
	FightRole[0].ready=1;				//û�н���
	FightRole[0].status=0;				//״̬����
	FightRole[0].x=FightSystemInit.ReadInt("ս����ʼ��","Role1_x");		//վλ	
	FightRole[0].y=FightSystemInit.ReadInt("ս����ʼ��","Role1_y");	
	FightRole[0].use=0;					//��ʹ��
	FightRole[0].stop_temp=1;			//��������
	FightRole[0].surf=FightRole[0].header=NULL;		//ͷ��

	FightRole[1].ready=1;
	FightRole[1].status=0;
	FightRole[1].x=FightSystemInit.ReadInt("ս����ʼ��","Role2_x");
	FightRole[1].y=FightSystemInit.ReadInt("ս����ʼ��","Role2_y");
	FightRole[1].use=0;
	FightRole[1].stop_temp=2;
	FightRole[1].surf=FightRole[1].header=NULL;

	FightRole[2].ready=1;
	FightRole[2].status=0;
	FightRole[2].x=FightSystemInit.ReadInt("ս����ʼ��","Role3_x");
	FightRole[2].y=FightSystemInit.ReadInt("ս����ʼ��","Role3_y");
	FightRole[2].use=0;
	FightRole[2].stop_temp=3;
	FightRole[2].surf=FightRole[2].header=NULL;

	EnemyRole[0].use=0;
	EnemyRole[0].ready=1;
	EnemyRole[0].status=0;
	EnemyRole[0].x=FightSystemInit.ReadInt("ս����ʼ��","Enemy1_x");
	EnemyRole[0].y=FightSystemInit.ReadInt("ս����ʼ��","Enemy1_y");
	EnemyRole[0].stop_temp=1;
	EnemyRole[0].surf=NULL;

	EnemyRole[1].use=0;
	EnemyRole[1].ready=1;
	EnemyRole[1].status=0;
	EnemyRole[1].x=FightSystemInit.ReadInt("ս����ʼ��","Enemy2_x");
	EnemyRole[1].y=FightSystemInit.ReadInt("ս����ʼ��","Enemy2_y");
	EnemyRole[1].stop_temp=2;
	EnemyRole[1].surf=NULL;

	EnemyRole[2].use=0;
	EnemyRole[2].ready=1;
	EnemyRole[2].status=0;
	EnemyRole[2].x=FightSystemInit.ReadInt("ս����ʼ��","Enemy3_x");
	EnemyRole[2].y=FightSystemInit.ReadInt("ս����ʼ��","Enemy3_y");
	EnemyRole[2].stop_temp=3;
	EnemyRole[2].surf=NULL;

	EnemyRole[3].use=0;
	EnemyRole[3].ready=1;
	EnemyRole[3].status=0;
	EnemyRole[3].x=FightSystemInit.ReadInt("ս����ʼ��","Enemy4_x");
	EnemyRole[3].y=FightSystemInit.ReadInt("ս����ʼ��","Enemy4_y");
	EnemyRole[3].stop_temp=1;
	EnemyRole[3].surf=NULL;

	//��ʼ��ָ��
	Command[0].player=1;
	Command[1].player=1;
	Command[2].player=1;
	Command[0].kill_who=1;
	Command[1].kill_who=1;
	Command[2].kill_who=1;
	command_count=0;

	Enemy_Command[0].player=0;
	Enemy_Command[1].player=0;
	Enemy_Command[2].player=0;
	Enemy_Command[3].player=0;
	Enemy_Command[0].kill_who=1;
	Enemy_Command[1].kill_who=1;
	Enemy_Command[2].kill_who=1;
	Enemy_Command[3].kill_who=1;

	main_count=MAIN;		//��ʼ������ѭ�� 
	nNonce = 0;				//Ҫ�����Ķ���

	Role_Temp = NULL;

	m_nAttackCountMove = 0;
	m_nAttackCountLife = 0;
	m_nLifeupCountMove = 0;
	m_nLifeupCountLife = 0;

	m_bBoss = false;
	m_bHidden = false;
	m_nRoundCount = 1;
	m_nMagicCount = 1;
}

CFight::~CFight()
{
	_DELETE_ARRAY( m_listEnemy );
}

//��ʼ��
void CFight::Init(void)
{
	CreateBitmap(status_pic, 0, 0, "pic\\fight\\status.bmp");//����״̬��Ѫ����
	DDSetColorKey(status_pic,RGB(255,0,255));

	//������
	cursor_role.LoadAni("ini\\fight\\cursor.ini", "ѡ�˹��");
	cursor_enemy.LoadAni("ini\\fight\\cursor.ini", "���˹��");

	//��ʼ����ť
	Button[0].LoadIni("ini\\fight\\button.ini", "�ظ�");
	Button[1].LoadIni("ini\\fight\\button.ini", "����");
	Button[2].LoadIni("ini\\fight\\button.ini", "ħ��");
	Button[3].LoadIni("ini\\fight\\button.ini", "����");
	Button[4].LoadIni("ini\\fight\\button.ini", "����");

	font.InitFont(16, "����");

	//����	
	FMusic.g_pMIDISeg=NULL;
	WCHAR* str = WChar("music\\fight.mid");
	FMusic.LoadMusic(str);
	_DELETE( str );

	//��Ч
	Sound_Click.LoadSoundFX(hWnd, "sound\\fight\\click.wav");
	Sound_High.LoadSoundFX(hWnd, "sound\\fight\\high.wav");
	
	//���߳�ʼ��
	m_fightgoods.LoadWindowIni("ini\\fight\\fightgoodslist.ini", "��Ʒ�б��");
	//ħ����ʼ��
	m_fightmagic.LoadWindowIni("ini\\fight\\fightmagiclist.ini", "ħ���б��");
	//ս�������ʼ��
	char *strTemp = GameIni.ReadText("�����ļ�","����");
	FightFont.LoadFont(strTemp, "ս������");
	_FREE(strTemp);
}

//ս��������������
void CFight::Clear(void)
{
	KillTimer(hWnd, 2);

	main_count=MAIN;
	command_count=0;
	enemy_count=0;
	temp_swap=0;
	kill_count=0;

	m_bBoss = false;
	m_bHidden = false;

	for(int i=0; i<3; i++)
	{
		FightRole[i].ready=1;
		FightRole[i].status=0;
		FightRole[i].use=0;
		FightRole[i].stop_temp=i+1;
		Command[i].player=1;
		_RELEASE( FightRole[i].surf );
		_RELEASE( FightRole[i].header );
	}

	for(i=0; i<4; i++)
	{
		EnemyRole[i].use=0;
		EnemyRole[i].ready=1;
		EnemyRole[i].status=0;
		EnemyRole[i].stop_temp=i+1;
		Enemy_Command[i].player=0;
		_RELEASE( EnemyRole[i].surf);
	}

	//�ͷű���ͼ
	for(i=0; i<Number; i++)
	{
		_RELEASE( background[i] );
	}
	_RELEASE(background_build[0]);
	_DELETE(background_build);
	_DELETE( background );
}

// background:��ս�۵ı���ͼƬ�Լ����ݵ������ļ���
// enemy_1��enemy_4���ĸ����˵�����
// ���enemy1~4��ֵΪNULL���Ǹ�λ��û�е���
CFight::Start(char *bg_name,
			 stRolePropertiy * role_1,
			 stRolePropertiy * role_2,
			 stRolePropertiy * role_3,
			 char *enemy_1,
			 char *enemy_2,
			 char *enemy_3,
			 char *enemy_4)
{
	int ret=0;
	m_nRoundCount = 1;			//�غϼ�����1
	m_nMagicCount = 1;
	g_ePlayState=PS_FIGHT;		//����ս��ѭ����־

	//ս������
	Music.Pause();				//ֹͣԭ��������
	FMusic.Play();				//��ʼս������
	// ����ͼƬ���������ȡ
	CIniSet FightBackGroundInit(bg_name);
	Number = FightBackGroundInit.ReadInt("��������","Number");

	background = new LPDIRECTDRAWSURFACE[Number];
	background_build = new LPDIRECTDRAWSURFACE[1];	// ս��ʱ�ı����ڵ���ǰ�����ľ

	char* strTemp = FightBackGroundInit.ReadText("��������","Background_Name");
	CreateBitmap(background[Number-1], 0, 0, strTemp);
	_FREE(strTemp);

	CreateBitmap(background_build[0], 0, 0, "pic\\fight\\background_build.bmp");
	DDSetColorKey(background_build[0],RGB(255,0,255));

	// ��ɫ�Ķ�ȡ
	FightRoleInit.Open("ini\\fight\\FightRoleInit.ini");

	for(int i=2; i>=0; i--)
	if( i==0 && role_1 || i==1 && role_2 ||	i==2 && role_3 )
	{
		FightRole[i].bLive=true;
		command_count=i;				//��ʼ����������ǰ�������
		switch( i )	
		{
		case 0: Hero[i]=role_1; break;	//������������
		case 1: Hero[i]=role_2; break;
		case 2: Hero[i]=role_3; break;
		}
		FightRole[i].use=1;				//ʹ�������ɫ
		FightRole[i].status=1;			

		strTemp = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Header");//ͷ��
		CreateBitmap(FightRole[i].header,0,0, strTemp);
		_FREE(strTemp);

		strTemp = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Role_Pic_File");//�������ͼƬ
		CreateBitmap(FightRole[i].surf,0,0, strTemp);
		_FREE(strTemp);

		strTemp = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Move_Ini");
		Role_Move[i].Open(strTemp);//�����ƶ�����
		_FREE( strTemp );

		strTemp = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name,"Tool_Ini");
		Role_Tool[i].Open(strTemp);
		_FREE(strTemp);

		DDSetColorKey(FightRole[i].surf, ColorKey);
		
		//Ѫ�ۡ����۵ĳ���
		FightRole[i].BroodLong=Hero[i]->Hp * 100 / Hero[i]->MaxHp;
		FightRole[i].MagicLong=Hero[i]->Mp * 100 / Hero[i]->MaxMp;
		//��ֹ�Ķ�����������
		FightRole[i].Rstop[0].left=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"1��ֹ��λͼ_x");
		FightRole[i].Rstop[0].top=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"1��ֹ��λͼ_y");
		FightRole[i].Rstop[0].right=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"1��ֹ��λͼ_x1");
		FightRole[i].Rstop[0].bottom=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"1��ֹ��λͼ_y1");

		FightRole[i].Rstop[1].left=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"2��ֹ��λͼ_x");
		FightRole[i].Rstop[1].top=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"2��ֹ��λͼ_y");
		FightRole[i].Rstop[1].right=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"2��ֹ��λͼ_x1");
		FightRole[i].Rstop[1].bottom=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"2��ֹ��λͼ_y1");

		FightRole[i].Rstop[2].left=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"3��ֹ��λͼ_x");
		FightRole[i].Rstop[2].top=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"3��ֹ��λͼ_y");
		FightRole[i].Rstop[2].right=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"3��ֹ��λͼ_x1");
		FightRole[i].Rstop[2].bottom=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"3��ֹ��λͼ_y1");

		FightRole[i].Rstop[3].left=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"4��ֹ��λͼ_x");
		FightRole[i].Rstop[3].top=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"4��ֹ��λͼ_y");
		FightRole[i].Rstop[3].right=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"4��ֹ��λͼ_x1");
		FightRole[i].Rstop[3].bottom=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"4��ֹ��λͼ_y1");
		//������ͼƬ����
		FightRole[i].Rbruise.left=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"������λͼ_x");
		FightRole[i].Rbruise.top=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"������λͼ_y");
		FightRole[i].Rbruise.right=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"������λͼ_x1");
		FightRole[i].Rbruise.bottom=FightRoleInit.ReadInt(Hero[i]->Fight_Ini_Name,"������λͼ_y1");

		//��������
		char*  strSound1 = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name, "sound_Fight");
		char*  strSound2 = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name, "sound_Defend");
		char*  strSound3 = FightRoleInit.ReadText(Hero[i]->Fight_Ini_Name, "sound_Dead");
		FightRole[i].sound_Fight.LoadSoundFX(hWnd, strSound1);
		FightRole[i].sound_Defend.LoadSoundFX(hWnd, strSound2);
		FightRole[i].sound_Dead.LoadSoundFX(hWnd, strSound3);
		_FREE(strSound1);
		_FREE(strSound2);
		_FREE(strSound3);
	}
	else
		FightRole[i].bLive=false;

//---------------------------------------------------------------
//***************************************************************
//---------------------------------------------------------------

	// �������ݵĶ�ȡ����
	EnemyInit.Open("ini\\fight\\enemy.ini");

	for(i=3; i>=0; i--)
	{
		Enemy[i].Hp = 0;		//��ʼ����0
		if( i==0 && enemy_1 || i==1 && enemy_2 || i==2 && enemy_3 || i==3 && enemy_4 )
		{
			EnemyRole[i].bLive=true;
			char *enemy;
			switch( i )
			{
			case 0:	enemy=enemy_1; break;
			case 1:	enemy=enemy_2; break;
			case 2:	enemy=enemy_3; break;
			case 3:	enemy=enemy_4; break;
			}
			
			Enemy[i].Hp=EnemyInit.ReadInt(enemy, "Hp");
			Enemy[i].MaxHp=EnemyInit.ReadInt(enemy, "MaxHp");
			Enemy[i].Attack=EnemyInit.ReadInt(enemy, "Attack");
			Enemy[i].Defend=EnemyInit.ReadInt(enemy, "Defend");
			Enemy[i].MagicAct=EnemyInit.ReadInt(enemy, "MagicAct");
			Enemy[i].MagicDef=EnemyInit.ReadInt(enemy, "MagicDef");
			Enemy[i].Bp=EnemyInit.ReadInt(enemy,"Bp");
			Enemy[i].Ip=EnemyInit.ReadInt(enemy,"Ip");
			Enemy[i].Stamina=EnemyInit.ReadInt(enemy,"Stamina");
			Enemy[i].Luck=EnemyInit.ReadInt(enemy,"Luck");
			Enemy[i].Speed=EnemyInit.ReadInt(enemy,"Speed");
			
			Enemy[i].Money=EnemyInit.ReadInt(enemy, "Money");
			Enemy[i].Arms=EnemyInit.ReadInt(enemy, "Arms");
			Enemy[i].Corselet=EnemyInit.ReadInt(enemy, "Corselet");
			Enemy[i].Exp=EnemyInit.ReadInt(enemy, "Exp");
			
			Enemy[i].Act=EnemyInit.ReadInt(enemy,"Act");	//����ͼƬ����
			//��ɫ��ȱʡ���������ʼ����������ǰ��ĵ���
			Command[0].kill_who=i;
			Command[1].kill_who=i;
			Command[2].kill_who=i;
			
			strcpy(EnemyRole[i].name, enemy);
			EnemyRole[i].use=1;
			
			strTemp = EnemyInit.ReadText(enemy, "Role_Pic_File");
			CreateBitmap(EnemyRole[i].surf, 0,0, strTemp);
			_FREE(strTemp);
			DDSetColorKey(EnemyRole[i].surf, ColorKey);
			
			// ����Դ�ļ����ȡλͼ��������
			EnemyRole[i].Rstop[0].left=EnemyInit.ReadInt(enemy,"1��ֹ��λͼ_x");
			EnemyRole[i].Rstop[0].top=EnemyInit.ReadInt(enemy,"1��ֹ��λͼ_y");
			EnemyRole[i].Rstop[0].right=EnemyInit.ReadInt(enemy,"1��ֹ��λͼ_x1");
			EnemyRole[i].Rstop[0].bottom=EnemyInit.ReadInt(enemy,"1��ֹ��λͼ_y1");
			
			EnemyRole[i].Rstop[1].left=EnemyInit.ReadInt(enemy,"2��ֹ��λͼ_x");
			EnemyRole[i].Rstop[1].top=EnemyInit.ReadInt(enemy,"2��ֹ��λͼ_y");
			EnemyRole[i].Rstop[1].right=EnemyInit.ReadInt(enemy,"2��ֹ��λͼ_x1");
			EnemyRole[i].Rstop[1].bottom=EnemyInit.ReadInt(enemy,"2��ֹ��λͼ_y1");
			
			EnemyRole[i].Rstop[2].left=EnemyInit.ReadInt(enemy,"3��ֹ��λͼ_x");
			EnemyRole[i].Rstop[2].top=EnemyInit.ReadInt(enemy,"3��ֹ��λͼ_y");
			EnemyRole[i].Rstop[2].right=EnemyInit.ReadInt(enemy,"3��ֹ��λͼ_x1");
			EnemyRole[i].Rstop[2].bottom=EnemyInit.ReadInt(enemy,"3��ֹ��λͼ_y1");
			
			EnemyRole[i].Rstop[3].left=EnemyInit.ReadInt(enemy,"4��ֹ��λͼ_x");
			EnemyRole[i].Rstop[3].top=EnemyInit.ReadInt(enemy,"4��ֹ��λͼ_y");
			EnemyRole[i].Rstop[3].right=EnemyInit.ReadInt(enemy,"4��ֹ��λͼ_x1");
			EnemyRole[i].Rstop[3].bottom=EnemyInit.ReadInt(enemy,"4��ֹ��λͼ_y1");
			
			EnemyRole[i].Rbruise.left=EnemyInit.ReadInt(enemy,"������λͼ_x");
			EnemyRole[i].Rbruise.top=EnemyInit.ReadInt(enemy,"������λͼ_y");
			EnemyRole[i].Rbruise.right=EnemyInit.ReadInt(enemy,"������λͼ_x1");
			EnemyRole[i].Rbruise.bottom=EnemyInit.ReadInt(enemy,"������λͼ_y1");
			
			//��������
			char* str1 = EnemyInit.ReadText(enemy, "sound_Fight");
			char* str2 = EnemyInit.ReadText(enemy, "sound_Defend");
			char* str3 = EnemyInit.ReadText(enemy, "sound_Dead");
			EnemyRole[i].sound_Fight.LoadSoundFX(hWnd, str1);
			EnemyRole[i].sound_Defend.LoadSoundFX(hWnd, str2);
			EnemyRole[i].sound_Dead.LoadSoundFX(hWnd, str3);
			_FREE(str1);
			_FREE(str2);
			_FREE(str3);
		}
		else
			EnemyRole[i].bLive=false;
	}
	//ħ�����ֵĳ�ʼ��
	for(i=0;i<CMagic::MAX_MAGIC;i++)
	{
		Role_Magic[i].Open(CMagic::MAGIC[i].MagicFileName);
	}
	
	FadeOut(2);			

	//������ʱ��
	SetTimer(hWnd, 2, 250, NULL);
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	//ս��������������������������������������������������������������
	MSG msg;
	while(RunGame)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//��ȡ������Ϣ
			GetInputData();
			
			//��ѭ��
			switch( Loop(mouse, point) )
			{
			case 1:			//���ܳɹ�
				Script.SetValue("$FightRet", 1);
				ret=1;
				goto _Next;
				
			case 2:			//ʤ��
				Script.SetValue("$FightRet", 2);
				ret=2;
				goto _Next;
				
			case 3:			//ʧ��
				Script.SetValue("$FightRet", 3);
				ret=3;
				goto _Next;
			}
			//��ʾ���
			ShowMouseCursor(lpDDSBack, false);
			//��ʾFPS
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();
		}
		else WaitMessage();	//�ȴ���Ϣ
	}

_Next:
	//�ָ�����
	FMusic.Stop();
	Music.Resume();
	g_ePlayState=PS_MAIN;		//�л�����ѭ��
	return ret;
}

//�ȵ���ս��
int CFight::RandomStart(int nFreq, bool bMouse)
{
	int ret = 0;

	//����ȵ���
	random(1200);
	if( bRandomFight && nFreq && random(1200) < nFreq )
	{
		// ��Ļ���ٱ��
		FadeIn(2, 20, RGB16(255,255,255));
		
		//����վλ
		stRolePropertiy *rps[3];
		rps[0]=&role[0].Propertiy;
		if( RoleNum >1 ) 
			rps[1]=&role[1].Propertiy;
		else 
			rps[1]=NULL;

		if( RoleNum >2 ) 
			rps[2]=&role[2].Propertiy;
		else 
			rps[2]=NULL;

		//����ѡȡ
		char *r1, *r2, *r3, *r4;
		int num=random(100);
		int EnemyNum = map.GetEnemyNum();
		
		// һ������
		//����վλ
		//		1
		//	3		2
		//		4
		//
		if( num < 40 )
		{
			r1=NULL;
			r2=NULL;
			r3=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r4=NULL;
		}
		// ��������
		else if( num < 70 )
		{
			r1=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r2=NULL;
			r3=NULL;
			r4=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
		}
		// ��������
		else if( num < 90 )
		{
			r1=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r2=NULL;
			r3=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r4=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
		}
		// �ĸ�����
		else
		{
			r1=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r2=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r3=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
			r4=GetEnemyName( map.GetEnemyList(random(EnemyNum)) );
		}
		//��������
		bLeave=true;
		//վ��
		ret=Start("ini\\fight\\background1.ini", rps[0], rps[1], rps[2], r1, r2, r3, r4);

		if( ret==1 || ret==2 )	//Win
		{
			g_ePlayState=PS_Null;
			FadeIn(2, 30);
			map.ShowMap(true);	//��ʾԭ���ĵ�ͼ����Ϸ����
			g_ePlayState=PS_MAIN;
		}
		if( ret==3 )	//GameOver
		{
			Music.Stop();
			WCHAR* str = WChar("music\\BGM_lost.mid");
			Music.LoadMusic(str);
			_DELETE(str);
			Music.Play();

			g_ePlayState=PS_Null;
			ColorIn(100, RGB16(0,0,0));
	
			//���뱳��ҳ��
			DDReLoadBitmap(lpDDSBackGround, "pic\\fight\\gameover.bmp");
			Delay(500);
			ColorOut(lpDDSBackGround, 100);

			do
			{
				GetInputData();
			}
			while(!(A_UP||mouse == LB_UP));
			ColorIn(100, RGB16(0,0,0));
			//��ʼ����
			Title.DoMenu(lpDDSBack);
			g_ePlayState=PS_MAIN;
		}
	}
	return ret;
}