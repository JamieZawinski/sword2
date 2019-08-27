//********************************************
//	DirectDraw��ش�����
//  softboy ������2000��1��25��
//********************************************

#include <windows.h>
#include <stdio.h>
#include <fstream.h>
#include <ddraw.h>
#include "ddutil.h"
#include "gp_init.h"
#include "gp_draw.h"
#include "gp_input.h"
#include "gp_alpha.h"
#include "gp_text.h"
#include "gp_other.h"

//*****************��Ϸ��ʾ����*****************
DWORD ColorKey=RGB(255,0,255);	//͸��ɫ
WORD ColorKey16;				//16λ͸��ɫ
bool Is555 = true;				//�Ƿ���555��ʽ
long nFrames=0;					//֡������
RECT RectScreen={0,0,ScreenWidth,ScreenHeight};	//ȫ����ʾ��Ŀ�����
RECT RectWindow;								//������ʾ��Ŀ�����
POINT g_pointClient;							//���ںͿͻ�����ƫ��

//*****************DDraw����********************
LPDIRECTDRAW        lpDD;			//DirectDraw����
LPDIRECTDRAWSURFACE	lpDDSPrimary;	// ��ҳ��
LPDIRECTDRAWSURFACE	lpDDSBack=NULL;	// ��̨ҳ��
extern LPDIRECTDRAWSURFACE lpDDSSour, lpDDSTemp;

//****************Lock ���*********************
WORD *GraphBuffer = 0;		// ��ͼ������
int GraphPitch = 0;			// ���������
int GraphWidth = 0;			//ҳ����
int GraphHeight = 0;		//ҳ��߶�

DDSURFACEDESC		ddsd;
HRESULT				ddrval;

//16λRGB����
WORD RGB16(WORD r, WORD g, WORD b)
{
	//��
	if( Is555 )
		//rrrrr|ggggg|bbbbb    0xf8 = 11111000b
		return ((r&0xf8)<<7) | ((g&0xf8)<<2) | ((b&0xf8)>>3);
	else
		//rrrrr|gggggg|bbbbb    0xfc = 11111100
		return ((r&0xf8)<<8) | ((g&0xfc)<<3) | ((b&0xf8)>>3);
}

//24λת16λ
WORD RGB16(DWORD color)
{
	WORD r,g,b;
	//Ҳ�Ƚϼ�
	r=(WORD)(color>>16);	
	g=(WORD)(color>>8);
	b=(WORD)color;

	if( Is555 )
		return ((r&0xf8)<<7) | ((g&0xf8)<<2) | ((b&0xf8)>>3);
	else
		return ((r&0xf8)<<8) | ((g&0xfc)<<3) | ((b&0xf8)>>3);
}

//���ܣ����λͼ�ļ��ĳߴ��С
//���������ھ����λͼ�ļ���������λͼ��ȣ�����λͼ�߶�(���ò���)
BOOL LoadBitmapFileInfo(HWND hwnd, LPCTSTR filename, int &dx, int &dy)
{
	FILE *fp;
	if( (fp=fopen(filename, "rb"))==NULL )
	{
			char b[MAX_PATH*2];
			sprintf(b,"Error open BMP file: %s",filename);
			MessageBox(hwnd,b,"Load file info!",MB_OK);
			return FALSE;
	}
	//�����ļ�ͷ
	BITMAPFILEHEADER bmpFileHeader;		
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//���BM��־
	char *ptr=(char*)&bmpFileHeader.bfType;
	if(*ptr!='B' || *++ptr!='M')
	{
		MessageBox(hwnd,"Invalid bitmap file!","error",MB_OK);
		return FALSE;
	}
	//��Ϣͷ
	BITMAPINFOHEADER bmpInfoHeader;
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);
	fclose(fp);
	//��ȣ��߶�
	dx=bmpInfoHeader.biWidth;
	dy=bmpInfoHeader.biHeight;
	return TRUE;
}

