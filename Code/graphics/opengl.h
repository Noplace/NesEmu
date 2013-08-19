#ifndef UISYSTEM_GRAPHICS_OPENGL_H
#define UISYSTEM_GRAPHICS_OPENGL_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "graphics.h"

namespace graphics {

class OpenGL : public Graphics {
 public:
  OpenGL();
  ~OpenGL();
  void Initialize(HWND window_handle,int width,int height);
  void Deinitialize();
  void SetDisplaySize(int width,int height);
  void Clear(RGBQUAD color);
  void Render();
  void SwitchThread() {
    wglMakeCurrent( device_context_, render_context_ );
  }
 private:
  HDC device_context_;
  HGLRC render_context_;
  bool init_;
};

}

#endif