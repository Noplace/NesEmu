
#include "nes.h"
//#include "../../sound/wave_output.h"
//#include "../../sound/wasapi.h"
//#include "../../audio/audio.h"
#include "../../audio2/directsound.h"
//#include "../../sound/WaveOut.h"

float pal_mul[7][3] = {
  {  0.915f, 1.239f, 0.743f },
  {  1.086f, 0.794f, 0.882f },
  {  0.980f, 1.019f, 0.653f },
  {  1.026f, 0.905f, 1.277f },
  {  0.908f, 1.023f, 0.979f },
  {  0.987f, 0.741f, 1.001f },
  {  0.750f, 0.750f, 0.750f }
};

uint32_t palette1[64] = {
0x808080, 0x003DA6, 0x0012B0, 0x440096,
0xA1005E, 0xC70028, 0xBA0600, 0x8C1700,
0x5C2F00, 0x104500, 0x054A00, 0x00472E,
0x004166, 0x000000, 0x050505, 0x050505,
0xC7C7C7, 0x0077FF, 0x2155FF, 0x8237FA,
0xEB2FB5, 0xFF2950, 0xFF2200, 0xD63200,
0xC46200, 0x358000, 0x058F00, 0x008A55,
0x0099CC, 0x212121, 0x090909, 0x090909,
0xFFFFFF, 0x0FD7FF, 0x69A2FF, 0xD480FF,
0xFF45F3, 0xFF618B, 0xFF8833, 0xFF9C12,
0xFABC20, 0x9FE30E, 0x2BF035, 0x0CF0A4,
0x05FBFF, 0x5E5E5E, 0x0D0D0D, 0x0D0D0D,
0xFFFFFF, 0xA6FCFF, 0xB3ECFF, 0xDAABEB,
0xFFA8F9, 0xFFABB3, 0xFFD2B0, 0xFFEFA6,
0xFFF79C, 0xD7E895, 0xA6EDAF, 0xA2F2DA,
0x99FFFC, 0xDDDDDD, 0x111111, 0x111111
};

/*RGBQUAD palette2[64] = {
{ 117, 117, 117   }, 
{ 39, 27, 143  },
{ 0, 0, 171  },
{ 71, 0, 159 },
{ 143, 0, 119   },
{ 171, 0, 19 },
{ 167, 0, 0   },
{ 127, 11, 0 },
{ 67, 47, 0  },
{ 0, 71, 0   },
{ 0, 81, 0   },
{ 0, 63, 23    },
{ 27, 63, 95   },
{ 0, 0, 0  },
{ 0, 0, 0  },
{ 0, 0, 0  },
{ 188, 188, 188   },
{ 0, 115, 239  },
{ 35, 59, 239  },
{ 131, 0, 243   },
{ 191, 0, 191   },
{ 231, 0, 91 },
{ 219, 43, 0 },
{ 203, 79, 15  },
{ 139, 115, 0   },
{ 0, 151, 0  },
{ 0, 171, 0  },
{ 0, 147, 59   },
{ 0, 131, 139  },
{ 0, 0, 0  },
{ 0, 0, 0  },
{ 0, 0, 0  },
{ 255, 255, 255   },
{ 63, 191, 255 },
{ 95, 151, 255 },
{ 167, 139, 253   },
{ 247, 123, 255   },
{ 255, 119, 183   },
{ 255, 119, 99 },
{ 255, 155, 59 },
{ 243, 191, 63 },
{ 131, 211, 19 },
{ 79, 223, 75  },
{ 88, 248, 152 },
{ 0, 235, 219  },
{ 0, 0, 0    },
{ 0, 0, 0    },
{ 0, 0, 0    },
{ 255, 255, 255   },
{ 171, 231, 255   },
{ 199, 215, 255   },
{ 215, 203, 255   },
{ 255, 199, 255   },
{ 255, 199, 219   },
{ 255, 191, 179   },
{ 255, 219, 171   },
{ 255, 231, 163   },
{ 227, 255, 163   },
{ 171, 243, 191   },
{ 179, 255, 207   },
{ 159, 255, 243   },
{ 0, 0, 0   },
{ 0, 0, 0   },
{ 0, 0, 0   }
};*/



namespace IO
{

  HWND window_handle;