//*********************************
//���ܣ�����ҳ��
//������Ҫ������ҳ��ָ�룬��ȣ��߶ȣ��ļ������ڴ��־
BOOL CreateBitmap(LPDIRECTDRAWSURFACE &lpTemp,int x,int y,char *BitmapFile, DWORD MemoryFlag )
{
	DDSURFACEDESC	 ddsd;
	HRESULT          ddrval;

	//���λͼ�ļ��ĳߴ�
	if( x == 0 && y == 0 )
		if( LoadBitmapFileInfo(hWnd, BitmapFile, x, y)==FALSE )
			return FALSE;
	//����һ��ҳ��
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | MemoryFlag;
	ddsd.dwWidth = x;
	ddsd.dwHeight = y;
	ddrval = lpDD->CreateSurface( &ddsd, &lpTemp, NULL );
	if( ddrval != DD_OK )
	{
		return initFail(BitmapFile);
	}
	
	if( BitmapFile!=NULL )
		DDReLoadBitmap(lpTemp,BitmapFile);
	
	return TRUE;
}

//***********************************************************
//���ܣ�λͼ����
//������Ŀ����棬Ŀ��x��Ŀ��y��ԭ���棬������Χ���Ƿ��͸��ɫ
void Blt(LPDIRECTDRAWSURFACE SS,int x,int y,
		 LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flag)
{
	//�߽���
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}
	else 
		if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
		{ 
			rcRect.right = rcRect.left + ScreenWidth - x; 
		}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}
	else 
		if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
		{ 
			rcRect.bottom = rcRect.top + ScreenHeight - y; 
		}
	
	while( 1 )
	{
		ddrval = SS->BltFast( x, y, DS, &rcRect, Flag);

		if(	ddrval == DD_OK )
		{
			return;
		}
		
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
		    {
				return;
		    }
		}
		
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
		    return;
		}
    }
}

//***********************************************************
//���ܣ�����λͼ����
//������Ŀ����棬Ŀ��x��Ŀ��y��ԭ���棬������Χ���Ƿ��͸��ɫ
void BltFast(LPDIRECTDRAWSURFACE SS,int x,int y,
		 LPDIRECTDRAWSURFACE DS,RECT rcRect,DWORD Flag)
{
	//�߽���
	if( x<0 ) 
	{  
		rcRect.left = rcRect.left - x; 
		x=0; 
	}
	else 
		if( x+ rcRect.right - rcRect.left > ScreenWidth ) 
		{ 
			rcRect.right = rcRect.left + ScreenWidth - x; 
		}

	if( y<0 ) 
	{  
		rcRect.top  = rcRect.top - y; 
		y=0; 
	}
	else 
		if( y+ rcRect.bottom - rcRect.top > ScreenHeight ) 
		{
			rcRect.bottom = rcRect.top + ScreenHeight - y; 
		}

	SS->BltFast( x, y, DS, &rcRect, Flag);
}

//***********************************************************
//���ܣ������ŵ�λͼ����
//������Ŀ����棬Ŀ�����ԭ���棬������Χ���Ƿ��͸��ɫ
BOOL SBlt(LPDIRECTDRAWSURFACE SS,RECT sr,
		 LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flag)
{
  DWORD Flags= (Flag==FALSE)?0:DDBLT_KEYSRC;

  //�߽���
  int SWidth=sr.right-sr.left, SHeight=sr.bottom-sr.top;

  int DWidth=dr.right-dr.left, DHeight=dr.bottom-dr.top;

  if( sr.left<0 )
  {
	  dr.left += (DWidth * (-sr.left)) / SWidth;
	  sr.left=0;
  }
  if( sr.top<0 ) 
  {
	  dr.top += (DHeight * (-sr.top)) / SHeight;
	  sr.top=0; 
  }
  if( sr.right > ScreenWidth ) 
  {
	  dr.right -= (DWidth * (sr.right-ScreenWidth)) / SWidth;	
	  sr.right=ScreenWidth; 
  }
  if( sr.bottom > ScreenHeight ) 
  {  
	  dr.bottom -= (DHeight *(sr.bottom-ScreenHeight)) / SHeight;
	  sr.bottom=ScreenHeight;
  }

 while( 1 )
 {
    ddrval = SS->Blt( &sr, DS, &dr, Flags, 0);

	if( ddrval == DD_OK )
	{
	   return TRUE;
	}
	if( ddrval == DDERR_SURFACELOST )
	{
	    ddrval = restoreAll();
	    if( ddrval != DD_OK )
	    {
		return TRUE;
	    }
	}
	if( ddrval != DDERR_WASSTILLDRAWING )
	{
	    return FALSE;
	}
    }
    if(ddrval != DD_OK)
    {
	return FALSE;
    }
}

