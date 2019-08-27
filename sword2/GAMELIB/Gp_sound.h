//********************************************
//	DirectSound 相关处理函数
//  创建于2000年1月25日
//********************************************
#ifndef _GP_SOUND_
#define _GP_SOUND_

#include <dsound.h>

//声音类
class CSound{
	static BOOL bEnable;			//允许音效
	static BOOL bExist;				//是否已经初始化
	static LPDIRECTSOUND lpDSound;	//设备
	static LPDIRECTSOUNDBUFFER lpPrimarySoundBuffer;	//主缓冲区
public:
	LPDIRECTSOUNDBUFFER DSSoundFX;	//后台缓冲区

public:
	CSound();
	~CSound();								//析构
	static BOOL InitDirectSound(HWND hwnd);	//初始化声音设备
	static bool CreatePrimarySoundBuffer();	//创建主声音缓冲区
	static void ReleaseDirectSound(void);	//释放声音设备
	static void EnableSound(bool);			//音效开关

	bool LoadSoundFX(HWND hwnd,char *filename);	//装入声音
	void Play(DWORD loop=0);	//播放声音
	void Stop();				//停止播放
	bool IsPlaying();			//缓冲区是否正在播放


	//填充一个WAV声音缓冲区
	bool LoadSoundBuffer(LPDIRECTSOUNDBUFFER lpSoundBuffer, char *filename);
};

#endif