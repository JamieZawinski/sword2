//********************************************
//	脚本 相关函数
//  创建于2000年5月8日
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

//静态成员
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

//析构
CScript::~CScript()
{
	_DELETE( Data );
}

//初始化
bool CScript::Init(char *CommandFileName, char *VariableFileName)
{
	if( !LoadCommandList(CommandFileName) )	//读指令
		return false;

	if( !LoadVariableList(VariableFileName) )	//读全局变量
		return false;

	return true;
}

//读取指令列表
bool CScript::LoadCommandList(char *filename)
{
	char* strTemp;
	char *index="指令列表";
	CIniSet Ini(filename);
	MAX_COMMAND=Ini.GetContinueDataNum(index);
	COMMAND=(stCommand *)realloc(COMMAND, sizeof(stCommand)*MAX_COMMAND);
	for(int i=0; i<MAX_COMMAND; i++)
	{
		char* str = Ini.ReadData(index, i);
		COMMAND[i].Id=atoi(str);			//获得指令Id
		free( str );
		strTemp = Ini.ReadText(index, i);
		strcpy( COMMAND[i].Name, strTemp );	//获得指令名字
		free(strTemp);
	}
	return true;
}

//读取全局变量列表
bool CScript::LoadVariableList(char *filename)
{
	char *index="全局变量列表";
	CIniSet Ini(filename);
	MAX_VARIABLE=Ini.GetContinueDataNum( "全局变量列表" );
	VARIABLE=(stVariable *)realloc(VARIABLE, sizeof(stVariable)*MAX_VARIABLE);
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		char* str = Ini.ReadData(index, i);
		strcpy( VARIABLE[i].Name, str);	//获得变量名字
		free( str );
		VARIABLE[i].SValue=VARIABLE[i].Value=Ini.ReadInt(index, i);			//获得变量值
	}
	return true;
}

//读取进度时候
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

//存储进度的时候
void CScript::SaveVariable(FILE *fp)
{
	fwrite(&MAX_VARIABLE, 4, 1, fp);
	fwrite(VARIABLE, sizeof(stVariable), MAX_VARIABLE, fp);
}

//释放
bool CScript::Release()
{
	_FREE( COMMAND );
	_FREE( VARIABLE );
	return true;
}

//返回变量的值
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
	return ERROR_DATA;	//无此值
}

//设置变量的值
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
	return ERROR_DATA;	//无此值
}

//改变变量的值(增加)
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
	return ERROR_DATA;	//无此值
}

//恢复变量的原始值
void CScript::RestoreValue()
{
	for(int i=0; i<MAX_VARIABLE; i++)
	{
		VARIABLE[i].Value=VARIABLE[i].SValue;
	}
}

//打开文件并读入
bool CScript::Open(char *filename)
{
	strcpy(FileName, filename);

	_DELETE( Data );

	DataLen=GetFileLength(FileName);	//取得文件长度
	if( DataLen == -1 )
	{
		ShowMessage("Can't found file %s", FileName);
		return false;
	}

	Data=new char[DataLen];

	//读数据
	FILE *fp;
	fp=fopen(filename, "rb");
	fread(Data, DataLen, 1, fp);
	fclose(fp);

	//初始化指针
	Point=0;				//当前指针
	Line=Word=0;			//当前行列
	bInString=false;

	return true;
}

//读取一条指令
char *CScript::ReadCommand(int lp)
{
	if( lp==-1 ) 
		lp=Point;

	//临时变量
	char chr;
	char *Ret;
	int m=0;

	Ret=(char *)malloc(DataLen-lp+1);
	memset(Ret, 0, DataLen-lp+1);

	for(int i=lp; i<DataLen; i++)
	{
		chr=Data[i];

		//注释
		if( Data[i] == '/' && Data[i+1] == '/' )//是注释，跳到下一行
		{
			i+=GotoNextLine(i)-1;
			continue;
		}

		//指令开始
		if( chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z' )
		{
			for(int j=i; j<DataLen; j++)
			{
				chr=Data[j];
				if( chr == ';' || chr == '\t' )	//指令结束
				{
					Point=j;					//移动当前指针
					return Ret;		//返回
				}

				if( chr == '\r' )
				{
					Point=j+1;					//回车，将当前指针移动到回车的后一个字符
					return Ret;		//返回
				}

				Ret[m]=chr;			//保存在Ret中
				m++;				//下一个
			}
			
			Point=DataLen-1;		//在文件完的时候都没有结束的话，将当前指针移到最后
			return Ret;
		}
	}

	Point=DataLen-1;				//没有找到指令语句
	return Ret;
}

//获取指令名字
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
		//'('表示参数开始，而其他表示该指令没有参数，并且结束
		if( chr == '(' || chr == ' ' || chr == '\t' || chr == '\r' || chr == ';' )
		{
			return Ret;		//返回名字
		}
		Ret[n]=chr;			//放到Ret
		n++;				//下一个
	}

	return Ret;
}

//取得整形参数
int CScript::GetIntParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//查找参数头
	for(int i=0; i<Len; i++)
	{
		chr=CmdStr[i];
		if( chr == '(' )		//参数开始
		{
			goto _Next;	
		}
	}
	//没有找到参数头'('
	ShowMessage("Command [%s]'s Param %d not found!", CmdStr, num);	
	goto _Error;

