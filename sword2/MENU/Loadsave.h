//********************************************
//	���� ��غ���
//  ������2000��10��09��
//********************************************

#ifndef _LOADSAVE_H_
#define _LOADSAVE_H_

#define MAX_RECORD 10

//********��Ʒ��*********
class CLoadSave : public CListWindow
{
	//�浵�ṹ
	struct stRecord{
		char Name[8];		//����
		char About[32];		//˵��
	};

public:
	int PicWidth, PicHeight;			//��ƷͼƬ�Ĵ�С
	stRecord Record[MAX_RECORD];	//�浵

public:
	CLoadSave();
	~CLoadSave();

	void LoadWindowIni(char *, char *);		//���봰�ڵ�����
	void LoadRecord(char *);				//�����¼�б�
	void SaveRecord(char *);				//д���¼�б�

	void LoadGame(int);						//�������
	void SaveGame(int);						//�洢����
	void GetGameTime(char *);				//��ȡ����ʱ��

	void ShowLoadSaveWindow(LPDIRECTDRAWSURFACE surf, bool bLoad=true, int left=-1, int top=-1);	//��ʾ

	bool DoMenu(LPDIRECTDRAWSURFACE& , bool, int, int);	//�˵�ѭ��
};

#endif