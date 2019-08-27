//********************************************
//	杂相函数
//  金点工作组 softboy 创建于2000年1月25日
//  E-mail:    wj77@163.net
//  Homepage:  goldpoint.126.com (金点时空)
//********************************************

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gp_draw.h"
#include "gp_init.h"
#include "gp_other.h"

//*********************************
//设置矩形
RECT GetRect(int x1,int y1,int x2,int y2)
{
	RECT TempRect={x1,y1,x2,y2};
	return TempRect;
}

//*********************
//随机数
int random(int nMax)
{ 
	return rand()*nMax/RAND_MAX;
}

//*************************
//按下键后松开
void PressKey(DWORD Key,int x)
{
	if( x==0 )
	while( !GetAsyncKeyState(Key) )
		{}
	while( GetAsyncKeyState(Key) )
		{}
}

//*************************
//等待按键
void WaitKey(DWORD Key)
{
	while( GetAsyncKeyState(Key) )	{}
	while( !GetAsyncKeyState(Key) )	{}
}

//*************************
//延时
void Delay(long time)
{
	static long old_clock, new_clock; //延时变量
	new_clock=old_clock=timeGetTime();

	MSG msg;
	while( new_clock < old_clock + time )
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			new_clock=timeGetTime();
			if( GetAsyncKeyState(VK_F4) )		//F4结束
			{
				return;
			}
		}
		else WaitMessage();	//等待消息
	}
}

//*********************
//速度测试
int CountSpeed()
{
	static long old_clock, new_clock; //延时变量
	static long ret;

	new_clock=timeGetTime();
	if( new_clock > old_clock + 1000 )
	{
		old_clock=new_clock;
		ret=nFrames;
		nFrames=0;
	}	
	return ret;
}

//***************************
//获取文件长度
int GetFileLength(char *name)
{
	int fh, nbytes;

	fh = _open( name, _O_RDONLY );	//只读模式
	if( fh== -1 )
	{
		return -1;
	}
	
	nbytes=_filelength(fh);
	_close(fh);
	
	return nbytes;
}


//**************************
//判断文件是否存在
bool CheckFile(char *name)
{
	//查找文件
	WIN32_FIND_DATA fdt;
	HANDLE hFile = FindFirstFile(name, &fdt);

	if( hFile ==INVALID_HANDLE_VALUE )
		return false;
	else 
	{
		FindClose(hFile);
		return true;
	}
}

//**************************
//显示调试信息
void ShowMessage(char *msg,...)
{
	va_list va;
	char str[256];

	va_start(va,msg);
	vsprintf(str,msg,va);
	va_end(va);

	MessageBox(hWnd, str, "Message",MB_OK);
}

//*************************
//输出调试信息
void PutDebugString(char *msg)
{
	static int num=0;
	char t[16];\

	FILE *fp;
	if( (fp=fopen("debug.txt","a+")) == NULL )
	{
		ShowMessage("Can't open Debug.txt (%s)", msg);
		return;
	}
	
	num++;
	sprintf(t, "\n%d:", num);
	fseek(fp, 0, SEEK_END);
	fwrite(t, strlen(t), 1, fp);
	fwrite(msg, strlen(msg), 1, fp);
	fclose(fp);
}

//*************************
//由文件名获取文件路径
char *GetFilePath(char *filename)
{
	char *Ret;
	int Len=strlen(filename);
	Ret=(char *)malloc(Len+1);
	strcpy(Ret, filename);

	for(int i=Len-1; i>=0; i--)
	{
		if( Ret[i] == '\\' )			//从后向前找'\'
			break;
		Ret[i]=0;						//设置成结束点
	}

	return Ret;
}
//*************************
//从文件名含路径分离出文件名不含路径
char* GetFileName(char* filename)
{
	char * szTemp = strrchr(filename,'\\');		//找最后一个'\'
	szTemp++;									//跳过'\'
	char *Ret = (char*)malloc(strlen(szTemp)+1);
	strcpy(Ret,szTemp);
	return Ret;
}
//*************************
//char to wchar
WCHAR* WChar(char *string)
{
	WORD *wchr;
	//将后两个参数设置成NULL和0
	//得到需要的缓冲区长度
	int wsize=MultiByteToWideChar(CP_ACP, NULL, string, strlen(string), NULL, 0);

	wchr=new WORD[wsize+1];		//多出一个放0
	MultiByteToWideChar(CP_ACP, NULL, string, strlen(string), wchr, wsize);
	wchr[wsize]=0;
	return wchr;
}

//*************************
//wchar to char
char* Char(WCHAR *string)						//想想intelCPU的放置方法
{												//比如一个L"tiamo"
	char *chr;									//那么应该是这样的方式：每个字符占16位
	chr=new char[wcslen(string)+1];				//依次为：
	memset(chr, 0, wcslen(string)+1);			//0x0074,0x0069,0x0061,0x006d,0x006f,0x0000
	for(unsigned int i=0; i<wcslen(string); i++)//而intel的CPU是这样放的
	{											//7400|6900|6100|6d00|6f00|0000
		chr[i]=(unsigned char)string[i];		//好了，这下来个强制类型转换……
	}										
	return chr;									
}
