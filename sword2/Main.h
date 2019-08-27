//********************************************
//	��ģ�� ��غ���
//  ������2000��4��10��
//********************************************
#include <ddraw.h>

#define ScreenWidth	640		//��Ϸ�����
#define ScreenHeight 480	//��Ϸ���߶�
#define CellWidth 32		//�������ӵĿ��
#define CellHeight 32		//�������ӵĸ߶�

//#define _grid_x(x)	((x)/CellWidth)	//����x�������ĸ���
//#define _grid_y(y)	((y)/CellHeight)	//����y�������ĸ���
//#define _point_x(x)	((x)*CellWidth)	//����X���ӵľ�������
//#define _point_y(y) ((y)*CellHeight)	//����Y���ӵľ�������

#define _grid_x(x)	((x)>>5)	//����x�������ĸ���
#define _grid_y(y)	((y)>>5)	//����y�������ĸ���
#define _point_x(x)	((x)<<5)	//����X���ӵľ�������
#define _point_y(y) ((y)<<5)	//����Y���ӵľ�������

//��Ϸ��ǰ״̬
enum ePLAYSTATE{
	PS_Null=0,		//��ѭ��
	PS_MAIN=1,		//��ѭ��
	PS_DIALOG,		//�Ի�
	PS_SCROLLMAP,	//����ͼ
	PS_EDIT,		//�༭����ѭ��
	PS_FIGHT,		//ս��
};

//��ʾ״
enum eDISPLAYSTATE{
	DS_NORMAL=0,	//��
	DS_NIGHT		//ҹ��
};

/////////////////////////////////
//ͨ�ñ���
/////////////////////////////////
class CMouse;
class CKey;

extern enum ePLAYSTATE g_ePlayState;		//��ǰ״̬
extern enum eDISPLAYSTATE g_eDisplayState;	//��ǰ��ʾģʽ

extern LPDIRECTDRAWSURFACE lpDDSSour, lpDDSTemp;
extern LPDIRECTDRAWSURFACE lpDDSTools;		// �༭��ר��ͼ��ҳ��
extern LPDIRECTDRAWSURFACE lpDDSBackGround;	// ����ҳ��
extern LPDIRECTDRAWSURFACE *lpDDSMap;		// ��ͼ����ҳ��

extern bool bShowFps;				//�Ƿ���ʾFPS
extern POINT point;					//�������
extern int cx,cy;					//������

extern int StartX,StartY;			//������ʼ����
extern int EndX,EndY;				//������������
extern int MainWidth,MainHeight;	//������С
extern int ShowCellW, ShowCellH;	//��ʾ���������

class CScript;
class CMessage;
class CEvents;
class CTitle;
class CMenu;
class CMap;
class CGpFont;
class CRole;
class CTalk;
class CEmote;
class CIniSet;
class CMsgWindow;
class CFight;

extern int RoleNum;				//����������1-4��

extern CIniSet GameIni;			//��Ϸ�����ļ�
extern CScript Script;			//��Ϸ�ű���
extern CMessage Message;		//��Ϣ�����
extern CEvents Events;			//�����¼�
extern CTitle Title;			//����
extern CMenu Menu;				//�˵�
extern CMap map;				//��ͼ
extern CGpFont font;			//�������
extern CRole role[4];			//����
extern CTalk talk;				//�Ի�
extern CEmote emote;			//����
extern CMsgWindow GameMessage;	//��Ϸ��Ϣ��
extern CSound Sound;			//��Ч
extern CMusic Music;			//��������
extern char CurrentPath[MAX_PATH];		//��ǰ����Ŀ¼
extern unsigned mouse, key, joystick;	//��ꡢ���̺��ֱ��Ļ��������ݽ���
extern bool bControl;			//�Ƿ�ɿ���
extern CFight Fight;			//ս��

extern int NowTick, OldTick, DelayTick;	//����
extern int CurrentNpc;					//��ǰ���ָ���NPC���(-1=NULL)

///////////////////////////////////////////////////
//�����б�
///////////////////////////////////////////////////
void NewGame();					//����Ϸ��ʼ������
void GetInputData();			//��ȡ������Ϣ
void GameLoop();				//��Ϸ��ѭ��
void UpdateScreen_Dialog(LPDIRECTDRAWSURFACE);				//������Ļ���Ի�
void ShowMouseCursor(LPDIRECTDRAWSURFACE lpSurf=lpDDSBack, bool Normal=true);	//�������
void GetPrimarySurface(LPDIRECTDRAWSURFACE surf);			//�ѵ�ǰ��Ļ���Ƶ���̨����
void GetGameSurface(LPDIRECTDRAWSURFACE surf);				//����Ϸ���渴�Ƶ�һ�������ϣ���������FPS�ȣ�
