//********************************************
//	进度 相关函数
//  创建于2000年10月09日
//********************************************
#include "stdio.h"
#include "..\gamelib\goldpoint2.h"
#include "..\interface\interface2.h"
#include "..\main.h"
#include "..\mapedit\mapedit.h"
#include "..\map.h"
#include "..\role.h"
#include "..\script.h"
#include "menu_inc.h"

CLoadSave::CLoadSave()
{
	Select=0;
	PicWidth=PicHeight=32;
	strcpy(Record[0].Name, "进度一:");
	strcpy(Record[1].Name, "进度二:");
	strcpy(Record[2].Name, "进度三:");
	strcpy(Record[3].Name, "进度四:");
	strcpy(Record[4].Name, "进度五:");
	strcpy(Record[5].Name, "进度六:");
	strcpy(Record[6].Name, "进度七:");
	strcpy(Record[7].Name, "进度八:");
	strcpy(Record[8].Name, "进度九:");
	strcpy(Record[9].Name, "进度十:");
}

CLoadSave::~CLoadSave()
{
}

//读取窗口
void CLoadSave::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);
	LoadRecord("save\\sword");
}

//读入记录列表
void CLoadSave::LoadRecord(char *filename)
{
	char temp[MAX_PATH];
	FILE *fp;
	for(int i=0; i<MAX_RECORD; i++)
	{
		sprintf(temp, "%s%d.dat", filename, i);
		if( (fp=fopen(temp, "rb"))==NULL )		//文件存在
		{
			memset(Record[i].About, 0, 32);
			strcpy(Record[i].About, "--------------");
			continue;
		}
		fseek(fp, 16, SEEK_SET);		//跳过文件头
		fread(Record[i].About, 32, 1, fp);	//读出时间，显示到列表窗口
		fclose(fp);
	}
}

//显示
void CLoadSave::ShowLoadSaveWindow(LPDIRECTDRAWSURFACE surf, bool bLoad, int left, int top)
{
	int n;
	static int MouseY=-1;
	//显示列表窗口框
	ShowListWindow(surf, false, left, top);

	//鼠标移动选择
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			//当前选择
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( Select >= 10 )		//越界修正
				Select=9;
		}
	}

	//显示
	int shownum=10;
	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//高亮显示当前选择项目
		if( n==Select )
		{
			//留下位置画滚动条
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, SelectColor);
		}
		//阴影显示文字
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, Record[i].Name);
		Font.PrintText(surf, x+Column[3].x+1, y+4+LineHeight*i+1, Record[i].About);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, Record[i].Name);
		Font.PrintText(surf, x+Column[3].x, y+4+LineHeight*i, Record[i].About);
	}
}

//菜单循环
bool CLoadSave::DoMenu(LPDIRECTDRAWSURFACE& surf, bool bLoad, int left, int top)
{
	x=left+32;
	y=top+32;
	Blt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, false);
	ShowWindow(lpDDSBackGround, true, left, top);

	//菜单循环
	MSG msg;
	while(RunGame)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//获取输入信息
			GetInputData();
			
			if( B_UP || mouse == RB_UP ) 
			{
				CMenu::Sound_Cancel.Play();
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				return false;
			}

			//点击确定
			if( mouse == LB_UP )
			{
				if( point.x > x && point.x < x+Width-ScrollBar.Width &&
					point.y > y && point.y < y+Height )
				{
					CMenu::Sound_Click.Play();
					int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
					if( sel == Select )	//确定
					{
						if( bLoad )		//读档
						{
							if( Record[Select].About[0] != '-' )
							{
								LoadGame(Select);
								return true;
							}
						}
						else			//存档
						{
							SaveGame(Select);
							GameMessage.DoMenu(lpDDSBack, "存储成功！");
							CMenu::ReInitMenu();
							return true;
						}
					}
				}
			}

			//点击确定
			if( A_UP )
			{
				CMenu::Sound_Click.Play();
				if( bLoad )		//读档
				{
					if( Record[Select].About[0] != '-' )
					{
						LoadGame(Select);
						return true;
					}
				}
				else			//存档
				{
					SaveGame(Select);
					GameMessage.DoMenu(lpDDSBack, "存储成功！");
					CMenu::ReInitMenu();
					return true;
				}
			}
			
			//下一个
			if( DOWN_DOWN && Select < ScrollBar.MaxNum-1 )
			{
				CMenu::Sound_Change.Play();
				Select++;
				if( Select >= ScrollBar.CurNum + ScrollBar.PageNum )
					ScrollBar.SetCurNum(ScrollBar.CurNum+1);
			}

			//上一个
			if( UP_DOWN && Select > 0 )
			{
				CMenu::Sound_Change.Play();
				Select--;
				if( Select < ScrollBar.CurNum )
					ScrollBar.SetCurNum(ScrollBar.CurNum-1);
			}
			//显示到surf
			Blt(surf, 0,0, lpDDSBackGround, RectScreen, false);
			ShowLoadSaveWindow(surf, bLoad, left, top);

			ShowMouseCursor(surf, false);
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(surf, 20, 460, "FPS:%d", CountSpeed());
			}
			_UpdateScreen();
		}
		else WaitMessage();	//等待消息
	}

	return false;
}

