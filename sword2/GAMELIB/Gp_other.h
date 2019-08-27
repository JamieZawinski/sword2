//********************************************
//	杂相函数
//   softboy 创建于2000年1月25日
//********************************************
#ifndef _GP_OTHER_
#define _GP_OTHER_

RECT GetRect(int x1,int y1,int x2,int y2);	//设置矩形
int random(int nMax);						//随机数
void PressKey(DWORD Key,int x=0);			//按下键后松开
void WaitKey(DWORD Key);
void Delay(long time);				//延时
int CountSpeed();					//计算速度
int GetFileLength(char *name);		//取文件长度
bool CheckFile(char *name);			//判断文件是否存在
void ShowMessage(char *msg,...);	//显示信息
void PutDebugString(char *msg);		//输出调试信息
char *GetFilePath(char *filename);	//由文件名获取文件路径
WCHAR* WChar(char *string);			//char to wchar
char* Char(WCHAR *string);			//wchar to char
char* GetFileName(char* filename);	//从文件名含路径分离出文件名不含路径
#endif