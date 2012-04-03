#include "ui.h"

namespace ui {

UserInterface::UserInterface(graphics::GDI& graphics) : graphics_ (graphics) {
    
}

UserInterface::~UserInterface() {

}

void UserInterface::Init() {
  
}

void UserInterface::Render() {

  std::vector<Object*>::iterator i;
  
  for (i = object_list_.begin(); i != object_list_.end(); i++) {
    (*i)->Draw();
  }
  
  //int x = static_cast<int>(dx*100);

  


}
  
}