//********************************************
//	AVI ��غ���
//  ������2000��6��06��
//********************************************
#ifndef _GP_AVI_
#define _GP_AVI_

#include <dsound.h>
#include <VFW.H>

//Avi������
class CAvi{
public:
	BOOL			VideoPlayingFlag;	//���ű�־

	DWORD			*rawdata;			//ԭʼ����
	DWORD			*VideoDataBuffer;	//��ѹ������

	PAVISTREAM 		VideoStream;		//��Ƶ��
	AVISTREAMINFO	VideoStreamInfo;	//��Ƶ��Ϣ
	HIC				decomp;				//��ѹ

	long			fmtlen;				//��Ƶ�ṹ����
	long			VideoBufLen;		//��Ƶ��������С
	long			VideoWidth, VideoHeight;	//�ߴ�
	long			VideoBits;					//λ��
	long			VideoRate, VideoScale;		//�ٶ�
	long			StartVideoFrame, EndVideoFrame;	//��ʼ��ͽ�����

	BYTE			VideoSrcFmt[40];	//Դ��Ƶ�ṹ
	BYTE			VideoDstFmt[40];	//Ŀ����Ƶ�ṹ

	PAVISTREAM 		AudioStream;
	AVISTREAMINFO	AudioStreamInfo;

	WAVEFORMATEX 	AudioSrcFmt;

	long			StartAudioFrame, EndAudioFrame;
	long			AudioBufLen;

	LPDIRECTSOUNDBUFFER		lpAudioDataBuffer;

public:
	CAvi();		//�����ʼ��
	void OpenAudioStream(char *FileName);	//����Ƶ��
	int OpenVideoStream(char *FileName);	//����Ƶ��
	void LoadVideoFrame(long num);			//����Ƶ��
	int OpenVideoFile(char *FileName);		//����Ƶ�ļ�
	void CloseVideoFile(void);				//�ر���Ƶ�ļ�
	void FilpWholeVideoBuffer(void);		// filp video image
	void RefreshScreen(void);
	void PlayAVIFile(char *FileName);		//������Ƶ
	void LoopPlayAVIFile(char *FileName);	//ѭ��������Ƶ
};

#endif