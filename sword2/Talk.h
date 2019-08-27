//********************************************
//	�Ի� ��غ���
//  ������2000��5��01��
//********************************************

#ifndef _TALK_H_
#define _TALK_H_

#include "interface\interface.h"

class CIniSet;
class CAni;

class CTalk : public CWindow{
public:
	char FileName[MAX_PATH];		//�Ի��ļ�
	CIniSet Dialog;			//�Ի�����
	char Index[64];			//�Ի���־
	int CurrentLine;		//��ǰ��
	
	int photox, photoy;		//ͷ��λ�ã���λ�����أ�
	int photox2,photoy2;	//ͷ��λ�ö�
	int photow, photoh;		//ͷ���С����λ�����أ�
	int PhotoPlace;			//ͷ��λ��(0,1)

	char PhotoFilePath[128];	//�Ի�ͷ��ͼƬĿ¼

	LPDIRECTDRAWSURFACE lpBack;	//���浱ǰ��Ļ
	LPDIRECTDRAWSURFACE lpPhoto;//ͷ��

	bool bPhoto;			//�Ƿ���ʾͷ��
	CAni Continue;			//��˸��С��ͷ
	BOOL bFullDialog;		//�Ƿ�ʹ����ͼ�Ի���

	unsigned long NextTick;	//���ֳ���ʱ����ʱ

	//----------------------------------------
	LPDIRECTDRAWSURFACE lpTalk;	//�Ի����ͼ
	RECT TalkRect;				//��Χ
	int TalkX, TalkY;	//λ��
	WORD TalkAlphaColor;		//��͸������ɫ

public:

	CTalk();
	~CTalk();
	bool Init(char *, char *);			//��ʼ���Ի���
	void OpenFile(char *);				//���ļ�
	void CreatePhoto(char *filename);	//����ͷ��
	void ShowPhoto(LPDIRECTDRAWSURFACE, int bPhotoInleft);	//��ʾͷ��
	void ShowText(LPDIRECTDRAWSURFACE, char *);	//��ʾ�ı�
	void Talk(LPDIRECTDRAWSURFACE&, char *, int bPhotoInLeft=1);	//�Ի�
	void Say(LPDIRECTDRAWSURFACE&, char *, char *, int bPhotoInLeft=1);	//˵һ�仰
};

#endif