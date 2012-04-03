#include "wave_output.h"
#pragma comment(lib,"Winmm")

#pragma warning(disable : 4035)
int32_t BitScanF(uint32_t i)
{
  DWORD result;
  _BitScanForward(&result,i);

  /*	_asm
	{
		xor		edx,edx
		bsf		eax,[i]
		setnz	dl
		dec		edx
		or		eax,edx
	}
*/
  return result;
}


WaveOutput::WaveOutput() {

	//Initialize variables --------------------
	memset(&wav.time,0,sizeof(MMTIME));		//Erase time structure
	wav.time.wType=TIME_SAMPLES;
	wav.samples=0;
	wav.ttime=0;
  device=WAVE_MAPPER;
	buffer.asize=0;								//No buffers have been allocated
	buffer.pIn=NULL;
	buffer.pOut=NULL;
	buffer.total_bytes=0;
	buffer.total_time=0;

	paused=0;									//Output isn't paused
	open=0;									//Device isn't open

	vol=255;									//Full volume
	pan=0;									//Center balance
}

WaveOutput::~WaveOutput() {

}

int WaveOutput::Initialize() {
  return S_OK;
}

int WaveOutput::Deinitialize() {
	if (open) Close();						//Close output device, incase someone forgot to
	SafeDeleteArray(&buffer.pOut);							//Free up allocated memory
	SafeDeleteArray(&buffer.pIn);
  return S_OK;
}

