//********************************************
//	DirectSound ��ش�����
//  ������2000��1��25��
//********************************************
#ifndef _GP_SOUND_
#define _GP_SOUND_

#include <dsound.h>

//������
class CSound{
	static BOOL bEnable;			//������Ч
	static BOOL bExist;				//�Ƿ��Ѿ���ʼ��
	static LPDIRECTSOUND lpDSound;	//�豸
	static LPDIRECTSOUNDBUFFER lpPrimarySoundBuffer;	//��������
public:
	LPDIRECTSOUNDBUFFER DSSoundFX;	//��̨������

public:
	CSound();
	~CSound();								//����
	static BOOL InitDirectSound(HWND hwnd);	//��ʼ�������豸
	static bool CreatePrimarySoundBuffer();	//����������������
	static void ReleaseDirectSound(void);	//�ͷ������豸
	static void EnableSound(bool);			//��Ч����

	bool LoadSoundFX(HWND hwnd,char *filename);	//װ������
	void Play(DWORD loop=0);	//��������
	void Stop();				//ֹͣ����
	bool IsPlaying();			//�������Ƿ����ڲ���


	//���һ��WAV����������
	bool LoadSoundBuffer(LPDIRECTSOUNDBUFFER lpSoundBuffer, char *filename);
};

#endif