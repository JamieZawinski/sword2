//********************************************
//	�ű� ��غ���
//  ������2000��5��8��
//********************************************
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "gamelib\goldpoint.h"
#include "main.h"
#include "message.h"
#include "role.h"
#include "script.h"

//��̬��Ա
int CScript::MAX_COMMAND=0;
CScript::stCommand *CScript::COMMAND=NULL;
int CScript::MAX_VARIABLE=0;
CScript::stVariable *CScript::VARIABLE=NULL;

char CScript::RecursionFile[8][MAX_PATH];
int CScript::RecursionPoint[8];
int CScript::RecursionLevel=0;

CScript::CScript()
{
	Data=NULL;
}

//����
CScript::~CScript()
{
	_DELETE( Data );
}

//��ʼ��
bool CScript::Init(char *CommandFileName, char *VariableFileName)
{
	if( !LoadCommandList(CommandFileName) )	//��ָ��
		return false;

	if( !LoadVariableList(VariableFileName) )	//��ȫ�ֱ���
		return false;

	return true;
}

//��ȡָ���б�
bool CScript::LoadCommandList(char *filename)
{
	char* strTemp;
	char *index="ָ���б�";
	CIniSet Ini(filename);
	MAX_COMMAND=Ini.GetContinueDataNum(index);
	COMMAND=(stCommand *)realloc(COMMAND, sizeof(stCommand)*MAX_COMMAND);
	for(int i=0; i<MAX_COMMAND; i++)
	{
		char* str = Ini.ReadData(index, i);
		COMMAND[i].Id=atoi(str);			//���ָ��Id
		free( str );
		strTemp = Ini.ReadText(index, i);
		strcpy( COMMAND[i].Name, strTemp );	//���ָ������
		free(strTemp);
	}
	return true;
}

//��ȡȫ�ֱ����б�
bool CScript::LoadVariableList(char *filename)
{
	char *index="ȫ�ֱ����б�";
	CIniSet Ini(filename);
	MAX_VARIABLE=Ini.GetContinueDataNum( "ȫ�ֱ����б�" );
	VARIABLE=(stVariable *)realloc(VARIABLE, sizeof(stVariable)*MAX_VARIABLE);
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		char* str = Ini.ReadData(index, i);
		strcpy( VARIABLE[i].Name, str);	//��ñ�������
		free( str );
		VARIABLE[i].SValue=VARIABLE[i].Value=Ini.ReadInt(index, i);			//��ñ���ֵ
	}
	return true;
}

//��ȡ����ʱ��
void CScript::LoadVariable(FILE *fp)
{
	int num;
	fread(&num, 4, 1, fp);
	if( num != MAX_VARIABLE )
	{
		_FREE( VARIABLE );
	}
	MAX_VARIABLE=num;
	fread(VARIABLE, sizeof(stVariable), MAX_VARIABLE, fp);
}

//�洢���ȵ�ʱ��
void CScript::SaveVariable(FILE *fp)
{
	fwrite(&MAX_VARIABLE, 4, 1, fp);
	fwrite(VARIABLE, sizeof(stVariable), MAX_VARIABLE, fp);
}

//�ͷ�
bool CScript::Release()
{
	_FREE( COMMAND );
	_FREE( VARIABLE );
	return true;
}

//���ر�����ֵ
int CScript::GetValue(char *Var)
{
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		if( stricmp(Var, VARIABLE[i].Name)==0 )
		{
			return VARIABLE[i].Value;
		}
	}

	ShowMessage("Variable [%s] not define!", Var);
	return ERROR_DATA;	//�޴�ֵ
}

//���ñ�����ֵ
int CScript::SetValue(char *Var, int num)
{
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		if( stricmp(Var, VARIABLE[i].Name)==0 )
		{
			VARIABLE[i].Value=num;
			return 1;
		}
	}

	ShowMessage("Variable [%s] not define!", Var);
	return ERROR_DATA;	//�޴�ֵ
}

//�ı������ֵ(����)
int CScript::AddValue(char *Var, int num)
{
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		if( stricmp(Var, VARIABLE[i].Name)==0 )
		{
			VARIABLE[i].Value+=num;
			return 1;
		}
	}
	return ERROR_DATA;	//�޴�ֵ
}

