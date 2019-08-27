//********************************************
//	动画 相关函数
//  创建于2000年4月16日
//********************************************
#include <windows.h>
#include <ddraw.h>
#include <VFW.H>
#include "gp_init.h"
#include "gp_draw.h"
#include "gp_other.h"
#include "gp_avi.h"


//构造初始化
CAvi::CAvi()
{
	VideoStream=0;
	rawdata=NULL;
	VideoDataBuffer=NULL;
	decomp=0;

	AVIFileInit();
}

//打开音频流
void CAvi::OpenAudioStream(char *FileName)
{
	/*
	long res;

	if (AudioStream)
	{
		AVIStreamRelease(AudioStream);
		AudioStream = 0;
	}

	res = AVIStreamOpenFromFile(&AudioStream, FileName, streamtypeAUDIO, 0, OF_READ, 0);

	//成功
	if(res == 0)
	{
		res = AVIStreamFormatSize(AudioStream, 0, &fmtlen);
		if(res == 0 && fmtlen <= 16)
		{
			res = AVIStreamReadFormat(AudioStream, 0, &AudioSrcFmt, &fmtlen);
			if(res == 0)
			{
				if(AudioSrcFmt.wFormatTag==WAVE_FORMAT_PCM)
				{
					StartAudioFrame = AVIStreamStart(AudioStream);
					EndAudioFrame = AVIStreamEnd(AudioStream);
				
					AudioBufLen = (EndAudioFrame - StartAudioFrame) * AudioSrcFmt.nChannels * (AudioSrcFmt.wBitsPerSample/8);

					_RELEASE(lpAudioDataBuffer);
					if(CreateDirectSoundBuffer((LPBYTE)(&AudioSrcFmt), AudioBufLen, &lpAudioDataBuffer))
					{
						{
						    LPVOID		lpvPtr1; 
							DWORD		dwBytes1; 
							LPVOID		lpvPtr2; 
							DWORD		dwBytes2; 
							HRESULT		hres;

							hres = lpAudioDataBuffer->SetCurrentPosition(0);
							hres = lpAudioDataBuffer->Lock(0, AudioBufLen, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

						    if(DSERR_BUFFERLOST == hres) 
							{
								lpAudioDataBuffer->Restore();
								hres = lpAudioDataBuffer->Lock(0, AudioBufLen, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
							}

							if(DS_OK == hres) 
							{
								res = AVIStreamRead(AudioStream, StartAudioFrame, EndAudioFrame - StartAudioFrame, lpvPtr1, AudioBufLen, 0, 0);
								hres = lpAudioDataBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
							}
						}
					}
				}
			}
		}
	}
	if (AudioStream)
	{
		AVIStreamRelease(AudioStream);
		AudioStream = 0;
	}
	*/
}

