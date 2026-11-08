#pragma once

// very simple 3-component vector

#include <math.h>

class Vector3D
{
public:
   float x,y,z;

   //! construct a zero vector
   Vector3D() : x(0.0f), y(0.0f), z(0.0f) { }

   //! construct initialized vector x,y,z
   Vector3D(float x, float y, float z)
      : x(x), y(y), z(z)
   {
   }

   //! add vectors
   Vector3D operator + (const Vector3D& v) const
   {
      return Vector3D(x+v.x, y+v.y, z+v.z);
   }

   //! subtract vectors
   Vector3D operator - (const Vector3D& v) const
   {
      return Vector3D(x-v.x, y-v.y, z-v.z);
   }

   //! dot product
   float operator * (const Vector3D& v) const
   {
      return x*v.x + y*v.y + z*v.z;
   }

   //! cross product
   Vector3D operator % (const Vector3D& v) const
   {
      return Vector3D( (y*v.z - z*v.y), (z*v.x - x*v.z), (x*v.y - y*v.x) );
   }

   //! normalize vector to given length
   void normalize(float len = 1.0f)
   {
      const float lengthSquared = *this * *this;
      if (lengthSquared == 0.0f)
      {
         return;
      }
      float t = static_cast<float>(len / sqrt(lengthSquared));
      x*= t;
      y*= t;
      z*= t;
   }
};

