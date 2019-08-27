//********************************************
//	对话 相关函数
//  创建于2000年5月01日
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gamelib\goldpoint.h"
#include "interface\interface.h"
#include "main.h"
#include "role.h"
#include "map.h"
#include "talk.h"
#include "Cursor.h"

extern bool bShowFps;

//构造
CTalk::CTalk()
{
	lpPhoto=NULL;
}

//析构
CTalk::~CTalk()
{
	_RELEASE(lpPhoto);
}

//初始化
bool CTalk::Init(char *ini, char *index)
{
	LoadWindowIni(ini, index);	//初始化窗口
	dialogx-=32;
	dialogy-=32;
	dialogw+=64;
	dialogh+=64;

	CIniSet Ini(ini);
	photox=Ini.ReadInt(index, "photox");
	photoy=Ini.ReadInt(index, "photoy");
	photox2=Ini.ReadInt(index, "photox2");
	photoy2=Ini.ReadInt(index, "photoy2");
	photow=Ini.ReadInt(index, "photow");
	photoh=Ini.ReadInt(index, "photoh");
	PhotoPlace=0;
	
	//头像图片
	char* strTemp = Ini.ReadText(index, "PhotoFilePath");
	strcpy(PhotoFilePath, strTemp);
	_FREE(strTemp);

	CreateBitmap(lpPhoto, photow, photoh);
	DDSetColorKey(lpPhoto, ColorKey);
	bPhoto=false;

	//继续用的小箭头
	strTemp = Ini.ReadText(index, "ContinueFile");
	char* strTemp2 = Ini.ReadText(index, "ContinueIndex");
	Continue.LoadAni(strTemp, strTemp2);
	_FREE(strTemp);
	_FREE(strTemp2);

	//使用整图对话框吗
	bFullDialog=Ini.ReadInt(index, "bFullDialog");

	if( bFullDialog )
	{
		//读取整图对话框背景
		strTemp = Ini.ReadText(index, "TalkPicFile");
		CreateBitmap(lpTalk, 0,0, strTemp);
		_FREE(strTemp);
		DDSetColorKey(lpTalk, ColorKey);
		//得到整个表面大小
		int ww, hh;
		GetSurfaceSize(lpTalk, ww, hh);
		TalkRect=GetRect(0, 0, ww, hh);

		TalkX=Ini.ReadInt(index, "TalkX");
		TalkY=Ini.ReadInt(index, "TalkY");

		//初始化Alpha值
		TalkAlphaColor=RGB16(Ini.ReadInt(index, "Talk_Red"),
							 Ini.ReadInt(index, "Talk_Green"),
							 Ini.ReadInt(index, "Talk_Blue"));
	}
	//延时
	NextTick=timeGetTime();

	return true;
}

//打开对话文件
void CTalk::OpenFile(char *filename)
{
	Dialog.Open(filename);
	strcpy(FileName, filename);
}

//生成头像
void CTalk::CreatePhoto(char *filename)
{
	if( filename==NULL ) return;

	char File[256];
	sprintf(File, "%s%s%s", PhotoFilePath, filename, ".bmp");

	//无头像
	if( strcmp(filename, "旁白")==0 || CheckFile(File) == false )
	{
		bPhoto=false;
		return;
	}

	//读入头像
	LPDIRECTDRAWSURFACE Photo;
	BOOL ret=CreateBitmap(Photo, 0, 0, File);
	if( ret==TRUE )
	{
		Blt(lpPhoto, 0, 0, Photo, GetRect(0,0,photow,photoh), false);
		_RELEASE(Photo);
		bPhoto=true;
		return;
	}

	bPhoto=false;
}

//显示头像
void CTalk::ShowPhoto(LPDIRECTDRAWSURFACE lpDDSTemp, int bPhotoInLeft)
{
	if( bPhoto )
	{
		int left =0 , top = 0;
		if( bPhotoInLeft )			//头像在左边
		{
			left=photox;
			top=photoy;
		}
		else
		{
			left=photox2;
			top=photoy2;
		}

		Blt(lpDDSTemp, left, top, lpPhoto, GetRect(0,0,photow,photoh), true);
	}
}

//显示对话
void CTalk::ShowText(LPDIRECTDRAWSURFACE lpDDSTemp, char *string)
{
	//用双色显示
	DWORD c=Font.GetColor();
	Font.SetColor(0);
	Font.SetChangeColor(false);
	Font.PrintText(lpDDSTemp, dialogx+42, dialogy+42, string);

	Font.SetColor(c);
	Font.SetChangeColor(true);
	Font.PrintText(lpDDSTemp, dialogx+40, dialogy+40, string);
}

