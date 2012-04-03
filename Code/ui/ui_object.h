#ifndef UISYSTEM_UI_OBJECT_H
#define UISYSTEM_UI_OBJECT_H

#include "ui.h"

namespace ui {

class Object {
 public:
  explicit Object(UserInterface& ui) : ui_(ui) {}
  virtual ~Object() {}
  UserInterface& ui() { return ui_; }  
  int  left()  { return left_; } 
  void set_left(int left) { left_ = left; }
  int  top() { return top_; } 
  void set_top(int top) { top_ = top; }
  int  width() { return width_; } 
  void set_width(int width) { width_ = width; }
  int  height() { return height_; } 
  void set_height(int height) { height_ = height; }
  virtual void Draw() = 0;
 protected:
  int left_;
  int top_;
  int width_;
  int height_;
 private:
  UserInterface& ui_;
};

}

#endif