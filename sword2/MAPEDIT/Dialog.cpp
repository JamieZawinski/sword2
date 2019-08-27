//********************************************
//	对话框 相关函数
//  创建于2000年3月17日
//********************************************
#include <windows.h>
#include "Shlwapi.h"
#include <stdio.h>
#include "shlobj.h"
#include "..\resource.h"
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "..\cursor.h"
#include "..\map.h"
#include "..\fight\fight.h"
#include "mapedit.h"
#include "dialog.h"
#pragma comment(lib,"shlwapi.lib")

int CGetType;	//类型 0=字符串 1=数值 3=资源图片名
char *CGetDlgMessage;	//提示文字
char *CGetStringDlgBuffer;	//字符串
int CGetIntDlg;				//数值

char strHelp[]="\
[鼠标左键]	=填充图块\n\
[鼠标右键]	=弹出菜单\n\
\n\
--组合健--\n\
[Ctrl+左键]	=删除物体\n\
[Alt+左键]	=修改格子阻挡关系\n\
[Shift+左键]	=获取当前图块\n\
[Space+左健]	=修改图块层次\n\
[Ctrl+Z]	=反悔一步误操作\n\
\n\
  - 金点时空(www.gpgame.com) -\n\
 \n\
　主策划 : sleepd\n\
执行策划 : ended-man\n\
　　程序 : softboy、LDH\n\
　　美工 : ended-man、\n\
　　　　　 sleepd、zrhspy、\n\
　　　　　 星仔、qinyong";

/////////////////////////////////////////////////////////////////////
//
//************************
//创建模态对话框
void CreateDlg(int Templete, DLGPROC Proc)
{
	DialogBox( 	hInst, 
				MAKEINTRESOURCE(Templete),
				hWnd, 
				Proc);
}

/////////////////////////////////////////////////////////////////////
//
//************************
//创建非模态对话框
void CreateDlg2(int Templete, DLGPROC Proc)
{
	CreateDialog( 	hInst, 
					MAKEINTRESOURCE(Templete),
					hWnd, 
					Proc);
}

/////////////////////////////////////////////////////////////////////
//
//***********************
//输入一个字符串
char *GetStringDlg(char *Msg)
{
	CGetDlgMessage=Msg;
	CGetType=0;
	CreateDlg( IDD_DIALOG_GetString, (DLGPROC)CGetStringDlgProc );
	return CGetStringDlgBuffer;
}

/////////////////////////////////////////////////////////////////////
//
//***********************
//输入一个数值
int GetIntDlg(char *Msg)
{
	CGetDlgMessage=Msg;
	CGetType=1;
	CreateDlg( IDD_DIALOG_GetInt, (DLGPROC)CGetStringDlgProc );
	return CGetIntDlg;
}

/////////////////////////////////////////////////////////////////////
//
//*****************************
//输入窗口消息
BOOL APIENTRY CGetStringDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT idCommand;

    switch (message)
    {
        case WM_INITDIALOG:
			//定位到中心点
			{
			RECT rect;
			GetWindowRect(hDlg, &rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			if( WindowMode==1 )	//窗口
				MoveWindow(hDlg, RectWindow.left+(ScreenWidth-w)/2, RectWindow.top+(ScreenHeight-h)/2, w, h, TRUE);
			else
				MoveWindow(hDlg, (ScreenWidth-w)/2, (ScreenHeight-h)/2, w, h, TRUE);
			}

			//显示提示信息
			if( CGetDlgMessage==NULL )
			{
				if( CGetType==0 )
					SetDlgItemText( hDlg, IDC_EDIT_Message, "请输入一串字符：[结束按OK]");
				if( CGetType==1 )
					SetDlgItemText( hDlg, IDC_EDIT_Message, "请输入一个数字：[结束按OK]");
			}
			else
				SetDlgItemText( hDlg, IDC_EDIT_Message, CGetDlgMessage);

			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hWnd);

            return TRUE;
            break;

		case WM_COMMAND:
			idCommand=LOWORD(wParam);
			switch( idCommand )
			{
			case IDOK:
				if( CGetType==0 )	//字符串
				{
					CGetStringDlgBuffer=(char *)malloc(65535);
					GetDlgItemText(hDlg, IDC_EDIT_String, CGetStringDlgBuffer, 65535);
				}
				else if( CGetType==1 )	//数值
				{
					CGetIntDlg=GetDlgItemInt(hDlg, IDC_EDIT_Int, NULL, true);
				}
				EndDialog(hDlg,false);
				return true;
			
			case IDCANCEL:
				if( CGetType==0 )	//字符串
				{
					CGetStringDlgBuffer=NULL;
				}
				else if( CGetType==1 )	//数值
				{
					CGetIntDlg=-9999;
				}
				PressKey(VK_ESCAPE, 1);
				EndDialog(hDlg,false);
				return true;
			}

	    case WM_DESTROY:
            return TRUE;
            break;

        default:
            return FALSE;
    }
return true;
}

/////////////////////////////////////////////////////////////////////
//
//*****************************
//处理主窗口消息
BOOL APIENTRY DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	static char strBackBmp[128];
	UINT idCommand;
	Cell_Struct *tmp;

	HWND hComBoxLevel;
	hComBoxLevel=GetDlgItem(hDlg, IDC_COMBO_Level);

    switch (message)
    {

        case WM_INITDIALOG:

			//定位到中心点
			{
			RECT rect;
			GetWindowRect(hDlg, &rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			if( WindowMode==1 )	//窗口
				MoveWindow(hDlg, RectWindow.left+(ScreenWidth-w)/2, RectWindow.top+(ScreenHeight-h)/2, w, h, TRUE);
			else
				MoveWindow(hDlg, (ScreenWidth-w)/2, (ScreenHeight-h)/2, w, h, TRUE);
			
			//显示复选控件
			if( bShowBackGround )
				CheckDlgButton(hDlg, IDC_CHECK_ShowBackGround, 1);

			if( bShowCell )
				CheckDlgButton(hDlg, IDC_CHECK_ShowCell, 1);

			if( bShowBlock )
				CheckDlgButton(hDlg, IDC_CHECK_ShowBlock, 1);

			if( bShowObject )
				CheckDlgButton(hDlg, IDC_CHECK_ShowObject, 1);

			if( bShowCurrent )
				CheckDlgButton(hDlg, IDC_CHECK_ShowCurrent, 1);

			if( bShowEdit )
				CheckDlgButton(hDlg, IDC_CHECK_ShowEdit, 1);

			if( bShowOnlyObject )
				CheckDlgButton(hDlg, IDC_CHECK_ShowOnlyObject, 1);

			//设置地图信息
			SetDlgItemInt( hDlg, IDC_EDIT_Id, map.Id, TRUE);
			SetDlgItemText( hDlg, IDC_EDIT_Name, map.Name);
			SetDlgItemInt( hDlg, IDC_EDIT_Width, map.Width-1, TRUE);
			SetDlgItemInt( hDlg, IDC_EDIT_Height, map.Height-1, TRUE);
			SetDlgItemText( hDlg, IDC_EDIT_BackBmp, map.m_strBackBmpFilename);

			//初始化层次复选框
			for(int i=0; i<Max_Level; i++)
			{
				SendMessage(hComBoxLevel, CB_ADDSTRING, 0, (LPARAM)Level[i]);
			}
			SendMessage(hComBoxLevel, CB_SETCURSEL, Cur_Level, 0);

			//设置窗口为当前文件名
			SetWindowText(hDlg, map.CurrentMapFileName);

			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hDlg);
			}

            return TRUE;
            break;

		case WM_COMMAND:
			idCommand=LOWORD(wParam);
			switch( idCommand )
			{
			case IDOK:
				//检查网格显示
				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowBackGround) )
					bShowBackGround=true;
				else 
					bShowBackGround=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowCell) )
					bShowCell=true;
				else 
					bShowCell=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowBlock) )
					bShowBlock=true;
				else 
					bShowBlock=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowObject) )
					bShowObject=true;
				else 
					bShowObject=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowCurrent) )
					bShowCurrent=true;
				else 
					bShowCurrent=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowEdit) )
					bShowEdit=true;
				else 
					bShowEdit=false;

				if( IsDlgButtonChecked(hDlg, IDC_CHECK_ShowOnlyObject) )
					bShowOnlyObject=true;
				else 
					bShowOnlyObject=false;

				// 读入背景图片
				if( strBackBmp[0] )
				{
					if( strcmp(strBackBmp, map.m_strBackBmpFilename) != 0 )
					{
						strcpy(map.m_strBackBmpFilename, strBackBmp);
						CreateBitmap(map.m_pBackBmp, 0,0, map.m_strBackBmpFilename);
					}
				}
				else
				{
					map.m_strBackBmpFilename[0] = 0;
					_RELEASE( map.m_pBackBmp );
				}
				
				EndDialog(hDlg,false);
				return true;

			//返回
			case IDCANCEL:
				EndDialog(hDlg,false);
				return true;

			//更新
			case IDC_EDIT_Name:
				switch( HIWORD(wParam) )
				{
					case EN_CHANGE:
						GetDlgItemText(hDlg, IDC_EDIT_Name, map.Name, 64);
						break;
				}
				break;

			//更新
			case IDC_EDIT_BackBmp:
				switch( HIWORD(wParam) )
				{
					case EN_CHANGE:
						GetDlgItemText(hDlg, IDC_EDIT_BackBmp, strBackBmp, 128);
						break;
				}
				break;

			//更新
			case IDC_EDIT_Id:
				switch( HIWORD(wParam) )
				{
					case EN_CHANGE:
						map.Id=GetDlgItemInt(hDlg, IDC_EDIT_Id, NULL, NULL);
						break;
				}
				break;

			//编辑层次
			case IDC_COMBO_Level:
				switch( HIWORD(wParam) )
				{
					case CBN_SELCHANGE:
						Cur_Level=SendMessage(hComBoxLevel, CB_GETCURSEL, 0,0);
						break;
				}
				break;

			//高级-陷阱
			case IDC_BUTTON_ChangePoint:
				CreateDlg( IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);
				break;

			//高级-格子属性
			case IDC_BUTTON_Object:
				CreateDlg( IDD_DIALOG_Object, (DLGPROC)DialogObjectProc);
				break;

			// 敌人设置
			case IDC_BUTTON_Enemy:
				CreateDlg(IDD_DIALOG_EnemySet, DialogEnemySetListProc);
				break;

			//导出地图
			case ID_MENUITEM_SaveFullMap:
				{
					char* str = GetFileDialog(false);
					if( str )
					{
						map.SaveFullMap(str, 1.f);
					}
					delete str;
				}
				break;

			//退出
			case ID_MENUITEM_Exit:
				if( MessageBox(hDlg, "确定吗？（注意保存地图）", "Exit", MB_OKCANCEL)==IDOK )
				{
					RunGame=false;
					EndDialog(hDlg,false);
					return true;
				}
				break;

			//帮助
			case ID_MENUITEM_Help:
				MessageBox(hDlg, strHelp, "Help", MB_OK);
				break;

			//新建地图
			case ID_MENUITEM_New:
				_NewMap(hDlg);
				EndDialog(hDlg,false);
				break;

			//重新读入当前地图
			case ID_MENUITEM_ReOpen:
				if( MessageBox(hWnd, "重新读入当前地图吗？","Reopen", MB_YESNO )==IDYES )
				{
					char strTemp1[MAX_PATH];
					char* strTemp2 = strTemp1;

					strcpy(strTemp1,map.CurrentMapFileName);				

					strTemp2 += strlen(CurrentPath)+1;			//跳过路径
					SetCurrentDirectory(CurrentPath);
					/*char strTemp1[256];				//保存当前路径名字
					strcpy(strTemp1,CurrentPath);

					char* strTemp2 = GetFilePath(map.CurrentMapFileName);	//得到文件路径
					SetCurrentDirectory(strTemp2);							//设为当前路径
					strTemp2[strlen(strTemp2)-1] = 0;

					strcpy(CurrentPath,strTemp2);
					_FREE(strTemp2);
					strTemp2 = GetFileName(map.CurrentMapFileName);			//得到文件名
					*/
					map.LoadMap(strTemp2);									//加载地图
					//_FREE(strTemp2);

					//strcpy(CurrentPath,strTemp1);							//恢复当前路径
					//SetCurrentDirectory(CurrentPath);*/
				}
				break;

			//打开文件名
			case ID_MENUITEM_Open:
				if( _LoadMap(hDlg) )
				{
					SetWindowText(hDlg, map.CurrentMapFileName);

					//显示地图信息
					SetDlgItemText( hDlg, IDC_EDIT_Name, map.Name);
					SetDlgItemInt( hDlg, IDC_EDIT_Width, map.Width-1, TRUE);
					SetDlgItemInt( hDlg, IDC_EDIT_Height, map.Height-1, TRUE);

					EndDialog(hDlg,false);
					return true;
				}
				break;

			//保存地图
			case ID_MENUITEM_Save:
				_SaveMap(hDlg);
				EndDialog(hDlg,false);
				break;

			//另存地图
			case ID_MENUITEM_SaveAs:
				if( _SaveAsMap(hDlg) )
				{
					SetWindowText(hDlg, map.CurrentMapFileName);
					GetDlgItemText(hDlg, IDC_EDIT_Name, map.Name, 32);
					EndDialog(hDlg,false);
					return true;
				}
				break;

			case ID_MENUITEM_Undo:		//反悔一步

				tmp=map.Cell;
				map.Cell=UndoMap;
				UndoMap=tmp;

				EndDialog(hDlg,false);
				return true;

			case ID_MENUITEM_RestoreLevel:	//恢复层次关系
				{
					static l=1;
					l=1-l;
					memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
					map.RestoreLevel(l);
					EndDialog(hDlg,false);
					return true;
				}

			case ID_MENUITEM_RestoreBlock:	//恢复阻挡关系
				{
					static l=1;
					l=1-l;
					memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
					map.RestoreBlock(l);
					EndDialog(hDlg,false);
					return true;
				}

			case ID_MENUITEM_RestoreMouseType:	//恢复鼠标状态
				{
					memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
					map.RestoreMouseType(CCursor::CS_NORMAL);
					EndDialog(hDlg,false);
					return true;
				}

			//设置敌人
			case ID_MENUITEM_Enemy:
				CreateDlg(IDD_DIALOG_EnemySet, DialogEnemySetListProc);
				break;

			//格子属性
			case ID_MENUITEM_Cell:
				CreateDlg(IDD_DIALOG_Object, DialogObjectProc);
				break;

			//设置陷阱
			case ID_MENUITEM_SetHook:
				CreateDlg(IDD_DIALOG_Hook, (DLGPROC)DialogChangePointListProc);
				break;

			//删除陷阱
			case ID_MENUITEM_DelHook:
				memcpy( UndoMap,map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
				map.Cell[map.Width*cy+cx].CP=BLOCK_CP;
				break;

			//显示缩略图
			case ID_MENUITEM_FullMap:
				map.ShowFullMap();
				break;
			}
			break;

	    case WM_DESTROY:
            return TRUE;
            break;

        default:
            return false;
    }
