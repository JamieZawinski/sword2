
//--------------------------------------------------------------------
//	ħ�� ��غ���
//  ������2002��4��25��
//--------------------------------------------------------------------

#ifndef _MAGIC_H_
#define _MAGIC_H_
//ħ������
enum MAGICCLASS{
	MC_SELF=0,			//�Լ�
	MC_ENEMY,			//�Ե���
	MC_OHTER,			//����
};
struct stMagic{
	int Id;					//���
	char Name[32];			//����
	char About[128];		//����
	int Class;				//����

	int Hp;			//����
	int MaxHp;		//�������ֵ
	int Mp;			//ħ��

	int Attack;		//����
	int Defend;		//����

	int Magic;		//ħ��
	int Stamina;	//����
	int Luck;		//����
	int Speed;		//����
	int Bp;			//����
	int Ip;			//����
	char PicFileName[64];		//ͼƬ�ļ���
	LPDIRECTDRAWSURFACE Pic;	//ͼƬ
	char EffectPicFileName[64];		//ͼƬ�ļ���
	LPDIRECTDRAWSURFACE EffectPic;	//Ч��ͼƬ
	char MagicFileName[64];		//ս��ʱ�������ļ���
	char SoundFileName[64];		//ս��ʱ�����������ļ���
};

//-----------ħ��---------------------
class CMagic : public CListWindow 
{
public:
	static int GetMagicNum(char * name);	// �õ�ħ��id
	int MagicEffect(int Num, CRole *Role,CRole *RoleReceive);	// ħ��Ч��
	void UseMagic(int sel,int sel_role);	// ʹ��ħ��
	void ShowMagicListWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow = true, int left =-1, int top=-1);	// ��ʾħ������
	void LoadMagicIni(char * filename);		// ��ʼ��ħ������
	void ShowAboutWindow(LPDIRECTDRAWSURFACE surf, bool bShowWindow);	// about����
	void DoMenu(LPDIRECTDRAWSURFACE surf, bool bShow);	
	static int MAX_MAGIC;			//ħ������
	static stMagic *MAGIC;			//ħ���б�
	static RECT MagicRect;			//ħ��ͼƬ��С
	static int PicWidth, PicHeight;	//ħ��ͼƬ�Ĵ�С
	CWindow AboutWindow;			//˵������

	CMagic();
	void Init(LPDIRECTDRAWSURFACE);						//��ʼ������
	void LoadWindowIni(char *filename, char *index);	// ��ʼ������
	virtual ~CMagic();
};

#endif 