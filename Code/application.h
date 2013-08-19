#ifndef UISYSTEM_APPLICATION_H
#define UISYSTEM_APPLICATION_H

#include "debug.h"
#include <math.h>
#include <WinCore/windows/windows.h>
#include <WinCore/windows/application.h>
#include <WinCore/timer/timer2.h>
#include <Shellapi.h>
#include <thread>
#include <atomic>
#include "graphics/gdi.h"
#include "graphics/opengl.h"
#include "emulation/nes/nes.h"
#include "resource.h"
#include "dialogs/options.h"
#include "display_window.h"

namespace app {

/*
  Class Name  : Application
  Description : this is the application's class
*/
class Application : core::windows::Application {
  public:
    explicit Application(HINSTANCE instance , LPSTR command_line, int show_command);
    ~Application();
    int Run();
    DisplayWindow& display_window() {
      return display_window_;
    }
  protected:
    
  private:
    DisplayWindow display_window_;
};

}

#endif