return true;
}

/////////////////////////////////////////////////////////////////////
//
//*****************************
//陷阱点列表窗口消息
BOOL APIENTRY DialogChangePointListProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT idCommand;
	static int Sel=-1;

	HWND hListBox, hComBoxMouseType,hComBoxHookType;
	hListBox=GetDlgItem(hDlg, IDC_LIST_ChangePoint);
	hComBoxMouseType=GetDlgItem(hDlg, IDC_COMBO_CPType);
	hComBoxHookType = GetDlgItem(hDlg, IDC_COMBO_HOOKTYPE);

    switch (message)
    {
        case WM_INITDIALOG:
			{

			//定位到中心点
			RECT rect;
			GetWindowRect(hDlg, &rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			if( WindowMode==1 )	//窗口
				MoveWindow(hDlg, RectWindow.left+(ScreenWidth-w)/2, RectWindow.top+(ScreenHeight-h)/2, w, h, TRUE);
			else
				MoveWindow(hDlg, (ScreenWidth-w)/2, (ScreenHeight-h)/2, w, h, TRUE);
			
			//初始化列表框
			if( map.Hook_Num>0 )
			{
				for(int i=map.Hook_Num-1; i>=0; i--)
				{
					char List[128];
					sprintf(List, "(%d) %s",i,map.Hook[i]);
					SendMessage(hListBox, LB_INSERTSTRING, 0, (LPARAM)List);
				}
			}

			SetDlgItemInt(hDlg, IDC_EDIT_ox, cx, true);
			SetDlgItemInt(hDlg, IDC_EDIT_oy, cy, true);

			Sel=-1;

			//初始化陷阱类型复选框
			for(int i=0; i<CCursor::GetCursorNum(); i++)
			{
				SendMessage(hComBoxMouseType, CB_ADDSTRING, 0, (LPARAM)CCursor::GetCursor(i).strName);
			}
			SendMessage(hComBoxHookType, CB_ADDSTRING, 0, (LPARAM)"非切换点");
			SendMessage(hComBoxHookType, CB_ADDSTRING, 0, (LPARAM)"切换点");
			SendMessage(hComBoxMouseType, CB_SETCURSEL, map.Cell[map.Width*cy+cx].MouseType, 0);
			SendMessage(hComBoxHookType, CB_SETCURSEL, map.Cell[map.Width*cy+cx].CPType, 0);

			//显示当前的选项
			if( map.Cell[map.Width*cy+cx].CP != BLOCK_CP )
			{
				Sel=map.Cell[map.Width*cy+cx].CP;
				SendMessage(hListBox, LB_SETCURSEL, Sel, 0);
			}

			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hWnd);
			
			}
            return TRUE;
            break;

		case WM_COMMAND:
			idCommand=LOWORD(wParam);
			switch( idCommand )
			{
			//确定
			case IDOK:
				map.Cell[map.Width*cy+cx].CP=Sel;
				map.Cell[map.Width*cy+cx].MouseType=SendMessage(hComBoxMouseType, CB_GETCURSEL, 0, 0);
				map.Cell[map.Width*cy+cx].CPType= SendMessage(hComBoxHookType, CB_GETCURSEL, 0, 0);
				EndDialog(hDlg,false);
				return true;

			//取消
			case IDCANCEL:
				EndDialog(hDlg,false);
				return true;


			//添加
			case IDC_BUTTON_Add:
			{
				if( map.Hook_Num >= MAX_HOOK-1 )
				{
					ShowMessage("陷阱数量已满");
					break;
				}

				char *Name=GetStringDlg("请输入陷阱的脚本文件:");
				if( Name!=NULL )
				{
					//添加
					char temp[64];
					sprintf(temp,"(%d) %s",map.Hook_Num, Name);
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)temp);
					
					strcpy(map.Hook[map.Hook_Num], Name);
					Sel=map.Hook_Num;
					SendMessage(hListBox, LB_SETCURSEL, map.Hook_Num, 0);
					map.Hook_Num++;

					UpdateWindow(hDlg);
				}
			}

			break;

			//删除
			case IDC_BUTTON_Delete:
			{
				Sel=SendMessage(hListBox, LB_GETCURSEL, 0, 0 );
				int j;
				map.Hook_Num--;
				
				SendMessage(hListBox, LB_DELETESTRING, Sel, 0);
				
				//修正后面的指针
				for(j=Sel; j<map.Hook_Num; j++)		//陷阱文件名向前移动
				{
					strcpy(map.Hook[j],map.Hook[j+1]);
				}

				//修正地图上的陷阱编号
				for(j=0; j<map.Width * map.Height; j++)
				{
					if( map.Cell[j].CP==(unsigned)Sel )
						map.Cell[j].CP=BLOCK_CP;

					if( map.Cell[j].CP != BLOCK_CP && map.Cell[j].CP > (unsigned)Sel )
						map.Cell[j].CP--;
				}

				if(	map.Hook_Num==0 )
					Sel=BLOCK_CP;
				else
					SendMessage(hListBox, LB_SETCURSEL, Sel, 0);
			}
			break;

			//修改
			case IDC_BUTTON_Modify:
			{
				char *Name=GetStringDlg("请输入要改成的陷阱脚本文件:");
				if( Name!=NULL )
				{
					strcpy(map.Hook[Sel], Name);
					char temp[64];
					sprintf(temp,"(%d) %s",Sel, map.Hook[Sel]);
					SendMessage(hListBox, LB_DELETESTRING, Sel, 0);
					SendMessage(hListBox, LB_INSERTSTRING, Sel, (LPARAM)temp);
					SendMessage(hListBox, LB_SETCURSEL, Sel, 0);
					UpdateWindow(hDlg);
				}
			}
				break;

			//列表框事件
			case IDC_LIST_ChangePoint:
				switch( HIWORD(wParam) )
				{
				//改变列表选择
				case LBN_SELCHANGE:
					Sel=SendMessage(hListBox, LB_GETCURSEL, 0, 0 );
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_Modify),true);
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_Delete),true);
					break;

				//双击鼠标
				case LBN_DBLCLK:
					map.Cell[map.Width*cy+cx].CP=Sel;
					EndDialog(hDlg,false);
					return true;
				}
				break;
			}
			break;

	    case WM_DESTROY:
            return TRUE;
            break;

        default:
            return FALSE;
    }
