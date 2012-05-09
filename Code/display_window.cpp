#include "application.h"
#include "graphics/eagle.h"

void hq2x_filter_render(
  uint32_t *output, unsigned outputPitch,
  const uint32_t *input, unsigned inputPitch,
  unsigned width, unsigned height
);


namespace IO {
  extern HWND window_handle;
}

namespace app {

DisplayWindow::DisplayWindow() : Window() {
  output = new uint32_t[256*2*240*2];
//  counter = 0;
  instance = GetModuleHandle(nullptr);
}

DisplayWindow::~DisplayWindow() {
  delete []output;
}

void DisplayWindow::Init() {
  exit_signal_ = false;
  PrepareClass("NesEmu");
  window_class.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
  window_class.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,LR_DEFAULTSIZE));
  Create();
  SetMenu(LoadMenu(instance, MAKEINTRESOURCE(IDR_MENU1)));
  DragAcceptFiles(handle(),true);

  gfx.Initialize(handle(),0,0);
  nes.set_on_render([this]() {
    switch(display_mode) {
      case ID_VIDEO_STD256X240:
        //glDrawPixels(256,240,GL_BGRA_EXT,GL_UNSIGNED_BYTE,nes.frame_buffer);
        //glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexImage2D(GL_TEXTURE_2D,0,4,256,240,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,nes.frame_buffer);
        glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
        glTexCoord2d(1.0,0.0); glVertex2d(256.0,0.0);
        glTexCoord2d(1.0,1.0); glVertex2d(256.0,240.0);
        glTexCoord2d(0.0,1.0); glVertex2d(0.0,240.0);
        glEnd();
      break;
      
      case ID_VIDEO_EAGLE512X480:
        ScaleImageEagle2X(nes.frame_buffer,256,240,output);
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,512,480,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
        glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
        glTexCoord2d(1.0,0.0); glVertex2d(256.0*2,0.0);
        glTexCoord2d(1.0,1.0); glVertex2d(256.0*2,240.0*2);
        glTexCoord2d(0.0,1.0); glVertex2d(0.0,240.0*2);
        glEnd();
        //hq2x_filter_render(output,256,nes.frame_buffer,256,256,240);
        //glDrawPixels(512,480,GL_BGRA_EXT,GL_UNSIGNED_BYTE,output);
      break;
    }

    gfx.Render();
    timing.render_time_span = 0;
    ++timing.fps_counter;
  });

  nes.set_on_vertical_blank([this]() {
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0) {//while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  });

  //auto a = glGetError();
  //glPixelZoom(1, -1);
  //a = glGetError();
  //glRasterPos2i(0,0);
  //a = glGetError();
  glEnable( GL_TEXTURE_2D );
  auto a = glGetError();
  glGenTextures( 1, &texture );
  a = glGetError();
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  
  //settings setup
  options.nes = &nes;
  IO::window_handle = handle();
  IO::Init();
  ResetTiming();
  
  OnCommand(ID_VIDEO_STD256X240,0);
  Center();
  ShowWindow(handle(), SW_SHOW); 
  UpdateWindow(handle()); 
  
  

 //nes.Open("D:\\Personal\\Projects\\NesEmu\\mario.nes");

  //CPU Tests
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_reset\\ram_after_reset.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_timing_test6\\cpu_timing_test.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_interrupts_v2\\rom_singles\\1-cli_latency.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_interrupts_v2\\rom_singles\\2-nmi_and_brk.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_interrupts_v2\\cpu_interrupts.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\nestest.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\instr_timing\\instr_timing.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\instr_timing\\instr_timing.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\NEStress\\NEStress.NES");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_dummy_writes\\cpu_dummy_writes_ppumem.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_flag_concurrency\\test_cpu_flag_concurrency.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\cpu_exec_space\\test_cpu_exec_space_apu.nes");
  


  //APU Tests
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\apu_reset\\4015_cleared.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\apu_reset\\4017_timing.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\apu_reset\\4017_written.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\blargg_apu\\01.len_ctr.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\blargg_apu\\09.reset_timing.nes");
  
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\nes_saw\\square_scale.nes");

  //MMC3
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\mmc3_irq_tests\\1.Clocking.nes");
  nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\mmc3_test_2\\rom_singles\\1-clocking.nes");
  //nes.Open("F:\\NESRen\\Pirate\\Captain Tsubasa Vol 2 - Super Striker (J) [p1].nes");

  //PPU
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\blargg_ppu_tests\\vram_access.nes");
  //nes.Open("D:\\Personal\\Projects\\NesEmu\\testroms\\sprite_hit_tests_2005.10.05\\10.timing_order.nes");
  
  //MMC1
  //nes.Open("F:\\NESRen\\USA\\Legend of Zelda, The (U) (PRG 0).nes");
  //nes.Open("F:\\NESRen\\Translated\\Captain Tsubasa (J) [T-Eng].nes");
  OnCommand(ID_MODE_NTSC,0);
}

void DisplayWindow::ResetTiming() {
  memset(&timing,0,sizeof(timing));
  timer.Calibrate();
  timing.prev_cycles = timer.GetCurrentCycles();
}

