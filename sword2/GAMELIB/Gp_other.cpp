//********************************************
//	���ຯ��
//  ��㹤���� softboy ������2000��1��25��
//  E-mail:    wj77@163.net
//  Homepage:  goldpoint.126.com (���ʱ��)
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
//���þ���
RECT GetRect(int x1,int y1,int x2,int y2)
{
	RECT TempRect={x1,y1,x2,y2};
	return TempRect;
}

//*********************
//�����
int random(int nMax)
{ 
	return rand()*nMax/RAND_MAX;
}

//*************************
//���¼����ɿ�
void PressKey(DWORD Key,int x)
{
	if( x==0 )
	while( !GetAsyncKeyState(Key) )
		{}
	while( GetAsyncKeyState(Key) )
		{}
}

//*************************
//�ȴ�����
void WaitKey(DWORD Key)
{
	while( GetAsyncKeyState(Key) )	{}
	while( !GetAsyncKeyState(Key) )	{}
}

//*************************
//��ʱ
void Delay(long time)
{
	static long old_clock, new_clock; //��ʱ����
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
			if( GetAsyncKeyState(VK_F4) )		//F4����
			{
				return;
			}
		}
		else WaitMessage();	//�ȴ���Ϣ
	}
}

//*********************
//�ٶȲ���
int CountSpeed()
{
	static long old_clock, new_clock; //��ʱ����
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
//��ȡ�ļ�����
int GetFileLength(char *name)
{
	int fh, nbytes;

	fh = _open( name, _O_RDONLY );	//ֻ��ģʽ
	if( fh== -1 )
	{
		return -1;
	}
	
	nbytes=_filelength(fh);
	_close(fh);
	
	return nbytes;
}


//**************************
//�ж��ļ��Ƿ����
bool CheckFile(char *name)
{
	//�����ļ�
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
//��ʾ������Ϣ
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
//���������Ϣ
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
//���ļ�����ȡ�ļ�·��
char *GetFilePath(char *filename)
{
	char *Ret;
	int Len=strlen(filename);
	Ret=(char *)malloc(Len+1);
	strcpy(Ret, filename);

	for(int i=Len-1; i>=0; i--)
	{
		if( Ret[i] == '\\' )			//�Ӻ���ǰ��'\'
			break;
		Ret[i]=0;						//���óɽ�����
	}

	return Ret;
}
//*************************
//���ļ�����·��������ļ�������·��
char* GetFileName(char* filename)
{
	char * szTemp = strrchr(filename,'\\');		//�����һ��'\'
	szTemp++;									//����'\'
	char *Ret = (char*)malloc(strlen(szTemp)+1);
	strcpy(Ret,szTemp);
	return Ret;
}
//*************************
//char to wchar
WCHAR* WChar(char *string)
{
	WORD *wchr;
	//���������������ó�NULL��0
	//�õ���Ҫ�Ļ���������
	int wsize=MultiByteToWideChar(CP_ACP, NULL, string, strlen(string), NULL, 0);

	wchr=new WORD[wsize+1];		//���һ����0
	MultiByteToWideChar(CP_ACP, NULL, string, strlen(string), wchr, wsize);
	wchr[wsize]=0;
	return wchr;
}

//*************************
//wchar to char
char* Char(WCHAR *string)						//����intelCPU�ķ��÷���
{												//����һ��L"tiamo"
	char *chr;									//��ôӦ���������ķ�ʽ��ÿ���ַ�ռ16λ
	chr=new char[wcslen(string)+1];				//����Ϊ��
	memset(chr, 0, wcslen(string)+1);			//0x0074,0x0069,0x0061,0x006d,0x006f,0x0000
	for(unsigned int i=0; i<wcslen(string); i++)//��intel��CPU�������ŵ�
	{											//7400|6900|6100|6d00|6f00|0000
		chr[i]=(unsigned char)string[i];		//���ˣ���������ǿ������ת������
	}										
	return chr;									
}
