//********************************************
//	DirectSound 相关处理函数
//  创建于2000年2月4日
//********************************************

#include <windows.h>
#include <stdio.h>
#include <dsound.h>
#include <direct.h>
#include "gp_init.h"
#include "gp_other.h"
#include "gp_sound.h"

#define WAV_HDR_SIZE  44
#define SND_GATE  0
#define SND_PING  1
#define SND_STATIC 2
#define SND_WALK 3

extern HWND hWnd;

BOOL CSound::bExist=false;
BOOL CSound::bEnable=false;
LPDIRECTSOUND CSound::lpDSound=NULL;
LPDIRECTSOUNDBUFFER CSound::lpPrimarySoundBuffer=NULL;

CSound::CSound()
{
	DSSoundFX=NULL;
}

//-----------------
//释放声音缓冲区
CSound::~CSound()
{
	if( lpDSound && DSSoundFX )
	{
		_RELEASE( DSSoundFX );
	}
}

//-----------------
//初始化声音设备
BOOL CSound::InitDirectSound(HWND hwnd)	
{	
	if( bExist || bEnable ) return true;
	
	if( DirectSoundCreate(NULL,&lpDSound,NULL)!=DS_OK )
	{
		MessageBox(hwnd,"Create DirectSound Failed!\n初始化DirectSound过程中出错！\
			请检查你是否正确的安装了DirectX 5.0以上版本，或有无其他设备正占用声卡！\
			否则你将无法听到声音。","error",MB_OK);

		bEnable=FALSE;
		return FALSE;
	}

	if( lpDSound->SetCooperativeLevel(hwnd,DSSCL_NORMAL)!=DS_OK)
	{
		MessageBox(hwnd,"error on set DirectSound CooperativeLevel\n初始化DirectSound\
			过程中出错！请检查你是否正确的安装了DirectX 5.0以上版本，或有无其他设备正占\
			用声卡！否则你将无法听到声音。","error",MB_OK);

		bEnable=FALSE;
		return FALSE;
	}
	bEnable = true;
	bExist=true;
	return TRUE;  // 初始化成功
}

//----------------
//创建主声音缓冲区
bool CSound::CreatePrimarySoundBuffer()
{
	if( bExist || !bEnable ) 
		return true;

    DSBUFFERDESC	dsbdesc;
	WAVEFORMATEX	pcmwf;

    // 设置wave格式结构
    memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
    pcmwf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.nChannels = 1;
    pcmwf.nSamplesPerSec = 8000;
    pcmwf.nBlockAlign = 4;
    pcmwf.nAvgBytesPerSec = 
    pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    pcmwf.wBitsPerSample = 8;
	pcmwf.cbSize = 0;

    // 设置 DSBUFFERDESC 结构
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    // 主缓冲区必须是0
    dsbdesc.dwBufferBytes = 0; 
    dsbdesc.lpwfxFormat = NULL; // 主缓冲区必须是NULL

	if( (lpDSound->CreateSoundBuffer(&dsbdesc, &lpPrimarySoundBuffer, NULL)) == DS_OK)
	{
		if( (lpPrimarySoundBuffer->SetFormat(&pcmwf)) == DS_OK )
			return true;
	}
	
	return false;
}

//-------------
//释放声音设备
void CSound::ReleaseDirectSound(void)
{
	if( !bExist ) 
		return;
	bExist=false;

	if( lpDSound != NULL )
	{
		_RELEASE( lpPrimarySoundBuffer );
		_RELEASE( lpDSound );
	}
}

//---------------------
//音效开关
void CSound::EnableSound(bool sw)
{
	CSound::bEnable=sw;
}

//---------------------
//创建一个WAV声音缓冲区
bool CSound::LoadSoundFX(HWND hwnd,char *filename)
{
	if( !bEnable ) return true;

	DSBUFFERDESC dsbd;	//结构
	LPWAVEFORMATEX wav;	//声音格式
	DWORD len; 			//文件长度
	BYTE *buf; 			//临时缓冲区
	LPVOID ptr1;		//指针1
	LPVOID ptr2;		//指针2
	DWORD size1;		
	DWORD size2;
	DWORD BufferSize;	//DirectSoundBuffer 大小

	FILE *f;
	long WavLong;
	if( (f=fopen(filename,"rb"))==NULL )
	{
		MessageBox(hwnd,"error open file",filename,MB_OK);
		return FALSE;
	}
	fseek(f,0x04,SEEK_SET);
	fread(&WavLong,sizeof(long),1,f);	//读取声音数据长度
	len=WavLong+WAV_HDR_SIZE;			//文件长度
	BufferSize=len-WAV_HDR_SIZE-800;	//设置缓冲区大小

	buf=(BYTE *)malloc(len);
	if( buf==NULL )
	{
		MessageBox(hwnd,"error buf",filename,MB_OK);
		return FALSE;
	}

	//读取声音数据到临时缓冲区
	fseek(f,0l,SEEK_SET);
	fread(buf,len,1,f);
	fclose(f);

	ptr1=buf+20;
	wav=(LPWAVEFORMATEX)(ptr1);
	
	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_STATIC;
	dsbd.dwBufferBytes=len-WAV_HDR_SIZE;
	dsbd.lpwfxFormat=wav;

	_RELEASE( DSSoundFX );

	if(lpDSound->CreateSoundBuffer(&dsbd,&DSSoundFX,NULL)!=DS_OK)
	{
		MessageBox(hwnd,"error createsoundbuffer","error",MB_OK);
		return FALSE;
	}

	if( (DSSoundFX->Lock(0,BufferSize,&ptr1,&size1,&ptr2,&size2,0))!=DS_OK)
	{
		MessageBox(hwnd,"error Lock","error",MB_OK);
		return FALSE;
	}
	memcpy(ptr1,buf+WAV_HDR_SIZE,size1); //拷贝数据到声音缓冲区
	if(size2)
		memcpy(ptr2,buf+size1+WAV_HDR_SIZE,size2);//拷贝数据到声音缓冲区
	if( DSSoundFX->Unlock(ptr1,size1,ptr2,size2)!=DS_OK)
	{
		MessageBox(hwnd,"error UnLock","error",MB_OK);
		return FALSE;
	}
	
	_FREE(buf);
	
	return true;
}

