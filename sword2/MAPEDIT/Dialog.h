//********************************************
//	�Ի��� ��غ���
//  ������2000��3��17��
//********************************************

void CreateDlg(int, DLGPROC);		//����һ��ģʽ�Ի���
void CreateDlg2(int, DLGPROC);		//������ģ̬�Ի���

char *GetStringDlg(char *Msg=NULL);	//���һ���ַ���
int GetIntDlg(char *Msg=NULL);		//���һ������
BOOL APIENTRY CGetStringDlgProc(HWND, UINT, WPARAM, LPARAM);	//���һ���ַ���

BOOL APIENTRY DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogChangePointListProc( HWND, UINT, WPARAM, LPARAM);

char *GetFileDialog(bool Load=true);	//���ļ��Ի���
char *GetPathDialog();					//ȡ��·��

BOOL APIENTRY DialogChangePointListProc( HWND, UINT, WPARAM, LPARAM);	//���ڵ�
BOOL APIENTRY DialogEnemySetListProc( HWND, UINT, WPARAM, LPARAM);		//��ͼ�����趨������Ϣ
BOOL APIENTRY DialogObjectProc( HWND, UINT, WPARAM, LPARAM);			//��������
BOOL APIENTRY DialogPopMenuProc( HWND, UINT, WPARAM, LPARAM );			//�Ҽ������˵�������Ϣ

//------------------------------------
void _NewMap(HWND);			//�½���ͼ
bool _LoadMap(HWND);		//�����ͼ
void _SaveMap(HWND);		//�����ͼ
bool _SaveAsMap(HWND);		//���Ϊ
