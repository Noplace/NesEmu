#include <stdint.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>


/*
  auto img_scale_nearest_2x = [](uint32_t* in_rgb,int in_width,int in_height,uint32_t* output) {
    for (int y=1;y<in_height-1;++y) {
      for (int x=1;x<in_width-1;++x) {
        auto C = in_rgb[(x+0)+((y+0)*in_width)];
    
          output[(x<<1)+(y<<2)*in_width] = C;
          output[(1+(x<<1))+(y<<2)*in_width] = C;
          output[(x<<1)+(2+(y<<2))*in_width] = C;
          output[(1+(x<<1))+(2+(y<<2))*in_width] = C;

        
      }
    }
  };*/


//    first:        |Then 
//. . . --\ CC  |S T U  --\ 1 2
//. C . --/ CC  |V C W  --/ 3 4
//. . .         |X Y Z
//              | IF V==S==T => 1=S
//              | IF T==U==W => 2=U
//              | IF V==X==Y => 3=X
//              | IF W==Z==Y => 4=Z
//

 

void img_scale_eagle(uint32_t* in_rgb,int in_width,int in_height,uint32_t* output) {
    for (int y=1;y<in_height-1;++y) {
      auto line = &in_rgb[y*in_width];
      for (int x=1;x<in_width-1;++x) {
        auto S = (line-in_width)[x-1];
        auto T = (line-in_width)[x+0];
        auto U = (line-in_width)[x+1];
        auto V = line[x-1];
        auto C = line[x+0];
        auto W = line[x+1];
        auto X = (line+in_width)[x-1];
        auto Y = (line+in_width)[x+0];
        auto Z = (line+in_width)[x+1];

        auto test = (V==S && V==T);

        output[(x<<1)+(y<<2)*in_width] = S*test + C*(1-test);
        output[(1+(x<<1))+(y<<2)*in_width] = U*((T==U && T==W)) + C*(1-(T==U && T==W));
        output[(x<<1)+(2+(y<<2))*in_width] = X*((V==X && X==Y)) + C*(1-(V==X && X==Y));
        output[(1+(x<<1))+(2+(y<<2))*in_width] = Z*((W==Z && W==Y)) + C*(1-(W==Z && W==Y));
        
      }
    }
  };


void ScaleImageEagle2X(uint32_t* in_rgb,int in_width,int in_height,uint32_t* output) {
  #define S  (line-in_width)[x-1]
  #define T  (line-in_width)[x+0]
  #define U  (line-in_width)[x+1]
  #define V  line[x-1]
  #define C  line[x+0]
  #define W  line[x+1]
  #define X  (line+in_width)[x-1]
  #define Y  (line+in_width)[x+0]
  #define Z  (line+in_width)[x+1]
  const __m128i set_one = _mm_set1_epi32(1);
  
  for (int y=1;y<in_height-1;++y) {
    auto line = &in_rgb[y*in_width];
    auto output_line1 = &output[2+(y<<1)*(in_width<<1)];
    auto output_line2 = &output[2+(1+(y<<1))*(in_width<<1)];
    for (int x=1;x<in_width-1;++x) {
      __m128i r1,r2,r3;
      const __m128i C_splat = _mm_set1_epi32(C);
        
      r1 = _mm_set_epi32(V,T,V,W);
      r2 = _mm_set_epi32(S,U,X,Z);
      r3 = _mm_set_epi32(T,W,Y,Y);

      auto factor = _mm_and_si128(
        _mm_and_si128(_mm_cmpeq_epi32(r1,r2),set_one),
        _mm_and_si128(_mm_cmpeq_epi32(r2,r3),set_one));
      auto inv_factor = _mm_sub_epi32(set_one,factor);

      auto pixels = _mm_add_epi32(_mm_mullo_epi32(r2,factor),_mm_mullo_epi32(C_splat,inv_factor));

      *output_line1++ = pixels.m128i_u32[3];
      *output_line1++ = pixels.m128i_u32[2];
      *output_line2++ = pixels.m128i_u32[1];
      *output_line2++ = pixels.m128i_u32[0];
    }
  }
#undef S
#undef T
#undef U
#undef V
#undef W
#undef X
#undef Y
#undef Z
#undef C
};