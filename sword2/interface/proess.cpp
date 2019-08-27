//********************************************
//	������ ��غ���
//  ������2001��5��12��
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "proess.h"

//����
CProess::CProess()
{
	fCurCount=0.f;
	Pic=NULL;
}

//����
CProess::~CProess()
{
	_RELEASE( Pic );
}

//��INI�ļ�����
bool CProess::LoadIni(char *filename, char *index)
{
	char *str;
	CIniSet Ini(filename);	

	Id=Ini.ReadInt(index,"Id");			//ID
	str=Ini.ReadText(index,"Name");
	strcpy(Name, str);					//��ť��
	_FREE(str);
	x=Ini.ReadInt(index,"x");			//��ʾλ��
	y=Ini.ReadInt(index,"y");			//��ʾλ��
	Width=Ini.ReadInt(index,"Width");	//��С
	Height=Ini.ReadInt(index,"Height");	//��С

	Normal_Rect.left=0;
	Normal_Rect.top=0;
	Normal_Rect.right=Width;
	Normal_Rect.bottom=Height;

	Over_Rect.left=0;
	Over_Rect.top=Height;
	Over_Rect.right=Width;
	Over_Rect.bottom=Height*2;

	DestRect.left=x;
	DestRect.top=y;
	DestRect.right=x+Width;
	DestRect.bottom=y+Height;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);	//ͼƬ�ļ���
	_FREE(str);

	str=Ini.ReadText(index,"Dest");
	strcpy(strDest, str);	//��ʾ��str
	_FREE(str);
	
	_RELEASE(Pic);
	CreateBitmap(Pic, 0, 0, PicFileName);			//����ҳ��
	DDSetColorKey(Pic, ColorKey);

	return true;
}

//��ʾ
void CProess::Show(LPDIRECTDRAWSURFACE surf)
{
	Over_Rect.right = int(Width * fCurCount);		//������ʾ�Ķ��٣��������ĳ���
	DestRect.right = x + Over_Rect.right;
	surf->Blt( &DestRect, Pic, &Over_Rect, DDBLT_KEYSRC, 0);

	DestRect.right = x + Width;
	surf->Blt( &DestRect, Pic, &Normal_Rect, DDBLT_KEYSRC, 0);	//���ϱ߿�
}

//��ť���ѭ��
bool CProess::CheckLoop(LPDIRECTDRAWSURFACE surf)
{
	return false;
}