  Joystick joy1,joy2;
  //HWAVEOUT waveout;
  //CWaveOut wave;
  //WaveOutput wave;
  //audio::DirectSound ds_audio;
  audio::output::DirectSound audio_;

  void Init() {
    memset(&joy1,0,sizeof(joy1));
    memset(&joy2,0,sizeof(joy2));
      //signal(SIGINT, SIG_DFL);
    /*WAVEFORMATEX wfx;
    wfx.nSamplesPerSec = 48000; /* sample rate */
    //wfx.wBitsPerSample = 16; /* sample size */
    //wfx.nChannels = 2; /* channels*/
    //wfx.cbSize = 0; /* size of _extra_ info */
    /*wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    waveOutOpen(&waveout,WAVE_MAPPER,&wfx,0,0,CALLBACK_NULL);
    */
    /*wave.SetBit(16);
    wave.SetChannel(2);
    wave.SetSample(44100);
    wave.StartPlay();*/
    //wave.Initialize();
    //wave.Open(44100,2,16,0,0);
    //WASAPI_Initialize(44100,2,16);
    //ds_audio.set_window_handle(window_handle);
    //ds_audio.Initialize(44100,2,16);
    audio_.set_window_handle(window_handle);
    audio_.Initialize(44100,2,16);
  }

  void Deinit() {
    audio_.Deinitialize();
    //waveOutClose(waveout);
    //wave.StopPlay();
    //wave.Close();
    //wave.Deinitialize();
    //WASAPI_Deinitialize();
    //ds_audio.Deinitialize();
  }


  void writeAudioBlock(void* block, uint32_t size) {
    /*WAVEHDR header;
    ZeroMemory(&header, sizeof(WAVEHDR));
    header.dwBufferLength = size;
    header.lpData = (LPSTR)block;
    waveOutPrepareHeader(waveout, &header, sizeof(WAVEHDR));
    waveOutWrite(waveout, &header, sizeof(WAVEHDR));

    while(waveOutUnprepareHeader(waveout,&header,sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)
      Sleep(10);*/
    //wave.Play((char*)block,size);
    //wave.Write((int8_t*)block,size);
    //WASAPI_WriteData(block,size);
    //ds_audio.Write(block,size);
    audio_.Write(block,size);
  }

