#ifndef UISYSTEM_UI_WINDOW_H
#define UISYSTEM_UI_WINDOW_H

namespace ui {

class Window : public Object {
 public:
  explicit Window(UserInterface& ui) : Object(ui) {}
  ~Window() {}
  virtual void Draw();
 private:
  
};

}

#endif