//***********************************************************
//���ܣ������ŵ�λͼ�����������߽��飩
//������Ŀ����棬Ŀ��x��Ŀ��y��ԭ���棬������Χ���Ƿ��͸��ɫ
BOOL _SBlt(LPDIRECTDRAWSURFACE SS,RECT sr,
		 LPDIRECTDRAWSURFACE DS,RECT dr,BOOL Flag)
{
	DWORD Flags= (Flag==FALSE)?0:DDBLT_KEYSRC;
	while( 1 )
	{
	    ddrval = SS->Blt( &sr, DS, &dr, Flags, 0);

		if( ddrval == DD_OK )
		{
			return TRUE;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
			{
				return TRUE;
		    }
		}
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
			return FALSE;
		}
    }
}

//*****************************************************
//���ܣ���תҳ��
void Flip(void)
{
	HRESULT ddrval;
	while( 1 )
	{
		ddrval = lpDDSPrimary->Flip( NULL, 0 );
		if( ddrval == DD_OK )
		{
		    break;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		    if( ddrval != DD_OK )
		    {
				break;
		    }
		}
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
		    break;
		}
	}
}

//********************
//���ܣ��ָ�ϵͳҳ��
HRESULT restoreAll( void )
{
    HRESULT	ddrval;
	
	if( lpDDSPrimary )
		ddrval = lpDDSPrimary->Restore();

	if( lpDDSBack )
		ddrval = lpDDSBack->Restore();
    return ddrval;
}

//********************************************
//���ܣ�����
//������Ŀ����棬��ɫֵ
void Clrscr(LPDIRECTDRAWSURFACE surf,WORD color )
{
	DDBLTFX ddbltfx;
	ddbltfx.dwSize=sizeof(ddbltfx);
	ddbltfx.dwFillColor=color;
	surf->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx);
}

//-------------------------------

//���ܣ����µ���Ļ
//������Ŀ����棬���·�Χ
void _UpdateScreen(LPDIRECTDRAWSURFACE lpSurf, RECT DestRect)
{
	if( WindowMode==0 )	//ȫ��
	{
		Blt(lpDDSPrimary, 0, 0, lpSurf, DestRect, false);
	}
	else	//����
	{
		lpDDSPrimary->Blt( &RectWindow, lpSurf, &DestRect, DDBLTFAST_NOCOLORKEY, 0);
	}
}

/////////////////////////////////////////////////////////////////
//��Ԫ��
/////////////////////////////////////////////////////////////////
void PutPixel(LPDIRECTDRAWSURFACE surf, int x, int y, WORD color)
{
	BeginDraw(surf);
	EndDraw(surf);
	GraphBuffer[GraphPitch*y+x]=color;	
}
//����һ�������ɫΪcolor
void _PutPixel(int x, int y, WORD color)
{
	GraphBuffer[GraphPitch*y+x]=color;
}