int WaveOutput::Open(int32_t rate, uint32_t chn, int32_t bits, int32_t bufferlenms, int32_t prebufferms)
{
	//int8_t	str[256];
	int32_t	error,i;


	if (chn>2) return -1;						//No more than stereo

	if (open) Close();						//Verify device was closed

	//if (cfg.changed)	{
		blk.num=4;//cfg.numBlk;
		blk.min=2*1024;//cfg.minBlk*1024;
		blk.max=64*1024;//cfg.maxBlk*1024;
    SafeDeleteArray(&buffer.pOut);
    buffer.pOut= new uint8_t[(blk.num * blk.max)];
  //}

	//Build format structure -------------------
	wav.fmt.Format.wFormatTag				= WAVE_FORMAT_EXTENSIBLE;
	wav.fmt.Format.nChannels				= chn;
	wav.fmt.Format.nSamplesPerSec			= rate;
	wav.fmt.Format.wBitsPerSample			= (bits+7)&~7;		//Round bits per sample up to nearest byte
	wav.fmt.Format.nBlockAlign			= chn * wav.fmt.Format.wBitsPerSample>>3;
	wav.fmt.Format.nAvgBytesPerSec		= wav.fmt.Format.nBlockAlign * wav.fmt.Format.nSamplesPerSec;
	wav.fmt.Format.cbSize					= 22;
	wav.fmt.Samples.wValidBitsPerSample	= bits;
	wav.fmt.dwChannelMask					= chn==2 ? 3 : 4;	//Select left & right (stereo) or center (mono)
	wav.fmt.SubFormat						= KSDATAFORMAT_SUBTYPE_PCM;

  int cfbufferLen = 2000;
	//Check size of ring buffer ----------------
	buffer.size=(wav.fmt.Format.nSamplesPerSec *
				wav.fmt.Format.nBlockAlign * 
				cfbufferLen) / 1000;

	if (buffer.asize < buffer.size)				//If allocated size is less than needed size
	{
		//buffer.pIn=realloc(buffer.pIn,buffer.size);
    SafeDeleteArray(&buffer.pIn);
    buffer.pIn = new uint8_t[buffer.size];
		buffer.asize=buffer.size;
	}

	//Open audio device ------------------------
  int cfg_direct = 0;//WAVE_FORMAT_DIRECT
	error=waveOutOpen(&wav.handle,device,(tWAVEFORMATEX*)&wav.fmt,DWORD_PTR(WaveOutDone),DWORD_PTR(this),CALLBACK_FUNCTION|WAVE_ALLOWSYNC|cfg_direct);
	switch (error)
	{
	case(MMSYSERR_NOERROR):
		wav.idle=(uint32_t)~0>>(32-blk.num);		//All blocks are free

		//Initialize wave blocks ---------------
		for (i=0;i<blk.num;i++)
		{
			memset(&wav.hdr[i],0,sizeof(WAVEHDR));

			wav.hdr[i].lpData=(LPSTR)buffer.pOut+(blk.max*i);
			wav.hdr[i].dwBufferLength=blk.max;
			waveOutPrepareHeader(wav.handle,&wav.hdr[i],sizeof(WAVEHDR));

			wav.hdr[i].dwBufferLength=0;
			wav.hdr[i].dwUser=(DWORD_PTR)(1<<i);			//Bit corresponding to block
			wav.hdr[i].dwFlags|=WHDR_DONE;
		}

		memset(&wav.time,0,sizeof(MMTIME));
		wav.time.wType=TIME_SAMPLES;
		wav.samples=0;
		wav.smpCnt=0;

		buffer.bytes=0;							//No bytes have been written

		buffer.length=0;							//Nothing is in input buffer
		buffer.queued_length=0;
		buffer.write=0;
		buffer.read=0;

		blk.cnt=0;
		blk.size=0;

		paused=0;								//Output isn't paused
		open=1;								//Device is open

		return ((buffer.size / wav.fmt.Format.nBlockAlign) * 1000) / wav.fmt.Format.nSamplesPerSec;

	case(MMSYSERR_ALLOCATED):
		//sprintf(str,"%s is already in use",wav.cap.szPname);
		//MessageBox(outMod.hMainWindow,str,"WaveOut WDM",MB_ICONERROR|MB_OK);
		break;

	case(MMSYSERR_BADDEVICEID):
		//sprintf(str,"Invalid output device ID [%i].  Check your configuration.",cfg.device);
		//MessageBox(outMod.hMainWindow,str,"WaveOut WDM",MB_ICONERROR|MB_OK);
		break;

	case(MMSYSERR_NODRIVER):
		//sprintf(str,"No driver is loaded for %s",wav.cap.szPname);
		//MessageBox(outMod.hMainWindow,str,"WaveOut WDM",MB_ICONERROR|MB_OK);
		break;

	case(MMSYSERR_NOMEM):
		//MessageBox(outMod.hMainWindow,"Unable to allocate memory for output device","WaveOut WDM",MB_ICONERROR|MB_OK);
		break;

	case(WAVERR_BADFORMAT):
		//MessageBox(outMod.hMainWindow,"The extensible format is not supported by your driver.  Get the WDM drivers for your soundcard.","WaveOut WDM",MB_ICONERROR|MB_OK);
		break;

	default:
		//sprintf(str,"Unknown error [%i] trying to open output device %s",error,wav.cap.szPname);
		//MessageBox(outMod.hMainWindow,str,"WaveOut WDM",MB_ICONERROR|MB_OK);
    break;
	}

	return S_FALSE;
}

int WaveOutput::Close() {
	int32_t	i;
  
	wav.ttime+=(int32_t)(wav.samples * 1000.0 / (int32_t)wav.fmt.Format.nSamplesPerSec);
	buffer.total_time+=(int32_t)(buffer.bytes * 1000.0 / (int32_t)wav.fmt.Format.nAvgBytesPerSec);

	open=0;									//Device will no longer be open
	wav.idle=(uint32_t)~0>>(32-blk.num);			//All blocks are free

	waveOutReset(wav.handle);					//Tell Windows to reset the output device

	for (i=0;i<blk.num;i++)					//Unlock memory used by blocks
		waveOutUnprepareHeader(wav.handle,&wav.hdr[i],sizeof(WAVEHDR));

	waveOutClose(wav.handle);					//Close output device

	//if (g_hCfg) WriteStats();
  return S_OK;
}