return true;
}

/////////////////////////////////////////////////////////////////////
//
//*****************************
//地图敌人设定窗口消息
BOOL APIENTRY DialogEnemySetListProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT idCommand;
	static int Sel=-1;

	HWND hListBoxAllEnemy, hListBoxEnemy;
	
	hListBoxAllEnemy = GetDlgItem(hDlg, IDC_LIST_AllEnemy);
	hListBoxEnemy = GetDlgItem(hDlg, IDC_LIST_Enemy);

    switch (message)
    {
        case WM_INITDIALOG:
			{

			//定位到中心点
			RECT rect;
			GetWindowRect(hDlg, &rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			if( WindowMode==1 )	//窗口
				MoveWindow(hDlg, RectWindow.left+(ScreenWidth-w)/2, RectWindow.top+(ScreenHeight-h)/2, w, h, TRUE);
			else
				MoveWindow(hDlg, (ScreenWidth-w)/2, (ScreenHeight-h)/2, w, h, TRUE);
			
			//初始化所有敌人列表框
			int lEnemy = CFight::GetEnemyListNum();
			int lMapEnemy = map.GetEnemyNum();
			int n=0;
			for(int i=0; i<lEnemy; i++)
			{
				bool b=true;
				b = true;
				for(int j=0; j<lMapEnemy; j++)
				{
					if( map.GetEnemyList(j) == i )
					{
						b=false;
						break;
					}
				}

				if( b == true )
				{
					SendMessage(hListBoxAllEnemy, LB_INSERTSTRING, n, (LPARAM)CFight::GetEnemyName(i));
					n++;
				}
			}

			//初始化该地图敌人列表框
			for(i=0; i<lMapEnemy; i++)
			{
				SendMessage(hListBoxEnemy, LB_INSERTSTRING, i, (LPARAM)CFight::GetEnemyName(map.GetEnemyList(i)));
			}

			// 初始化遇敌频率
			SetDlgItemInt(hDlg, IDC_EDIT_Freq, map.GetEnemyFrequency(), true);

			Sel=-1;

			//显示当前的选项
			SendMessage(hListBoxAllEnemy, LB_SETCURSEL, Sel, 0);

			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hWnd);
			
			}
            return TRUE;
            break;

		case WM_COMMAND:
			idCommand=LOWORD(wParam);
			switch( idCommand )
			{
			//确定
			case IDOK:
				{
					map.ClearEnemyList();
					int count = SendMessage(hListBoxEnemy, LB_GETCOUNT, 0, 0 );
					map.SetEnemyNum(count);
					for(int i=0; i<count; i++)
					{
						char strTemp[32];
						SendMessage(hListBoxEnemy, LB_GETTEXT, i, (LPARAM)&strTemp );
						for(int j=0; j<CFight::GetEnemyListNum(); j++)
						{
							if( strcmp( strTemp, CFight::GetEnemyName(j)) == 0 )
							{
								map.SetEnemyList(i, j);
							}
						}
					}

					// 遇敌频率
					count = GetDlgItemInt(hDlg, IDC_EDIT_Freq, NULL, true);
					if( count < 0 ) count = 0;
					if( count > 100 ) count = 100;
					map.SetEnemyFrequency( count );
				}
				memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
				EndDialog(hDlg,false);
				return true;

			//取消
			case IDCANCEL:
				EndDialog(hDlg,false);
				return true;				
			
			//添加
			case IDC_BUTTON_Add:
				{
					Sel=SendMessage(hListBoxAllEnemy, LB_GETCURSEL, 0, 0 );
					if( Sel != LB_ERR )
					{
						char strTemp[32];
						SendMessage(hListBoxAllEnemy, LB_GETTEXT, Sel, (LPARAM)&strTemp );
						SendMessage(hListBoxEnemy, LB_INSERTSTRING, 0, (LPARAM)strTemp);
						SendMessage(hListBoxAllEnemy, LB_DELETESTRING, Sel, 0);
					}
				}
				break;

			//删除
			case IDC_BUTTON_Delete:
				{
					Sel=SendMessage(hListBoxEnemy, LB_GETCURSEL, 0, 0 );
					if( Sel != LB_ERR )
					{
						char strTemp[32];
						SendMessage(hListBoxEnemy, LB_GETTEXT, Sel, (LPARAM)&strTemp );
						SendMessage(hListBoxAllEnemy, LB_INSERTSTRING, 0, (LPARAM)strTemp);
						SendMessage(hListBoxEnemy, LB_DELETESTRING, Sel, 0);
					}
				}
				break;

			//列表框事件
			case IDC_LIST_AllEnemy:
				switch( HIWORD(wParam) )
				{
				//改变列表选择
				case LBN_SELCHANGE:
					Sel=SendMessage(hListBoxAllEnemy, LB_GETCURSEL, 0, 0 );
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_Add),true);
					break;

				//双击鼠标
				case LBN_DBLCLK:
					return true;
				}
				break;
			}
			break;

	    case WM_DESTROY:
            return TRUE;
            break;

        default:
            return FALSE;
    }
