//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef MATH_RAY_HPP
#define MATH_RAY_HPP

#include "Vector.hpp"

class Ray
{
public:
  Ray(const Vector& _origin = Vector::ORIGIN, const Vector& _direction = Vector::ZERO)
  {
    origin = _origin;
    direction = _direction;
  }
  
  inline const Vector& getOrigin() const {return origin;}
  inline const Vector& getDirection() const {return direction;}

  Vector origin;
  Vector direction;
};

#endif