int WaveOutput::Flush(int32_t t) {
	buffer.length=0;								//Set size to 0 so reset won't start new buffers
	buffer.write=0;
	buffer.read=0;
	buffer.bytes=0;
	wav.samples=0;
	wav.smpCnt=0;

	waveOutReset(wav.handle);					//Reset output device
  return S_OK;
}

int32_t WaveOutput::Write(int8_t *pIn, int32_t len)
{
	uint32_t		copy;


	if (!open || len>buffer.size-(buffer.length-buffer.queued_length))
		return S_FALSE;								//If device isn't open, or buffer is full

	//Copy data to input buffer ----------------
	if (len<=buffer.size-buffer.write)			//Will data fit without wrapping around?
	{
		memcpy((void*)((uintptr_t)buffer.pIn+buffer.write),pIn,len);
		buffer.write+=len;
	}
	else
	{
		copy=buffer.size-buffer.write;
		memcpy((void*)((uintptr_t)buffer.pIn+buffer.write),pIn,copy);
		buffer.write=len-copy;
		memcpy(buffer.pIn,pIn+copy,buffer.write);
	}
	buffer.length+=len;
	buffer.bytes+=(uint32_t)len;						//Casting as a uint32_t generates less code
	buffer.total_bytes+=(uint32_t)len;

	if (wav.idle && buffer.length>=blk.min)		//If any blocks are empty, and there's enough data,
	{											// add a block to the queue
		WaveOutDone(wav.handle,WOM_DONE,(DWORD_PTR)this,&wav.hdr[BitScanF(wav.idle)],0);
	}

	return S_OK;
}
 
void CALLBACK WaveOutput::WaveOutDone(HWAVEOUT hwo, uint32_t uMsg, DWORD_PTR dwInstance, WAVEHDR *pWHdr, DWORD_PTR dwParam2) {
	int32_t	copy,end;

  WaveOutput* self = reinterpret_cast<WaveOutput*>(dwInstance);

	if (uMsg==WOM_DONE && self->open)	{			//If a wave block is through playing, and the output
												// device is open
		copy=self->buffer.length;							//Get the current length of data in the input buffer
		if (copy>=self->blk.min)					//Is there enough data to fill a block?
		{
			//Calculate copy size --------------
			if (copy>self->blk.max) copy=self->blk.max;	//Limit copy size to max block size
												//Remove any partial samples (needed for 24-bit output)
			copy=(copy/self->wav.fmt.Format.nBlockAlign)*self->wav.fmt.Format.nBlockAlign;

			//Copy samples to wave block -------
			if (copy<=self->buffer.size-self->buffer.read)	//Does copy size wrap around input buffer?
			{									//Straight linear copy
				memcpy(pWHdr->lpData,(void*)((uintptr_t)self->buffer.pIn+self->buffer.read),copy);
				self->buffer.read+=copy;				//Move read index forward
			}
			else
			{
				end=self->buffer.size-self->buffer.read;		//Bytes to end of input buffer
				memcpy(pWHdr->lpData,(void*)((uintptr_t)self->buffer.pIn+self->buffer.read),end);
				self->buffer.read=copy-end;			//Bytes left in buffer
				memcpy((void*)((uintptr_t)pWHdr->lpData+end),self->buffer.pIn,self->buffer.read);
			}

			//Send block to driver -------------
			self->buffer.queued_length-=pWHdr->dwBufferLength;	//Update stats
			self->buffer.queued_length+=copy;
			self->blk.cnt++;
			self->blk.size+=copy;

			self->buffer.length-=copy;					//Update input buffer length
			pWHdr->dwBufferLength=copy;			//Set data length in block
			waveOutWrite(self->wav.handle,pWHdr,sizeof(WAVEHDR));

			self->wav.idle&=~pWHdr->dwUser;			//This block is in use
		}
		else
			self->wav.idle|=pWHdr->dwUser;			//Mark the corresponding bit that this block is idle
	}
}