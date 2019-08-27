//********************************************
//	�ű� ��غ���
//  ������2000��5��8��
//********************************************

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

//�ű���
class CScript
{
public:

	//�ű�ִ�з���
	enum SCRIPTRETURN{
		SR_OK=0,	//����
		SR_ERROR,	//����
		SR_RECURSION,//�ݹ�
	};

	//ָ��ṹ
	struct stCommand{
		int Id;			//ָ����
		char Name[32];	//ָ������
	};

	//�����ṹ
	struct stVariable{
		char Name[32];	//����
		int Value;		//��ǰֵ
		int SValue;		//ԭʼֵ
	};

public:
	static char RecursionFile[8][MAX_PATH];	//�ݹ��ļ���(���˲�ݹ�)
	static int RecursionPoint[8];		//�ݹ�ϵ�
	static int RecursionLevel;			//�ݹ���

private:
	static int MAX_COMMAND;				//��ָ����
	static stCommand *COMMAND;			//ָ���б�

	static int MAX_VARIABLE;			//ȫ�ֱ�����
	static stVariable *VARIABLE;	//ȫ�ֱ����б�

	char FileName[MAX_PATH];		//�ļ���
	int DataLen;			//�ļ�����
	char *Data;				//�ļ�����

	int Point;				//��ǰָ��
	int Line, Word;			//��ǰ����
	bool bInString;			//��ǰָ���Ƿ���һ���ַ����� "in"

public:
	CScript();
	~CScript();						//����
	
	bool Init(char *, char *);		//��ʼ��
	bool LoadCommandList(char *);	//��ȡָ���б�
	bool LoadVariableList(char *);	//��ȡȫ�ֱ����б�
	void LoadVariable(FILE *);		//�����ȱ���
	void SaveVariable(FILE *);		//����ȱ���
	bool Release();					//�ͷ�ȫ���б�

	int GetValue(char *);			//ȡ�ñ�����ֵ
	int SetValue(char *, int);		//���ñ�����ֵ
	int AddValue(char *, int);		//���ñ�����ֵ
	void RestoreValue();			//�ָ�������ԭʼֵ

	bool Open(char *);				//�򿪽ű��ļ�
	int Goto(char *);				//ת�Ƶ�ָ����λ��
	char *ReadCommand(int lp=-1);	//����һ��ָ��
	
	char *GetCommandName(char *);		//ȡ��ָ������
	int GetIntParam(char *,int);		//ȡ��ĳ�����β���
	char *GetStringParam(char *, int);	//ȡ��ĳ���ַ�������
	char *GetVariableParam(char *, int);//ȡ�ñ����β���
	char GetOperation(char *, int);		//ȡ��ĳ���ָ���
	bool IsOperation(char );			//���ַ��Ƿ��Ƿָ���
	
	bool RunCommand(char *);		//ִ��һ��ָ��
	enum SCRIPTRETURN Run(char *, char *index="", int iPoint=0);	//ִ��һ���ű�
	int GotoNextLine(int lp=-1);	//����
};

#endif