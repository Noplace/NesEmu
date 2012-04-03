#ifndef UISYSTEM_MATH_VECTOR_H
#define UISYSTEM_MATH_VECTOR_H

namespace math {

struct vector_1x2 {
  /*vector_1x2() {
    v0 = v1 = 0;
  }

  //vector_1x2(double v0_,double v1_) : v0(v0_), v1(v1_) {}
  vector_1x2(const double v0_,const double v1_) : v0(v0_), v1(v1_) {}
  //vector_1x2(double v_[]) : v0(v_[0]), v1(v_[1]) {}
  vector_1x2(const double v_[]) : v0(v_[0]), v1(v_[1]) {}
  
  vector_1x2& operator=(int v_[]) {
    v0 = v_[0];
    v1 = v_[1];
    return *this;
  }*/
  
  double& operator ()(int index) {
    return v[index];
  }
  
  double dot_product(const vector_1x2& rhs) {
    return this->v0*rhs.v0 + this->v1*rhs.v1;
  }
  
  union {
    double v[2];
    struct {
      double v0;
      double v1;
    };
    struct {
      double x;
      double y;
    };
    
  };
  
};

}

#endif