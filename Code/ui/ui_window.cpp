#include "ui.h"
#include "ui_window.h"

namespace ui {

void Window::Draw() {
  
  int x=left_;
  int y=top_;
  int w=width_;
  int h=height_;
  
  graphics::GDI& gfx = ui().graphics();
  
  gfx.BeginFill(graphics::Color(90,90,90),0.7);
  gfx.DrawRectangle(x,y,w,h);
  
  gfx.BeginFill(graphics::Color(225,155,50),1);
  gfx.DrawLine(x,y,x,y+h);
  gfx.DrawLine(x,y,x+w,y);
  gfx.DrawLine(x+w,y+h,x,y+h);
  gfx.DrawLine(x+w,y+h,x+w,y);
  
  
  RGBQUAD* colors = new RGBQUAD[2];
  colors[0] = graphics::Color(225,155,50);
  colors[1] = graphics::Color(255,255,255);
  double* alphas = new double[2];
  alphas[0] = 1;
  alphas[1] = 1;
  double* ratios = new double[2];
  ratios[0] = 0;
  ratios[1] = 1;
  
  
  gfx.BeginGradientFill(colors,alphas,ratios,2,graphics::kGradientHorizontal);
  gfx.DrawRectangle(x+1,y+1,w-30,20);


  gfx.BeginFill(graphics::Color(255,255,255),1);
  gfx.DrawTriangle(x+w-30,y+1,x+w-30,y+20,x+w-1,y+1);

}

}