//读入进度
void CLoadSave::LoadGame(int num)
{
	char *file="save\\sword";
	char temp[MAX_PATH];
	sprintf(temp, "%s%d.dat", file, num);

	FILE *fp;
	if( (fp=fopen(temp, "rb"))==NULL )
	{
		ShowMessage("Load %s error!", file);
		return;
	}

	fseek(fp, 16+32, SEEK_SET);
	fread(&g_ePlayState, 4, 1, fp);
	fread(&g_eDisplayState, 4, 1, fp);
	fread(&RoleNum, 4, 1, fp);
	
	// 读入角色的数据
	for(int i=0; i<RoleNum; i++)
	{
		sprintf(temp, "%s%d.r%d",file, num, i);
		role[i].LoadRole(temp);
	}

	//变量
	Script.LoadVariable(fp);

	//背景音乐
	Music.Stop();
	bool bTemp;
	fread(&bTemp,1,1,fp);
	fread(&CMusic::bPlay, 1, 1, fp);
	fread(Music.MusicFile, sizeof(WCHAR), MAX_PATH, fp);
	fread(&Music.PauseTime, sizeof(MUSIC_TIME), 1, fp);
	fclose(fp);
	if(bTemp)
		Music.LoadMusic(Music.MusicFile);

	//读入当前地图
	sprintf(temp, "%s%d.map",file, num);
	map.Load(temp);
	map.Block[role[0].State.Y*map.Width+role[0].State.X] = 1;
}

//存储进度
void CLoadSave::SaveGame(int num)
{
	char *file="save\\sword";
	char temp[MAX_PATH];
	sprintf(temp, "%s%d.dat", file, num);

	FILE *fp;
	if( (fp=fopen(temp, "wb"))==NULL )
	{
		ShowMessage("Save %s error!", file);
		return;
	}

	fwrite("Sword II record!", 16, 1, fp);
	char time[32];
	GetGameTime(time);
	fwrite(time, 32, 1, fp);
	strcpy(Record[num].About, time);
	fwrite(&g_ePlayState, 4, 1, fp);
	fwrite(&g_eDisplayState, 4, 1, fp);
	fwrite(&RoleNum, 4, 1, fp);
	
	for(int i=0; i<RoleNum; i++)
	{
		sprintf(temp, "%s%d.r%d",file, num, i);
		role[i].SaveRole(temp);
	}

	//变量
	Script.SaveVariable(fp);

	//背景音乐
	fwrite(&CMusic::bMusic,1, 1, fp);
	fwrite(&CMusic::bPlay, 1, 1, fp);
	fwrite(Music.MusicFile, sizeof(WCHAR), MAX_PATH, fp);
	fwrite(&Music.PauseTime, sizeof(MUSIC_TIME), 1, fp);

	fclose(fp);

	//保存当前地图
	sprintf(temp, "%s%d.map",file, num);
	map.Save(temp);
}

//获取时间
void CLoadSave::GetGameTime(char *Tmp)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char show[2][5];
	//两位的月份
	if( st.wMonth > 9 ) 
		sprintf(show[0], "%d", st.wMonth);
	else 
		sprintf(show[0], "0%d", st.wMonth);
	//两位的日期
	if( st.wDay > 9 ) 
		sprintf(show[1], "%d", st.wDay);
	else 
		sprintf(show[1], "0%d", st.wDay);
	//两位的小时
	if( st.wHour > 9 ) 
		sprintf(show[2], "%d", st.wHour);
	else 
		sprintf(show[2], "0%d", st.wHour);
	//两位的分钟
	if( st.wMinute > 9 ) 
		sprintf(show[3], "%d", st.wMinute);
	else 
		sprintf(show[3], "0%d", st.wMinute);
	//两位的秒数
	if( st.wSecond > 9 ) 
		sprintf(show[4], "%d", st.wSecond);
	else 
		sprintf(show[4], "0%d", st.wSecond);

	sprintf(Tmp, "%s.%s %s:%s:%s", show[0], show[1], show[2], show[3], show[4]);
}