return true;
}

/////////////////////////////////////////////////////////////////////
//
//*****************************
//物体属性窗口消息
BOOL APIENTRY DialogObjectProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UINT idCommand;
	static int Sel=-1, Sel2=-1;

	HWND hComBoxMouseType;
	hComBoxMouseType=GetDlgItem(hDlg, IDC_COMBO_CPType);

    switch (message)
    {
        case WM_INITDIALOG:
			{

			//定位到中心点
			RECT rect;
			GetWindowRect(hDlg, &rect);
			int w=rect.right-rect.left;
			int h=rect.bottom-rect.top;
			if( WindowMode==1 )	//窗口
				MoveWindow(hDlg, RectWindow.left+(ScreenWidth-w)/2, RectWindow.top+(ScreenHeight-h)/2, w, h, TRUE);
			else
				MoveWindow(hDlg, (ScreenWidth-w)/2, (ScreenHeight-h)/2, w, h, TRUE);
			
			//坐标
			SetDlgItemInt(hDlg, IDC_EDIT_ox, cx, true);
			SetDlgItemInt(hDlg, IDC_EDIT_oy, cy, true);

			//地面材质
			SetDlgItemInt(hDlg, IDC_EDIT_Ground, map.Cell[map.Width*cy+cx].Ground, true);
			SetDlgItemInt(hDlg, IDC_EDIT_GroundPage, map.Cell[map.Width*cy+cx].GroundPic, true);
			SetDlgItemInt(hDlg, IDC_EDIT_Object, map.Cell[map.Width*cy+cx].Obj, true);
			SetDlgItemInt(hDlg, IDC_EDIT_ObjectPage, map.Cell[map.Width*cy+cx].ObjPic, true);
			SetDlgItemInt(hDlg, IDC_EDIT_Object2, map.Cell[map.Width*cy+cx].Obj2, true);
			SetDlgItemInt(hDlg, IDC_EDIT_ObjectPage2, map.Cell[map.Width*cy+cx].Obj2Pic, true);

			//Hook
			SetDlgItemInt(hDlg, IDC_EDIT_Hook, map.Cell[map.Width*cy+cx].CP, true);

			//能否通过
			if( map.Cell[map.Width*cy+cx].Block )
				CheckDlgButton(hDlg, IDC_CHECK_Block, 1);

			//初始化物体层次
			Sel=map.Cell[map.Width*cy+cx].Level;
			switch( Sel )
			{
			case CMap::OL_NORMAL:
				CheckDlgButton(hDlg, IDC_RADIO_Normal, 1);
				break;

			case CMap::OL_UP:
				CheckDlgButton(hDlg, IDC_RADIO_Up, 1);
				break;
			}

			//初始化陷阱类型复选框
			for(int i=0; i<CCursor::GetCursorNum(); i++)
			{
				SendMessage(hComBoxMouseType, CB_ADDSTRING, 0, (LPARAM)CCursor::GetCursor(i).strName);
			}
			SendMessage(hComBoxMouseType, CB_SETCURSEL, map.Cell[map.Width*cy+cx].MouseType, 0);

			Sel2=map.Cell[map.Width*cy+cx].Level2;
			switch( Sel2 )
			{
			case CMap::OL_NORMAL:
				CheckDlgButton(hDlg, IDC_RADIO_Normal2, 1);
				break;

			case CMap::OL_UP:
				CheckDlgButton(hDlg, IDC_RADIO_Up2, 1);
				break;
			}

			ShowWindow(hDlg,SW_SHOW);
			UpdateWindow(hWnd);
			
			}
            return TRUE;
            break;

		case WM_COMMAND:
			idCommand=LOWORD(wParam);
			switch( idCommand )
			{
			//确定
			case IDOK:
				if( IsDlgButtonChecked(hDlg, IDC_CHECK_Block) )
					map.Cell[map.Width*cy+cx].Block=1;
				else
					map.Cell[map.Width*cy+cx].Block=0;

				map.Cell[map.Width*cy+cx].Level=Sel;
				map.Cell[map.Width*cy+cx].Level2=Sel2;
				Cur_MouseType = SendMessage(hComBoxMouseType, CB_GETCURSEL, 0, 0);
				map.Cell[map.Width*cy+cx].MouseType = Cur_MouseType;
				memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
				EndDialog(hDlg,false);
				return true;

			//取消
			case IDCANCEL:
				EndDialog(hDlg,false);
				return true;

			//地面层次
			case IDC_RADIO_Normal:
				Sel=CMap::OL_NORMAL;
				break;

			//地面层次
			case IDC_RADIO_Up:
				Sel=CMap::OL_UP;
				break;

			//地面层次2
			case IDC_RADIO_Normal2:
				Sel2=CMap::OL_NORMAL;
				break;

			//地面层次2
			case IDC_RADIO_Up2:
				Sel2=CMap::OL_UP;
				break;
			}

	    case WM_DESTROY:
            return TRUE;
            break;

        default:
            return FALSE;
    }
