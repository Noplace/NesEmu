#ifndef EMULATION_NES_NES_H
#define EMULATION_NES_NES_H

#pragma warning( disable:4800 )
#include "../../debug.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>
#include <functional>
#include <WinCore/types.h>
#include <WinCore/log/log_manager.h>
#include "register_bit.h"
#include "../../utilities/ring_buffer.h"

enum MachineMode{
  NTSC = 0,
  PAL = 1,
  Dendy = 2
};

class Nes;
class Component {
 public:
  Component() : nes_(nullptr),init_(false) { }
  virtual int Initialize(Nes* nes) {
    nes_ = nes;
    init_ = true;
    return S_OK;
  }
  virtual void OnSettingsChanged() {
  }
  Nes& nes() { return *nes_; }
  void set_mode(MachineMode mode) { mode_ = mode; }
 protected:
  Nes* nes_;
  MachineMode mode_;
  bool init_;
};

//io.h

namespace IO
{
    
    void Init();
    void Deinit();
    void writeAudioBlock(void* block, uint32_t size);
    unsigned MakeRGBcolor(unsigned pixel);
    unsigned int PutPixel(unsigned px,unsigned py, unsigned pixel, int offset);
    void FlushScanline(unsigned py);
    struct Joystick {
      bool a,b,start,select,up,left,down,right;
    };
    extern Joystick joy1,joy2;
    //extern int joy_current[2], joy_next[2], joypos[2];
    void JoyStrobe(unsigned v);
    uint8_t JoyRead(unsigned idx);
    uint8_t JoyRead2();
    void JoyWrite2(uint8_t data);
}

#include "gamepak.h"
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "ppu/ppu2.h"
#include "apu/apu.h"

class Nes {
 public:
  struct Settings {
    double cpu_freq_hz;
    MachineMode machine_mode;
  }settings;
  double time_acc_;
  core::LogManager log;
  bool on,pause;
  Nes();
  ~Nes();
  void Initialize();
  void Deinitialize();
  void Open(const char* filename);
  void Power();
  void Reset();
  double Step(double dt);
  Cpu& cpu() { return cpu_; }
  GamePak& gamepak() { return gamepak_; }
  Ppu& ppu() { return ppu_; }
  Apu& apu() { return apu_; }
  uint32_t* frame_buffer;
  double frequency_mhz() { return cpu_.frequency_mhz_; }
  uint64_t cycles_per_second() { return cpu_.cycles_per_second; }

  std::function <void ()> on_render;
  void set_on_render(const  std::function <void ()>& on_render) {
    if (on_render != nullptr)
      this->on_render = on_render;
  } 

  std::function <void ()> on_vertical_blank;
  void set_on_vertical_blank(const  std::function <void ()>& on_vertical_blank) {
    if (on_vertical_blank != nullptr)
      this->on_vertical_blank = on_vertical_blank;
  } 
  void OnSettingsChanged() {
    gamepak_.OnSettingsChanged();
    cpu_.OnSettingsChanged();
    ppu_.OnSettingsChanged();
    apu_.OnSettingsChanged();
  }
  void set_mode(MachineMode mode) {
    if (mode == NTSC) {
      settings.cpu_freq_hz = 1789772.72727;
    } else if (mode == PAL) {
      settings.cpu_freq_hz = 1662607.03125;
    }
    gamepak_.set_mode(mode);
    cpu_.set_mode(mode);
    ppu_.set_mode(mode);
    apu_.set_mode(mode);
    OnSettingsChanged();
  }
 protected:
  bool init_;
  Cpu cpu_;
  GamePak gamepak_;
  Ppu ppu_;
  Apu apu_;
};

#endif