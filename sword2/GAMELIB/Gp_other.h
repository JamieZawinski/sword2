//********************************************
//	���ຯ��
//   softboy ������2000��1��25��
//********************************************
#ifndef _GP_OTHER_
#define _GP_OTHER_

RECT GetRect(int x1,int y1,int x2,int y2);	//���þ���
int random(int nMax);						//�����
void PressKey(DWORD Key,int x=0);			//���¼����ɿ�
void WaitKey(DWORD Key);
void Delay(long time);				//��ʱ
int CountSpeed();					//�����ٶ�
int GetFileLength(char *name);		//ȡ�ļ�����
bool CheckFile(char *name);			//�ж��ļ��Ƿ����
void ShowMessage(char *msg,...);	//��ʾ��Ϣ
void PutDebugString(char *msg);		//���������Ϣ
char *GetFilePath(char *filename);	//���ļ�����ȡ�ļ�·��
WCHAR* WChar(char *string);			//char to wchar
char* Char(WCHAR *string);			//wchar to char
char* GetFileName(char* filename);	//���ļ�����·��������ļ�������·��
#endif