//********************************************
//	��ѡ��ť ��غ���
//  ������2000��7��10��
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "check.h"

//����
CCheck::CCheck()
{
	Pic=NULL;
}

//����
CCheck::~CCheck()
{
	_RELEASE( Pic );
}

//��INI�ļ�����
bool CCheck::LoadIni(char *filename, char *index)
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

	bChecked=(Ini.ReadInt(index, "bChecked"))?true:false;
	
	//��ʼ������
	char *fontfile=Ini.ReadText(index, "FontFile");
	char *fontindex=Ini.ReadText(index, "FontIndex");
	Font.LoadFont(fontfile, fontindex);
	_FREE(fontfile);
	_FREE(fontindex);

	Normal_Rect.left=0;
	Normal_Rect.top=0;
	Normal_Rect.right=Width;
	Normal_Rect.bottom=Height;

	Over_Rect.left=0;
	Over_Rect.top=Height;
	Over_Rect.right=Width;
	Over_Rect.bottom=Height*2;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);	//ͼƬ�ļ���
	_FREE(str);
	
	_RELEASE(Pic);
	CreateBitmap(Pic, 0, 0, PicFileName);			//����ҳ��
	DDSetColorKey(Pic, ColorKey);

	return true;
}

//��ʾ
void CCheck::Show(LPDIRECTDRAWSURFACE surf)
{
	if( !bChecked )
		Blt(surf, x, y, Pic, Normal_Rect, true);
	else
		Blt(surf, x, y, Pic, Over_Rect, true);

	Font.PrintText(surf, x+Width+4, y, Name);
}

//��ť���ѭ��
bool CCheck::CheckLoop(LPDIRECTDRAWSURFACE surf)
{
	if( point.x > x && point.x < x+Width && point.y > y && point.y < y+Height )
	{
		if( mouse == LB_UP )
		{
			bChecked=!bChecked;
		}
	}

	Show(surf);
	return bChecked;
}