//��Բ
void Circle(LPDIRECTDRAWSURFACE surf, int x0, int y0, int r,WORD color)
{
	long flag;
	int x,y,xmax;
	static double SIN45=0.707106781186548;
	y=r;
	x=0;
	xmax=(int)(r*SIN45);			//ֻ��Ҫɨ��45�ȵĽ�
	flag=(1-r*2);
	//softҲ�Ǻ��ˣ�һ��ע�Ͷ�û�У������˰����֪����
	//����ʵ��һ���ܳ������㷨
	//������Bresenham�㷨
	//��ֻ˵�����
	//��ϸ�ľ�Ҫ�Լ�ȥ���ⷽ��Ľ���
	if( BeginDraw(surf) )
	{
		while( x<=xmax )			//ѭ������������xһֱ�ӣ��ӵ�xmaxΪֹ��Ҳ���Ǵ�0�ȵ�45��
		{
			if(flag>=0)							//      |y          
			{									//      |           
				flag+=(6+((x-y)<<2));			//  \ ..... /
				y--;							//   .  |  .     
			}									//  . \ | / .       
			else								//  .  \|/  .     
				flag+=((x<<2)+2);				//--.---+---.--   ��ϸ������Щ�������
												//  .  /|\  . x     
			//��8���㣬Ҳ����4����(���ظ��ĵ�)	//  . / | \ .    
			_PutPixel(x0+y, y0+x, color);		//   .  |  .        	
			_PutPixel(x0+x, y0+y, color);		//  / ..... \       
			_PutPixel(x0-x, y0+y, color);		//      |
			_PutPixel(x0-y, y0+x, color);		//      |
			_PutPixel(x0-y, y0-x, color);		//�����ȿ�������Ļ��ߵĺ���Ȼ���ٿ��������
			_PutPixel(x0-x, y0-y, color);		//���ȽϺö�����Ҫ�������Բ�����ؼ��Ǿ�����
			_PutPixel(x0+x, y0-y, color);		//һ����÷ŵ�ʲôλ�ã�������һ���������ж�
			_PutPixel(x0+y, y0-x, color);		//����:Midpoint ������������
			x++;								//����Ų�����ô��������ˣ��Լ�ȥ�ҹ��ڼ���
		}										//��ͼ��ѧ���������ɡ�
		EndDraw(surf);							//����Ҫ�����õ������Դ��������һ����ʥ��Դ
	}											//���򵼶����ļ��Ļ��������ɣ������еġ�
}

//����
void Line(LPDIRECTDRAWSURFACE surf, int left, int top, int right, int bottom, WORD color)
{
    register int t;
    int distance;
    int x=0,y=0,delta_x,delta_y,incx,incy;
    delta_x =right-left;
    delta_y=bottom-top;
	//����x��y�ı仯������x��y��������ʽ(ÿ�μ�1���Ǽ�1�����ǲ���)
    if(delta_x>0) 
		incx=1;
    else 
		if (delta_x==0) 
			incx=0;
		else 
		{
			delta_x=-delta_x; 
			incx=-1;
		}

    if (delta_y>0) 
		incy=1;
    else 
		if (delta_y==0) 
			incy=0;
		else 
		{
			delta_y=-delta_y; 
			incy=-1;
		}

    if(delta_x>delta_y)
        distance=delta_x;
    else
        distance=delta_y;

    //��ʼ������	
	//һ����Bresenham�㷨
	//�����ұߵ�ͼ��
	if( BeginDraw(surf) )
	{
		//�����˵�ҲҪ��
		for (t=0; t<distance+2; t++)
		{
			//����
			_PutPixel(left, top, color);	 //  o-------                         |
			x+=delta_x;					     // p1       --------                 | delta_y
			y+=delta_y;						 //                  -------      p2  |
			if(x>distance)					 //                         -------o  |
			{								 //------------------------------------	
				x-=distance;				 //                delta_x
				left+=incx;					 //����һ��������ʾ��ͼ
			}								 //������������delta_y<delta_x��ʱ�򣬾ͻ���x��
			if(y>distance)					 //�����ˮƽ�ߣ���ôʲôʱ�����y++�أ�
			{								 //�Ƚϼ򵥵ľ�����x����delta_x/delta_y��ʱ���
				y-=distance;				 //��y++,��Ȼ���Ǹ�����ȷ���뷨,��������㷨
				top+=incy;					 //ȷ�������Ϊ�����ġ�ֻ�����ǻ����ַ�ʽ���ѣ���
			}								 //���ǼӼ��������ǳ������������ͼ�������㷨��
		}									 //��������--------------------------------
		EndDraw(surf);						 //���˵�soft����Ϊ�˿�������㷨�����˺ö����	
	}										 //Ŷ����һ��ע�Ͷ�û�У���ô˵ҲҪ�����㷨������
}											 //����ǵġ������ģ�����Ц�ġ�
											 //����Ϊ�˷�����������ע�ͣ��ҿɻ��˲��ٵĹ���								

//������
void Rectangle(LPDIRECTDRAWSURFACE surf, int x1, int y1, int x2, int y2, WORD color)
{
	Line(surf, x1, y1, x2, y1, color);
	Line(surf, x1, y1, x1, y2, color);
	Line(surf, x2, y1, x2, y2, color);
	Line(surf, x1, y2, x2, y2, color);
}