//�ָ�������ԭʼֵ
void CScript::RestoreValue()
{
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		VARIABLE[i].Value=VARIABLE[i].SValue;
	}
}

//���ļ�������
bool CScript::Open(char *filename)
{
	strcpy(FileName, filename);

	_DELETE( Data );

	DataLen=GetFileLength(FileName);	//ȡ���ļ�����
	if( DataLen == -1 )
	{
		ShowMessage("Can't found file %s", FileName);
		return false;
	}

	Data=new char[DataLen];

	//������
	FILE *fp;
	fp=fopen(filename, "rb");
	fread(Data, DataLen, 1, fp);
	fclose(fp);

	//��ʼ��ָ��
	Point=0;				//��ǰָ��
	Line=Word=0;			//��ǰ����
	bInString=false;

	return true;
}

//��ȡһ��ָ��
char *CScript::ReadCommand(int lp)
{
	if( lp==-1 ) 
		lp=Point;

	//��ʱ����
	char chr;
	char *Ret;
	int m=0;

	Ret=(char *)malloc(DataLen-lp+1);
	memset(Ret, 0, DataLen-lp+1);

	for(int i=lp; i<DataLen; i++)
	{
		chr=Data[i];

		//ע��
		if( Data[i] == '/' && Data[i+1] == '/' )//��ע�ͣ�������һ��
		{
			i+=GotoNextLine(i)-1;
			continue;
		}

		//ָ�ʼ
		if( chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z' )
		{
			for(int j=i; j<DataLen; j++)
			{
				chr=Data[j];
				if( chr == ';' || chr == '\t' )	//ָ�����
				{
					Point=j;					//�ƶ���ǰָ��
					return Ret;		//����
				}

				if( chr == '\r' )
				{
					Point=j+1;					//�س�������ǰָ���ƶ����س��ĺ�һ���ַ�
					return Ret;		//����
				}

				Ret[m]=chr;			//������Ret��
				m++;				//��һ��
			}
			
			Point=DataLen-1;		//���ļ����ʱ��û�н����Ļ�������ǰָ���Ƶ����
			return Ret;
		}
	}

	Point=DataLen-1;				//û���ҵ�ָ�����
	return Ret;
}

//��ȡָ������
char *CScript::GetCommandName(char *CmdStr)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		//'('��ʾ������ʼ����������ʾ��ָ��û�в��������ҽ���
		if( chr == '(' || chr == ' ' || chr == '\t' || chr == '\r' || chr == ';' )
		{
			return Ret;		//��������
		}
		Ret[n]=chr;			//�ŵ�Ret
		n++;				//��һ��
	}

	return Ret;
}

//ȡ�����β���
int CScript::GetIntParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//���Ҳ���ͷ
	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		if( chr == '(' )		//������ʼ
		{
			goto _Next;	
		}
	}
	//û���ҵ�����ͷ'('
	ShowMessage("Command [%s]'s Param %d not found!", CmdStr, num);	
	goto _Error;

_Next:
	{
		int ParamNum=0;
		bInString=false;
		for(int j=i; j<Len; j++)
		{
			chr=CmdStr[j];
			
			//�ڼ�����
			if( chr == '\"' ) 
				bInString=!bInString;//���������һ��string������������������ַ������뿼��
									//ParamNum���ᱻ�Ӽӣ������������string����	
			if( IsOperation(chr) )	
				ParamNum++;				//�Ǹ��ָ�������ô������һ�������Ŀ�ʼ
			
			//ֱ����ֵ����
			//Ҫ���������������������
			if( ParamNum==num && ( (chr >= '0' && chr <= '9') || chr == '-' ) )
			{
				for(int k=j; k<Len; k++)
				{
					chr=CmdStr[k];		//�õ�����ַ�
					//�����������Ҳ���Ǹ���
					if( (chr <'0' || chr > '9') && chr != '-' )
					{
						//ת��������
						int ret=atoi(Ret);
						_FREE(Ret);
						return ret;
					}
					//���滹�����֣��ŵ�һ�𣬵ȵ�������ʱ��ת��
					Ret[n]=chr;
					n++;
				}
				
				goto _Error;
			}
			
			//������ֵ����
			//��$��ͷ����ȫ�ֱ���
			if( ParamNum==num && chr == '$' )
			{
				for(int k=j; k<Len; k++)
				{
					chr=CmdStr[k];
					//����ǿո�')'�������Ƿָ������ͷ������ȫ�ֱ�����ֵ
					if( chr == ' ' || chr == ')' || IsOperation(chr) )
					{
						int ret = GetValue(Ret);
						_FREE(Ret);
						return ret;
					}
					//����
					Ret[n]=chr;
					n++;
				}
				
				ShowMessage("Command [%s]'s Param %d not found!", CmdStr, num);
				goto _Error;
			}
		}
	}