return true;
}

//---------------------------------------------------------

//新建地图
void _NewMap(HWND hWnd)
{
	if( MessageBox(hWnd, "新建地图吗？（注意保存当前地图）", "Exit", MB_OKCANCEL)==IDOK )
	{
		int w=GetIntDlg("请输入地图宽度（单位：格子 >= 20）");
		while( w<20 )
		{
			w=GetIntDlg("请输入地图宽度（单位：格子 >= 20）");
		}
		int h=GetIntDlg("请输入地图高度（单位：格子 >= 15）");
		while( h<15 )
		{
			h=GetIntDlg("请输入地图高度（单位：格子 >= 15）");
		}
		map.InitMap(w, h);

		// 初始鼠标状态
		map.RestoreMouseType(CCursor::CS_NORMAL);

		//取消当前选择的物体
		strcpy(map.CurrentMapFileName,"Unknow");	//当前编辑的地图文件名
		strcpy(map.CurrentMapFilePath,"Unknow");	//当前编辑的地图文件名

		//初始化UNDO功能
		_DELETE( UndoMap );
		UndoMap=new Cell_Struct[map.Width*map.Height];
		memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
	}
}

//读入地图
bool _LoadMap(HWND hWnd)
{
	char tmp[256]="*.map";
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = "地图文件（*.map）";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.nMaxCustFilter = 0;	
	ofn.lpstrFile = tmp;
	ofn.nMaxFile = 1024;
	ofn.lpstrTitle = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = CurrentPath;
	ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST 
							| OFN_FILEMUSTEXIST | OFN_SHAREAWARE;
	ofn.lpstrDefExt = "";
	ofn.lpfnHook = NULL;
	if( GetOpenFileName((LPOPENFILENAME)&ofn)==0 )
	{
		//设置当前路径
		SetCurrentDirectory(CurrentPath);
		return false;
	}
	char strTemp1[MAX_PATH];
	char* strTemp2 = strTemp1;

	strcpy(strTemp1,ofn.lpstrFile);				

	strTemp2 += strlen(CurrentPath)+1;			//跳过路径
	SetCurrentDirectory(CurrentPath);
	if( map.LoadMap(strTemp2) )					//读入地图
	{
		//人物位置复位
		role[0].State.x = 16;
		role[0].State.y = 16;
		role[0].State.X = 0;
		role[0].State.Y = 0;

		//初始化UNDO功能
		_DELETE(UndoMap);
		UndoMap=new Cell_Struct[map.Width*map.Height];
		memcpy(UndoMap, map.Cell, sizeof(Cell_Struct)*map.Width*map.Height);
		return true;
	}
	return false;
}