//一段对话
void CTalk::Talk(LPDIRECTDRAWSURFACE& surf, char *index, int bPhotoInLeft)
{
	CurrentLine=0;
	strcpy(Index, index);

	GetGameSurface(lpDDSBackGround);			//得到一个游戏画面
	if( !bFullDialog ) 
		PopWindow(lpDDSBack, PopWindowStyle, PopWindowStep);	//弹出窗口

	Font.DelayTick=CGpFont::DELAYTICK[3];	//文字出现速度还原
	char *photo, *dlg=NULL;//
	int len=0, stringlen=0;
	char *str=NULL;
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	MSG msg;
	while(RunGame)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//延时控速
			NowTick=timeGetTime();
			if( NowTick < OldTick+DelayTick ) continue;
			OldTick=NowTick;

			//获取输入信息
			GetInputData();

			//直接完毕对话
			if( key==DIK_E + 128 ) 
			{
				g_ePlayState = PS_MAIN;
				if( g_eDisplayState == DS_NIGHT )
					Clrscr(lpDDSPrimary);
			}

			//下一句
			if( PressAnyKey()!=0 || A_UP || CurrentLine==0 )
			{
				if( len < stringlen )	//全显对话
				{
					len=stringlen-2;
				}
				else
				{
					photo=Dialog.ReadData(Index, CurrentLine);	//获取头像
					if( photo[0]==0 )
					{
						g_ePlayState=PS_MAIN;
						if( g_eDisplayState==DS_NIGHT )
							Clrscr(lpDDSPrimary);
						return;		//结束
					}
					
					CreatePhoto(photo);	//创建头像
					_DELETE(dlg);
					dlg=Dialog.ReadText(Index, CurrentLine);	//获取对话
					
					//分配缓存
					stringlen=strlen(dlg);
					_DELETE(str);
					str=new char[stringlen+1];
					len=0;

					CurrentLine++;
				}
			}

			//复制文字
			if( len < stringlen && timeGetTime()>=NextTick )
			{
				NextTick=timeGetTime()+Font.DelayTick;

				len+=2;					//处理控制字符
				if( len > stringlen ) 
					len=stringlen;
				else if( dlg[len-2]=='<' )
				{
					len+=2;
				}
				else if( dlg[len-1]=='<' )
				{
					len+=3;
				}
				memcpy(str, dlg, len);
				memset(&str[len], 0, 1);
			}

			map.ShowMap();

			if( g_eDisplayState==DS_NIGHT )
			{
				Clrscr(lpDDSBackGround, 0);
				UpdateScreen(lpDDSBackGround, RectScreen);
			}
			else
				Blt(surf, 0,0, lpDDSBack, RectScreen, false);

			ShowPhoto(surf, bPhotoInLeft);				//显示头像
			if( !bFullDialog ) 
				ShowWindow(surf);		//显示对话框
			else 
				AlphaColorBlt(surf, TalkX, TalkY, lpTalk, TalkRect, ColorKey16, TalkAlphaColor, Alpha);
			ShowText(surf, str);	//显示文字
			if( len == stringlen )
				Continue.ShowAni(surf, dialogx+dialogw+Continue.Getx(), 
					dialogy+dialogh+Continue.Gety());	//闪烁的小箭头
			//画上FPS
			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}

			if( g_eDisplayState==DS_NIGHT )
			{
				ShowMouseCursor(surf,false);
				UpdateScreen_Dialog(surf);
			}
			else
			{
				ShowMouseCursor(surf,false);
				_UpdateScreen(surf);
			}
		}
		else WaitMessage();	//等待消息
	}

	_DELETE( str );
}

//一句话
void CTalk::Say(LPDIRECTDRAWSURFACE& surf, char *string, char *photo, int bPhotoInLeft)
{
	GetGameSurface(lpDDSBackGround);	//得到一个游戏画面
	if( !bFullDialog ) 
		PopWindow(lpDDSBack, PopWindowStyle, PopWindowStep);	//弹出窗口

	Font.DelayTick=CGpFont::DELAYTICK[3];	//文字出现速度还原
	CreatePhoto(photo);						//创建头像
	int len=0, stringlen=strlen(string);
	char *str=new char[stringlen+1];
	CCursor::GetCursor(CCursor::CS_CHANGEMAP).pPic.SetShowCount(0);
	MSG msg;
	while(RunGame)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
            if (!GetMessage(&msg, NULL, 0, 0)) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if(bActive)
		{
			//延时控速
			NowTick=timeGetTime();
			if( NowTick < OldTick+DelayTick ) continue;
			OldTick=NowTick;

			//获取输入信息
			GetInputData();

			if( mouse==LB_UP || A_UP || B_UP )
			{
				if( len < stringlen )	//全显对话
				{
					len=stringlen-2;
				}
				else	//完毕对话
				{
					g_ePlayState=PS_MAIN;
					if( g_eDisplayState==DS_NIGHT )
						Clrscr(lpDDSPrimary);
					break;
				}
			}

			//复制文字
			if( len < stringlen && timeGetTime()>=NextTick )
			{
				NextTick=timeGetTime()+Font.DelayTick;

				len+=2;					//处理控制字符
				if( len > stringlen ) 
					len=stringlen;
				else if( string[len-2]=='<' )
				{
					len+=2;
				}
				else if( string[len-1]=='<' )
				{
					len+=3;
				}
				memcpy(str, string, len);
				memset(&str[len], 0, 1);
			}

			map.ShowMap();

			if( g_eDisplayState==DS_NIGHT )
			{
				Clrscr(lpDDSBackGround, 0);
				UpdateScreen(lpDDSBackGround, RectScreen);
			}
			else
				Blt(surf, 0,0, lpDDSBack, RectScreen, false);

			ShowPhoto(surf, bPhotoInLeft);		//显示头像
			if( !bFullDialog ) 
				ShowWindow(surf);	//显示对话框
			else 
				AlphaColorBlt(surf, TalkX, TalkY, lpTalk, TalkRect, ColorKey16, TalkAlphaColor, Alpha);
			ShowText(surf, str);	//显示文字
			if( len == stringlen )
				Continue.ShowAni(surf, dialogx+dialogw+Continue.Getx(), dialogy+dialogh+Continue.Gety());	//闪烁的小箭头

			if( bShowFps)
			{
				nFrames++;
				font.PrintText(lpDDSBack, 20, 460, "FPS:%d", CountSpeed());
			}
			if( g_eDisplayState==DS_NIGHT )
			{
				ShowMouseCursor(surf,false);
				UpdateScreen_Dialog(surf);
			}
			else
			{
				ShowMouseCursor(surf,false);
				_UpdateScreen(surf);
			}
		}
		else WaitMessage();	//等待消息
	}
	
	_DELETE( str );
}


