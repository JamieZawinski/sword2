//********************************************
//	���� ��غ���
//  ������2000��10��09��
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
	strcpy(Record[0].Name, "����һ:");
	strcpy(Record[1].Name, "���ȶ�:");
	strcpy(Record[2].Name, "������:");
	strcpy(Record[3].Name, "������:");
	strcpy(Record[4].Name, "������:");
	strcpy(Record[5].Name, "������:");
	strcpy(Record[6].Name, "������:");
	strcpy(Record[7].Name, "���Ȱ�:");
	strcpy(Record[8].Name, "���Ⱦ�:");
	strcpy(Record[9].Name, "����ʮ:");
}

CLoadSave::~CLoadSave()
{
}

//��ȡ����
void CLoadSave::LoadWindowIni(char *filename, char *index)
{
	LoadListWindowIni(filename, index);
	LoadRecord("save\\sword");
}

//�����¼�б�
void CLoadSave::LoadRecord(char *filename)
{
	char temp[MAX_PATH];
	FILE *fp;
	for(int i=0; i<MAX_RECORD; i++)
	{
		sprintf(temp, "%s%d.dat", filename, i);
		if( (fp=fopen(temp, "rb"))==NULL )		//�ļ�����
		{
			memset(Record[i].About, 0, 32);
			strcpy(Record[i].About, "--------------");
			continue;
		}
		fseek(fp, 16, SEEK_SET);		//�����ļ�ͷ
		fread(Record[i].About, 32, 1, fp);	//����ʱ�䣬��ʾ���б���
		fclose(fp);
	}
}

//��ʾ
void CLoadSave::ShowLoadSaveWindow(LPDIRECTDRAWSURFACE surf, bool bLoad, int left, int top)
{
	int n;
	static int MouseY=-1;
	//��ʾ�б��ڿ�
	ShowListWindow(surf, false, left, top);

	//����ƶ�ѡ��
	if( MouseY != point.y )
	{
		MouseY=point.y;
		if( point.x > x && point.x < x+Width-ScrollBar.Width &&
			point.y > y && point.y < y+Height )
		{
			//��ǰѡ��
			Select=(point.y-y)/LineHeight + ScrollBar.CurNum;
			if( Select >= 10 )		//Խ������
				Select=9;
		}
	}

	//��ʾ
	int shownum=10;
	for(int i=0; i<shownum; i++)
	{		
		n=ScrollBar.CurNum+i;
		//������ʾ��ǰѡ����Ŀ
		if( n==Select )
		{
			//����λ�û�������
			if( ScrollBar.PageNum < ScrollBar.MaxNum-ScrollBar.MinNum )
				Bar(surf, x, y+LineHeight*i+2, x+Width-ScrollBar.Width, y+LineHeight*(i+1)-2, SelectColor);
			else
				Bar(surf, x, y+LineHeight*i+2, x+Width-1, y+LineHeight*(i+1)-2, SelectColor);
		}
		//��Ӱ��ʾ����
		DWORD c=Font.GetColor();
		Font.SetColor(0);
		Font.PrintText(surf, x+Column[1].x+1, y+4+LineHeight*i+1, Record[i].Name);
		Font.PrintText(surf, x+Column[3].x+1, y+4+LineHeight*i+1, Record[i].About);
		Font.SetColor(c);
		Font.PrintText(surf, x+Column[1].x, y+4+LineHeight*i, Record[i].Name);
		Font.PrintText(surf, x+Column[3].x, y+4+LineHeight*i, Record[i].About);
	}
}

