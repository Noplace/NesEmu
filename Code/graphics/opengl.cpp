#include "opengl.h"

#pragma comment(lib,"opengl32")
#pragma comment(lib,"glu32")

namespace graphics {


OpenGL::OpenGL() :init_(false),device_context_(nullptr),render_context_(nullptr) {

}

OpenGL::~OpenGL() {
  Deinitialize();
}

void OpenGL::Initialize(HWND window_handle,int width,int height) {
  PIXELFORMATDESCRIPTOR pfd;
  int iFormat;
  if (init_ == true)
    return;
  Graphics::Initialize(window_handle,width,height);
  init_ = true;
  // get the device context (DC)
  device_context_ = GetDC( window_handle_ );
  if (device_context_ == nullptr) {
    //error
    return;
  }
  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize = sizeof( pfd );
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;
  iFormat = ChoosePixelFormat( device_context_, &pfd );
  SetPixelFormat( device_context_, iFormat, &pfd );

  // create and enable the render context (RC)
  render_context_ = wglCreateContext( device_context_ );
  wglMakeCurrent( device_context_, render_context_ );
  
  //const GLubyte* str = glGetString(GL_EXTENSIONS);
  glShadeModel(GL_SMOOTH);
  SetDisplaySize(width,height);
}

void OpenGL::Deinitialize() {
  if (init_ == true) {
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( render_context_ );
    ReleaseDC( window_handle_, device_context_ );
    init_ = false;
  }
}

void OpenGL::SetDisplaySize(int width,int height) {
  display_width_ = width;
  display_height_ = height;
  glClearColor(0.25f, 0.25f, 0.5f, 0.0f);
  glViewport(0, 0, display_width_, display_height_);					// Reset The Current Viewport
	/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)display_width_/(GLfloat)display_height_,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
  */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, display_width_, display_height_, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();							// Reset The Modelview Matrix
}

void OpenGL::Clear(RGBQUAD color) {

}

void OpenGL::Render() {
  SwapBuffers(device_context_);
}


}