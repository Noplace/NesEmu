#include <stdio.h>
#include "nes.h"

Nes::Nes() : frame_buffer(nullptr) , init_(false),on(false),pause(false) {
  frame_buffer = new uint32_t[256*240];
  on_render = [](){};
  on_vertical_blank = [](){};
  //default settings
  //NTSC = 6*(39375000/11) / 16 
  //#define NTSC_CPU_HZ (6*(39375000.0/11) / 16)
  //settings.cpu_freq_hz = 1789772.7272727272727272727272727;//1789800.0; NTSC
  //settings.cpu_freq_hz = 1662607.03125;//1789800.0; PAL
  settings.machine_mode = NTSC;
}

Nes::~Nes() {
  Deinitialize();
  SafeDeleteArray(&frame_buffer);
}

void Nes::Initialize() {
  if (init_ == true) 
    return;
  init_ = true;
  #ifdef _DEBUG
    {
      char filename[MAX_PATH] = "";
      char app_path[MAX_PATH];
      GetModuleFileName(0, filename, sizeof(filename) - 1);
      strncpy(app_path,filename,strrchr(filename,'\\')-filename);
      app_path[strrchr(filename,'\\')-filename] = '\0';
      log.Initialize(strcat(app_path,"\\logs\\"));
    }
  #endif

  time_acc_ = 0 ;


  ppu_.Initialize(this);
  cpu_.Initialize(this);
  apu_.Initialize(this);

  set_mode(settings.machine_mode);
}

void Nes::Deinitialize() {
  if (init_ == true) {
    cpu_.Deinitialize();
    //
    //
    gamepak_.Deinitialize();
    #ifdef _DEBUG
      log.Deinitialize();
    #endif
    init_ = false;
  }
}

void Nes::Power() {
  cpu_.Power();
  apu_.Power();
  ppu_.Power();
  gamepak_.mapper->Initialize();
  on = true;
  apu_.StartupRoutine();
}

void Nes::Reset() {
  cpu_.Reset();
  apu_.Reset();
  ppu_.Reset();
  gamepak_.mapper->Initialize();
  on = true;
  apu_.StartupRoutine();
}

void Nes::Open(const char* filename) {
  on = false;
  Deinitialize();
  // Open the ROM file specified on commandline
  
  //gamepak must be initialized before anything can work
  if (gamepak_.InitializeFromFile(this,filename)!=S_FALSE) {
    Initialize();
    Power();
  }
}

double Nes::Step(double dt) {
  
  /*auto time_per_cycle = nes.cpu().current_inst_cycles!=0?(timing.span_accumulator / nes.cpu().current_inst_cycles):1;
  if (time_per_cycle < dt) {
    timing.span_accumulator += time_span;
  } else {
    nes.Step(time_span);
    timing.span_accumulator = time_span;
  }*/  
  cpu_.Op();
  register auto t = dt*cpu_.current_inst_cycles;
  time_acc_ += t;
  if (time_acc_ > 1000.0) {
    cpu_.cycles_per_second = cpu_.cycles - cpu_.last_cpu_cycles_;
    cpu_.frequency_mhz_ = double( cpu_.cycles_per_second ) * 0.000001f;
    cpu_.last_cpu_cycles_ = cpu_.cycles;
    time_acc_ = 0;
  }

  return t;
}