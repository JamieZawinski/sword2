//********************************************
//	脚本 相关函数
//  创建于2000年5月8日
//********************************************

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

//脚本类
class CScript
{
public:

	//脚本执行返回
	enum SCRIPTRETURN{
		SR_OK=0,	//正常
		SR_ERROR,	//错误
		SR_RECURSION,//递归
	};

	//指令结构
	struct stCommand{
		int Id;			//指令编号
		char Name[32];	//指令名称
	};

	//变量结构
	struct stVariable{
		char Name[32];	//名称
		int Value;		//当前值
		int SValue;		//原始值
	};

public:
	static char RecursionFile[8][MAX_PATH];	//递归文件名(最大八层递归)
	static int RecursionPoint[8];		//递归断点
	static int RecursionLevel;			//递归层次

private:
	static int MAX_COMMAND;				//总指令数
	static stCommand *COMMAND;			//指令列表

	static int MAX_VARIABLE;			//全局变量数
	static stVariable *VARIABLE;	//全局变量列表

	char FileName[MAX_PATH];		//文件名
	int DataLen;			//文件长度
	char *Data;				//文件内容

	int Point;				//当前指针
	int Line, Word;			//当前行列
	bool bInString;			//当前指针是否在一个字符串内 "in"

public:
	CScript();
	~CScript();						//析构
	
	bool Init(char *, char *);		//初始化
	bool LoadCommandList(char *);	//读取指令列表
	bool LoadVariableList(char *);	//读取全局变量列表
	void LoadVariable(FILE *);		//读进度变量
	void SaveVariable(FILE *);		//存进度变量
	bool Release();					//释放全局列表

	int GetValue(char *);			//取得变量的值
	int SetValue(char *, int);		//设置变量的值
	int AddValue(char *, int);		//设置变量的值
	void RestoreValue();			//恢复变量的原始值

	bool Open(char *);				//打开脚本文件
	int Goto(char *);				//转移到指定的位置
	char *ReadCommand(int lp=-1);	//读入一行指令
	
	char *GetCommandName(char *);		//取得指令名称
	int GetIntParam(char *,int);		//取得某个整形参数
	char *GetStringParam(char *, int);	//取得某个字符串参数
	char *GetVariableParam(char *, int);//取得变量形参数
	char GetOperation(char *, int);		//取得某个分隔符
	bool IsOperation(char );			//该字符是否是分隔符
	
	bool RunCommand(char *);		//执行一条指令
	enum SCRIPTRETURN Run(char *, char *index="", int iPoint=0);	//执行一个脚本
	int GotoNextLine(int lp=-1);	//提行
};

#endif