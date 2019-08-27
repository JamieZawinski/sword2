//********************************************
//	对话框 相关函数
//  创建于2000年3月17日
//********************************************

void CreateDlg(int, DLGPROC);		//创建一个模式对话框
void CreateDlg2(int, DLGPROC);		//创建非模态对话框

char *GetStringDlg(char *Msg=NULL);	//获得一个字符串
int GetIntDlg(char *Msg=NULL);		//获得一个数字
BOOL APIENTRY CGetStringDlgProc(HWND, UINT, WPARAM, LPARAM);	//获得一个字符串

BOOL APIENTRY DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogChangePointListProc( HWND, UINT, WPARAM, LPARAM);

char *GetFileDialog(bool Load=true);	//打开文件对话框
char *GetPathDialog();					//取得路径

BOOL APIENTRY DialogChangePointListProc( HWND, UINT, WPARAM, LPARAM);	//出口点
BOOL APIENTRY DialogEnemySetListProc( HWND, UINT, WPARAM, LPARAM);		//地图敌人设定窗口消息
BOOL APIENTRY DialogObjectProc( HWND, UINT, WPARAM, LPARAM);			//物体属性
BOOL APIENTRY DialogPopMenuProc( HWND, UINT, WPARAM, LPARAM );			//右键弹出菜单窗口消息

//------------------------------------
void _NewMap(HWND);			//新建地图
bool _LoadMap(HWND);		//读入地图
void _SaveMap(HWND);		//保存地图
bool _SaveAsMap(HWND);		//另存为
