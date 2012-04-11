#include <dsound.h>
#include "audio.h"

#define BUFFER_SIZE 512*20
#define AUDIO_BUFFER_SIZE 512*2


BOOL RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB )	{
		HRESULT hr;
    BOOL pbWasRestored=false;
	
		DWORD dwStatus;
		pDSB->GetStatus( &dwStatus );    

		if( dwStatus & DSBSTATUS_BUFFERLOST )
		{
			do 
			{
				hr = pDSB->Restore();
				if( hr == DSERR_BUFFERLOST )
					Sleep( 10 );
			}
			while( ( pDSB->Restore() ) == DSERR_BUFFERLOST );

			pbWasRestored = true;
		}
    return pbWasRestored;
}


BOOL AppWriteDataToBuffer( 
        LPDIRECTSOUNDBUFFER lpDsb,  // The buffer.
        DWORD dwOffset,              // Our own write cursor.
        LPBYTE lpbSoundData,         // Start of our data.
        DWORD dwSoundBytes)          // Size of block to copy.
    { 
      LPVOID  lpvPtr1; 
      DWORD dwBytes1; 
      LPVOID  lpvPtr2; 
      DWORD dwBytes2; 
      HRESULT hr; 
     
      // Obtain memory address of write block. This will be in two parts
      // if the block wraps around.
     
      hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, 
          &dwBytes1, &lpvPtr2, &dwBytes2, 0); 
     
      // If the buffer was lost, restore and retry lock. 
      if (hr == DSERR_PRIOLEVELNEEDED) {
        int a= 1;
      }
      if (DSERR_BUFFERLOST == hr) 
      { 
        lpDsb->Restore(); 
        hr = lpDsb->Lock(dwOffset, dwSoundBytes, 
            &lpvPtr1, &dwBytes1,
            &lpvPtr2, &dwBytes2, 0); 
      } 
      if (SUCCEEDED(hr)) 
      { 
        // Write to pointers. 
     
        CopyMemory(lpvPtr1, lpbSoundData, dwBytes1); 
        if (NULL != lpvPtr2) 
        { 
          CopyMemory(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2); 
        } 
     
        // Release the data back to DirectSound. 
     
        hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, 
        dwBytes2); 
        if (SUCCEEDED(hr)) 
        { 
          // Success. 
          return TRUE; 
        } 
      } 
     
      // Lock, Unlock, or Restore failed. 
     
      return FALSE; 
    } 

namespace audio {

struct HiddenMembers{
  IDirectSound8* ds8;
  LPDIRECTSOUNDBUFFER primary_buffer;
  LPDIRECTSOUNDBUFFER secondary_buffer;
};

DirectSound::DirectSound() : window_handle_(nullptr), offset_(0) {
  encapsulated = new HiddenMembers();
}

DirectSound::~DirectSound() {
  delete encapsulated;
}
  
int DirectSound::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {
  audio_data = new BYTE[AUDIO_BUFFER_SIZE];
  HRESULT hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback,&encapsulated->ds8,nullptr);
  if (FAILED(hr)) return S_FALSE;
  hr = encapsulated->ds8->SetCooperativeLevel((HWND)window_handle_,DSSCL_PRIORITY);
  if (FAILED(hr)) return S_FALSE;
  DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       =   DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;


	hr = encapsulated->ds8->CreateSoundBuffer( &dsbd,&encapsulated->primary_buffer, NULL ) ;
  if (FAILED(hr)) return S_FALSE;

	ZeroMemory( &wave_format_, sizeof(WAVEFORMATEX) ); 
	wave_format_.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wave_format_.nChannels       =  channels; 
	wave_format_.nSamplesPerSec  = sample_rate; 
	wave_format_.wBitsPerSample  = (WORD) bits; 
	wave_format_.nBlockAlign     = (WORD) (wave_format_.wBitsPerSample / 8 * wave_format_.nChannels);
	wave_format_.nAvgBytesPerSec = (DWORD) (wave_format_.nSamplesPerSec * wave_format_.nBlockAlign);
  wave_format_.cbSize = 0;
	hr = encapsulated->primary_buffer->SetFormat(&wave_format_);
  if (FAILED(hr)) return S_FALSE;
  hr = encapsulated->primary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;

	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       =   DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2;;
	dsbd.dwBufferBytes = BUFFER_SIZE;//(wfx.nAvgBytesPerSec / 50)*4;
	dsbd.lpwfxFormat   = &wave_format_;
  // Create a temporary sound buffer with the specific buffer settings.
	hr = encapsulated->ds8->CreateSoundBuffer(&dsbd, &encapsulated->secondary_buffer, NULL);
  if (FAILED(hr)) return S_FALSE;

  buffer_size = dsbd.dwBufferBytes;
  written_bytes = 0;
  hr = encapsulated->secondary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;
  hr = encapsulated->primary_buffer->SetVolume(DSBVOLUME_MAX);
  if (FAILED(hr)) return S_FALSE;
  return S_OK;
}

int DirectSound::Deinitialize() {
  encapsulated->secondary_buffer->Stop();
  encapsulated->primary_buffer->Stop();
  SafeRelease(&encapsulated->secondary_buffer);
  SafeRelease(&encapsulated->primary_buffer);
  SafeRelease(&encapsulated->ds8);
  delete [] audio_data;
  return S_OK;
}

int DirectSound::Write(void* data, size_t size) {
  //BYTE* buffer_data;
 // DWORD locked_size;
  RestoreBuffer(encapsulated->secondary_buffer);
  if (written_bytes + size > AUDIO_BUFFER_SIZE) {
    //do something
  }

  memcpy(&audio_data[written_bytes],data,size);
  written_bytes += size;
  if (written_bytes >= AUDIO_BUFFER_SIZE) {
    written_bytes = 0;

    if (AppWriteDataToBuffer(this->encapsulated->secondary_buffer,offset_,audio_data,AUDIO_BUFFER_SIZE)==TRUE)
      offset_ += AUDIO_BUFFER_SIZE;

		/*secondary_buffer->Lock(offset, AUDIO_BUFFER_SIZE, (LPVOID*)&buffer_data, &locked_size, NULL, 0, 0);
		memcpy(buffer_data, audio_data, locked_size);
		secondary_buffer->Unlock(buffer_data, locked_size, NULL, 0);*/
    //offset += AUDIO_BUFFER_SIZE;
    if (offset_ == BUFFER_SIZE)
      offset_ = 0;
  }


  
  /*
  
  primary_buffer->Lock(written_bytes,bytes, (LPVOID*)&buffer_data, &locked_size, NULL, NULL, 0L); 
  memcpy(buffer_data,data,bytes);
  primary_buffer->Unlock(buffer_data,locked_size,null,null);
  written_bytes += bytes;*/


  //if (AppWriteDataToBuffer(secondary_buffer,written_bytes,(LPBYTE)data,bytes)==TRUE) 
  //  written_bytes += bytes;

 

  return S_OK;
}

}