_Error:
	_FREE(Ret);
	return ERROR_DATA;
}

//ȡ���ַ�������
char *CScript::GetStringParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//���Ҳ���ͷ
	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		if( chr == '(' )
		{
			goto _Next;
		}
	}
	
	goto _ERROR;

_Next:
	{
	int ParamNum=0;
	bInString=false;
	for(int j=i; j<Len; j++)
	{
		chr=CmdStr[j];
		
		//�ڼ�����
		//������ĺ���һ�����߼�
		if( chr == '\"' ) 
			bInString=!bInString;
		if( IsOperation(chr) )	
			ParamNum++;

		//������ʼ
		//����ͷ
		if( ParamNum==num && chr == '\"' )
		{
			//����"
			for(int k=j+1; k<Len; k++)
			{
				chr=CmdStr[k];
				//������Ե�"
				if( chr == '\"' )
					return Ret;		//�ҵ����������������������""

				Ret[n]=chr;
				n++;
			}
			
			goto _ERROR;
		}
	}}

_ERROR:
	ShowMessage("Command [%s]'s Param %d not found!", CmdStr, num);
	return Ret;
}

//ȡ�ñ����β���
char *CScript::GetVariableParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//���Ҳ���ͷ
	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		if( chr == '(' )
		{
			goto _Next;
		}
	}
	goto _ERROR;

_Next:
	{
		int ParamNum=0;
		bInString=false;
		for(int j=i; j<Len; j++)
		{
			chr=CmdStr[j];
			
			//�ڼ�����
			//������������ һ��
			if( chr == '\"' ) 
				bInString=!bInString;
			if( IsOperation(chr) )	
				ParamNum++;
			
			//������ֵ����
			//��$��ͷ����������GetIntParam��ע��
			if( ParamNum==num && chr == '$' )
			{
				for(int k=j; k<Len; k++)
				{
					chr=CmdStr[k];
					if( chr == ' ' || chr == ')' || IsOperation(chr) )
						return Ret;
					
					Ret[n]=chr;
					n++;
				}
				goto _ERROR;
			}
		}
	}

_ERROR:
	ShowMessage("Command [%s]'s Param %d not found!", CmdStr, num);
	return Ret;
}

//ȡ��ĳ���ָ���
char CScript::GetOperation(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;

	//���Ҳ���ͷ
	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		if( chr == '(' )
		{
			goto _Next;
		}
	}
	goto _ERROR;

_Next:
	{
		int ParamNum=0;
		bInString=false;
		for(int j=i; j<Len; j++)
		{
			chr=CmdStr[j];
			
			//������ֵ����
			//�򵥶���
			if( ParamNum==num && IsOperation(chr) )
			{
				return chr;
			}
			
			//�ڼ�����
			//����û��������
			if( chr == '\"' ) 
				bInString=!bInString;
			if( IsOperation(chr) )	
				ParamNum++;
		}
	}

_ERROR:
	ShowMessage("Command [%s]'s Operation %d not found!", CmdStr, num);
	return 0;
}

//ĳ�ַ��Ƿ��Ƿָ���
bool CScript::IsOperation(char chr)
{
	//���˵�string�����е����ж���
	//, = > < ! ���Ƿָ���
	if( !bInString && ( chr == ',' || chr == '=' || chr == '>' 
		|| chr == '<' || chr == '!' ))
		return true;

	return false;
}