//保存地图
void _SaveMap(HWND hWnd)
{
	if( strcmp(map.CurrentMapFileName, "Unknow") !=0 )
	{
		if( map.SaveMap(map.CurrentMapFileName) )
		{
			char* str = GetFilePath(map.CurrentMapFileName);
			strcpy(map.CurrentMapFilePath, str);
			_FREE( str );
			MessageBox(hWnd, "地图文件已保存", "Heroland", MB_OK );

			//设置当前路径
			SetCurrentDirectory(CurrentPath);
		}
	}
	else
		MessageBox(hWnd, "请使用'SaveAs'。", "Heroland", MB_OK );

}

//另存为
bool _SaveAsMap(HWND hWnd)
{
	char tmp[256]="*.map";
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = "地图文件（*.map）";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.nMaxCustFilter = 0;	
	ofn.lpstrFile = tmp;
	ofn.nMaxFile = 1024;
	ofn.lpstrTitle = NULL;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = CurrentPath;
	ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST 
						| OFN_FILEMUSTEXIST | OFN_SHAREAWARE;
	ofn.lpstrDefExt = "";
	ofn.lpfnHook = NULL;
	if( GetSaveFileName((LPOPENFILENAME)&ofn)==0 )
	{
		//设置当前路径
		SetCurrentDirectory(CurrentPath);
		return false;
	}

	if( map.SaveMap(ofn.lpstrFile) )
	{
		strcpy(map.CurrentMapFileName,ofn.lpstrFile);
		char* str = GetFilePath(map.CurrentMapFileName);
		strcpy(map.CurrentMapFilePath, str);
		_FREE( str );
		MessageBox(hWnd, "地图文件已保存", "Heroland", MB_OK );

		//设置当前路径
		SetCurrentDirectory(CurrentPath);
		return true;
	}

	return false;
}