_Next:
	{
		int ParamNum=0;
		bInString=false;
		for(int j=i; j<Len; j++)
		{
			chr=CmdStr[j];
			
			//第几参数
			if( chr == '\"' ) 
				bInString=!bInString;//进入和跳出一个string参数，这里面的所以字符都不与考虑
									//ParamNum不会被加加，除非跳出这个string参数	
			if( IsOperation(chr) )	
				ParamNum++;				//是个分隔符，那么就是下一个参数的开始
			
			//直接数值参数
			//要的是这个参数并且是数字
			if( ParamNum==num && ( (chr >= '0' && chr <= '9') || chr == '-' ) )
			{
				for(int k=j; k<Len; k++)
				{
					chr=CmdStr[k];		//得到这个字符
					//如果不是数字也不是负号
					if( (chr <'0' || chr > '9') && chr != '-' )
					{
						//转化成整数
						int ret=atoi(Ret);
						_FREE(Ret);
						return ret;
					}
					//后面还有数字，放到一起，等到结束的时候转化
					Ret[n]=chr;
					n++;
				}
				
				goto _Error;
			}
			
			//变量数值参数
			//用$开头的是全局变量
			if( ParamNum==num && chr == '$' )
			{
				for(int k=j; k<Len; k++)
				{
					chr=CmdStr[k];
					//如果是空格，')'，或者是分隔符，就返回这个全局变量的值
					if( chr == ' ' || chr == ')' || IsOperation(chr) )
					{
						int ret = GetValue(Ret);
						_FREE(Ret);
						return ret;
					}
					//继续
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

//取得字符串参数
char *CScript::GetStringParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//查找参数头
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
		
		//第几参数
		//和上面的函数一样的逻辑
		if( chr == '\"' ) 
			bInString=!bInString;
		if( IsOperation(chr) )	
			ParamNum++;

		//参数开始
		//“开头
		if( ParamNum==num && chr == '\"' )
		{
			//跳过"
			for(int k=j+1; k<Len; k++)
			{
				chr=CmdStr[k];
				//查找配对的"
				if( chr == '\"' )
					return Ret;		//找到，返回这个参数，不包括""

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

//取得变量形参数
char *CScript::GetVariableParam(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;
	char *Ret;

	Ret=(char *)malloc(Len);
	memset(Ret, 0, Len);

	//查找参数头
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
			
			//第几参数
			//……………… 一样
			if( chr == '\"' ) 
				bInString=!bInString;
			if( IsOperation(chr) )	
				ParamNum++;
			
			//变量数值参数
			//以$开头…………看GetIntParam的注释
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

//取得某个分隔符
char CScript::GetOperation(char *CmdStr, int num)
{
	int Len=strlen(CmdStr);
	int n=0;
	char chr;

	//查找参数头
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
			
			//变量数值参数
			//简单多了
			if( ParamNum==num && IsOperation(chr) )
			{
				return chr;
			}
			
			//第几参数
			//……没有语言了
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

//某字符是否是分隔符
bool CScript::IsOperation(char chr)
{
	//过滤掉string参数中的所有东东
	//, = > < ! 都是分隔符
	if( !bInString && ( chr == ',' || chr == '=' || chr == '>' 
		|| chr == '<' || chr == '!' ))
		return true;

	return false;
}

//提行
int CScript::GotoNextLine(int lp)
{
	if( lp== -1 ) 
		lp=Point;

	for(int i=lp; i<DataLen; i++)
	{
		if( Data[i]=='\n' )	//是回车
		{
			Point=i+1;			//移动到回车的下一个字符
			return Point-lp;	//返回偏移量	
		}
	}

	Point=i;					//没有找到回车，移动到末尾
	return Point-lp;
}

//转移到指定位置
int CScript::Goto(char *index)
{
	int lp=Point;	//现场	保存
	Point=0;		//回到开头
	char *CmdStr;
	while(1)
	{
		CmdStr=ReadCommand();	//从开头起循环读一条指令
		if( CmdStr[strlen(CmdStr)-1] == ':' )	//是标号加上一个':'
		{
			CmdStr[strlen(CmdStr)-1] = 0;		//将它变成标号，不要后面的 :
			if( strcmp( CmdStr, index ) == 0 )	//是我们想要跳转的标号
			{
				_FREE(CmdStr);
				return Point;					//返回位置
			}
		}
		
		if( Point >= DataLen-1 )				//没有找到
		{
			ShowMessage("Prompt <%s> in file %s is not found!", index, FileName);
			Point=lp;
			_FREE(CmdStr);
			return 0;
		}
	}
	_FREE(CmdStr);
}

//运行一段脚本
CScript::SCRIPTRETURN CScript::Run(char *filename, char *index, int iPoint)
{
	char *CmdStr;
	char *CmdName;

	//递归层次
	strcpy( RecursionFile[RecursionLevel], filename );

	if( Open(filename) )		//打开文件
	{
		Point=iPoint;			//移动到要开始执行的地方
		if( index != "" ) 
			Goto(index);		//如果指定了开始执行的标号，就跳转到该位置

		while(1)
		{
			CmdStr=ReadCommand();			//读一条指令
			CmdName=GetCommandName(CmdStr);	//获取指令名称

			if( CmdStr[strlen(CmdStr)-1] == ':' )//是标号
			{
				_FREE( CmdName );
				_FREE( CmdStr );
				continue;
			}

			//暂时挂起脚本
			if( stricmp( CmdStr, "break" ) == 0 )	//是break就挂起
			{
				RecursionPoint[RecursionLevel]=Point;	//保存断点
				Message.PutMessage(MSG_BREAKSCRIPT, Point, 0, 0, NULL);//放入一个消息
				_FREE( CmdName );
				_FREE( CmdStr );
				return SR_RECURSION;		//表示在递归，挂起
			}

			//没有指令，或者是return 就返回
			else if( stricmp( CmdStr, "return" ) == 0 || CmdStr==NULL )	
			{
				_FREE( CmdName );
				_FREE( CmdStr );
				if( RecursionLevel >0 )		//在递归中
				{
					RecursionLevel--;		//返回上一级
				}
				return SR_OK;
			}

			//调用其它脚本(递归调用)
			else if( stricmp( CmdName, "call" ) == 0 )
			{
				RecursionPoint[RecursionLevel]=Point;	//保存断点
				RecursionLevel++;				//级数加一
	
				CScript SubScript;				//调用的脚本
				char *str=GetStringParam(CmdStr, 0);	//得到脚本名字（第一个参数）
				if( SubScript.Run( str ) == SR_RECURSION )	//挂起
				{
					_FREE(str);
					_FREE( CmdName );
					_FREE( CmdStr );
					return SR_RECURSION;				//挂起
				}
				_FREE(str);
				goto _Next3;							//继续执行
			}
_Next3:
			RunCommand(CmdStr);		//执行
			_FREE( CmdName );
			_FREE( CmdStr );
		}
	}
	_FREE( CmdName );
	_FREE( CmdStr );

	return SR_ERROR;				//应该是以return的方式结束的，所以返回错误
}

//运行一条语句
bool CScript::RunCommand(char *CmdStr)
{
	char *str1=NULL, *str2=NULL, *str3=NULL;
	int CommandId=-1;
	char *Cmd=GetCommandName(CmdStr);		//得到指令名字
	
	if( CmdStr == NULL || Cmd == NULL ) 
		return false;

	for(int i=0; i<MAX_COMMAND; i++)
	{
		if( stricmp(Cmd, COMMAND[i].Name)==0 )		//找到该指令
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
	case 10:		//Goto	一个参数  
		{			//例如：goto("tiamo");
			char *str=GetStringParam(CmdStr, 0);
			Goto( str );
			free(str);
		}
		break;

	case 11:		//if	一个int型接一个分隔符接一个int型接一个分隔符接一个string型
		{			//例如：if($Process=1,"tiamo")
			int tmp1=GetIntParam(CmdStr, 0);
			int tmp2=GetIntParam(CmdStr, 1);
			char oper=GetOperation(CmdStr, 0);
			char *str=GetStringParam(CmdStr, 2);
			switch( oper )
			{
			case ',':
			case '=':
				if( tmp1==tmp2 )		//if($process=1,"tiamo")表示如果$process等于1的话
					Goto(str);			//跳转到标号叫tiamo的地方执行
				break;

			case '>':					//大于就跳转
				if( tmp1>tmp2 )
					Goto(str);
				break;

			case '<':					//小于就跳转
				if( tmp1<tmp2 )
					Goto(str);
				break;

			case '!':					//不等于就跳转
				if( tmp1 != tmp2 )
					Goto(str);
				break;
			}
			free(str);
		}
		break;

	case 16:		//set	设置全局变量的值,两个参数，一个全局变量，一个int变量
		{			//例如：set($tiamo,1);
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, GetIntParam(CmdStr, 1));
			free(var);
		}
		break;

	case 17:		//add 改变全局变量的值，两个参数，一个全局变量，一个int变量
		{			//例如：add($tiamo,1); 让$tiamo的值加1
			char *var=GetVariableParam(CmdStr, 0);
			AddValue(var, GetIntParam(CmdStr, 1));
			free(var);
		}
		break;

	case 18:		//random 产生随机数，两个参数，一个全局变量，一个int变量
		{			//例如：random($tiamo,100);产生一个100以内的随机数并赋值给$tiamo
			random(1);
			int num=GetIntParam(CmdStr, 1);
			int ran=random( num );
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, ran);
			free(var);
		}
		break;

	case 19:		//RGB合成三原色	四个参数，一个全局变量，三个int变量
		{			//例如：RGB($tiamo,0,0,0);让$tiamo变成黑色的RGB值
			int r=GetIntParam(CmdStr, 1);
			int g=GetIntParam(CmdStr, 2);
			int b=GetIntParam(CmdStr, 3);
			char *var=GetVariableParam(CmdStr, 0);
			SetValue(var, RGB(r, g, b));
			free(var);
		}
		break;

	//----------------------PutMessage--------------------------//

	case 20:		//SetPlayState	一个int参数：0 (挂起游戏，这时游戏脚本连续执行) 
					//						  1 (打开游戏，脚本单步执行)
		Message.PutMessage( MSG_SETPLAYSTATE, GetIntParam(CmdStr, 0), 0, 0, NULL);
		break;

	case 21:		//SetDisplayState	一个int参数：0 (白天) ；1 (夜晚)
		Message.PutMessage( MSG_SETDISPLAYSTATE, GetIntParam(CmdStr, 0), 0, 0, NULL);
		break;

	case 22:		//BackToTitle	没有参数，返回开始画面
		Message.PutMessage( MSG_BACKTOTITLE );
		break;

	case 23:		//TextTitle	两个string参数，filename,index，显示一段文字
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_TEXTTITLE, 0, 0, 0, str1, str2);
		break;

	case 24:		//Fight	三个string参数接三个int参数，打斗
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		str3 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_FIGHT, GetIntParam(CmdStr, 3), GetIntParam(CmdStr, 4), 
			GetIntParam(CmdStr, 5), str1, str2, str3);
		break;

	case 25:		//EnableFight	一个int参数，是否踩地雷
		Message.PutMessage( MSG_ENABLEFIGHT, GetIntParam(CmdStr, 0));
		break;

	case 26:		//BossFight     一个int参数，是否是boss战
		Message.PutMessage( MSG_BOSSFIGHT, GetIntParam(CmdStr,0));
		break;

	case 29:		//LoadMap		一个string参数，地图文件名
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_LOADMAP, 0, 0, 0, str1);
		break;

	case 30:		//SaveMap		一个string参数，地图文件名
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SAVEMAP, 0, 0, 0, str1);
		break;

	case 31:		//AddMapHook	三个参数，两个int表示坐标，接一个string表示脚本文件
		str1 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_ADDMAPHOOK, GetIntParam(CmdStr, 0), 
			GetIntParam(CmdStr, 1), 0, str1);
		break;

	case 32:		//SetMapHook	三个int参数，坐标两个接一个陷阱的id
		Message.PutMessage( MSG_SETMAPHOOK, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1), 
						GetIntParam(CmdStr, 2), NULL);
		break;

	case 33:		//SetMapCell	五个int参数，设置地图level(1)层的(x,y)点(2和3)为
					//								第page页(4)的第num号(5)元素
		Message.PutMessage( MSG_SETMAPCELL, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1), 
					GetIntParam(CmdStr, 2), (char *)GetIntParam(CmdStr,3), 
					(char *)GetIntParam(CmdStr,4));
		break;

	case 34:		//SetMapBlock	三个int参数，坐标接阻挡关系
		Message.PutMessage( MSG_SETMAPBLOCK, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1),
					GetIntParam(CmdStr, 2) );
		break;

	case 35:		//SetMapLevel	四个int参数，设置地图(x,y)点(1和2)的阻挡关系
					//							为level1\level2(3和4)
		Message.PutMessage( MSG_SETMAPLEVEL, GetIntParam(CmdStr, 0), GetIntParam(CmdStr, 1),
			GetIntParam(CmdStr, 2), (char *)GetIntParam(CmdStr,3), 
			(char *)GetIntParam(CmdStr,4));
		break;
		
	case 40:		//CenterMap		两个int(坐标)参数，以x,y为地图中心点
		Message.PutMessage( MSG_CENTERMAP, GetIntParam(CmdStr, 0), GetIntParam(CmdStr,1) );
		break;

	case 41:		//ScrollMapTo	三个int参数，两个坐标跟一个速度，卷动地图到x,y点(1和2)
		Message.PutMessage( MSG_SCROLLMAPTO, GetIntParam(CmdStr,0), GetIntParam(CmdStr,1), 
			GetIntParam(CmdStr,2) );
		break;

	case 50:		//LoadNpc		一个string参数，加载一个NPC文件
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_LOADNPC, 0,0,0, str1);
		break;

	case 60:
		Message.PutMessage(MSG_SETCPTYPE,GetIntParam(CmdStr,0),GetIntParam(CmdStr,1)
							,GetIntParam(CmdStr,2));
		break;

	case 100:		//MoveTo		两个int参数，强制主角走动到x,y点
		Message.PutMessage( MSG_MOVE, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1) ,0, NULL);
		break;

	case 101:		//SetPlayerDir	一个int参数，设置主角的方向，0=下 1=左 2=上 3=右
		Message.PutMessage( MSG_SETDIR, GetIntParam(CmdStr,0), 0 ,0, NULL);
		break;

	case 102:		//SetPlayerPos	三个int参数，设置主角的位置前两个是坐标，后一个不是必须
					//							表示是否清除原位置的地图阻挡，只要有，就清除
		Message.PutMessage( MSG_SETPOSITION, GetIntParam(CmdStr,0), GetIntParam(CmdStr,1) ,
					GetIntParam(CmdStr,2), NULL);
		break;

	case 103:		//CenterPlayer	没有参数，以主角为地图中心
		Message.PutMessage( MSG_CENTERPLAYER );
		break;

	case 104:		//SetPlayerShow	一个int参数		是否显示主角，1：是，0否
		Message.PutMessage( MSG_SETPLAYERSHOW, GetIntParam(CmdStr,0));
		break;

	case 110:		//LoadPlayer	三个参数，一个int表示角色编号，接两个string表示
					//						 ini文件和index索引，从ini文件读入主角
		str1 = GetStringParam(CmdStr, 1);
		str2 = GetStringParam(CmdStr, 2);
		Message.PutMessage( MSG_LOADPLAYER, GetIntParam(CmdStr,0),0,0, str1, str2);
		break;

	case 120:		//SwitchPlayer	两个int参数，表示主角编号，交换主角
		Message.PutMessage( MSG_SWITCHPLAYER, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1));
		break;

	case 121:		//SetPlayerNum	一个int参数，表示个数，设置主角个数
		Message.PutMessage( MSG_SETPLAYERNUM, GetIntParam(CmdStr,0));
		break;

	case 122:		//MoveDirectTo 两个int参数,一个是方向一个是步数
		Message.PutMessage(MSG_MOVEDIRECTTO,GetIntParam(CmdStr,0),GetIntParam(CmdStr,1));
		break;

	case 200:		//Talk			一个string参数，显示开场白对话，对话内容中<CR>代表换行
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_TALK, 0, 0, 0, str1 );
		break;

	case 201:		//Say		三个参数，两个string参数接一个int(不是必须)参数，缺省是1（左）
		{			//				前两个是对话，和人物头像，后一个是显示头像在左边还是右边
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

	case 210:		//AddMoney		一个int参数表示增加的金钱数目
		Message.PutMessage( MSG_ADDMONEY, GetIntParam(CmdStr, 0));
		break;

	case 221:		//AddObject		一个string接一个int，添加string物品int个
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_ADDOBJECT, GetIntParam(CmdStr, 1), 0, 0, str1);
		break;

	case 222:		//DelObject		一个string接一个int，删除string物品int个
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_DELOBJECT, GetIntParam(CmdStr, 1), 0, 0, str1);
		break;

	case 223:		//CheckObject	一个string参数，一个全局变量，检查string物品
		{			//								有多少，结果放到全局变量中
					//								在这个语句后请用break；
			char *strGoods = GetStringParam(CmdStr, 0);
			char *var=GetVariableParam(CmdStr, 1);
			int num = role[0].CheckObject(strGoods);
			SetValue(var, num);
		}
		break;

	case 224:		//AddMagic 一个string参数接一个int参数
		{			//			一个是魔法名，一个是学会这个魔法的主角
			str1 = GetStringParam(CmdStr,0);
			int rolenum = GetIntParam(CmdStr,1);
			Message.PutMessage(MSG_ADDMAGIC,rolenum,0,0,str1);
			break;
		}

	case 225:		//DelMagic 一个string参数接一个int参数
		{			//			一个是魔法名，一个是拥有这个魔法的主角
			str1 = GetStringParam(CmdStr,0);
			int rolenum = GetIntParam(CmdStr,1);
			Message.PutMessage(MSG_DELMAGIC,rolenum,0,0,str1);
			break;
		}

	case 250:		//MoveNpcTo		三个参数，一个string接两个int，移动名字叫string的
					//				NPC到两个int构成的坐标，这个时候是不会执行下一条消息的
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_MOVENPCTO, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2),
							0, str1);
		break;

	case 251:		//MoveNPC		和上一个基本相同，但是会继续执行消息
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_MOVENPC, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 0, 
							str1);
		break;

	case 252:		//SetNpcPos		四个参数，一个string三个int，设置名叫string到后两个int
					//						  组成的坐标，最后一个不是必须的，有，表示清除原
					//						  位置的阻挡关系
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCPOS, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 
					GetIntParam(CmdStr,3), str1 );
		break;

	case 253:		//SetNpcDir		一个string参数接一个int参数，和SetPlayDir相同
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCDIR, GetIntParam(CmdStr,1), 0,0, str1 );
		break;

	case 254:		//SetNpcStep	一个string参数接一个int参数，
					//				设置string的NPC的当前步是int的值
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCSTEP, GetIntParam(CmdStr,1), 0,0, str1 );
		break;

	case 255:		//AddNpc		两个string参数，接两个不必须的int参数
					//				NPC放在第一个string表示的ini文件的由第二个string表示的
					//				index索引中，后两个是坐标，没有的话就用ini文件里的坐标
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_ADDNPC, GetIntParam(CmdStr,2), GetIntParam(CmdStr,3),
					0, str1, str2);
		break;

	case 256:		//DelNpc		一个表示NPC名字的string参数
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_DELNPC, 0, 0, 0, str1);
		break;

	case 257:		//SetNpcActive	一个表示NPC名字的string参数接一个表示是否活动的int参数
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCACTIVE, GetIntParam(CmdStr,1), 0,0, str1);
		break;

	case 258:		//SetNpcShow	一个表示NPC名字的string参数接一个表示是否显示的int参数
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SETNPCSHOW, GetIntParam(CmdStr,1), 0,0, str1);
		break;

	case 259:		//SetNpcScript	一个表示NPC名字的string参数接一个表示脚本文件的string参数
		str1 = GetStringParam(CmdStr, 0);
		str2 = GetStringParam(CmdStr, 1);
		Message.PutMessage( MSG_SETNPCSCRIPT, 0,0,0, str1, str2);
		break;

	case 300:		//Delay			一个int参数，延时
		Message.PutMessage( MSG_DELAY, GetIntParam(CmdStr,0), 0, 0, NULL);
		break;

	case 301:		//FadeIn		三个int参数，效果颜色时间(后两个不是必须)，屏幕淡入
		Message.PutMessage( MSG_FADEIN, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1), 
			GetIntParam(CmdStr,2), NULL);
		break;
	
	case 302:		//FadeOut		两个int参数,效果加时间(时间不是必须的)，屏幕淡出
		Message.PutMessage( MSG_FADEOUT, GetIntParam(CmdStr,0), GetIntParam(CmdStr, 1), 0,
			NULL);
		break;

	case 303:		//PlayMusic		一个string参数，播放背景音乐（循环）
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_PLAYMUSIC, 0,0,0, str1 );
		break;

	case 304:		//StopMusic		没有参数，停止播放背景音乐
		Message.PutMessage( MSG_STOPMUSIC );
		break;

	case 305:		//PauseMusic	没有参数，继续播放背景音乐
		Message.PutMessage( MSG_PAUSEMUSIC );
		break;

	case 306:		//ResumeMusci	没有参数，暂停播放背景音乐
		Message.PutMessage( MSG_RESUMEMUSIC );
		break;

	case 310:		//PlaySound		一个string参数，播放一个音效（单次）
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_PLAYSOUND, 0,0,0, str1 );
		break;

	case 311:		//StopSound		没有参数，停止播放音效
		Message.PutMessage( MSG_STOPSOUND );
		break;

	case 340:		//GameMessage	四个参数，一个string接三个int，显示一条游戏消息
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_GAMEMESSAGE, GetIntParam(CmdStr,1), 
			GetIntParam(CmdStr,2), GetIntParam(CmdStr,3), str1);
		break;

	case 350:		//ShowMessage	四个参数，一个string接三个int，显示一条系统消息
		str1 = GetStringParam(CmdStr, 0);
		Message.PutMessage( MSG_SHOWMESSAGE, GetIntParam(CmdStr,1), GetIntParam(CmdStr,2), 
			GetIntParam(CmdStr,3), str1);
		break;
	}

	return true;
}
