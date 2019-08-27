//********************************************
//	DMUSIC ��غ���
//  ������2000��4��18��
//********************************************

#ifndef _Header_Music
#define _Header_Music

#include <dmusicc.h>
#include <dmusici.h>

//DMusic��
class CMusic{
public:
	static bool bMusic;		//�Ƿ�������
	static bool bExist;		//�������Ƿ��Ѵ���
	static bool bPlay;		//�Ƿ񲥷�
	static IDirectMusicPerformance* g_pPerf;
	static IDirectMusicLoader* g_pLoader;
	IDirectMusicSegment* g_pMIDISeg;
	MUSIC_TIME PauseTime;			//��ͣ��
	WCHAR MusicFile[MAX_PATH];		//�ļ�

public:
	CMusic();
	~CMusic();		//����
	static IDirectMusicPerformance* CreatePerformance(void);
	static IDirectMusicLoader* CreateLoader(void);
	static IDirectMusicSegment* LoadMIDISegment(IDirectMusicLoader* pLoader, WCHAR *wszMidiFileName );
	static BOOL InitMusic();			//��ʼ��DMUSIC
	
	BOOL LoadMusic(WCHAR *);	//����
	BOOL Play(int start=0);		//����
	BOOL Stop();				//ֹͣ
	BOOL Pause();				//��ͣ
	BOOL Resume();				//����
	BOOL IsPlay();				//�Ƿ����ڲ���
};

#endif