//-------------------------
//功能：播放声音
//参数：loop=-1 循环
void CSound::Play(DWORD loop)
{
	if( bEnable==FALSE || !DSSoundFX ) 
		return;
	
	DWORD *Status1=NULL;
	if( (DSSoundFX->GetStatus(Status1))!=DS_OK ) 
		goto _next;
	if( *Status1 & DSBSTATUS_PLAYING == DSBSTATUS_PLAYING  ) 
		return;

_next:
	DSSoundFX->SetCurrentPosition(0);
	if( loop==-1 ) 
		loop=DSBPLAY_LOOPING;	//循环播放
	DSSoundFX->Play(0,0,loop);
}

//------------------------
//停止播放
void CSound::Stop()
{
	if( bEnable==FALSE || DSSoundFX==NULL ) 
		return;

	DSSoundFX->Stop();
}

//-------------------------
//缓冲区是否正在播放
bool CSound::IsPlaying()
{
	if( bEnable==FALSE || DSSoundFX==NULL )
		return true;

	DWORD pos, wpos;
	DSBCAPS dsbc;
	
	memset(&dsbc,0,sizeof(DSBCAPS));
	dsbc.dwSize=sizeof(DSBCAPS);
	if( (DSSoundFX->GetCaps(&dsbc)) != DS_OK ) 
	{
		MessageBox(hWnd, "DSSoundFX->GetCaps","error",MB_OK);
		return true;
	}
	pos=dsbc.dwBufferBytes;
	
	if( (DSSoundFX->GetCurrentPosition(&wpos,NULL)) != DS_OK )
	{
		MessageBox(hWnd, "DSSoundFX->GetCurrentPosition","error",MB_OK);
		return true;
	}

	if( wpos >= pos/7.16 ) 
		return false;
	return true;
}

//---------------------
//填充一个WAV声音缓冲区
bool CSound::LoadSoundBuffer(LPDIRECTSOUNDBUFFER lpSoundBuffer, char *filename)
{
	if( bEnable==FALSE ) return FALSE;
	
	FILE *f;
	DSBUFFERDESC dsbd;		//结构
	LPWAVEFORMATEX wav;  	//声音格式
	DWORD FileLen; 			//文件长度
	BYTE *Buf;	 			//临时缓冲区
	long BufLong;			//声音缓冲区长度

	if( (f=fopen(filename,"rb"))==NULL )
	{
		MessageBox(hWnd,"Not found the sound file!", filename, MB_OK);
		return FALSE;
	}
	fseek(f,0x28,SEEK_SET);
	fread(&BufLong,sizeof(long),1,f);	//读取声音数据长度
	FileLen=BufLong+WAV_HDR_SIZE;		//文件长度

	Buf=(BYTE *)malloc(FileLen);
	if( Buf==NULL )
	{
		MessageBox(hWnd,"error alloc soundbuffer","error",MB_OK);
		return FALSE;
	}
	
	//读取声音数据到临时缓冲区
	fseek(f,0x00,SEEK_SET);
	fread(Buf,FileLen,1,f);
	fclose(f);

	wav=(LPWAVEFORMATEX)(Buf+20);

	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_STATIC;
	dsbd.dwBufferBytes=BufLong;
	dsbd.lpwfxFormat=wav;
	
	//创建缓冲区并读取数据
	if(	lpDSound->CreateSoundBuffer(&dsbd, &lpSoundBuffer, NULL) )
	{		
		LPVOID ptr1;		//指针1
		LPVOID ptr2;		//指针2
		DWORD size1;		
		DWORD size2;

		if( ( lpSoundBuffer->Lock(0,BufLong,&ptr1,&size1,&ptr2,&size2,0))!=DS_OK)
		{
			MessageBox(hWnd,"error Lock DSoundBuffer","error",MB_OK);
			return FALSE;
		}
		
		//拷贝数据到声音缓冲区
		memcpy(ptr1,Buf+WAV_HDR_SIZE,size1);
		if(size2) memcpy(ptr2,Buf+size1+WAV_HDR_SIZE,size2);
		
		if( lpSoundBuffer->Unlock(ptr1,size1,ptr2,size2)!=DS_OK)
		{
			MessageBox(hWnd,"error UnLock SoundBuffer","error",MB_OK);
			return FALSE;
		}
	}
	_FREE(Buf);
	return true;//返回
}