//打开视频流文件
int CAvi::OpenVideoStream(char *FileName)
{	
	long res;

	LPBITMAPINFOHEADER	lpsrcfmt = (LPBITMAPINFOHEADER)VideoSrcFmt;
	LPBITMAPINFOHEADER	lpdstfmt = (LPBITMAPINFOHEADER)VideoDstFmt;

	//关闭先前打开的AVI文件流
	if( VideoStream )
	{
		AVIStreamRelease(VideoStream);
		VideoStream = 0;
	}

	//打开新的文件流
	res = AVIStreamOpenFromFile(&VideoStream, FileName, streamtypeVIDEO, 0, OF_READ, 0);

	//成功
	if(res == 0)
	{
		res = AVIStreamFormatSize(VideoStream, 0, &fmtlen);	//获取格式长度
		if(res == 0 && fmtlen <= 40)
		{
			//读取视频流格式
			res = AVIStreamReadFormat(VideoStream, 0, VideoSrcFmt, &fmtlen);
			if(res == 0)
			{
				//参数
				VideoWidth = lpsrcfmt->biWidth;
				VideoHeight = lpsrcfmt->biHeight;
				VideoBits = lpsrcfmt->biBitCount;

				//开始桢和结束桢
				StartVideoFrame = AVIStreamStart(VideoStream);
				EndVideoFrame = AVIStreamEnd(VideoStream);

				//目标流结构
				memcpy(VideoDstFmt, VideoSrcFmt, fmtlen);
				lpdstfmt->biBitCount = 32;
				lpdstfmt->biCompression = BI_RGB;
				lpdstfmt->biSizeImage = VideoWidth * VideoHeight * 4;

				if(VideoWidth <= ScreenWidth && VideoHeight <= ScreenHeight )
				{
					//获取一些视频的信息
					res = AVIStreamInfo(VideoStream, &VideoStreamInfo, sizeof(VideoStreamInfo));
					if(res == 0)
					{
						VideoBufLen = lpdstfmt->biSizeImage;
						VideoRate = VideoStreamInfo.dwRate;
						VideoScale = VideoStreamInfo.dwScale;

						//得到一个解压句柄
						if(decomp)
						{
							ICClose(decomp);
						}
						decomp = ICDecompressOpen(ICTYPE_VIDEO, VideoStreamInfo.fccHandler, lpsrcfmt, lpdstfmt);

						if(decomp==0)
						{
							ShowMessage("Can't create a ICDecompress for Avi file %s", FileName);
							return false;
						}
					}

					if( rawdata != NULL )
					{
						delete []rawdata;
					}
					rawdata=new DWORD[VideoBufLen];

					if( VideoDataBuffer != NULL )
					{
						delete []VideoDataBuffer;
					}
					VideoDataBuffer=new DWORD[VideoBufLen];

					return true;
				}
			}
		}
	}
	ShowMessage("Error open Avi file %s", FileName );
	return FALSE;
}

/*--------------------------------------*/

void CAvi::LoadVideoFrame(long num)
{
	long res;
	LPBITMAPINFOHEADER	lpsrcfmt = (LPBITMAPINFOHEADER)VideoSrcFmt;
	LPBITMAPINFOHEADER	lpdstfmt = (LPBITMAPINFOHEADER)VideoDstFmt;

	res = AVIStreamRead(VideoStream, num, 1, rawdata, VideoBufLen, 0, 0);
	ICDecompress(decomp, 0, lpsrcfmt, rawdata, lpdstfmt, VideoDataBuffer);
}

/*--------------------------------------*/