//����
int CScript::GotoNextLine(int lp)
{
	if( lp== -1 ) 
		lp=Point;

	for(int i=lp; i<DataLen; i++)
	{
		if( Data[i]=='\n' )	//�ǻس�
		{
			Point=i+1;			//�ƶ����س�����һ���ַ�
			return Point-lp;	//����ƫ����	
		}
	}

	Point=i;					//û���ҵ��س����ƶ���ĩβ
	return Point-lp;
}

//ת�Ƶ�ָ��λ��
int CScript::Goto(char *index)
{
	int lp=Point;	//�ֳ�	����
	Point=0;		//�ص���ͷ
	char *CmdStr;
	while(1)
	{
		CmdStr=ReadCommand();	//�ӿ�ͷ��ѭ����һ��ָ��
		if( CmdStr[strlen(CmdStr)-1] == ':' )	//�Ǳ�ż���һ��':'
		{
			CmdStr[strlen(CmdStr)-1] = 0;		//������ɱ�ţ���Ҫ����� :
			if( strcmp( CmdStr, index ) == 0 )	//��������Ҫ��ת�ı��
			{
				_FREE(CmdStr);
				return Point;					//����λ��
			}
		}
		
		if( Point >= DataLen-1 )				//û���ҵ�
		{
			ShowMessage("Prompt <%s> in file %s is not found!", index, FileName);
			Point=lp;
			_FREE(CmdStr);
			return 0;
		}
	}
	_FREE(CmdStr);
}

//����һ�νű�
CScript::SCRIPTRETURN CScript::Run(char *filename, char *index, int iPoint)
{
	char *CmdStr;
	char *CmdName;

	//�ݹ���
	strcpy( RecursionFile[RecursionLevel], filename );

	if( Open(filename) )		//���ļ�
	{
		Point=iPoint;			//�ƶ���Ҫ��ʼִ�еĵط�
		if( index != "" ) 
			Goto(index);		//���ָ���˿�ʼִ�еı�ţ�����ת����λ��

		while(1)
		{
			CmdStr=ReadCommand();			//��һ��ָ��
			CmdName=GetCommandName(CmdStr);	//��ȡָ������

			if( CmdStr[strlen(CmdStr)-1] == ':' )//�Ǳ��
			{
				_FREE( CmdName );
				_FREE( CmdStr );
				continue;
			}

			//��ʱ����ű�
			if( stricmp( CmdStr, "break" ) == 0 )	//��break�͹���
			{
				RecursionPoint[RecursionLevel]=Point;	//����ϵ�
				Message.PutMessage(MSG_BREAKSCRIPT, Point, 0, 0, NULL);//����һ����Ϣ
				_FREE( CmdName );
				_FREE( CmdStr );
				return SR_RECURSION;		//��ʾ�ڵݹ飬����
			}

			//û��ָ�������return �ͷ���
			else if( stricmp( CmdStr, "return" ) == 0 || CmdStr==NULL )	
			{
				_FREE( CmdName );
				_FREE( CmdStr );
				if( RecursionLevel >0 )		//�ڵݹ���
				{
					RecursionLevel--;		//������һ��
				}
				return SR_OK;
			}

			//���������ű�(�ݹ����)
			else if( stricmp( CmdName, "call" ) == 0 )
			{
				RecursionPoint[RecursionLevel]=Point;	//����ϵ�
				RecursionLevel++;				//������һ
	
				CScript SubScript;				//���õĽű�
				char *str=GetStringParam(CmdStr, 0);	//�õ��ű����֣���һ��������
				if( SubScript.Run( str ) == SR_RECURSION )	//����
				{
					_FREE(str);
					_FREE( CmdName );
					_FREE( CmdStr );
					return SR_RECURSION;				//����
				}
				_FREE(str);
				goto _Next3;							//����ִ��
			}
_Next3:
			RunCommand(CmdStr);		//ִ��
			_FREE( CmdName );
			_FREE( CmdStr );
		}
	}
	_FREE( CmdName );
	_FREE( CmdStr );

	return SR_ERROR;				//Ӧ������return�ķ�ʽ�����ģ����Է��ش���
}

