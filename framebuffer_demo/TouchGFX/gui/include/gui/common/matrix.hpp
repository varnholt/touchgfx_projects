#pragma once


#include <math.h>
#include "vector.hpp"

/*! \brief Matrix implements a very simple 4x3 float matrix
 *
 *  3x3 transform + translation in 4th column
 */
class Matrix
{
public:

   //! constructor
   Matrix(float x= 1.0f, float y= 1.0f, float z= 1.0f)
      : xx(x), xy(0), xz(0), xw(0),
        yx(0), yy(y), yz(0), yw(0),
        zx(0), zy(0), zz(z), zw(0)
   { }

   //! create a rotation matrix for rotation around X
   static Matrix rotateX(float a);

   //! create a rotation matrix for rotation around Y
   static Matrix rotateY(float a);

   //! create a rotation matrix for rotation around Z
   static Matrix rotateZ(float a);

   //! \return matrix * vector
   Vector3D operator * (const Vector3D& v) const
   {
      return Vector3D(
          xx*v.x + xy*v.y + xz*v.z + xw,
          yx*v.x + yy*v.y + yz*v.z + yw,
          zx*v.x + zy*v.y + zz*v.z + zw );
   }

   //! \return matrix*matrix
   Matrix operator * (const Matrix& m) const
   {
      Matrix r;
      r.xx= m.xx*xx + m.xy*yx + m.xz*zx;
      r.xy= m.xx*xy + m.xy*yy + m.xz*zy;
      r.xz= m.xx*xz + m.xy*yz + m.xz*zz;
      r.xw= m.xx*xw + m.xy*yw + m.xz*zw + m.xw;

      r.yx= m.yx*xx + m.yy*yx + m.yz*zx;
      r.yy= m.yx*xy + m.yy*yy + m.yz*zy;
      r.yz= m.yx*xz + m.yy*yz + m.yz*zz;
      r.yw= m.yx*xw + m.yy*yw + m.yz*zw + m.yw;

      r.zx= m.zx*xx + m.zy*yx + m.zz*zx;
      r.zy= m.zx*xy + m.zz*zy + m.zy*yy;
      r.zz= m.zx*xz + m.zy*yz + m.zz*zz;
      r.zw= m.zx*xw + m.zy*yw + m.zz*zw + m.zw;

      return r;
   }

   //! \return 3x3-transpose of matrix, translation is reset
   Matrix transpose3x3() const;

   //! set translation component of matrix to x,y,z
   void setTranslation(float x, float y, float z);

public:
   float xx, xy, xz, xw,
         yx, yy, yz, yw,
         zx, zy, zz, zw;
};
