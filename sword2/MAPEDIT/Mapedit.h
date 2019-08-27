//********************************************
//	��ͼ�༭ ��غ���
//  ������2000��4��10��
//********************************************

#include "..\map.h"

//�༭����ǰ״̬
enum CURRENTLEVEL{
	CL_GROUND=0,		//����
	CL_OBJECT,			//����
	CL_OBJECT2,			//����ڶ���
	CL_PIC_SEL,			//ѡ��̬ͼ��
	CL_ANI_SEL,			//ѡ��̬ͼ��
};

///////////////////////////////////
//�����༭��ר��
///////////////////////////////////
extern int cx2, cy2;			//��ʱ������

extern bool bShowBackGround;	//��ʾ����
extern bool bShowCell;			//��ʾ���
extern bool bShowObject;		//��ʾ����
extern bool bShowBlock;			//��ʾ�赲��ϵ
extern bool bShowCurrent;		//��ʾ��ǰ����
extern bool bShowEdit;			//��ʾ�༭��ʾ�����塢�л��㡢����
extern bool bShowOnlyObject;	//��ʾ����
extern bool bShowHardCursor;	//�Ƿ���ʾӲ�����
extern bool bShowFps;			//�Ƿ���ʾFPS

extern int Max_GroundPic;		//��ͼ����ҳ����
extern int Cur_GroundPic;		//��ǰѡ�е���ı��
extern int Cur_Ground;			//��ǰѡ�еĵ���
extern int Cur_MouseType;		//��ǰѡ��������ʽ

extern int Max_Level;			//�ܲ���
extern int Cur_Level;			//��ǰ�༭�Ĳ�(0=���� 1=����)

extern char *Level[];

extern CMap::stCell *UndoMap;	//���ڵ�ͼ
typedef CMap::stCell Cell_Struct;
///////////////////////////////////////////////////
//�����б�
///////////////////////////////////////////////////
void EditLoop();					//�༭����ѭ��
void EditGround(char);				//�༭�����
void EditObj(char);					//�༭�����
void SelectGround(char);			//ѡ��̬ͼ��
void SelectAni(char);				//ѡ��̬ͼ��
void ClearEditData();				//��ձ༭����
void ShowCell(LPDIRECTDRAWSURFACE);	//��ʾ����
void PopupMenu();					//�����˵�

void MapEditProc(long msg);			//����༭���Ҽ��˵���Ϣ