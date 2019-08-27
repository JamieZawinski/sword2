//********************************************
//	��ͼ ��غ���
//  ������2000��3��14��
//********************************************

#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include <list>
using namespace std;

//����CELL
struct stAniCell
{
	int page;		//ҳ��
	int cell;		//ͼ��
	int time;		//��ʱ
};

class CNpc;
class CGpFont;

typedef vector<stAniCell>	vectorAniCell;
typedef vector<stAniCell>::iterator	itAniCell;
typedef vector<CNpc*> vectorNpc;
typedef vector<CNpc*>::iterator itNpc;

#define MAP_HEAD "Gold Point Sword II MapFile V1.0"

#define BLOCK_CELL 4095	//�ո���
#define BLOCK_CP 127	//������
#define MAX_HOOK 32		//�����������
#define CP_CHANGE 1		//�л���

//=============
//��ͼ��
class CMap
{
public:

	//������
	enum ObjLevel
	{
		OL_UP=1,		//��������
		OL_NORMAL=0,	//����ͬ��
	};

	//���ӽṹ
	struct stCell
	{
		unsigned GroundPic:6;	//����ҳ���ţ���ͨ0-59  ��̬60-63��
		unsigned Ground:12;		//������
		unsigned ObjPic:6;		//����ҳ���ţ���ͨ0-59  ��̬60-63��
		unsigned Obj:12;		//������
		unsigned Obj2Pic:6;		//����2ҳ���ţ���ͨ0-59  ��̬60-63��
		unsigned Obj2:12;		//����2���
		unsigned Block:1;		//�Ƿ��赲
		unsigned Level:4;		//�������ڲ��(0-15)
		unsigned CP:7;			//����
		unsigned Level2:4;		//����2���ڲ��(0-15)
		unsigned MouseType:3;	//�������
		unsigned CPType:1;		//��������
		unsigned res:13;		//����
	};	

	static vectorAniCell m_vAniCell[4096];	// ����CELL
	static int m_nAniCount[4096];			// �����Ľ���
	static int m_nAniFrame[4096];			// ������ǰ��

public:	
	static int AniCellNum;			//����CELL����
	static stAniCell *AniCell;		//����CELL
	long GetAniCellPage(const long ground, RECT& rect);	// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT
	long GetAniCellPageObj(const long ground, RECT& rect);	// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT
	long GetAniCellPageObj2(const long ground, RECT& rect);	// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT
	long GetAniPage(const long num, RECT& rect);		// ��ȡ��̬ͼ�صĵ�ǰҳ��RECT
	long AniLoop();					// ������һ��

	int Id;				//���
	char Name[32];		//����
	int Width, Height;	//��С ��λ������
	stCell *Cell;		//����

	int SX,SY;		//������������������(����)
	int DX,DY;		//���ɳ���ʱ����������ƫ��(���� -32<xxx<=0 )
	int sx,sy;		//�������������Ͼ�������(���� sx=SX*32-DX)

	int startx, starty;	//��ͼʱ����ʼ����
	int endx, endy;		//��ͼʱ�Ľ�������

	int Hook_Num;					//��������
	char Hook[MAX_HOOK][64];		//�����б�

	vectorNpc m_vNpc;		//NPC
	vectorNpc m_vSortNpc;

	int ScrollX, ScrollY;	//����ͼ��Ŀ���
	int ScrollStep;			//���ķ���

	CGpFont ShowNameFont;	//��ͼ������ʾ�������
	int ShowNameCount;		//��ͼ������ʾ������(ÿһ���һ)
	int ShowNameAlpha;		//��ͼ������ʾalpha��

	char CurrentMapFileName[MAX_PATH];	//��ǰ��ͼ�ļ���
	char CurrentMapFilePath[MAX_PATH];	//��ǰ��ͼ·��

	char m_strBackBmpFilename[128];	// ����ͼƬ����
	LPDIRECTDRAWSURFACE m_pBackBmp;	// ����ͼƬ

public:
	CMap();		//���죨��д��ʾ�����б�
	~CMap();	//����

	void LoadAniCell(char *file, char *index);	//���붯̬Ԫ��

	bool InitMap(int,int);	//��ʼ��һ�ŵ�ͼ
	bool LoadMap(char *);	//װ���ͼ
	bool SaveMap(char *);	//�����ͼ
	void Load(char *);		//�������
	void Save(char *);		//�������
	void ShowMap(bool active=true,LPDIRECTDRAWSURFACE suft = lpDDSBack);//��ʾ��ͼ
	void ShowEditMap();		//��ʾ�༭ʱ�ĵ�ͼ

	bool ChangeScene(int cp, bool bCenter=true);	//�����л�(cp=�л�����)
	void FadeIn(int Style=2, int time=50, WORD color=0);	//����
	void FadeOut(int Style=2, int time=50);					//����

	void CenterMap(int, int);		//���õ�ͼ���ĵ�
	void ScrollTo(int, int, int);	//����ͼ��ĳ��
	void ScrollLoop();				//��ѭ��

	void CountDisplayGrid();		//������ʾ����

	void AddHook(int x, int y, char *string);	//��ӵ�ͼ����
	void SetHook(int x, int y, int num);		//���õ�ͼ����

	void SetCell(int level, int x, int y, int num, int page);	//���õ�ͼĳ�����
	void SetBlock(int x, int y, int block);						//���õ�ͼ�赲
	void SetLevel(int x, int y, int level1, int level2);		//���õ�ͼ���

	bool LoadIniNpcList(char *);	//��Ini�ļ���ȡNPC�б�
	int GetNpcId(char *);			//�����ֻ�ȡNPC�ı��
	int AddNpc(char *, char *);		//����һ��NPC(���ر��)
	int DelNpc(int);				//ɾ��һ��NPC
	void ClearNpc();				//���NPC
	void SortNpc();					//����NPC

	int _GetCell(int,int,int);		//��ȡһ�������ڵľ��Ը���
	int _GetCellX(int,int);			//��ȡһ�������ڵĸ���X
	int _GetCellY(int,int);			//��ȡһ�������ڵĸ���Y
	int GetCell(int,int);			//��ȡһ�������ڵĸ���
	int GetCellX(int,int);			//��ȡһ�������ڵĸ���X
	int GetCellY(int,int);			//��ȡһ�������ڵĸ���Y
	RECT GetRectangle(int);			//��ȡһ�����ӵ�����
	RECT _GetRectangle(int,int);	//��ȡһ�����ӵľ�������

	void FillCell(int, int, WORD);	//���һ������

	void CreateFullMap(LPDIRECTDRAWSURFACE lpSurf, int lSurfWidth, int lSurfHeight);	//�����ŵ�ͼ���Ƶ�lpSurf
	void ShowFullMap();									//��ʾ���ŵ�ͼ
	void SaveFullMap(char* filename, float fPer=1.f);	// �������ŵ�ͼ

	void RestoreLevel(int n=0);		//�ָ����в�ι�ϵ
	void RestoreBlock(int n=0);		//�ָ������赲��ϵ
	void RestoreMouseType(int n=0);	//�ָ��������״̬��ϵ

////////////////////////////////////////////////////////////////
// �����б�
////////////////////////////////////////////////////////////////
private:
	BOOL m_bScrollRight;	// ���Ҿ��ͼ
	BOOL m_bScrollDown;		// ���¾��ͼ
	int m_pEnemyList[16];	// ���˱���б�
	int m_nEnemyNum;		// ��������
	int m_nEnemyFrequency;	// ���˳���Ƶ��	(0-100)

public:
	bool* Block;			//�ƶ�ʱ����赲����
	void SetCPType(int x,int y,int type);//������������
	int GetEnemyNum()				{ return m_nEnemyNum; }
	void SetEnemyNum(int n)			{ m_nEnemyNum = n; }
	int GetEnemyList(int n)			{ return m_pEnemyList[n]; }
	void SetEnemyList(int n, int m)	{ m_pEnemyList[n] = m; }
	void ClearEnemyList()			{ memset(m_pEnemyList, 16*4, 0); }

	int GetEnemyFrequency()			{ return m_nEnemyFrequency; }
	void SetEnemyFrequency(int n)	{ m_nEnemyFrequency = n; }
};

#endif