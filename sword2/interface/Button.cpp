//********************************************
//	��ť ��غ���
//  ������2000��6��19��
//********************************************
#include <windows.h>
#include "..\gamelib\goldpoint2.h"
#include "..\main.h"
#include "button.h"
#include <stdio.h>

//����
CCButton::CCButton()
{
	Pic=NULL;
	m_bTest = NULL;
}

//����
CCButton::~CCButton()
{
	_RELEASE( Pic );
	_DELETE_ARRAY(m_bTest);
}

//��INI�ļ�����
bool CCButton::LoadIni(char *filename, char *index)
{
	CIniSet Ini(filename);
	char *str;

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

	Down_Rect.left=0;
	Down_Rect.top=Height*2;
	Down_Rect.right=Width;
	Down_Rect.bottom=Height*3;

	str=Ini.ReadText(index,"PicFileName");
	strcpy(PicFileName, str);					//ͼƬ�ļ���
	_FREE(str);
	CreateBitmap(Pic, 0, 0, PicFileName);		//����ҳ��
	DDSetColorKey(Pic, ColorKey);

	int nUseRgn = Ini.ReadInt(index,"UseRgn");	//�Ƿ��ǲ�����İ�Ŧ
	if(nUseRgn)
	{
		if(!CreateRgn())
			return false;
	}
	return true;
}

//��ʾ
void CCButton::Show(LPDIRECTDRAWSURFACE surf, int bHighLight)
{
	if( bHighLight != -1 )
	{
		RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
		Blt(surf, x, y, Pic, rect, true);
	}
	else
		Blt(surf, x, y, Pic, Normal_Rect, true);
}

//��ť���ѭ��
// ���� ret:( 0=NULL, 1=����, 2=��������� 3=��ס����)
BOOL CCButton::CheckLoop(LPDIRECTDRAWSURFACE surf, int bHighLight)
{
	BOOL ret=false;

	//����ڰ�ť�Ϸ�����������������飬������ó����μ��
	if(CheckIn())
	{
		if( Mouse.DownButton() == LBUTTON )	//����
		{
			Blt(surf, x, y, Pic, Down_Rect, true);
			ret =3;
		}
		else
		{
			if( bHighLight != -1 )
			{
				RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
				Blt(surf, x, y, Pic, rect, true);
				ret=false;
			}
			else
			{
				Blt(surf, x, y, Pic, Over_Rect, true);
				ret=2;	//����״̬
			}
		}
		
		if( mouse == LB_UP )
		{
			mouse=0;
			return 1;
		}
	}
	else	//���û���ڰ�ť�Ϸ�
	{
		if( bHighLight != -1 )
		{
			RECT rect={0, Height*bHighLight, Width, Height*(bHighLight+1)};
			Blt(surf, x, y, Pic, rect, true);
		}
		else
			Blt(surf, x, y, Pic, Normal_Rect, true);
	}

	return ret;
}

//��������
//return true:�ɹ���false:ʧ��
bool CCButton::CreateRgn()
{
	if(BeginDraw(Pic))
	{
		WORD* pSurf = GraphBuffer;
		int Pitch = GraphPitch;
		m_bTest = new bool[Width*Height];
		for(int i=0;i<Height;i++)
			for(int j =0;j<Width;j++)
			{
				if(pSurf[i*Pitch+j] == ColorKey16)
					m_bTest[j+i*Width] = false;
				else
					m_bTest[j+i*Width] = true;
			}
		EndDraw(Pic);
		return true;
	}
	return false;
}
//����ǲ����ڰ�Ŧ��������
BOOL CCButton::CheckIn()
{
	if(point.x - x>=0&&point.x <x+Width&&point.y-y>=0&&point.y - y<Height)
		if(m_bTest)
			return m_bTest[(point.x - x)+(point.y - y)*Width];
		else
			return TRUE;
	return FALSE;
}
