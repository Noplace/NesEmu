#ifndef UISYSTEM_DISPLAY_WINDOW_H
#define UISYSTEM_DISPLAY_WINDOW_H

namespace app {

/*
  Class Name  : DisplayWindow
  Description : this is the application's main window
*/
class DisplayWindow: public core::windows::Window {
  public:
    DisplayWindow();
    ~DisplayWindow();
    void Init();
    void ResetTiming();
    void Step();
    bool exit_signal() { return exit_signal_; }
  protected:
    int OnCommand(WPARAM wParam,LPARAM lParam);
    int OnKeyDown(WPARAM wParam,LPARAM lParam);
    int OnKeyUp(WPARAM wParam,LPARAM lParam);
    int OnCreate(WPARAM wParam,LPARAM lParam);
    int OnDestroy(WPARAM wParam,LPARAM lParam);
    int OnDropFiles(WPARAM wParam,LPARAM lParam);
    int OnPaint(WPARAM wParam,LPARAM lParam);
  private:
    std::thread* emu_th;
    std::atomic_bool exit_signal_;
    int display_mode;
    int machine_mode;
    uint32_t* output;
    //graphics::GDI gfx;
    graphics::OpenGL gfx;
    GLuint texture;
    Nes nes;
    dialogs::Options options;

    utilities::Timer timer;
    struct {
      uint64_t extra_cycles;
      uint64_t current_cycles;
      uint64_t prev_cycles;
      uint64_t total_cycles;
      uint32_t fps_counter;
      uint32_t ups_counter;
      uint32_t fps;
      uint32_t ups;
      double render_time_span;
      double fps_time_span;
      double span_accumulator;

    } timing;
    HINSTANCE instance;
};

}

#endif