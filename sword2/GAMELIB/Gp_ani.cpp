//********************************************
//	���� ��غ���
//  ������2000��4��16��
//********************************************
#include <windows.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_iniset.h"
#include "gp_draw.h"
#include "gp_other.h"
#include "gp_ani.h"

CAni::CAni()
{
	Pics=NULL;
}

//�ͷ�
CAni::~CAni()
{
	_RELEASE( Pics );
}

//��ȡһ������
bool CAni::LoadAni(char *filename, char *index)
{
	CIniSet Ini(filename);

	x=Ini.ReadInt(index,"x");			//������ʾλ��
	y=Ini.ReadInt(index,"y");			//������ʾλ��
	tx=Ini.ReadInt(index,"tx");			//�ȵ�ƫ��
	ty=Ini.ReadInt(index,"ty");			//�ȵ�ƫ��
	Width=Ini.ReadInt(index,"Width");	//������С
	Height=Ini.ReadInt(index,"Height");	//������С

	Max_Pics=Ini.ReadInt(index,"Max_Pics");		//ͼƬ��Ŀ
	Cur_Pic=Ini.ReadInt(index,"Cur_Pic");		//��ʾ�ĵ�ǰ��
	ShowCount=Ini.ReadInt(index,"ShowCount");	//������

 	char *str=Ini.ReadText(index,"FileName");
	strcpy(FileName, str);						//ͼƬ�ļ���
	_FREE( str );

	CreateBitmap(Pics, 0,0, FileName);			//����ҳ��
	DDSetColorKey(Pics, ColorKey);
	WLines=Ini.ReadInt(index,"WLines");
	HLines=Ini.ReadInt(index,"HLines");			//ͼƬ�ĺ������и���
	bArrayH=Ini.ReadInt(index,"bArrayH");		//��������

	Delay=Ini.ReadInt(index,"Delay");			//��֡��ļ��ʱ��
	bColorKey=Ini.ReadInt(index,"bColorKey");	//��֡��ļ��ʱ��
	bActive=Ini.ReadInt(index,"bActive");		//�Ƿ�
	bShow=Ini.ReadInt(index,"bShow");			//�Ƿ���ʾ
	
	Cur_Rect=GetRect( 0, 0, Width, Height );
	
	OldTick=timeGetTime();
	return true;
}

//��ʾ
void CAni::ShowAni(LPDIRECTDRAWSURFACE Sour, int lx, int ly, int Cur_Picture)
{
	if( !bShow || ShowCount==0 )
		return;

	if( bActive && timeGetTime() > OldTick+Delay )	//ʱ�䵽
	{
		OldTick=timeGetTime();

		if( Cur_Picture== -1 ) 
			Cur_Picture=Cur_Pic;

		int x1, y1;
		if( bArrayH )	//������������
		{
			x1=Cur_Picture%WLines;
			y1=Cur_Picture/WLines;
		}
		else
		{
			x1=Cur_Picture%HLines;
			y1=Cur_Picture/HLines;
		}

		int W=Width*x1;
		int H=Height*y1;
		Cur_Rect=GetRect( W, H, W+Width, H+Height );
	
		Cur_Pic=(Cur_Pic+1)%Max_Pics;		//��һ֡
		if( ShowCount > 0 ) ShowCount--;	//��������1
	}

	Blt(Sour, lx+tx, ly+ty, Pics, Cur_Rect, bColorKey);	//��ʾ
}

//��ʾ
void CAni::ShowAni(LPDIRECTDRAWSURFACE Sour)
{
	if( !bShow || ShowCount==0 )
		return;

	if( bActive && timeGetTime() > OldTick+Delay )	//ʱ�䵽
	{
		OldTick=timeGetTime();

		int x1, y1;
		if( bArrayH )	//������������
		{
			x1=Cur_Pic%WLines;
			y1=Cur_Pic/WLines;
		}
		else
		{
			x1=Cur_Pic/HLines;
			y1=Cur_Pic/HLines;
		}

		int W=Width*x1;
		int H=Height*y1;
		Cur_Rect=GetRect( W, H, W+Width, H+Height );
	
		Cur_Pic=(Cur_Pic+1)%Max_Pics;		//��һ֡
		if( ShowCount > 0 ) ShowCount--;	//��������1
	}

	Blt(Sour, x+tx, y+ty, Pics, Cur_Rect, bColorKey);	//��ʾ
}
