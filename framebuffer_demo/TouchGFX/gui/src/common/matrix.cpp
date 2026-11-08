#include "gui/common/matrix.hpp"

Matrix Matrix::rotateX(float a)
{
   Matrix dst;
   const float c= (float)cos(a);
   const float s= (float)sin(a);

   dst.xx = 1; dst.xy = 0; dst.xz =  0; dst.xw = 0;
   dst.yx = 0; dst.yy = c; dst.yz =  s; dst.yw = 0;
   dst.zx = 0; dst.zy =-s; dst.zz =  c; dst.zw = 0;
   return dst;
}

Matrix Matrix::rotateY(float a)
{
   Matrix dst;
   const float c= (float)cos(a);
   const float s= (float)sin(a);

   dst.xx =  c; dst.xy = 0; dst.xz = s; dst.xw = 0;
   dst.yx =  0; dst.yy = 1; dst.yz = 0; dst.yw = 0;
   dst.zx = -s; dst.zy = 0; dst.zz = c; dst.zw = 0;

   return dst;
}

Matrix Matrix::rotateZ(float a)
{
   Matrix dst;
   const float c= (float)cos(a);
   const float s= (float)sin(a);

   dst.xx = c;  dst.xy =-s; dst.xz = 0; dst.xw = 0;
   dst.yx = s;  dst.yy = c; dst.yz = 0; dst.yw = 0;
   dst.zx = 0;  dst.zy = 0; dst.zz = 1; dst.zw = 0;

   return dst;
}

Matrix Matrix::transpose3x3() const
{
   Matrix dst;
   dst.xx = xx;  dst.xy = yx;  dst.xz = zx;  dst.xw = 0;
   dst.yx = xy;  dst.yy = yy;  dst.yz = zy;  dst.yw = 0;
   dst.zx = xz;  dst.zy = yz;  dst.zz = zz;  dst.zw = 0;
   return dst;
}

void Matrix::setTranslation(float x, float y, float z)
{
   xw= x;
   yw= y;
   zw= z;
}
