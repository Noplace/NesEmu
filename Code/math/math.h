#ifndef UISYSTEM_MATH_MATH_H
#define UISYSTEM_MATH_MATH_H

#include "vector.h"
#include "matrix.h"

namespace math {
 
   
inline matrix_2x2 multiply_2x2_2x2(const matrix_2x2& lhs, const matrix_2x2& rhs) {
  matrix_2x2 result;
  result.m00 = lhs.m00*rhs.m00+lhs.m01*rhs.m10;
  result.m01 = lhs.m00*rhs.m01+lhs.m01*rhs.m11;
  result.m10 = lhs.m10*rhs.m00+lhs.m11*rhs.m10;
  result.m11 = lhs.m10*rhs.m01+lhs.m11*rhs.m11;
  return result;
}

inline matrix_2x2 operator *(const matrix_2x2& lhs, const matrix_2x2& rhs) {
  return multiply_2x2_2x2(lhs,rhs);
}

inline vector_1x2 multiply_2x2_1x2(const matrix_2x2& lhs, const vector_1x2& rhs) {
  vector_1x2 result;
  result.v0 = lhs.m00*rhs.v0+lhs.m01*rhs.v1;
  result.v1 = lhs.m10*rhs.v0+lhs.m11*rhs.v1;
  return result;
}

inline vector_1x2 operator *(const matrix_2x2& lhs, const vector_1x2& rhs) {
  return multiply_2x2_1x2(lhs,rhs);
}

inline vector_1x2 multiply_1x2_2x2(const vector_1x2&lhs, const matrix_2x2& rhs) {
  vector_1x2 result;
  result.v0 = lhs.v0*rhs.m00+lhs.v1*rhs.m01;
  result.v1 = lhs.v0*rhs.m10+lhs.v1*rhs.m11;
  return result;
}

inline vector_1x2 operator *(const vector_1x2&lhs, const matrix_2x2& rhs) {
  return multiply_1x2_2x2(lhs,rhs);
}
  
  
inline double operator *(const vector_1x2&lhs, const vector_1x2& rhs) {
  return (const_cast<vector_1x2&>(lhs)).dot_product(rhs);
}
  
}

#endif