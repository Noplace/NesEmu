#ifndef UISYSTEM_GRAPHICS_GRAPHICS_H
#define UISYSTEM_GRAPHICS_GRAPHICS_H

namespace graphics {

enum FillMode    { kSolid , kGradientHorizontal, kGradientVertical, kGradientRadial };
enum CommandType { kBeginFill, kEndFill, kDrawRectangle , kDrawLine , kDrawTriangle };

struct Command {
  CommandType type;
  void* data;
};

struct Fill {
  RGBQUAD colors[4];
  double alphas[4];  
  double ratios[4];
  int count;
  FillMode mode;
};

struct Clip {
  int x;
  int y;
  int width;
  int height;
};

inline RGBQUAD Color(BYTE r,BYTE g,BYTE b) {
  RGBQUAD c;
  c.rgbRed = r;
  c.rgbGreen = g;
  c.rgbBlue = b;
  return c;
}

inline double interpolate_int(int a,int b,double s) {
  return a + s*(b-a);
}

inline double interpolate_double(double a,double b,double s) {
  return a + s*(b-a);
}

inline void swap_int(int& a,int& b) {
  int tmp;
  tmp = a;
  a   = b;
  b   = tmp;
}

inline RGBQUAD interpolate_color(RGBQUAD a,RGBQUAD b,double s) {
  RGBQUAD r;
  r.rgbRed   = static_cast<BYTE>(interpolate_int(a.rgbRed,b.rgbRed,s));
  r.rgbGreen = static_cast<BYTE>(interpolate_int(a.rgbGreen,b.rgbGreen,s));
  r.rgbBlue  = static_cast<BYTE>(interpolate_int(a.rgbBlue,b.rgbBlue,s));
  return r;
}

class Graphics {
 public:
  Graphics() {}
  virtual ~Graphics() {}
  void Initialize(HWND window_handle, int width, int height) {
    window_handle_ = window_handle;
    display_width_ = width;
    display_height_ = height;
  }
  void Deinitialize() {
  }
  virtual void Clear(RGBQUAD color) = 0;
  virtual void Render() = 0;
  HWND window_handle() { return window_handle_; }
 protected:
  HWND window_handle_;
  int display_width_;
  int display_height_;
};

}

#endif