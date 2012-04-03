#ifndef UISYSTEM_MATH_MATRIX_H
#define UISYSTEM_MATH_MATRIX_H

namespace math {


struct matrix_2x2 {

  matrix_2x2() {
    identity();
  }

 
  double& operator ()(int row,int column) {
    return m[row][column];
  }
  
  void identity() {
    m00 = m11 = 1;
    m01 = m10 = 0;
  }
  
  matrix_2x2 inverse() {
    matrix_2x2 result;
    double det = 1/(m00*m11-m01*m10);
    result(0,0) = det*m11;
    result(0,1) = -det*m01;
    result(1,0) = -det*m10;
    result(1,1) = det*m00;
    
    return result;
  }
  
  
  union {
    double m[2][2];
    struct {
      double m00;
      double m01;
      double m10;
      double m11;
    };
  };
};

}

#endif