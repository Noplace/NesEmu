#include <Windows.h>
#include "WaveOut.h"
#pragma comment(lib,"Winmm")

#ifdef _DEBUG
#define TRACE(x) OutputDebugString(x)
#else
#define TRACE
#endif


DWORD CWaveOut::s_dwInstance = 0;

DWORD WINAPI CWaveOut::AudioOutThreadProc(LPVOID lpParameter)
{
	TRACE("AudioOutThreadProc start.\n");
	CWaveOut *pWaveIn;
	pWaveIn = (CWaveOut *)lpParameter;

	MSG msg;
	while(GetMessage(&msg,0,0,0))
	{
		switch(msg.message )
		{
		case WOM_OPEN:
			TRACE("WOM_OPEN \n");
			break;
		case WOM_CLOSE:
			TRACE("WOM_CLOSE \n");
			break;
		case WOM_DONE:
			WAVEHDR* pwh=(WAVEHDR*)msg.lParam;
			waveOutUnprepareHeader((HWAVEOUT)msg.wParam,pwh,sizeof(WAVEHDR));
			pWaveIn->BufferSub ();
			delete []pwh->lpData;//ɾPlayʱڴ
			delete pwh;
			break;
		}
	}
  waveOutReset(pWaveIn->m_hOut);
	TRACE("AudioInThreadProc exit.\n");
	return (DWORD)msg.wParam;
}

CWaveOut::CWaveOut():
	 m_wChannel(1),
	 m_dwSample (WOSA_POOR),
	 m_wBit(16)
{
	m_hOut = 0;

	m_hAudioOut = 0;
	m_dwAudioOutId = 0;
	m_iBufferNum = 0;

	m_bThreadStart = FALSE;
	m_bDevOpen = FALSE;

	s_dwInstance ++;
  InitializeCriticalSection(&m_csLock);
}

CWaveOut::~CWaveOut()
{
  DeleteCriticalSection(&m_csLock);
}

BOOL CWaveOut::StartThread()
{
	if (m_bThreadStart)
	{
		TRACE("CWaveOut::StartThread: Wave out thread has run.\n");
		return FALSE;
	}

	m_hAudioOut=CreateThread(0,0,AudioOutThreadProc,this,0,&m_dwAudioOutId);
	if(!m_hAudioOut)
	{
		TRACE("CWaveOut::StartThread: Strat wave out thread fail.\n");
		return FALSE;
	}
	m_bThreadStart = TRUE;
	return TRUE;
}
BOOL CWaveOut::StopThread()
{
	if (!m_bThreadStart)
	{
		TRACE("CWaveOut::StopThread: Wave out thread hasn't run.\n");
		return FALSE;
	}

	if(m_hAudioOut)
	{
		int t=50;
		DWORD ExitCode;
		BOOL bEnd=FALSE;
		PostThreadMessage(m_dwAudioOutId,WM_QUIT,0,0);
		while(t)
		{
			GetExitCodeThread(m_hAudioOut,&ExitCode);
			if(ExitCode!= STILL_ACTIVE)
			{
				bEnd=TRUE;
				break;
			}
			else
				Sleep(10);
			t--;
		}
		if(!bEnd)
		{
			TerminateThread(m_hAudioOut,0);
			TRACE("CWaveOut::StopThread: TerminateThread wave out thread.\n");
		}
    CloseHandle(m_hAudioOut);
		m_hAudioOut=0;
	}
	m_bThreadStart = FALSE;
	return TRUE;
}

BOOL CWaveOut::OpenDev()
{
	if (m_bDevOpen)
	{
		TRACE("CWaveOut::OpenDev: Device has open.\n");
		return FALSE;
	}
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = m_dwSample;
	wfx.nBlockAlign = m_wBit * m_wChannel / 8;
	wfx.wBitsPerSample = m_wBit;
  wfx.nAvgBytesPerSec = wfx.nSamplesPerSec  * wfx.nBlockAlign;
	wfx.cbSize = 0;

	m_mmr=waveOutOpen (0,WAVE_MAPPER,&wfx,0,0,WAVE_FORMAT_QUERY);
	if(m_mmr)
	{
		TRACE("CWaveOut::OpenDev: waveOutOpen error.\n");
		return FALSE;
	}

	m_mmr=waveOutOpen(&m_hOut,WAVE_MAPPER,&wfx,m_dwAudioOutId,s_dwInstance,CALLBACK_THREAD);
	if(m_mmr)
	{
		TRACE("CWaveOut::OpenDev: waveOutOpen error.\n");
		return FALSE;
	}
	m_bDevOpen = TRUE;
	m_iBufferNum = 0;
	return TRUE;
}