//�˵�ѭ��
bool CLoadSave::DoMenu(LPDIRECTDRAWSURFACE& surf, bool bLoad, int left, int top)
{
	x=left+32;
	y=top+32;
	Blt(lpDDSBackGround, 0,0, lpDDSBack, RectScreen, false);
	ShowWindow(lpDDSBackGround, true, left, top);

	//�˵�ѭ��
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
			//��ȡ������Ϣ
			GetInputData();
			
			if( B_UP || mouse == RB_UP ) 
			{
				CMenu::Sound_Cancel.Play();
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
				return false;
			}

			//���ȷ��
			if( mouse == LB_UP )
			{
				if( point.x > x && point.x < x+Width-ScrollBar.Width &&
					point.y > y && point.y < y+Height )
				{
					CMenu::Sound_Click.Play();
					int sel=(point.y-y)/LineHeight + ScrollBar.CurNum;
					if( sel == Select )	//ȷ��
					{
						if( bLoad )		//����
						{
							if( Record[Select].About[0] != '-' )
							{
								LoadGame(Select);
								return true;
							}
						}
						else			//�浵
						{
							SaveGame(Select);
							GameMessage.DoMenu(lpDDSBack, "�洢�ɹ���");
							CMenu::ReInitMenu();
							return true;
						}
					}
				}
			}

			//���ȷ��
			if( A_UP )
			{
				CMenu::Sound_Click.Play();
				if( bLoad )		//����
				{
					if( Record[Select].About[0] != '-' )
					{
						LoadGame(Select);
						return true;
					}
				}
				else			//�浵
				{
					SaveGame(Select);
					GameMessage.DoMenu(lpDDSBack, "�洢�ɹ���");
					CMenu::ReInitMenu();
					return true;
				}
			}
			
			//��һ��
			if( DOWN_DOWN && Select < ScrollBar.MaxNum-1 )
			{
				CMenu::Sound_Change.Play();
				Select++;
				if( Select >= ScrollBar.CurNum + ScrollBar.PageNum )
					ScrollBar.SetCurNum(ScrollBar.CurNum+1);
			}

			//��һ��
			if( UP_DOWN && Select > 0 )
			{
				CMenu::Sound_Change.Play();
				Select--;
				if( Select < ScrollBar.CurNum )
					ScrollBar.SetCurNum(ScrollBar.CurNum-1);
			}
			//��ʾ��surf
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
		else WaitMessage();	//�ȴ���Ϣ
	}

	return false;
}

//�������
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
	
	// �����ɫ������
	for(int i=0; i<RoleNum; i++)
	{
		sprintf(temp, "%s%d.r%d",file, num, i);
		role[i].LoadRole(temp);
	}

	//����
	Script.LoadVariable(fp);

	//��������
	Music.Stop();
	bool bTemp;
	fread(&bTemp,1,1,fp);
	fread(&CMusic::bPlay, 1, 1, fp);
	fread(Music.MusicFile, sizeof(WCHAR), MAX_PATH, fp);
	fread(&Music.PauseTime, sizeof(MUSIC_TIME), 1, fp);
	fclose(fp);
	if(bTemp)
		Music.LoadMusic(Music.MusicFile);

	//���뵱ǰ��ͼ
	sprintf(temp, "%s%d.map",file, num);
	map.Load(temp);
	map.Block[role[0].State.Y*map.Width+role[0].State.X] = 1;
}

//�洢����
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

	//����
	Script.SaveVariable(fp);

	//��������
	fwrite(&CMusic::bMusic,1, 1, fp);
	fwrite(&CMusic::bPlay, 1, 1, fp);
	fwrite(Music.MusicFile, sizeof(WCHAR), MAX_PATH, fp);
	fwrite(&Music.PauseTime, sizeof(MUSIC_TIME), 1, fp);

	fclose(fp);

	//���浱ǰ��ͼ
	sprintf(temp, "%s%d.map",file, num);
	map.Save(temp);
}

//��ȡʱ��
void CLoadSave::GetGameTime(char *Tmp)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char show[2][5];
	//��λ���·�
	if( st.wMonth > 9 ) 
		sprintf(show[0], "%d", st.wMonth);
	else 
		sprintf(show[0], "0%d", st.wMonth);
	//��λ������
	if( st.wDay > 9 ) 
		sprintf(show[1], "%d", st.wDay);
	else 
		sprintf(show[1], "0%d", st.wDay);
	//��λ��Сʱ
	if( st.wHour > 9 ) 
		sprintf(show[2], "%d", st.wHour);
	else 
		sprintf(show[2], "0%d", st.wHour);
	//��λ�ķ���
	if( st.wMinute > 9 ) 
		sprintf(show[3], "%d", st.wMinute);
	else 
		sprintf(show[3], "0%d", st.wMinute);
	//��λ������
	if( st.wSecond > 9 ) 
		sprintf(show[4], "%d", st.wSecond);
	else 
		sprintf(show[4], "0%d", st.wSecond);

	sprintf(Tmp, "%s.%s %s:%s:%s", show[0], show[1], show[2], show[3], show[4]);
}