//***************************
//打开文件对话框
char *GetFileDialog(bool Load)
{
	char tmp[MAX_PATH]="*.*";
	char* strRet = new char[MAX_PATH];

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = hInst;
	ofn.lpstrFilter = "*.*";		
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;		
	ofn.nMaxCustFilter = 0;	
	ofn.lpstrFile = tmp;		
	ofn.nMaxFile = 1024;
	ofn.lpstrTitle = NULL;		
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = CurrentPath;   
	ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST 
					| OFN_FILEMUSTEXIST | OFN_SHAREAWARE;
	ofn.lpstrDefExt = "*";		
	ofn.lpfnHook = NULL;

	if( Load )	//打开
	{
		if( GetOpenFileName((LPOPENFILENAME)&ofn)==0 ) 
		{
			return NULL;
		}
		strcpy(strRet, ofn.lpstrFile);
		return strRet;
	}
	else	//保存
	{
		if( GetSaveFileName((LPOPENFILENAME)&ofn)==0 ) 
		{
			return NULL;
		}
		strcpy(strRet, ofn.lpstrFile);
		return strRet;
	}
}

//***************************
//取得路径
char *GetPathDialog()
{
	BROWSEINFO bi;
	char diskname[MAX_PATH], *path = new char[MAX_PATH];
	ITEMIDLIST *pidl;
	bi.hwndOwner=0;
	bi.pidlRoot=0;
	bi.pszDisplayName=diskname;
	bi.lpszTitle="你要显示的话";
	bi.ulFlags=0;
	bi.lpfn=0;
	bi.lParam=0;
	bi.iImage=0;

	if( pidl = SHBrowseForFolder(&bi) )
	{
		SHGetPathFromIDList(pidl, path);
		return path;
	}

	return NULL;
}