//���ܣ���ʵ�ľ���
//������Ŀ����棬x1,y1,x2,y2,��ɫ
void Bar(LPDIRECTDRAWSURFACE surf, int x1, int y1, int x2, int y2, WORD color)
{
    DDBLTFX             ddbltfx;
    RECT                dest;
	ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = color;			//��ָ������ɫ���
    dest.left = x1;
    dest.top = y1;
    dest.right = x2;
    dest.bottom = y2;

    if ( FAILED( surf->Blt( &dest, NULL, NULL,
                    DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx ) ) )
	{
        return;
    }
}

//--------------------------------------------------------

//��ʼ��ʧ�ܵĴ�����
BOOL initFail(char *str)
{
    FreeDDraw();
    MessageBox( hWnd, str, "Init Fail", MB_OK );
    DestroyWindow( hWnd );
    return FALSE;

} 

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//���ܣ���ȡһ������Ĵ�С
//������Ŀ����棬���ؿ����ظ�
//���أ�������
int GetSurfaceSize(LPDIRECTDRAWSURFACE surf,int &x,int &y)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		x=ddsd.dwWidth;
		y=ddsd.dwHeight;
		return ddsd.lPitch>>1;		//��˵��
	}	
	
	return false;
}

//****************************
//��ȡһ������Ŀ��
int GetSurfaceWidth(LPDIRECTDRAWSURFACE surf)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		return ddsd.dwWidth;
	}	
	
	return false;
}

//****************************
//��ȡһ������ĸ߶�
int GetSurfaceHeight(LPDIRECTDRAWSURFACE surf)
{
	DDSURFACEDESC ddsd;
	HRESULT		ddrval;

	ddsd.dwSize=sizeof(ddsd);
	ddrval=surf->GetSurfaceDesc(&ddsd);
	if( ddrval == DD_OK ) 
	{
		return ddsd.dwHeight;
	}	
	
	return false;
}

//************************************
//��ҳ�滺������Ȼ��ֱ���ڻ������в���
bool BeginDraw(LPDIRECTDRAWSURFACE lpSur)
{
    ddsd.dwSize = sizeof(ddsd);
    
	while( 1 )
	{ 
		ddrval = lpSur->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );	//����
		if( ddrval == DD_OK )
		{
		    break;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
		    ddrval = restoreAll();
		  	if( ddrval != DD_OK ) return( false );
		} 
	}

  	GraphBuffer = (WORD *)ddsd.lpSurface;		//�ڴ�ָ��
	GraphWidth = ddsd.dwWidth;					//���
	GraphHeight = ddsd.dwHeight;				//�߶�
	GraphPitch  = ddsd.lPitch >> 1;	//lPitch��ByteΪ��λ�����ģ�
									//GraphPitch��WORDΪ��λ������GraphPitch = lPitch / 2;
    return( true );
}

