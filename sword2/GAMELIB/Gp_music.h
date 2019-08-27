//********************************************
//	DMUSIC 相关函数
//  创建于2000年4月18日
//********************************************

#ifndef _Header_Music
#define _Header_Music

#include <dmusicc.h>
#include <dmusici.h>

//DMusic类
class CMusic{
public:
	static bool bMusic;		//是否有音乐
	static bool bExist;		//主缓冲是否已存在
	static bool bPlay;		//是否播放
	static IDirectMusicPerformance* g_pPerf;
	static IDirectMusicLoader* g_pLoader;
	IDirectMusicSegment* g_pMIDISeg;
	MUSIC_TIME PauseTime;			//暂停点
	WCHAR MusicFile[MAX_PATH];		//文件

public:
	CMusic();
	~CMusic();		//析构
	static IDirectMusicPerformance* CreatePerformance(void);
	static IDirectMusicLoader* CreateLoader(void);
	static IDirectMusicSegment* LoadMIDISegment(IDirectMusicLoader* pLoader, WCHAR *wszMidiFileName );
	static BOOL InitMusic();			//初始化DMUSIC
	
	BOOL LoadMusic(WCHAR *);	//读入
	BOOL Play(int start=0);		//播放
	BOOL Stop();				//停止
	BOOL Pause();				//暂停
	BOOL Resume();				//继续
	BOOL IsPlay();				//是否正在播放
};

#endif