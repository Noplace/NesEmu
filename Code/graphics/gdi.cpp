#include "gdi.h"


namespace graphics {


double interpolate_int_array(int* values, int count, double s) {
  double result=0;
  for (int i=0;i<count;i++) {
      double is = static_cast<double>(count)/static_cast<double>(count-(i+1));
      result += values[i]*(is-s);
  }
  return result;
}

GDI::GDI() {
  memset(&fill_,0,sizeof(fill_));
  back_buffer_ = NULL;
  frame_buffer_ = NULL;
  //int test[] = {1,4};
  
  //double r = interpolate_int_array(test,2,0);
}

GDI::~GDI() {

}

void GDI::Initialize(HWND window_handle,int width,int height) {

  Graphics::Initialize(window_handle,width,height);
  memset(&bitmap_info,0,sizeof(bitmap_info));
  bitmap_info.bmiHeader.biBitCount = 32;
  bitmap_info.bmiHeader.biPlanes = 1;
  bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
  bitmap_info.bmiHeader.biWidth = width;
  bitmap_info.bmiHeader.biHeight = -height;
  bitmap_info.bmiHeader.biSizeImage = 0;
  bitmap_info.bmiHeader.biCompression = BI_RGB;

  window_dc_ = GetDC(window_handle);//GetWindowDC(window_handle);//CreateCompatibleDC(GetDC(window_handle));
  hbmp = CreateDIBSection(window_dc_,&bitmap_info,DIB_RGB_COLORS,reinterpret_cast<void**>(&frame_buffer_),NULL,0);
  
  back_buffer_ = new RGBQUAD[display_width_*display_height_];
  // = CreateCompatibleBitmap(dc,200,200);
  //SelectObject(dc,hbmp);
  SetClippingArea(0,0,display_width_,display_height_);
}

void GDI::Deinitialize() {
  if (back_buffer_ != NULL)
    delete [] back_buffer_;
  DeleteObject(hbmp);
  Graphics::Deinitialize();
}

void GDI::Render() {
  //memset(back_buffer_,255,200*200*4);
  //SetDIBits(dc,hbmp,0,200,back_buffer,&bitmap_info,DIB_RGB_COLORS);
  //for (int i = 0; i < display_width_*display_height_; i++) {
  //  frame_buffer_[i] = back_buffer_[i];
  //}
  memcpy(frame_buffer_,back_buffer_,display_width_*display_height_*sizeof(RGBQUAD));
  SetDIBitsToDevice(window_dc_,0,0,display_width_,display_height_,0,0,0,display_height_,frame_buffer_,&bitmap_info,DIB_RGB_COLORS);
}

void GDI::Clear(RGBQUAD color) {
  for (int i = 0; i < display_width_*display_height_; ++i) {
    back_buffer_[i].rgbRed   = color.rgbRed;
    back_buffer_[i].rgbGreen = color.rgbGreen;
    back_buffer_[i].rgbBlue  = color.rgbBlue;
  }
}

void GDI::SetClippingArea(int x, int y, int width, int height) {
  clip_.x = x;
  clip_.y = y;
  clip_.width = width;
  clip_.height = height;
}

void GDI::BeginFill(RGBQUAD color, double alpha) {
  ClearFill();
  fill_.count = 1;
  fill_.colors[0].rgbRed = color.rgbRed;
  fill_.colors[0].rgbGreen = color.rgbGreen;
  fill_.colors[0].rgbBlue = color.rgbBlue;
  fill_.alphas[0] = alpha;
  fill_.mode = kSolid;
}

void GDI::BeginGradientFill(const RGBQUAD* colors, const double* alphas, const double* ratios, int count, FillMode mode) {
  ClearFill();
  fill_.count = count;
  for (int i = 0; i < fill_.count; i++) {
    fill_.colors[i].rgbRed   = colors[i].rgbRed;
    fill_.colors[i].rgbGreen = colors[i].rgbGreen;
    fill_.colors[i].rgbBlue  = colors[i].rgbBlue;
    fill_.alphas[i] = alphas[i];
    fill_.ratios[i] = ratios[i];
  }
  fill_.mode = mode;  
}

void GDI::EndFill() {

}
double theta = 0;

void GDI::DrawRectangle(int x, int y, int width, int height) {

  //DrawTriangle(x,y,x+width,y,x,y+height);
  //DrawTriangle(x,y+height,x+width,y+height,x+width,y);

  /*
  //theta += M_PI/100;
  double vx=0,vy=0;
  double c = cos(theta);
  double s = sin(theta);
  double m00 = c;//vx*vx+(1-vx*vx)*c;
  double m01 = -s;//vx*vy*(1-c);
  double m10 = s;//vx*vy*(1-c);
  double m11 = c;//vy*vy+(1-vy*vy)*c;
  */

  for (int v = y; v < (y+height); v++) {
    for (int u = x; u < (x+width); u++) {
      //int ud = static_cast<int>(u*m00+v*m01);//(u*cos(theta)-v*sin(theta));
      //int vd = static_cast<int>(u*m10+v*m11);//(u*sin(theta)+v*cos(theta));
      if ( TestBoundry(u,v) == true ) {
        double xindex = static_cast<double>(u-x)/static_cast<double>(width); 
        double yindex = static_cast<double>(v-y)/static_cast<double>(height); 
        //rotation

        //
        FillPixel(u,v,xindex,yindex);
      }
    }
  }
}

void GDI::DrawCircle(int x, int y, int radius) {
  
  for (int v = y; v < (y+radius*2); v++) {
    for (int u = x; u < (x+radius*2); u++) {
      if ( TestBoundry(u,v) == true ) {
        int test = (u-x-radius)*(u-x-radius) + (v-y-radius)*(v-y-radius);
        if (test < radius*radius) {
          double xindex = static_cast<double>(u-x)/static_cast<double>(radius*2); 
          double yindex = static_cast<double>(v-y)/static_cast<double>(radius*2);         
          FillPixel(u,v,xindex,yindex);
        }
      }
    }
  }
}

void GDI::DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {

  //rotation
  /*math::vector_1x2 v1={x0,y0};
  math::vector_1x2 v2={x1,y1};
  math::vector_1x2 v3={x2,y2};
  math::vector_1x2 rv;

  math::matrix_2x2 m1;
  math::matrix_2x2 m2,m3;

  theta += M_PI/100;

  double c = cos(theta);
  double s = sin(theta);
  m1(0,0) = c;
  m1(0,1) = -s;
  m1(1,0) = s;
  m1(1,1) = c;

  m2(0,0) = 2;
  m2(0,1) = 0;
  m2(1,0) = 0;
  m2(1,1) = 2;
  
  m1 = m1 * m2;
  m3 = m1.inverse();

  rv = v1 * m1;
  x0 = static_cast<int>(rv.x);
  y0 = static_cast<int>(rv.y);
  rv = v2 * m1;
  x1 = static_cast<int>(rv.x);
  y1 = static_cast<int>(rv.y);
  rv = v3 * m1;
  x2 = static_cast<int>(rv.x);
  y2 = static_cast<int>(rv.y);*/  

#define f12(x,y) ((y1-y2)*x + (x2-x1)*y + x1*y2 - x2*y1)
#define f20(x,y) ((y2-y0)*x + (x0-x2)*y + x2*y0 - x0*y2)
#define f01(x,y) ((y0-y1)*x + (x1-x0)*y + x0*y1 - x1*y0)

  int xmin = x0 > x1 ? x1 : x0;
  xmin = xmin > x2 ? x2 : xmin;
  
  int ymin = y0 > y1 ? y1 : y0;
  ymin = ymin > y2 ? y2 : ymin;  

  int xmax = x0 > x1 ? x0 : x1;
  xmax = xmax > x2 ? xmax : x2;

  int ymax = y0 > y1 ? y0 : y1;
  ymax = ymax > y2 ? ymax : y2;
  
  for (int v = ymin; v <= ymax; v++) {
    for (int u = xmin; u <= xmax; u++) {
      if ( TestBoundry(u,v) == true ) {    
        double alpha = static_cast<double>(f12(u,v))/static_cast<double>(f12(x0,y0));
        double beta  = static_cast<double>(f20(u,v))/static_cast<double>(f20(x1,y1));
        double gamma = static_cast<double>(f01(u,v))/static_cast<double>(f01(x2,y2));
        if ( alpha >= 0 && alpha <= 1 && 
             beta  >= 0 && beta  <= 1 && 
             gamma >= 0 && gamma <= 1) {
          double xindex = static_cast<double>(u-xmin)/static_cast<double>(xmax-xmin); 
          double yindex = static_cast<double>(v-ymin)/static_cast<double>(ymax-ymin);         
          FillPixel(u,v,xindex,yindex);
        }
      }
    }
  }
  

#undef f12
#undef f20
#undef f01

}

void GDI::DrawLine(int x0, int y0, int x1, int y1) {
  
  //int xmin = x0 > x1 ? x1 : x0;
 // int xmax = x0 > x1 ? x0 : x1;
  
 // int ymin = y0 > y1 ? y1 : y0;
 // int ymax = y0 > y1 ? y0 : y1;
  
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep == true) {
    //swap_int(x0,y0);
    //swap_int(x1,y1);
    std::swap<int>(x0,y0);
    std::swap<int>(x1,y1);    
  }
  
  if (x0 > x1) {
    std::swap<int>(x0,x1);
    std::swap<int>(y0,y1);
  }

  
  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = deltax / 2;
  int ystep;
  int y = y0;
  if (y0 < y1) 
    ystep = 1;
  else 
    ystep = -1;
    
  for (int x = x0; x <= x1; x++) {
    double xindex = static_cast<double>(x-x0)/static_cast<double>(x1-x0); 
    double yindex = static_cast<double>(y-y0)/static_cast<double>(y1-y0);         
    if (steep == true) {
      FillPixel(y,x,xindex,yindex);
    }
    else {
      FillPixel(x,y,xindex,yindex);
    }
    
    error = error - deltay;
    if (error < 0) {
      y = y + ystep;
      error = error + deltax;  
    }
    
  }
  

  /*double slope = (y1-y0)/(x1-x0);
  for (int i = i0; i <= i1; i++) {
    double ry = (slope*i)+y0;
    int y = static_cast<int>(ry);
    double xindex = static_cast<double>(i-xmin)/static_cast<double>(xmax-xmin); 
    double yindex = static_cast<double>(y-ymin)/static_cast<double>(ymax-ymin);         
    FillPixel(u,v,xindex,yindex);
  }*/

}

}
