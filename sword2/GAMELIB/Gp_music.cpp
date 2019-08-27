//********************************************
//	DMUSIC ��غ���
//  ������2000��4��18��
//********************************************
#include <windows.h>
#include <stdio.h>
#include "gp_init.h"
#include "gp_other.h"
#include "gp_music.h"

extern HWND hWnd;

IDirectMusicPerformance* CMusic::g_pPerf=NULL;
IDirectMusicLoader*	CMusic::g_pLoader=NULL;
bool CMusic::bMusic=false;
bool CMusic::bExist=false;
bool CMusic::bPlay=false;

CMusic::CMusic()
{
	g_pMIDISeg=NULL;
}

//����
CMusic::~CMusic()
{
	if( bExist )
	{
		if (g_pMIDISeg)
		{
			g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0);
			_RELEASE(g_pMIDISeg);
		}
	}
	bExist=false;
	bMusic=false;
}

//�������ӿ�
IDirectMusicPerformance* CMusic::CreatePerformance(void)
{
    IDirectMusicPerformance* pPerf;
 
    if (FAILED(CoCreateInstance(
            CLSID_DirectMusicPerformance,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicPerformance,
            (void**)&pPerf
        )))
    {
        pPerf = NULL;
    }
    return pPerf;
}

//����LOADER
IDirectMusicLoader* CMusic::CreateLoader(void)
{
    IDirectMusicLoader* pLoader;
 
    if (FAILED(CoCreateInstance(
            CLSID_DirectMusicLoader,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicLoader,
            (void**)&pLoader
        )))
    {
        pLoader = NULL;
    }
    return pLoader;
}

//����һ������
IDirectMusicSegment* CMusic::LoadMIDISegment(IDirectMusicLoader* pLoader, WCHAR *wszMidiFileName )
{
	if( !bMusic )		//û������
		return NULL;

	//����ļ��Ƿ����
	FILE *fp;
	char* str = Char(wszMidiFileName);
	fp=fopen(str , "rb");
	_DELETE( str );
	if( fp==NULL )
	{
		ShowMessage("Midi file %s not found!",Char(wszMidiFileName));
		return NULL;
	}
	fclose(fp);

    DMUS_OBJECTDESC ObjDesc; 
    IDirectMusicSegment* pSegment = NULL;

    ObjDesc.guidClass = CLSID_DirectMusicSegment;
    ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
    wcscpy(ObjDesc.wszFileName, wszMidiFileName );
    ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
 
    pLoader->GetObject(&ObjDesc,
            IID_IDirectMusicSegment, (void**) &pSegment);		//�õ�object
 
    pSegment->SetParam(GUID_StandardMIDIFile,
            -1, 0, 0, (void*)g_pPerf);							//�����ֶӵ�����
 
    pSegment->SetParam(GUID_Download, -1, 0, 0, (void*)g_pPerf);//�����Զ�����
 
    return pSegment;
}

//��ʼ��
BOOL CMusic::InitMusic()
{	
	if(bExist)		//���������
		return true;
    if (SUCCEEDED(CoInitialize(NULL)))		//��ʼ��COM
    {
		if( !bExist )						//�����岻����
		{
			bExist=true;					
			g_pPerf = CreatePerformance();	//�������ӿ�
			if (g_pPerf == NULL)
			{
				MessageBox(hWnd,"CreatePerformance\n��ʼ��DirectMusic���̳���\
					�������Ƿ���ȷ�İ�װ��DirectX 7.0���ϰ汾������������������\
					ռ�������������㽫�޷������������֡�","ERROR",MB_OK);
				bMusic=false;
				return false;
			}	

			if (FAILED(g_pPerf->Init(NULL, NULL, NULL)))
			{
				MessageBox(hWnd,"Init\nDirectMusic ��ʼ�������밲װDirectX 7.0��\
					�ϰ汾������������������ռ�������������㽫�޷������������֡�",
					"ERROR",MB_OK);
				bMusic=false;
				return false;
			}	
		
			if (FAILED(g_pPerf->AddPort(NULL)))		//���һ��port
			{	
				MessageBox(hWnd,"AddPort\nDirectMusic ��ʼ�������밲װDirectX 7.0��\
					�ϰ汾������������������ռ�������������㽫�޷������������֡�",
					"ERROR",MB_OK);
				bMusic=false;
				return false;
			}

			g_pLoader = CreateLoader();				//����һ��loader
			if (g_pLoader == NULL)
			{
				MessageBox(hWnd,"CreatLoader\nDirectMusic ��ʼ������\
					�밲װDirectX 7.0���ϰ汾������������������ռ�������������㽫�޷�\
					�����������֡�","ERROR",MB_OK);
				bMusic=false;
				return false;
			}
		}
		bMusic=true;
		return true;
	}
	bMusic=false;
	return false;
}

//����
BOOL CMusic::LoadMusic(WCHAR *filename)
{
	if( !bMusic )		//û������
		return true;
	wcscpy( MusicFile, filename );		
	if (g_pLoader)
	{
		_RELEASE(g_pMIDISeg);

		g_pMIDISeg = LoadMIDISegment(g_pLoader,filename);	//����һ����
		return true;
	}
	return false;
}

//���� 
BOOL CMusic::Play(int start)
{
	if( !bMusic )	//����
		return true;

	IDirectMusicSegmentState* g_pSegState;		//��ǰ��״̬
	if (g_pMIDISeg)
	{
		//����һ���Σ�������״̬
		g_pPerf->PlaySegment(g_pMIDISeg, DMUS_SEGF_REFTIME , start, &g_pSegState);	
	}
	bPlay=true;
	return true;
}

//�Ƿ����ڲ��� 
BOOL CMusic::IsPlay()
{
	if( !bMusic )	//û������
		return true;
	if (g_pMIDISeg)
	{
		if( (g_pPerf->IsPlaying(g_pMIDISeg,NULL))==S_OK )
			return true;
	}
	return false;
}

//ֹͣ���� 
BOOL CMusic::Stop()
{
	if( !bMusic ) //����
		return true;
	if (g_pMIDISeg)
	{
		if( (g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0))==S_OK )
		{
			bPlay=false;
			return true;
		}
	}
	return false;
}

//��ͣ����
BOOL CMusic::Pause()
{
	if( !bMusic ) //����
		return true;
	if (g_pMIDISeg)
	{
		g_pPerf->GetTime(NULL, &PauseTime);		//���ŵ���ʲôλ��

		if( (g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0))==S_OK )		//ֹͣ
		{
			bPlay=false;
			return true;
		}
		return false;
	}
	return false;
}

//��������
BOOL CMusic::Resume()
{
	if( !bMusic ) 
		return true;

	if (g_pMIDISeg)
	{
		g_pMIDISeg->SetStartPoint(PauseTime);			//��ʼ��λ��
		IDirectMusicSegmentState* g_pSegState;			//����
		g_pPerf->PlaySegment(g_pMIDISeg, DMUS_SEGF_REFTIME , 0, &g_pSegState);
		g_pMIDISeg->SetStartPoint(0);					
		bPlay=true;
		return true;
	}
	return false;
}