//******************
//�رջ�����
bool EndDraw(LPDIRECTDRAWSURFACE lpSur)
{
   	ddrval = lpSur->Unlock( NULL );				//����
	if( ddrval != DD_OK ) return( false );
	return true;
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//���ܣ��Ե�ɫ���ҳ��
//������Ŀ����棬��ɫ
void FillSurface(LPDIRECTDRAWSURFACE surf, DWORD color)
{
	WORD color16=RGB16(color);				//ת����16ɫ

	if( BeginDraw(surf) )
	{
	for( int i=0; i<GraphHeight; i++)
		for(int j=0; j<GraphWidth; j++)
			GraphBuffer[i*GraphPitch+j] = color16;		//ֱ�����
	EndDraw(surf);
	}
}

//�Ե�ɫ���ҳ��
void FillSurface(LPDIRECTDRAWSURFACE surf, WORD color)
{
	if( BeginDraw(surf) )
	{
	for( int i=0; i<GraphHeight; i++)
		for(int j=0; j<GraphWidth; j++)
			GraphBuffer[i*GraphPitch+j] = color;		//ֱ�����
	EndDraw(surf);
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//���ܣ���Ч����
//�����������ʱ��ȱʡ������ɫ��ȱʡ��
void FadeIn(int Style, int time, WORD Color)
{
	switch( Style )
	{
	case 0:	//��Ч��
		break;

	case 1:	//��Ҷ��
		ScreenIn();
		break;
	
	case 2:	//���뵭��
		ColorIn(time, Color);
		break;
	}
}

//���ܣ���Ч����
//�����������ʱ��ȱʡ����Ŀ����棨ȱʡ��
void FadeOut(int Style, int time, LPDIRECTDRAWSURFACE lpSurf)
{
	switch( Style )
	{
	case 0:	//��Ч��
		break;

	case 1:	//��Ҷ��
		ScreenOut(lpSurf);
		break;
	
	case 2:	//���뵭��
		ColorOut(lpSurf, time);
		break;
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//����(time=ÿһ֡��ʱ��)
void ColorIn(int time, WORD Color)
{
	unsigned int alpha=2, oldtick, newtick;
	WORD *lpSurf, *lpTemp, BlackColor=Color;

	//������ʱҳ��
	LPDIRECTDRAWSURFACE Temp;
	CreateBitmap(Temp, ScreenWidth, ScreenHeight);
	//���浽��ʱҳ��
	if( WindowMode==0 )	//ȫ��
		Blt(Temp, 0, 0, lpDDSPrimary, RectScreen, false);
	else
		Blt(Temp, 0, 0, lpDDSPrimary, RectWindow, false);

	if( BeginDraw(lpDDSBack) )
	{
		lpSurf=GraphBuffer;
		EndDraw(lpDDSBack);
	}

	if( BeginDraw(Temp) )
	{
		lpTemp=GraphBuffer;
		EndDraw(Temp);
	}

	oldtick=timeGetTime();	//��ʼʱ��

	for(int i=0; i<16; i++)
	{
		//��������ĵ�ʹ������ĵ����alpha��ϵ�lpDDSBack
		for(int j=0; j<ScreenWidth*ScreenHeight; j++)
		{
			lpSurf[j]=Alpha_Pixel(BlackColor, lpTemp[j], alpha);
		}

		//��ʱ
		newtick=timeGetTime();
		while( newtick-oldtick < (unsigned int)(time * i) )
		{
			newtick=timeGetTime();
		}
		
		//������Ļ
		_UpdateScreen();
		
		//�Զ�����
		i=(newtick-oldtick)/time;
		//alpha����
		alpha=i*2+2;

		if( i >= 15 ) //����
		{
			alpha=31;
			for(int j=0; j<ScreenWidth*ScreenHeight; j++)
			{
				lpSurf[j]=Alpha_Pixel(BlackColor, lpTemp[j], alpha);
			}
			_UpdateScreen();
			break;
		}
	}
	
	//�ͷ�
	_RELEASE( Temp );
}

//����(time=ÿһ֡��ʱ��)
void ColorOut(LPDIRECTDRAWSURFACE dest, int time)
{
	unsigned int alpha=2, oldtick, newtick;
	WORD *lpSour, *lpDest, *lpBack;

	if( WindowMode != 0 )	//����
	{
		Blt(lpDDSSour, 0,0, lpDDSPrimary, RectWindow, false);
	}
	else
	{
		Blt(lpDDSSour, 0,0, lpDDSPrimary, RectScreen, false);
	}

	if( BeginDraw(lpDDSSour) )
	{
		lpSour=GraphBuffer;
		EndDraw(lpDDSSour);
	}

	if( BeginDraw(dest) )
	{
		lpDest=GraphBuffer;
		EndDraw(dest);
	}

	if( BeginDraw(lpDDSTemp) )
	{
		lpBack=GraphBuffer;
		EndDraw(lpDDSTemp);
	}

	oldtick=timeGetTime();

	for(int i=0; i<16; i++)
	{
		for(int j=0; j<ScreenWidth*ScreenHeight; j++)
		{
			//��������ĺ����Ա�һ��
			//����Ǵ���Ļ�𽥱仯��destҳ��
			lpBack[j]=Alpha_Pixel(lpDest[j], lpSour[j], alpha);
		}

		//��ʱ
		newtick=timeGetTime();
		while( newtick-oldtick < (unsigned int)(time*i) )	
		{
			newtick=timeGetTime();
		}

		//������Ļ
		_UpdateScreen(lpDDSTemp);

		//�Զ�����
		i=(newtick-oldtick)/time;
		alpha=i*2+2;

		if( i >= 15 ) //����
		{
			alpha=31;
			for(int j=0; j<ScreenWidth*ScreenHeight; j++)
			{
				lpBack[j]=Alpha_Pixel(lpDest[j], lpSour[j], alpha);
			}
			_UpdateScreen(lpDDSTemp);
			break;
		}
	}
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//������(suf:Ŀ��ҳ�棬step:����Ķ��٣�time:��ʱ 20����)
void ScreenIn(int Step,int Time)
{
	long T1=0, T2=0;
	int tmp=ScreenHeight/Step;
	WORD *Dest;
	T1=T2=timeGetTime();
	
	//ȡҳ��ָ��
	if( BeginDraw(lpDDSBack) )
	{
		Dest=(WORD *)GraphBuffer;
		EndDraw(lpDDSBack);
	}
	
	for( int j=0; j<tmp; j++)
	{
		//��ʱ
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;

		for(int i=0; i<Step; i++)
			memset(&Dest[ScreenWidth*i*tmp+ScreenWidth*j], 0, ScreenWidth*2);
		
		_UpdateScreen();		//������Ļ
	}
}

//���񵭳�����ʼҳ�棬Ŀ��ҳ�棬����������ʱ��
void ScreenOut(LPDIRECTDRAWSURFACE dest,int Step,int Time)
{
	long T1, T2;
	WORD *Dest,*Sour;
	T1=T2=timeGetTime();
	
	//��ʱҳ��
	LPDIRECTDRAWSURFACE lpDDSTemp;
	CreateBitmap(lpDDSTemp, ScreenWidth, ScreenHeight);

	if( WindowMode != 0 )	//����
	{
		Blt(lpDDSTemp, 0,0, dest, RectScreen, false);
		Blt(lpDDSBack, 0,0, lpDDSPrimary, RectWindow, false);
	}
	else
	{
		Blt(lpDDSTemp, 0,0, dest, RectScreen, false);
		Blt(lpDDSBack, 0,0, lpDDSPrimary, RectScreen, false);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(lpDDSBack) )
	{
		Sour=(WORD *)GraphBuffer;
		EndDraw(lpDDSBack);
	}

	//ȡĿ��ҳ��ָ��
	if( BeginDraw(lpDDSTemp) )
	{
		Dest=(WORD *)GraphBuffer;
		EndDraw(lpDDSTemp);
	}
	
	for( int j=0; j<ScreenHeight/Step; j++)
	{
		//��ʱ
		while( T1-T2<Time )
		{
			T1=timeGetTime();
		}
		T2=T1;
	
		for(int i=0; i<Step; i++)
		{
			int sult=ScreenWidth*i*(ScreenHeight/Step)+ScreenWidth*j;
			memcpy( &Sour[sult], &Dest[sult], ScreenWidth*2);
		}

		_UpdateScreen();		//������Ļ
	}

	//�ͷ�
	_RELEASE( lpDDSTemp );
}

//--------------------------------------------------------------//
//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------//

//��һ������˳ʱ����ת90��(�����������������;���)
void RolSurface(LPDIRECTDRAWSURFACE SS,int w)
{
	WORD *Surf,*Temp;
	int Width;
	LPDIRECTDRAWSURFACE DS;
	
	//ȡҳ��ָ��
	if( BeginDraw(SS) )
	{
		Surf=(WORD *)GraphBuffer;
		Width=GraphPitch;
		EndDraw(SS);
	}

	CreateBitmap(DS, w, w, NULL, DDSCAPS_SYSTEMMEMORY);
	//ȡҳ��ָ��
	if( BeginDraw(DS) )
	{
		Temp=(WORD *)GraphBuffer;
		EndDraw(DS);
	}

	_asm{
		mov edi, Temp		//ָ�뱣�浽esi��edi��
		mov esi, Surf

		mov ebx, 0			//����ѭ������i,j
_loop1:	mov ecx, 0	
_loop2:	mov eax, Width		//eax�ŵ���Ҫ��ת�ı���Ŀ��
		mul ebx				//i*Width+j
		add eax, ecx
		shl eax, 1			//��2
		push eax			//ѹջ��(i*Width+j)*2
		
		mov eax, w			//�����еı߳�
		dec eax				//��һ
		sub eax, ecx		//��j
		mul Width			//���Ͽ��
		add eax, ebx		//����i
		shl eax, 1			//eax=((w-1-j)*Width+i)*2

		mov dx, [esi+eax]	//dx�ŵ��ǵ�ǰ���λ��
		pop eax				//�ָ�eax��ֵ��eax = (i*Width+j)*2
		mov [edi+eax], dx	//�ŵ��±������Ӧ��
		
		inc ecx				//j++
		cmp ecx, w			//j<w��
		jnz _loop2			//��û�оͻ����ڵڶ���ѭ��
		
		inc ebx				//����i++
		cmp ebx, w			//i<w��
		jnz _loop1			//��û�оͼ���ѭ��
	}
		
	memcpy(Surf, Temp, (Width*w)*2);	//��temp���Ƶ�surf��
	//free(Temp);
}
//��������inline�����õ�R��G��B��ֵ
inline unsigned char GetRed(WORD color)
{
	if( Is555 )
		return (color>>7) & 0xff;
	else
		return (color>>8) & 0xff;
}

inline unsigned char GetGreen(WORD color)
{
	if( Is555 )
		return (color>>2) & 0xff;
	else
		return (color>>3) & 0xff;
}
	
inline unsigned char GetBlue(WORD color)
{
	return (color & 0x1f) << 3;
}

// ���ܣ���һ��16λ��DirectDraw���棬��Ϊһ��24λBMPλͼ
// ���룺����ָ�룬������ļ���
// ������Ƿ�ɹ�
bool SaveToBitmapFile(LPDIRECTDRAWSURFACE lpSurface, char* filename)
{
	WORD* lpBuffer;			// ����ָ��
	int nPitch; 			// ������
	int nWidth, nHeight;	// ������
	
	// ���ļ�s
	FILE* fp;
	if( (fp=fopen(filename, "wb")) != NULL )
	{
		// ��������
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);
		HRESULT ddrval = lpSurface->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
		if( ddrval == DD_OK )
		{
  			lpBuffer = (WORD *)ddsd.lpSurface;
			nWidth = ddsd.dwWidth;
			nHeight = ddsd.dwHeight;
			//lPitch��ByteΪ��λ�����ģ�GraphPitch��WORDΪ��λ������GraphPitch = lPitch / 2;
			nPitch  = ddsd.lPitch >> 1;	
		}

		// �����ļ�ͷ
		BITMAPFILEHEADER FileHeader;
		FileHeader.bfType = 'BM';
		FileHeader.bfSize = nWidth * nHeight * 3 + 0x36;
		FileHeader.bfReserved1 = 0;
		FileHeader.bfReserved2 = 0;
		FileHeader.bfOffBits = 0x36;
		fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		
		// �����ļ���Ϣ
		BITMAPINFOHEADER Header;
		Header.biSize = sizeof(BITMAPINFOHEADER);	// �ṹ�Ĵ�С
		Header.biWidth = nWidth;					// ��
		Header.biHeight = nHeight;					// ��
		Header.biPlanes = 1;						// �̶�
		Header.biBitCount = 24;						// ��ɫ��
		Header.biCompression = BI_RGB;				// �Ƿ�ѹ��
		Header.biSizeImage = nWidth * nHeight * 3;	// ͼƬ�Ĵ�С
		Header.biXPelsPerMeter = 0;
		Header.biYPelsPerMeter = 0;
		Header.biClrUsed = 0;
		Header.biClrImportant = 0;
		fwrite(&Header, Header.biSize, 1, fp);

		// д���������(�������ϴ��)
		fseek(fp, 0x36, SEEK_SET);
		WORD word;
		lpBuffer += nWidth * (nHeight - 1);
		for(int i=0; i<nHeight; i++)
		{
			for(int j=0; j<nWidth; j++)
			{
				word = *lpBuffer;
				fputc( GetBlue( word ), fp);	// ��
				fputc( GetGreen( word ), fp);	// ��
				fputc( GetRed( word ), fp);		// ��
				lpBuffer++;
			}
			lpBuffer -= nPitch*2;	// ָ��ת����һ�еĿ�ʼ
		}

		fclose(fp);

		// ��������
		ddrval = lpSurface->Unlock( NULL );
		return true;
	}

	return false;
}