void DisplayWindow::Step() {
  //if (nes.on == false) return;
  const double dt =  1000.0 / nes.settings.cpu_freq_hz;//options.cpu_freq(); 0.00058f;//16.667f;
  timing.current_cycles = timer.GetCurrentCycles();
  double time_span =  (timing.current_cycles - timing.prev_cycles) * timer.resolution();
  if (time_span > 250.0) //clamping time
    time_span = 250.0;

  timing.span_accumulator += time_span;
  while (timing.span_accumulator >= dt) {
    timing.span_accumulator -= nes.Step(dt);
  }
    

  timing.total_cycles += timing.current_cycles-timing.prev_cycles;
  timing.prev_cycles = timing.current_cycles;
  

  //timing.render_time_span += time_span;
  //++timing.fps_counter;
  timing.fps_time_span += time_span;
  if (timing.fps_time_span >= 1000.0) {
    timing.fps = timing.fps_counter;
    timing.fps_counter = 0;
    timing.fps_time_span = 0;

    char caption[256];
    //sprintf(caption,"Freq : %0.2f MHz",nes.frequency_mhz());
    //sprintf(caption,"CPS: %llu ",nes.cycles_per_second());
    
    sprintf(caption,"FPS: %02d",timing.fps);
    SetWindowText(handle(),caption);
 
  }


}

int DisplayWindow::OnCreate(WPARAM wParam,LPARAM lParam) {
  return 0;
}

int DisplayWindow::OnDestroy(WPARAM wParam,LPARAM lParam) {
  IO::Deinit();
  glDeleteTextures(1,&texture);
  gfx.Deinitialize();
  exit_signal_ = true;
  PostQuitMessage(0);
  return 0;
}

int DisplayWindow::OnCommand(WPARAM wParam,LPARAM lParam) {
  if (HIWORD(wParam)==0) {
    
    if (LOWORD(wParam)==ID_VIDEO_STD256X240) {
      CheckMenuItem(menu_,ID_VIDEO_STD256X240,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_UNCHECKED);
      SetClientSize(256,240);
      gfx.SetDisplaySize(256,240);
      display_mode = ID_VIDEO_STD256X240;
    }

    if (LOWORD(wParam)==ID_VIDEO_EAGLE512X480) {
      CheckMenuItem(menu_,ID_VIDEO_EAGLE512X480,MF_CHECKED);
      CheckMenuItem(menu_,ID_VIDEO_STD256X240,MF_UNCHECKED);
      SetClientSize(512,480);
      gfx.SetDisplaySize(512,480);
      display_mode = ID_VIDEO_EAGLE512X480;
    }

    if (LOWORD(wParam)==ID_MODE_NTSC) {
      CheckMenuItem(menu_,ID_MODE_NTSC,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_PAL,MF_UNCHECKED);
      nes.settings.machine_mode = NTSC;
      nes.set_mode(nes.settings.machine_mode);
      machine_mode = ID_MODE_NTSC;
    }

    if (LOWORD(wParam)==ID_MODE_PAL) {
      CheckMenuItem(menu_,ID_MODE_PAL,MF_CHECKED);
      CheckMenuItem(menu_,ID_MODE_NTSC,MF_UNCHECKED);
      nes.settings.machine_mode = PAL;
      nes.set_mode(nes.settings.machine_mode);
      machine_mode = ID_MODE_PAL;
    }
    

    if (LOWORD(wParam)==ID_FILE_EXIT) {
      PostQuitMessage(0);
    }

    if (LOWORD(wParam)==ID_MACHINE_RESET) {
      nes.Reset();
      ResetTiming();
    }
     
    if (LOWORD(wParam)==ID_EDIT_OPTIONS) {
      options.Show(this);
    }
  
  }
  return 0;
}

int DisplayWindow::OnKeyDown(WPARAM wParam,LPARAM lParam) {

  if (wParam == 'Z')
    IO::joy1.a = true;
  if (wParam == 'X')
    IO::joy1.b = true;
  if (wParam == VK_RETURN)
    IO::joy1.start = true;
  if (wParam == VK_SPACE)
    IO::joy1.select = true;
  if (wParam == VK_UP)
    IO::joy1.up = true;
  if (wParam == VK_DOWN)
    IO::joy1.down = true;
  if (wParam == VK_LEFT)
    IO::joy1.left = true;
  if (wParam == VK_RIGHT)
    IO::joy1.right = true;
  return 0;
}

int DisplayWindow::OnKeyUp(WPARAM wParam,LPARAM lParam) {
  if (wParam == 'Z')
    IO::joy1.a = false;
  if (wParam == 'X')
    IO::joy1.b = false;
  if (wParam == VK_RETURN)
    IO::joy1.start = false;
  if (wParam == VK_SPACE)
    IO::joy1.select = false;
  if (wParam == VK_UP)
    IO::joy1.up = false;
  if (wParam == VK_DOWN)
    IO::joy1.down = false;
  if (wParam == VK_LEFT)
    IO::joy1.left = false;
  if (wParam == VK_RIGHT)
    IO::joy1.right = false;
  return 0;
}

int DisplayWindow::OnDropFiles(WPARAM wParam,LPARAM lParam) {
  auto len = DragQueryFile((HDROP)wParam,0,NULL,0);
  if (len==0)
    return 0;

  ++len;
  char* filename = new char[len];
  DragQueryFile((HDROP)wParam,0,filename,len);
  
  auto ext = strrchr(filename,'.');
  for (size_t i=0;i<strlen(ext);++i) {
    ext[i] = tolower(ext[i]);
  }

  if (strcmp(ext,".nes")==0) {
    nes.Open(filename);
    OnCommand(ID_MACHINE_RESET,0);
  }

  SafeDeleteArray(&filename);

  return 0;
}

}