//����һ�����
bool CScript::RunCommand(char *CmdStr)
{
	char *str1=NULL, *str2=NULL, *str3=NULL;
	int CommandId=-1;
	char *Cmd=GetCommandName(CmdStr);		//�õ�ָ������
	
	if( CmdStr == NULL || Cmd == NULL ) 
		return false;

	for(int i=0; i<MAX_COMMAND; i++)
	{
		if( stricmp(Cmd, COMMAND[i].Name)==0 )		//�ҵ���ָ��
		{
			CommandId=i;
			free(Cmd);
			goto _Next;
		}
	}
	ShowMessage("Command [%s] is not define!", Cmd);
	free(Cmd);
	free(CmdStr);
	return false;

_Next:
	switch( COMMAND[CommandId].Id )
	{
	case 10:		//Goto	һ������  
		{			//���磺goto("tiamo");
			char *str=GetStringParam(CmdStr, 0);
			Goto( str );
			free(str);
		}
		break;

	case 11:		//if	һ��int�ͽ�һ���ָ�����һ��int�ͽ�һ���ָ�����һ��string��
		{			//���磺if($Process=1,"tiamo")
			int tmp1=GetIntParam(CmdStr, 0);
			int tmp2=GetIntParam(CmdStr, 1);
			char oper=GetOperation(CmdStr, 0);
			char *str=GetStringParam(CmdStr, 2);
			switch( oper )
			{
			case ',':
			case '=':
				if( tmp1==tmp2 )		//if($process=1,"tiamo")��ʾ���$process����1�Ļ�
					Goto(str);			//��ת����Ž�tiamo�ĵط�ִ��
				break;

			case '>':					//���ھ���ת
				if( tmp1>tmp2 )
					Goto(str);
				break;

			case '<':					//С�ھ���ת
				if( tmp1<tmp2 )
					Goto(str);
				break;

			case '!':					//�����ھ���ת
				if( tmp1 != tmp2 )
					Goto(str);
				break;
			}
			free(str);
		}
		break;

	case 16:		//set	����ȫ�ֱ�����ֵ,����������һ��ȫ�ֱ�����һ��int����
		{			//���磺set($tiamo,1);
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, GetIntParam(CmdStr, 1));
			free(var);
		}
		break;

	case 17:		//add �ı�ȫ�ֱ�����ֵ������������һ��ȫ�ֱ�����һ��int����
		{			//���磺add($tiamo,1); ��$tiamo��ֵ��1
			char *var=GetVariableParam(CmdStr, 0);
			AddValue(var, GetIntParam(CmdStr, 1));
			free(var);
		}
		break;

	case 18:		//random ���������������������һ��ȫ�ֱ�����һ��int����
		{			//���磺random($tiamo,100);����һ��100���ڵ����������ֵ��$tiamo
			random(1);
			int num=GetIntParam(CmdStr, 1);
			int ran=random( num );
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, ran);
			free(var);
		}
		break;

	case 19:		//RGB�ϳ���ԭɫ	�ĸ�������һ��ȫ�ֱ���������int����
		{			//���磺RGB($tiamo,0,0,0);��$tiamo��ɺ�ɫ��RGBֵ
			int r=GetIntParam(CmdStr, 1);
			int g=GetIntParam(CmdStr, 2);
			int b=GetIntParam(CmdStr, 3);
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, RGB(r, g, b));
			free(var);
		}
		break;

	//----------------------PutMessage--------------------------//

	case 20:		//SetPlayState	һ��int������0 (������Ϸ����ʱ��Ϸ�ű�����ִ��) 
					//						  1 (����Ϸ���ű�����ִ��)
		Message.PutMessage( MSG_SETPLAYSTATE, GetIntParam(CmdStr, 0), 0, 0, NULL);
		break;

	case 21:		//SetDisplayState	һ��int������0 (����) ��1 (ҹ��)
		Message.PutMessage( MSG_SETDISPLAYSTATE, GetIntParam(CmdStr, 0), 0, 0, NULL);
		break;

	case 22:		//BackToTitle	û�в��������ؿ�ʼ����
		Message.PutMessage( MSG_BACKTOTITLE );
		break;

	case 23:		//TextTitle	����string������filename,index����ʾһ������
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_TEXTTITLE, 0, 0, 0, str1, str2);
		break;

	case 24:		//Fight	����string����������int��������
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		str3 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_FIGHT, GetIntParam(CmdStr, 3), GetIntParam(CmdStr, 4), 
			GetIntParam(CmdStr, 5), str1, str2, str3);
		break;

	case 25:		//EnableFight	һ��int�������Ƿ�ȵ���
		Message.PutMessage( MSG_ENABLEFIGHT, GetIntParam(CmdStr, 0));
		break;

	case 26:		//BossFight     һ��int�������Ƿ���bossս
		Message.PutMessage( MSG_BOSSFIGHT, GetIntParam(CmdStr,0));
		break;

	case 29:		//LoadMap		һ��string��������ͼ�ļ���
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_LOADMAP, 0, 0, 0, str1);
		break;

	case 30:		//SaveMap		һ��string��������ͼ�ļ���
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SAVEMAP, 0, 0, 0, str1);
		break;

	case 31:		//AddMapHook	��������������int��ʾ���꣬��һ��string��ʾ�ű��ļ�
		str1 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_ADDMAPHOOK, GetIntParam(CmdStr, 0), 
			GetIntParam(CmdStr, 1), 0, str1);
		break;

	case 32:		//SetMapHook	����int����������������һ�������id
		Message.PutMessage( MSG_SETMAPHOOK, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1), 
						GetIntParam(CmdStr, 2), NULL);
		break;

	case 33:		//SetMapCell	���int���������õ�ͼlevel(1)���(x,y)��(2��3)Ϊ
					//								��pageҳ(4)�ĵ�num��(5)Ԫ��
		Message.PutMessage( MSG_SETMAPCELL, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1), 
					GetIntParam(CmdStr, 2), (char *)GetIntParam(CmdStr,3), 
					(char *)GetIntParam(CmdStr,4));
		break;

	case 34:		//SetMapBlock	����int������������赲��ϵ
		Message.PutMessage( MSG_SETMAPBLOCK, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1),
					GetIntParam(CmdStr, 2) );
		break;

	case 35:		//SetMapLevel	�ĸ�int���������õ�ͼ(x,y)��(1��2)���赲��ϵ
					//							Ϊlevel1\level2(3��4)
		Message.PutMessage( MSG_SETMAPLEVEL, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1),
			GetIntParam(CmdStr, 2), (char *)GetIntParam(CmdStr,3), 
			(char *)GetIntParam(CmdStr,4));
		break;
		
	case 40:		//CenterMap		����int(����)��������x,yΪ��ͼ���ĵ�
		Message.PutMessage( MSG_CENTERMAP, GetIntParam(CmdStr, 0), GetIntParam(CmdStr,1) );
		break;

	case 41:		//ScrollMapTo	����int���������������һ���ٶȣ�����ͼ��x,y��(1��2)
		Message.PutMessage( MSG_SCROLLMAPTO, GetIntParam(CmdStr,0), GetIntParam(CmdStr,1), 
			GetIntParam(CmdStr,2) );
		break;

	case 50:		//LoadNpc		һ��string����������һ��NPC�ļ�
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_LOADNPC, 0,0,0, str1);
		break;

	case 60:
		Message.PutMessage(MSG_SETCPTYPE,GetIntParam(CmdStr,0),GetIntParam(CmdStr,1)
							,GetIntParam(CmdStr,2));
		break;

	case 100:		//MoveTo		����int������ǿ�������߶���x,y��
		Message.PutMessage( MSG_MOVE, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1) ,0, NULL);
		break;

	case 101:		//SetPlayerDir	һ��int�������������ǵķ���0=�� 1=�� 2=�� 3=��
		Message.PutMessage( MSG_SETDIR, GetIntParam(CmdStr,0), 0 ,0, NULL);
		break;

	case 102:		//SetPlayerPos	����int�������������ǵ�λ��ǰ���������꣬��һ�����Ǳ���
					//							��ʾ�Ƿ����ԭλ�õĵ�ͼ�赲��ֻҪ�У������
		Message.PutMessage( MSG_SETPOSITION, GetIntParam(CmdStr,0), GetIntParam(CmdStr,1) ,
					GetIntParam(CmdStr,2), NULL);
		break;

	case 103:		//CenterPlayer	û�в�����������Ϊ��ͼ����
		Message.PutMessage( MSG_CENTERPLAYER );
		break;

	case 104:		//SetPlayerShow	һ��int����		�Ƿ���ʾ���ǣ�1���ǣ�0��
		Message.PutMessage( MSG_SETPLAYERSHOW, GetIntParam(CmdStr,0));
		break;

	case 110:		//LoadPlayer	����������һ��int��ʾ��ɫ��ţ�������string��ʾ
					//						 ini�ļ���index��������ini�ļ���������
		str1 = GetStringParam(CmdStr, 1);
		str2 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_LOADPLAYER, GetIntParam(CmdStr,0),0,0, str1, str2);
		break;

	case 120:		//SwitchPlayer	����int��������ʾ���Ǳ�ţ���������
		Message.PutMessage( MSG_SWITCHPLAYER, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1));
		break;

	case 121:		//SetPlayerNum	һ��int��������ʾ�������������Ǹ���
		Message.PutMessage( MSG_SETPLAYERNUM, GetIntParam(CmdStr,0));
		break;

	case 122:		//MoveDirectTo ����int����,һ���Ƿ���һ���ǲ���
		Message.PutMessage(MSG_MOVEDIRECTTO,GetIntParam(CmdStr,0),GetIntParam(CmdStr,1));
		break;

	case 200:		//Talk			һ��string��������ʾ�����׶Ի����Ի�������<CR>������
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_TALK, 0, 0, 0, str1 );
		break;

	case 201:		//Say		��������������string������һ��int(���Ǳ���)������ȱʡ��1����
		{			//				ǰ�����ǶԻ���������ͷ�񣬺�һ������ʾͷ������߻����ұ�
			str1 = GetStringParam(CmdStr, 0);
			char *temp=GetStringParam(CmdStr, 1);
			int photo = GetIntParam(CmdStr,2);
			if( photo == ERROR_DATA )
			{
				photo = 1;
			}
			Message.PutMessage( MSG_SAY, photo, 0, 0, str1,temp);
		}
		break;

	case 210:		//AddMoney		һ��int������ʾ���ӵĽ�Ǯ��Ŀ
		Message.PutMessage( MSG_ADDMONEY, GetIntParam(CmdStr, 0));
		break;

	case 221:		//AddObject		һ��string��һ��int�����string��Ʒint��
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_ADDOBJECT, GetIntParam(CmdStr, 1), 0, 0, str1);
		break;

	case 222:		//DelObject		һ��string��һ��int��ɾ��string��Ʒint��
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_DELOBJECT, GetIntParam(CmdStr, 1), 0, 0, str1);
		break;

	case 223:		//CheckObject	һ��string������һ��ȫ�ֱ��������string��Ʒ
		{			//								�ж��٣�����ŵ�ȫ�ֱ�����
					//								�������������break��
			char *strGoods = GetStringParam(CmdStr, 0);
			char *var=GetVariableParam(CmdStr, 1);
			int num = role[0].CheckObject(strGoods);
			SetValue(var, num);
		}
		break;

	case 224:		//AddMagic һ��string������һ��int����
		{			//			һ����ħ������һ����ѧ�����ħ��������
			str1 = GetStringParam(CmdStr,0);
			int rolenum = GetIntParam(CmdStr,1);
			Message.PutMessage(MSG_ADDMAGIC,rolenum,0,0,str1);
			break;
		}

	case 225:		//DelMagic һ��string������һ��int����
		{			//			һ����ħ������һ����ӵ�����ħ��������
			str1 = GetStringParam(CmdStr,0);
			int rolenum = GetIntParam(CmdStr,1);
			Message.PutMessage(MSG_DELMAGIC,rolenum,0,0,str1);
			break;
		}

	case 250:		//MoveNpcTo		����������һ��string������int���ƶ����ֽ�string��
					//				NPC������int���ɵ����꣬���ʱ���ǲ���ִ����һ����Ϣ��
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_MOVENPCTO, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2),
							0, str1);
		break;

	case 251:		//MoveNPC		����һ��������ͬ�����ǻ����ִ����Ϣ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_MOVENPC, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 0, 
							str1);
		break;

	case 252:		//SetNpcPos		�ĸ�������һ��string����int����������string��������int
					//						  ��ɵ����꣬���һ�����Ǳ���ģ��У���ʾ���ԭ
					//						  λ�õ��赲��ϵ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCPOS, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 
					GetIntParam(CmdStr,3), str1 );
		break;

	case 253:		//SetNpcDir		һ��string������һ��int��������SetPlayDir��ͬ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCDIR, GetIntParam(CmdStr,1), 0,0, str1 );
		break;

	case 254:		//SetNpcStep	һ��string������һ��int������
					//				����string��NPC�ĵ�ǰ����int��ֵ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCSTEP, GetIntParam(CmdStr,1), 0,0, str1 );
		break;

	case 255:		//AddNpc		����string�������������������int����
					//				NPC���ڵ�һ��string��ʾ��ini�ļ����ɵڶ���string��ʾ��
					//				index�����У������������꣬û�еĻ�����ini�ļ��������
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_ADDNPC, GetIntParam(CmdStr,2), GetIntParam(CmdStr,3),
					0, str1, str2);
		break;

	case 256:		//DelNpc		һ����ʾNPC���ֵ�string����
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_DELNPC, 0, 0, 0, str1);
		break;

	case 257:		//SetNpcActive	һ����ʾNPC���ֵ�string������һ����ʾ�Ƿ���int����
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCACTIVE, GetIntParam(CmdStr,1), 0,0, str1);
		break;

	case 258:		//SetNpcShow	һ����ʾNPC���ֵ�string������һ����ʾ�Ƿ���ʾ��int����
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCSHOW, GetIntParam(CmdStr,1), 0,0, str1);
		break;

	case 259:		//SetNpcScript	һ����ʾNPC���ֵ�string������һ����ʾ�ű��ļ���string����
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_SETNPCSCRIPT, 0,0,0, str1, str2);
		break;

	case 300:		//Delay			һ��int��������ʱ
		Message.PutMessage( MSG_DELAY, GetIntParam(CmdStr,0), 0, 0, NULL);
		break;

	case 301:		//FadeIn		����int������Ч����ɫʱ��(���������Ǳ���)����Ļ����
		Message.PutMessage( MSG_FADEIN, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1), 
			GetIntParam(CmdStr,2), NULL);
		break;
	
	case 302:		//FadeOut		����int����,Ч����ʱ��(ʱ�䲻�Ǳ����)����Ļ����
		Message.PutMessage( MSG_FADEOUT, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1), 0,
			NULL);
		break;

	case 303:		//PlayMusic		һ��string���������ű������֣�ѭ����
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_PLAYMUSIC, 0,0,0, str1 );
		break;

	case 304:		//StopMusic		û�в�����ֹͣ���ű�������
		Message.PutMessage( MSG_STOPMUSIC );
		break;

	case 305:		//PauseMusic	û�в������������ű�������
		Message.PutMessage( MSG_PAUSEMUSIC );
		break;

	case 306:		//ResumeMusci	û�в�������ͣ���ű�������
		Message.PutMessage( MSG_RESUMEMUSIC );
		break;

	case 310:		//PlaySound		һ��string����������һ����Ч�����Σ�
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_PLAYSOUND, 0,0,0, str1 );
		break;

	case 311:		//StopSound		û�в�����ֹͣ������Ч
		Message.PutMessage( MSG_STOPSOUND );
		break;

	case 340:		//GameMessage	�ĸ�������һ��string������int����ʾһ����Ϸ��Ϣ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_GAMEMESSAGE, GetIntParam(CmdStr,1), 
			GetIntParam(CmdStr,2), GetIntParam(CmdStr,3), str1);
		break;

	case 350:		//ShowMessage	�ĸ�������һ��string������int����ʾһ��ϵͳ��Ϣ
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SHOWMESSAGE, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 
			GetIntParam(CmdStr,3), str1);
		break;
	}

	return true;
}