    unsigned MakeRGBcolor(unsigned pixel) 
    { 
        // The input value is a NES color index (with de-emphasis bits). 
        // We need RGB values. Convert the index into RGB. 
        // For most part, this process is described at: 
        //    http://wiki.nesdev.com/w/index.php/NTSC_video 

        // Decode the color index 
        int color = (pixel & 0x0F), level = color<0xE ? (pixel>>4) & 3 : 1; 

        // Voltage levels, relative to synch voltage 
        static const double black=.518, white=1.962, attenuation=.746, 
          levels[8] = {.350, .518, .962,1.550,  // Signal low 
                      1.094,1.506,1.962,1.962}; // Signal high 

        double lo_and_hi[2] = { levels[level + 4 * (color == 0x0)], 
                               levels[level + 4 * (color <  0xD)] }; 

        // Calculate the luma and chroma by emulating the relevant circuits: 
        double y=0.0, i=0.0, q=0.0, gamma=1.8; 
        auto wave = [](int p, int color) { return (color+p+8)%12 < 6; }; 
        for(int p=0; p<12; ++p) // 12 clock cycles per pixel. 
        { 
            // NES NTSC modulator (square wave between two voltage levels): 
            double spot = lo_and_hi[wave(p,color)]; 

            // De-emphasis bits attenuate a part of the signal: 
            if(((pixel & 0x40) && wave(p,12)) 
            || ((pixel & 0x80) && wave(p, 4)) 
            || ((pixel &0x100) && wave(p, 8))) spot *= attenuation; 

            // Normalize: 
            double v = (spot - black) / (white-black) / 12.0; 

            // Ideal TV NTSC demodulator: 
            y += v; 
            i += v * std::cos(3.141592653 * p / 6); 
            q += v * std::sin(3.141592653 * p / 6); // Or cos(... p-3 ... ) 
            // Note: Integrating cos() and sin() for p-0.5 .. p+0.5 range gives 
            //       the exactly same result, scaled by a factor of 2*cos(pi/12). 
        } 

        // Convert YIQ into RGB according to FCC-sanctioned conversion matrix. 
        auto gammafix = [=](double f) { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); }; 
        auto clamp = [](int v) { return v<0 ? 0 : v>255 ? 255 : v; }; 
        unsigned rgb = 0x10000*clamp(int(255.0 * gammafix(y +  0.946882*i +  0.623557*q))) 
                     + 0x00100*clamp(int(255.0 * gammafix(y + -0.274788*i + -0.635691*q))) 
                     + 0x00001*clamp(int(255.0 * gammafix(y + -1.108545*i +  1.709007*q))); 
        return rgb; 
    }

    unsigned int PutPixel2(unsigned px,unsigned py, unsigned pixel2, int offset)
    {

      
        // The input value is a NES color index (with de-emphasis bits).
        // We need RGB values. To produce a RGB value, we emulate the NTSC circuitry.
        // For most part, this process is described at:
        //    http://wiki.nesdev.com/w/index.php/NTSC_video
        // Incidentally, this code is shorter than a table of 64*8 RGB values.
        static unsigned palette[3][64][512] = {}, prev=~0u;
        // Caching the generated colors
        if(prev == ~0u)
            for(int o=0; o<3; ++o)
            for(int u=0; u<3; ++u)
            for(int p0=0; p0<512; ++p0)
            for(int p1=0; p1<64; ++p1)
            {
                // Calculate the luma and chroma by emulating the relevant circuits:
                auto s = "\372\273\32\305\35\311I\330D\357}\13D!}N";
                int y=0, i=0, q=0;
                for(int p=0; p<12; ++p) // 12 samples of NTSC signal constitute a color.
                {
                    // Sample either the previous or the current pixel.
                    int r = (p+o*4)%12, pixel = r < 8-u*2 ? p0 : p1; // Use pixel=p0 to disable artifacts.
                    // Decode the color index.
                    int c = pixel%16, l = c<0xE ? pixel/4 & 12 : 4, e=p0/64;
                    // NES NTSC modulator (square wave between up to four voltage levels):
                    int b = 40 + s[(c > 12*((c+8+p)%12 < 6)) + 2*!(0451326 >> p/2*3 & e) + l];
                    // Ideal TV NTSC demodulator:
                    y += b;
                    i += b * int(cos(M_PI * p / 6) * 5909);
                    q += b * int(sin(M_PI * p / 6) * 5909);
                }
                // Convert the YIQ color into RGB
                auto gammafix = [=](float f) { return f <= 0.f ? 0.f : pow(f, 2.2f / 1.8f); };
                auto clamp    = [](int v) { return v>255 ? 255 : v; };
                // Store color at subpixel precision
                if(u==2) palette[o][p1][p0] += 0x10000*clamp(255 * (int)gammafix(y/1980.f + i* 0.947f/9e6f + q* 0.624f/9e6f));
                if(u==1) palette[o][p1][p0] += 0x00100*clamp(255 * (int)gammafix(y/1980.f + i*-0.275f/9e6f + q*-0.636f/9e6f));
                if(u==0) palette[o][p1][p0] += 0x00001*clamp(255 * (int)gammafix(y/1980.f + i*-1.109f/9e6f + q* 1.709f/9e6f));
            }
        // Store the RGB color into the frame buffer.
        ////((u32*) s->pixels) [py * 256 + px] = palette[offset][prev%64][pixel];
        /*register RGBQUAD* buf = gfx->frame_buffer();
        auto pal_pixel = palette[offset][prev%64][pixel];
        buf[py * 256 + px].rgbRed = (pal_pixel>>16)&0xff;
        buf[py * 256 + px].rgbGreen = (pal_pixel>>8)&0xff;
        buf[py * 256 + px].rgbBlue = (pal_pixel)&0xff;
        */
        auto result = palette[offset][prev%64][pixel2];
        prev = pixel2;
        return result;
    }

    unsigned int PutPixel(unsigned px,unsigned py, unsigned pixel, int offset)
    {
        // The input value is a NES color index (with de-emphasis bits).
        // We need RGB values. To produce a RGB value, we emulate the NTSC circuitry.
        // For most part, this process is described at:
        //    http://wiki.nesdev.com/w/index.php/NTSC_video
        // Incidentally, this code is shorter than a table of 64*8 RGB values.
        static unsigned palette[3][64][512] = {}, prev=~0u;
        // Caching the generated colors
        if(prev == ~0u)
            for(int o=0; o<3; ++o)
            for(int u=0; u<3; ++u)
            for(int p0=0; p0<512; ++p0)
            for(int p1=0; p1<64; ++p1)
            {
                // Calculate the luma and chroma by emulating the relevant circuits:
                auto s = "\372\273\32\305\35\311I\330D\357}\13D!}N";
                int y=0, i=0, q=0;
                for(int p=0; p<12; ++p) // 12 samples of NTSC signal constitute a color.
                {
                    // Sample either the previous or the current pixel.
                    int r = (p+o*4)%12, pixel = r < 8-u*2 ? p0 : p1; // Use pixel=p0 to disable artifacts.
                    // Decode the color index.
                    int c = pixel%16, l = c<0xE ? pixel/4 & 12 : 4, e=p0/64;
                    // NES NTSC modulator (square wave between up to four voltage levels):
                    int b = 40 + s[(c > 12*((c+8+p)%12 < 6)) + 2*!(0451326 >> p/2*3 & e) + l];
                    // Ideal TV NTSC demodulator:
                    y += b;
                    i += b * int(std::cos(M_PI * p / 6) * 5909);
                    q += b * int(std::sin(M_PI * p / 6) * 5909);
                }
                // Convert the YIQ color into RGB
                auto gammafix = [=](float f) { return f <= 0.f ? 0.f : std::pow(f, 2.2f / 1.8f); };
                auto clamp    = [](int v) { return v>255 ? 255 : v; };
                // Store color at subpixel precision
                if(u==2) palette[o][p1][p0] += 0x10000*clamp(int(255.0 * gammafix(y/1980.f + i* 0.947f/9e6f + q* 0.624f/9e6f)));
                if(u==1) palette[o][p1][p0] += 0x00100*clamp(int(255.0 * gammafix(y/1980.f + i*-0.275f/9e6f + q*-0.636f/9e6f)));
                if(u==0) palette[o][p1][p0] += 0x00001*clamp(int(255.0 * gammafix(y/1980.f + i*-1.109f/9e6f + q* 1.709f/9e6f)));
            }
        // Store the RGB color into the frame buffer.
        auto result = palette[offset][prev%64][pixel];
        prev = pixel;
        return result;
    }

    void FlushScanline(unsigned py)
    {
        if(py == 239) {
           //render
          //ggfx->Render();
        }
         
    }

    int joy_current[2]={0,0}, joy_next[2]={0,0}, joypos[2]={0,0};
    void JoyStrobe(unsigned v)
    {

        if(v) { 
          joy_current[0] = joy_next[0]; 
        joypos[0]=0; 
        }
        if(v) { joy_current[1] = joy_next[1]; joypos[1]=0; }
    }

    uint8_t JoyRead(unsigned idx)
    {
     
        static const uint8_t masks[8] = {0x20,0x10,0x40,0x80,0x04,0x08,0x02,0x01};
        return ((joy_current[idx] & masks[joypos[idx]++ & 7]) ? 1 : 0);
    }


    int joy1_strobe=0xff;
    unsigned __int8 joy1_data;
    unsigned __int8 joy_current_bit=0;
    int joy_state = 0;

    uint8_t JoyRead2() {
		  if (joy_state == 1) {
		    unsigned __int8 result = (joy1_data>>joy_current_bit) & 0x1;
		    joy_current_bit = (joy_current_bit+1) & 0x7;
		    
		    return result;
		  }
      return 0xFF;
    }

    void JoyWrite2(uint8_t data) {
       //joy1_strobe == 1 && data == 0 // old
      if ((joy1_strobe & 0x1) == 1) {
		    joy_state = 1;
		    joy1_data = 0;
		    
		    
        if (joy1.a == true) 
          joy1_data |= 1<<0;		    
		    if (joy1.b == true) 
          joy1_data |= 1<<1;		    
		    if (joy1.select == true) 
          joy1_data |= 1<<2;	
		    if (joy1.start == true)
          joy1_data |= 1<<3;	
		    if (joy1.up == true) 
          joy1_data |= 1<<4;	
		    if (joy1.down == true) 
          joy1_data |= 1<<5;	
		    if (joy1.left == true) 
          joy1_data |= 1<<6;	
		    if (joy1.right == true) 
          joy1_data |= 1<<7;	
		    
			    
		  }
		  else 
		    joy_state = 0;
		  
		  joy1_strobe = data;
    }


}