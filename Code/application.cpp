#include "application.h"

namespace app {

Application::Application(HINSTANCE instance , LPSTR command_line, int show_command) {
  //OleInitialize(NULL);
}

Application::~Application() {
 
}

int Application::Run() {
  //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  unsigned old_fp_state;
  _controlfp_s(&old_fp_state, _PC_53, _MCW_PC);

  display_window_.Init();
  
  MSG msg;
  do {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      display_window_.Step(); 
    }
  } while(msg.message!=WM_QUIT && display_window_.exit_signal() == false);

 //CoUninitialize();
 //Return the exit code to the system. 
 return static_cast<int>(msg.wParam);
}

}

