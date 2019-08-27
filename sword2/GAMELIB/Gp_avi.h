//********************************************
//	AVI 相关函数
//  创建于2000年6月06日
//********************************************
#ifndef _GP_AVI_
#define _GP_AVI_

#include <dsound.h>
#include <VFW.H>

//Avi处理类
class CAvi{
public:
	BOOL			VideoPlayingFlag;	//播放标志

	DWORD			*rawdata;			//原始数据
	DWORD			*VideoDataBuffer;	//解压后数据

	PAVISTREAM 		VideoStream;		//视频流
	AVISTREAMINFO	VideoStreamInfo;	//视频信息
	HIC				decomp;				//解压

	long			fmtlen;				//视频结构长度
	long			VideoBufLen;		//视频缓冲区大小
	long			VideoWidth, VideoHeight;	//尺寸
	long			VideoBits;					//位深
	long			VideoRate, VideoScale;		//速度
	long			StartVideoFrame, EndVideoFrame;	//起始桢和结束桢

	BYTE			VideoSrcFmt[40];	//源视频结构
	BYTE			VideoDstFmt[40];	//目标视频结构

	PAVISTREAM 		AudioStream;
	AVISTREAMINFO	AudioStreamInfo;

	WAVEFORMATEX 	AudioSrcFmt;

	long			StartAudioFrame, EndAudioFrame;
	long			AudioBufLen;

	LPDIRECTSOUNDBUFFER		lpAudioDataBuffer;

public:
	CAvi();		//构造初始化
	void OpenAudioStream(char *FileName);	//打开音频流
	int OpenVideoStream(char *FileName);	//打开视频流
	void LoadVideoFrame(long num);			//读视频桢
	int OpenVideoFile(char *FileName);		//打开视频文件
	void CloseVideoFile(void);				//关闭视频文件
	void FilpWholeVideoBuffer(void);		// filp video image
	void RefreshScreen(void);
	void PlayAVIFile(char *FileName);		//播放视频
	void LoopPlayAVIFile(char *FileName);	//循环播放视频
};

#endif