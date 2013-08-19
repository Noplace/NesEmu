/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#include "directsound.h"

#pragma comment(lib, "dsound.lib")

namespace audio {
namespace output {

DirectSound::DirectSound() : last_write_cursor(0),last_cursor_pos(0) {
  window_handle_ = nullptr;
  buffer_size_ = 0;
}

DirectSound::~DirectSound() {
  Deinitialize();
}
  
int DirectSound::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {
  last_write_cursor=0xffffffff;
  HRESULT hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback,&ds8,nullptr);
  if (FAILED(hr)) {
    Deinitialize();
    return S_FALSE;
  }
  hr = ds8->SetCooperativeLevel((HWND)window_handle_,DSSCL_PRIORITY);
  if (FAILED(hr)) {
    hr = ds8->SetCooperativeLevel((HWND)window_handle_,DSSCL_NORMAL);  
    if (FAILED(hr)) {
      return S_FALSE;
    }
  }

  DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd,sizeof(DSBUFFERDESC));
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;


	hr = ds8->CreateSoundBuffer(&dsbd,&primary_buffer,nullptr) ;
  if (FAILED(hr)) return S_FALSE;

	ZeroMemory( &wave_format_, sizeof(WAVEFORMATEX) ); 
	wave_format_.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wave_format_.nChannels       =  channels; 
	wave_format_.nSamplesPerSec  = sample_rate; 
	wave_format_.wBitsPerSample  = (WORD) bits; 
	wave_format_.nBlockAlign     = (WORD) ((wave_format_.wBitsPerSample >> 3) * wave_format_.nChannels);
	wave_format_.nAvgBytesPerSec = (DWORD) (wave_format_.nSamplesPerSec * wave_format_.nBlockAlign);
  wave_format_.cbSize = 0;
	hr = primary_buffer->SetFormat(&wave_format_);
  if (FAILED(hr)) return S_FALSE;


  if (buffer_size_ == 0)
    buffer_size_ = uint32_t(wave_format_.nBlockAlign * wave_format_.nSamplesPerSec * 0.4); //400ms


	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwBufferBytes = buffer_size_;
	dsbd.lpwfxFormat   = &wave_format_;
  // Create a temporary sound buffer with the specific buffer settings.
	hr = ds8->CreateSoundBuffer(&dsbd, &secondary_buffer, NULL);
  if (FAILED(hr)) return S_FALSE;

    //hr = secondary_buffer->SetVolume(DSBVOLUME_MAX);
  //if (FAILED(hr)) return S_FALSE;

  return Play();
}

int DirectSound::Deinitialize() {
  Stop();
  SafeRelease(&secondary_buffer);
  SafeRelease(&primary_buffer);
  SafeRelease(&ds8);
  return S_OK;
}

int DirectSound::Play() {
  auto hr = primary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;
  hr = secondary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;
  hr = secondary_buffer->SetCurrentPosition(0);
  return hr;
}

int DirectSound::Stop() {
  if (secondary_buffer != nullptr) {
    auto hr = secondary_buffer->Stop();
    if (FAILED(hr)) return S_FALSE;
  }
  if (primary_buffer != nullptr) {
    auto hr = primary_buffer->Stop();
    if (FAILED(hr)) return S_FALSE;
  }
  return S_OK;
}

uint32_t DirectSound::GetBytesBuffered() {
 unsigned long cplay,cwrite;

 if(last_write_cursor==0xffffffff) return 0;

 secondary_buffer->GetCurrentPosition(&cplay,&cwrite);

 if(cplay>buffer_size_) return buffer_size_;

 if(cplay<last_write_cursor) return last_write_cursor-cplay;
 return (buffer_size_-cplay)+last_write_cursor;
}

void DirectSound::GetCursors(uint32_t& play, uint32_t& write) {
  secondary_buffer->GetCurrentPosition((LPDWORD)&play,(LPDWORD)&write);
}


int DirectSound::Write(void* data_pointer, uint32_t size_bytes) {
  LPVOID buf_ptr1, buf_ptr2;
  HRESULT hr;
  DWORD play_cursor,write_cursor;
  DWORD buf_size1,buf_size2; 

  for (;;) {
    hr = secondary_buffer->GetCurrentPosition(&play_cursor,&write_cursor);
    if (hr == S_OK)
    {
      hr = secondary_buffer->Lock(write_cursor,size_bytes,&buf_ptr1,&buf_size1,&buf_ptr2,&buf_size2,0);
    }

    if (hr == S_OK)
      break;
    else if (hr == DSERR_BUFFERLOST)
      secondary_buffer->Restore();
    else
      return S_FALSE;
  }  

  auto dest_buf=(uint8_t*)buf_ptr1;
  auto len=buf_size1;

  auto src_buf=(uint8_t*)data_pointer;
  while(len) {*dest_buf++=*src_buf++;len--;}

  if(buf_ptr2)  {
    dest_buf=(uint8_t*)buf_ptr2;
    len=buf_size2;
    while(len) {*dest_buf++=*src_buf++;len--;}
  }

  secondary_buffer->Unlock(buf_ptr1,buf_size1,buf_ptr2,buf_size2);
  return S_OK;
}

