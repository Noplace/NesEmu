#include "application.h"

namespace app {

Application::Application(HINSTANCE instance , LPSTR command_line, int show_command) {
  //OleInitialize(NULL);
}

Application::~Application() {
 
}

int Application::Run() {
  display_window_.Init();
  
  MSG msg;
  /*BOOL msg_return;
  while( (msg_return = GetMessage( &msg, NULL, 0, 0 )) != 0) { 
    if (msg_return == -1) {
      // handle the error and possibly exit
    }
    else {
      TranslateMessage(&msg); 
      DispatchMessage(&msg); 
    }
  } */
  
  do {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      display_window_.Step(); 
    }
  } while(msg.message!=WM_QUIT && display_window_.exit_signal() == false);

 
   //Return the exit code to the system. 
   return static_cast<int>(msg.wParam);
}

}

