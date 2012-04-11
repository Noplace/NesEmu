#include <Windows.h>
#include <WinCore/types.h>
#include <objbase.h>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#pragma warning(pop)


const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

IMMDeviceEnumerator* device_enumurator = NULL;
IMMDevice *device = NULL;
IAudioClient* audio_client = NULL;
IAudioRenderClient* render_client = NULL;
WAVEFORMATEX *pwfx = NULL;
UINT32 bufferFrameCount;
UINT32 numFramesAvailable;
UINT32 numFramesPadding;

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000
REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_MILLISEC*200;
REFERENCE_TIME hnsActualDuration;
UINT32 written_samples;
WAVEFORMATEXTENSIBLE fmt;

BYTE* audio_block;

int WASAPI_GetSampleRate() {
  return pwfx->nSamplesPerSec;
}

int WASAPI_Initialize(int rate,int channels,int bits) {
  
  //WAVEFORMATEX* closest_fmt = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
 // WAVEFORMATEX** pclosest_fmt = &closest_fmt;
  fmt.Format.wFormatTag			= WAVE_FORMAT_EXTENSIBLE;
  fmt.Format.nChannels				= channels;
  fmt.Format.nSamplesPerSec	= rate;
  fmt.Format.wBitsPerSample	= (bits+7)&~7;
  fmt.Format.nBlockAlign			= channels * fmt.Format.wBitsPerSample>>3;
  fmt.Format.nAvgBytesPerSec	= fmt.Format.nBlockAlign * fmt.Format.nSamplesPerSec;
  fmt.Format.cbSize					= 22;
  fmt.Samples.wValidBitsPerSample	= bits;
	fmt.dwChannelMask					= channels==2 ? 3 : 4;	//Select left & right (stereo) or center (mono)
	fmt.SubFormat						= KSDATAFORMAT_SUBTYPE_PCM;
  //pwfx = &fmt.Format;
  HRESULT hr;
  hr = CoCreateInstance(CLSID_MMDeviceEnumerator,NULL,CLSCTX_ALL,IID_IMMDeviceEnumerator,(void**)&device_enumurator);
  if (FAILED(hr)) return S_FALSE;
  hr = device_enumurator->GetDefaultAudioEndpoint(eRender,eConsole,&device);
  if (FAILED(hr)) return S_FALSE;
  hr = device->Activate(IID_IAudioClient, CLSCTX_ALL,NULL, (void**)&audio_client);
  if (FAILED(hr)) return S_FALSE;
  hr = audio_client->GetMixFormat(&pwfx);
  //if (FAILED(hr)) return S_FALSE;
  //hr = audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,(WAVEFORMATEX*)&fmt,pclosest_fmt);
  //hr = audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,(WAVEFORMATEX*)&fmt,NULL);
  //CoTaskMemFree(closest_fmt);
  hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED,0,hnsRequestedDuration,0,pwfx,NULL);
  if (FAILED(hr)) return S_FALSE;
  //hr = pMySource->SetFormat(pwfx);
  //if (FAILED(hr)) return S_FALSE;

  // Get the actual size of the allocated buffer.
  hr = audio_client->GetBufferSize(&bufferFrameCount);
  if (FAILED(hr)) return S_FALSE;

  hr = audio_client->GetService(IID_IAudioRenderClient,(void**)&render_client);
  if (FAILED(hr)) return S_FALSE;

  hnsActualDuration = REFERENCE_TIME((double)REFTIMES_PER_SEC *
                        bufferFrameCount / fmt.Format.nSamplesPerSec);

  written_samples = 0;

  audio_block = new BYTE[bufferFrameCount*pwfx->nBlockAlign];

  return S_OK;
}

int WASAPI_Deinitialize() {
  CoTaskMemFree(pwfx);
  SafeRelease(&device_enumurator);
  SafeRelease(&device);
  SafeRelease(&audio_client);
  SafeRelease(&render_client);
  delete [] audio_block;
  return S_OK;
}


int WASAPI_WriteData(void* data,size_t sample_count) {


  
  memcpy(&audio_block[written_samples*(fmt.Format.wBitsPerSample>>3)],data,sample_count*(fmt.Format.wBitsPerSample>>3));
  //audio_block += sample_count*fmt.Format.nBlockAlign;


  written_samples += sample_count;

  if (written_samples >= bufferFrameCount) {
    BYTE *pData;
    render_client->GetBuffer(sample_count, &pData);
    if (pData)
    memcpy(pData,audio_block,bufferFrameCount*(fmt.Format.wBitsPerSample>>3));
    render_client->ReleaseBuffer(sample_count, 0);
  
    written_samples = 0;
    audio_client->Start();
  }
  return S_OK;
}