/*
int DirectSound::Write(void* data_pointer, uint32_t size_bytes) {
  HRESULT hr;
  DWORD buffer_status;
  hr = secondary_buffer->GetStatus(&buffer_status);    
  if (FAILED(hr)) return S_FALSE;
  if( buffer_status & DSBSTATUS_BUFFERLOST ) {
    #ifdef _DEBUG
    OutputDebugString("DirectSound::Write : buffer lost\n");
    #endif
    hr = secondary_buffer->Restore();
    if (hr != DS_OK) return S_FALSE;
    secondary_buffer->Play(0,0,DSBPLAY_LOOPING);
  }
  DWORD play_cursor,write_cursor;
  secondary_buffer->GetCurrentPosition(&play_cursor,&write_cursor);
  
  if(last_write_cursor==0xffffffff) 
    last_write_cursor=write_cursor;

  LPVOID buf_ptr1, buf_ptr2;
  DWORD buf_size1,buf_size2; 
  hr = secondary_buffer->Lock(write_cursor,size_bytes,&buf_ptr1,&buf_size1,&buf_ptr2,&buf_size2,0);
  if (hr!=DS_OK) { 
    last_write_cursor=0xffffffff;
    return S_FALSE; 
  }

  auto lpSD=(uint8_t*)buf_ptr1;
  auto dw=buf_size1;

  auto lpSS=(uint8_t*)data_pointer;
  while(dw) {*lpSD++=*lpSS++;dw--;}

  if(buf_ptr2)  {
    lpSD=(uint8_t*)buf_ptr2;
    dw=buf_size2;
    while(dw) {*lpSD++=*lpSS++;dw--;}
  }

  secondary_buffer->Unlock(buf_ptr1,buf_size1,buf_ptr2,buf_size2);

  /*last_write_cursor += size_bytes;
  if (last_write_cursor >= buffer_size_) 
    last_write_cursor -= buffer_size_;
  last_write_cursor = play_cursor;*/
/*
  return S_OK;
}*/


int DirectSound::BeginWrite(uint32_t& samples) {
   DWORD curpos;
  int32_t nwrite = 0;
  for (;;) {
    HRESULT hr = secondary_buffer->GetCurrentPosition(&curpos, 0);
    if (hr == S_OK)
    {
      // find out how many bytes to write
      curpos &= ~31u;
      if (curpos == last_cursor_pos)
        return S_FALSE;

      nwrite = curpos - last_cursor_pos;
      if (nwrite < 0)
        nwrite += buffer_size_;

      hr = secondary_buffer->Lock(last_cursor_pos, nwrite, &buf1, &len1, &buf2, &len2, 0);
    }

    if (hr == S_OK)
      break;
    else if (hr == DSERR_BUFFERLOST)
      secondary_buffer->Restore();
    else
      return S_FALSE;
  }

  // we got the lock
  last_cursor_pos = curpos;
  //g_dsound.bufcnt += nwrite;
  samples = nwrite / 4;
  return S_OK;
}


static void clamp(void *dest, const float *src, int32_t nsamples)
{
  int16_t *dests = (int16_t *)dest;

  for (int32_t i=0; i < nsamples; i++)
  {
    float v = src[i] * 32768.0f;
    if (v >  32600.0f) v =  32600.0f;
    if (v < -32600.0f) v = -32600.0f;
    dests[i] = (int16_t)v;
  }
}


int DirectSound::EndWrite(void* data_pointer) {

  /*auto dest_buf=(uint8_t*)buf1;
  auto dw=len1;

  auto src_buf=(float*)data_pointer;
  while (dw) { 
    float v = *src_buf++;
    v *= 32768.0f; // * vol;
    if (v >  32600.0f) v =  32600.0f;
    if (v < -32600.0f) v = -32600.0f;
    *dest_buf++ = (short)v;
    dw--;
  }

  if(buf2)  {
    dest_buf=(uint8_t*)buf2;
    dw=len2;
    while(dw) {
      float v = *src_buf++;
      v *= 32768.0f; // * vol;
      if (v >  32600.0f) v =  32600.0f;
      if (v < -32600.0f) v = -32600.0f;
      *dest_buf++ = (short)v;
    dw--;
    }
  }*/

    if (buf1)
      clamp(buf1, (float*)data_pointer, len1/2);
    if (buf2)
      clamp(buf2, (float*)data_pointer + len1/2, len2/2);

  if (secondary_buffer->Unlock(buf1, len1, buf2, len2)==DS_OK)
    return S_OK;
  else
    return S_FALSE;
}


}
}