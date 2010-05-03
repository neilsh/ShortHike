//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef MATH_ANGLE_HPP
#define MATH_ANGLE_HPP

class Degree;

class Radian
{
public:
  Radian(float value);
  Radian(Degree value);

  float valueDegrees() const;
  float valueRadians() const;
  const Radian operator-() {mRadValue = -mRadValue; return *this;}  
  
private:
  float mRadValue;
};

class Degree
{
public:
  Degree(float value);
  Degree(Radian value);

  float valueDegrees() const;
  float valueRadians() const;
  const Degree operator-() {mDegValue = -mDegValue; return *this;}
private:
  float mDegValue;
};


//--------------------------------------------------------------------------------

inline Radian::Radian(float value)
    : mRadValue(value) {}  

inline Radian::Radian(Degree degrees)
    : mRadValue(degrees.valueRadians()) {}

inline float
Radian::valueDegrees() const
{
  return mRadValue * 180.0f / Math::PI;
}

inline float
Radian::valueRadians() const
{
  return mRadValue;
}


//--------------------------------------------------------------------------------


inline Degree::Degree(float value)
  : mDegValue(value) {}

inline Degree::Degree(Radian radians)
  : mDegValue(radians.valueDegrees()) {}  

inline float
Degree::valueDegrees() const
{
  return mDegValue;
}

inline float
Degree::valueRadians() const
{
  return mDegValue * Math::PI / 180;
}


#endif