BOOL CWaveOut::CloseDev()
{
	if (!m_bDevOpen)
	{
		TRACE("CWaveOut::CloseDev: Device hasn't opened.\n");
		return FALSE;
	}

	if(!m_hOut)
	{
		TRACE("CWaveOut::CloseDev: Device hasn't opened.\n");
		return FALSE;
	}

	m_mmr=waveOutClose(m_hOut);
	if(m_mmr)
	{
		TRACE("CWaveOut::CloseDev: waveOutClose error.\n");
		return FALSE;
	}
	m_hOut=0;
	m_bDevOpen = FALSE;
	return TRUE;
}

BOOL CWaveOut::StartPlay()
{
	BOOL bRet=FALSE;
	if (!StartThread())           
	{
		goto Exit;
	};
	if (!OpenDev())
	{
		goto Exit1;
	};
	bRet = TRUE;
	goto Exit;
Exit1:
	StopThread ();
Exit:
	return bRet;
}

BOOL CWaveOut::StopPlay()
{
	StopThread ();
  CloseDev();
	return TRUE;
}

MMRESULT CWaveOut::GetLastMMError()
{
	return m_mmr;
}

char* CWaveOut::GetLastErrorString()
{
	char buffer[256];
	memset(buffer,0,256);
	waveOutGetErrorText(m_mmr,buffer,256);
	return nullptr;
}


DWORD CWaveOut::GetInstance()
{
	return s_dwInstance;
}

WORD CWaveOut::GetBit()
{
	return m_wBit;
}

DWORD CWaveOut::GetSample()
{
	return m_dwSample;
}

WORD CWaveOut::GetChannel()
{
	return m_wChannel;
}

BOOL CWaveOut::Play(char* buf,UINT uSize)
{
	if (!m_bDevOpen)
	{
		TRACE("CWaveOut::Play: Device hasn't been open.\n");
		return FALSE;
	}
	if (GetBufferNum () > PLAY_DELAY)
	{
	//	TRACE("CWaveOut::Play: Delay too big pass over.\n");
		return TRUE;
	}
	char* p;
	LPWAVEHDR pwh=new WAVEHDR();
	if(!pwh)
	{
		TRACE("CWaveOut::Play: alloc WAVEHDR memoyr error.\n");
		return FALSE;
	}
	
	p=new char[uSize];
	if(!p)
	{
		TRACE("CWaveOut::Play: alloc data memoyr error.\n");
		return FALSE;
	}

	CopyMemory(p,buf,uSize);
	ZeroMemory(pwh,sizeof(WAVEHDR));
	pwh->dwBufferLength=uSize;
	pwh->lpData=p;
  pwh->dwFlags = 0;

	m_mmr=waveOutPrepareHeader(m_hOut,pwh,sizeof(WAVEHDR));
  	if (m_mmr)
	{
		TRACE("CWaveOut::CloseDev: waveOutPrepareHeader error.\n");
		return FALSE;
	}

	m_mmr=waveOutWrite(m_hOut,pwh,sizeof(WAVEHDR));
  	if (m_mmr)
	{
		TRACE("CWaveOut::CloseDev: waveOutWrite error.\n");
		return FALSE;
	}
	BufferAdd();
	return TRUE;
}

void CWaveOut::BufferAdd()
{
	EnterCriticalSection(&m_csLock);
	++m_iBufferNum;
  LeaveCriticalSection(&m_csLock);
}

void CWaveOut::BufferSub()
{
	EnterCriticalSection(&m_csLock);
	--m_iBufferNum;
  LeaveCriticalSection(&m_csLock);
}

int CWaveOut::GetBufferNum()
{
	//int iTemp;
	//EnterCriticalSection(&m_csLock);
	//iTemp = m_iBufferNum;
  //LeaveCriticalSection(&m_csLock);
	return m_iBufferNum;

}