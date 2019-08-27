//********************************************
//	��ɫ ��غ���
//  ������2000��4��14��
//********************************************

#ifndef _ROLE_H_
#define _ROLE_H_

#define CenterX 304
#define CenterY 240
#define MAXGOODS 64
#define MAXARM 32

//����Ĳ����ṹ
struct stRolePropertiy{
	int Level;		//�ȼ�
	int Exp;		//����
	int MaxExp;		//����һ������ľ���

	int Hp;			//����
	int MaxHp;		//�������ֵ
	int Mp;			//ħ��
	int MaxMp;		//ħ�����ֵ

	int Attack;		//����
	int Defend;		//����

	int Magic;		//ħ��
	int Stamina;	//����
	int Luck;		//����
	int Speed;		//����
	int Bp;			//����0%-100%
	int Ip;			//����0%-100%

	int Money;		//��Ǯ
	WORD Arms;		//����
	WORD Corselet;	//����

	int CurGoodsNum;			//��Ʒ����
	short Goods[MAXGOODS];		//��Ʒ�����ɴ�64�֣�
	short GoodsNum[MAXGOODS];	//ÿ����Ʒ������

	int CurArmNum;				//װ������
	short Arm[MAXARM];			//װ�������ɴ�32�֣�
	short ArmNum[MAXARM];		//ÿ��װ��������

	int MagicsNum;				//��ǰħ������
	short Magics[10];			//װ����ħ��

	char Fight_Ini_Name[64];	// ս���н�ɫͼƬ���ݵ�ini�ļ�
	int use;					// �����жϴ˽�ɫ�Ƿ�ʹ��
};

//����״̬
struct stRoleState{
	//------״̬����----------
	int x, y;			//��ǰ���� �ŵ����ĵ�
	int oldx,oldy;		//��һ��������
	int X, Y;			//���ڸ�������
	int oldX, oldY;		//��һ��
	byte CurrentStep;	//��ǰ����
	byte CurrentDir;	//��ǰ����
	byte LastDir;		//��һ���ķ���
	BOOL bActive;		//�Ƿ�
	BOOL bShow;			//�Ƿ�ɼ�
	
	LPDIRECTDRAWSURFACE Surface;			//���ߵ�ҳ��
	LPDIRECTDRAWSURFACE lpShadow;			//Ӱ��ҳ��
	int SW, SH;			//Ӱ�Ӵ�С
	char SurfaceFileName[32];	//��Ӧ�ļ�
};

//   2
// 1   3
//   0

//��ɫ��
class CRole
{
public:
	//------ͨ������----------
	int Id;				//��ɫ���
	char Name[32];		//����
	int Width, Height;	//���ųߴ�
	byte Max_Step;		//ÿ���������ߵĲ���
	byte Max_Dir;		//������
	byte StepList[4];	//����ͼƬ�任�б�
	byte DirList[4];	//���߷����б�
	POINT FootRect[4];	//�ŵ���ײ����
	int StepSize;		//ÿ���ߵľ���
	int StepSize2;		//б���ߵľ���

	char ScriptFile[64];				//��Ӧ�ű�
	stRolePropertiy Propertiy;		//��������
	stRoleState State;				//����״̬

	//------�ƶ�����-----------
	POINT *Path;		//����·��
	int PathSteps;		//�ܲ���
	int PathCurrentStep;//��ǰ����
	int DesX, DesY;		//����Ŀ���
	bool bMoving;		//�Ƿ������ƶ���־
	int StepStep;		//�����ж�
	BOOL bCuted;		//·���Ƿ񱻼���
	int MoveX, MoveY;	//��ǰ��������仯
	bool bShowed;		//�Ƿ��Ѿ���ʾ��һ��
	int oldAttack;
	int oldDefend;

public:
	void ReCalcPro();	//���¼��㹥���ͷ���
	void DelMagic(char *name);	//ɾ��ħ��
	void AddMagic(char* name);	//���һ��ħ��
	BOOL bCancelTalk;	//�Ƿ�ȡ����NPC�ĶԻ�
	void AdjustPosition();
	CRole();			//����
	~CRole();			//����
	void Release();		//�ͷ�
	bool InitRole();	//��ʼ��һ����ɫ
	bool LoadRoleIni(char *, char *);	//��ȡ��ɫ�趨
	bool CreateShadow();				//����������Ӱ

	bool LoadRole(char *);	//��ȡһ����ɫ
	bool SaveRole(char *);	//����һ����ɫ
	void ShowRole();		//��ʾһ����ɫ
	void ShowRole(int);		//alpha��ʾһ����ɫ
	void Move(int,int);		//��������
	void Show(int,int);		//��ʾ-������ײ����
	void ShowRole(int, int, int, int);	//��ָ��λ����ʾһ��ָ���Ľ�ɫ

	bool MoveTo(int, int);	//�ƶ���ĳ��
	void ChangeScene();		//�����л�
	void RunScript();		//��������

	void Goto(int, int);	//�˶���ĳ�㣨ѭ����

	//�������
	void AddObject(char *, int num=1);			//����һ������
	void DelObject(char *, int num=1);			//ɾ��һ������
	int CheckObject(char *);				//���һ����Ʒ������

	//װ�����
	void AddArm(char *, int num =1);			//����һ��װ��
	void DelArm(char *, int num=1);			//ɾ��һ��װ��
	int CheckArm(char *);				//���һ��װ��������
	void SetupArm(char *);				//װ��
	void SetupArm(int );				//װ��
	void UnSetupArm(char *);			//ж��װ��
	void UnSetupArm(int );			    //ж��װ��
};

#endif