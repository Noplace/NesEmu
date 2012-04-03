#ifndef UISYSTEM_UI_UI_H
#define UISYSTEM_UI_UI_H


#include <vector>
#include "../graphics/gdi.h"
#include "forward_decl.h"
#include "ui_object.h"

namespace ui {

class UserInterface {
 public:
  explicit UserInterface(graphics::GDI& graphics);
  ~UserInterface();
  graphics::GDI& graphics() { return graphics_; }
  void Init();
  void Render();
 private:
  graphics::GDI& graphics_;
  std::vector<Object*> object_list_;
};

}

#endif