int CAvi::OpenVideoFile(char *FileName)
{
	if(OpenVideoStream(FileName))
	{
	//	OpenAudioStream(FileName);
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------*/

void CAvi::CloseVideoFile(void)
{	
//	_RELEASE(lpAudioDataBuffer);

	if(decomp)
	{
		ICClose(decomp);
		decomp = NULL;
	}

	/*
	if(AudioStream)
	{
		AVIStreamRelease(AudioStream);
		AudioStream = NULL;
	}
	*/

	if(VideoStream)
	{
		AVIStreamRelease(VideoStream);
		VideoStream = NULL;
	}
}

/*--------------------------------------*/

void CAvi::FilpWholeVideoBuffer(void)		// filp video image
{
	WORD *Dest;
	
	//取页面指针
	if( BeginDraw(lpDDSBack) )
	{
		Dest=(WORD *)GraphBuffer;
		EndDraw(lpDDSBack);
	}
	
	for( int i=0; i<VideoHeight; i++)
		for( int j=0; j<VideoWidth; j++)
		{
			Dest[ScreenWidth*i+j]=RGB16(VideoDataBuffer[VideoWidth*(VideoHeight-i)+j]);
		}

	/*
	for( int j=0; j<VideoHeight; j++)
	{
		memcpy(&Dest[ScreenWidth*j], &VideoDataBuffer[VideoWidth*(VideoHeight-j)], VideoWidth*2);
	}
	*/

	/*
	LPDWORD		lpSrc = (LPDWORD)VideoDataBuffer + VideoWidth * (VideoHeight - 1);
	DWORD		LineOff = (ScreenWidth - VideoWidth) / 2 * 4;
	DWORD		LineOff2 = VideoWidth * 4 * 2;
	DWORD		HighOff = (ScreenHeight - VideoHeight) * ScreenWidth / 2 * 4;

	_asm {
		MOV		ESI, lpSrc
		MOV		EDI, lpRenderBuffer
		ADD		EDI, HighOff

		MOV		EDX,VideoHeight

ExpandVideoBuffer_LoopY:
		ADD		EDI, LineOff

		MOV		ECX, VideoWidth
		REP		MOVSD

		ADD		EDI, LineOff

		SUB		ESI,LineOff2

		DEC		EDX
		JNZ		ExpandVideoBuffer_LoopY
	}
	*/
}

/*--------------------------------------*/
void CAvi::RefreshScreen(void)
{
}

void CAvi::PlayAVIFile(char *FileName)
{
	long		frame;
	long		time1, time2, time3;
    MSG			msg;
	double		temp;

	if(OpenVideoFile(FileName)==FALSE) return;

	frame = StartVideoFrame;

//	ClearRenderBuffer(0);

	time1 = timeGetTime();
	time2 = time1;
	time3 = 0;

	VideoPlayingFlag = TRUE;

	do
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0)) return;// msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
		{
	        if(bActive)
		    {
				if(time3 > 0)
				{
					time2 += time3;
					time3 = 0;
				}

				time1 = timeGetTime();
				temp= (frame - StartVideoFrame) * VideoScale * 1000.0 / VideoRate;
				if((time1 - time2) >= (long)temp)
				{
					LoadVideoFrame(frame);
					frame++;
					if(frame >= EndVideoFrame)
					{
						VideoPlayingFlag = FALSE;
					}
					
					/*
					if(frame <= StartVideoFrame+1)
					{
						if(lpAudioDataBuffer)
							lpAudioDataBuffer->Play(NULL, NULL, 0);
					}
					*/

					FilpWholeVideoBuffer();
					//ShowMessage("continue");
				}
				_UpdateScreen();		//更新屏幕
			}
			else
			{
				time3 = timeGetTime() - time1;
			}
		}
	}	while(VideoPlayingFlag);

	CloseVideoFile();

	return;
}

/*--------------------------------------*/

void CAvi::LoopPlayAVIFile(char *FileName)
{
	long		frame;
	long		time1, time2, time3;
    MSG			msg;
	double		temp;

	if(OpenVideoFile(FileName)==FALSE) return;

	frame = StartVideoFrame;

//	ClearRenderBuffer(0);

	time1 = timeGetTime();
	time2 = time1;
	time3 = 0;

	VideoPlayingFlag = TRUE;

	do
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0)) return;// msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
		{
	        if(bActive)
		    {
				if(time3 > 0)
				{
					time2 += time3;
					time3 = 0;
				}

				time1 = timeGetTime();
				temp= (frame - StartVideoFrame) * VideoScale * 1000.0 / VideoRate;
				if((time1 - time2) >= (long)temp)
				{
					LoadVideoFrame(frame);
					frame++;
					if(frame >= EndVideoFrame)
					{
						frame = StartVideoFrame;
						time1 = timeGetTime();
						time2 = time1;
						time3 = 0;

						if(lpAudioDataBuffer)
						{
							lpAudioDataBuffer->Stop();
							lpAudioDataBuffer->SetCurrentPosition(0);
							lpAudioDataBuffer->Play(NULL, NULL, 0);
						}
					}

					if(frame <= StartVideoFrame+1)
					{
						if(lpAudioDataBuffer)
							lpAudioDataBuffer->Play(NULL, NULL, 0);
					}
//SetRenderPage(BACKPAGE);
					FilpWholeVideoBuffer();
//SetRenderPage(MAINPAGE);
				}
//AddRipple(rand()%RENDER_WIDTH, rand()%RENDER_HEIGHT, 3,rand()&0x1FF);
//RippleSpread();
//RenderRipple();
				RefreshScreen();
			}
			else
			{
				time3 = timeGetTime() - time1;
			}
		}
	}	while(VideoPlayingFlag);

	CloseVideoFile();
	return;
}
