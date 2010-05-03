//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import std.math;
private import Math;

// --------------------------------------------------------------------------------

class Radian
{
public:
  this(float value)
  {
    mRadValue = value;
  }  

  this(Degree degrees)
  {
    mRadValue = degrees.valueRadians();
  }

  float
  valueDegrees()
  {
    return mRadValue * 180.0f / PI;
  }

  float
  valueRadians()
  {
    return mRadValue;
  }

  Radian opNeg()
  {
    mRadValue = -mRadValue;
    return this;
  }

private:
  float mRadValue;
};

//--------------------------------------------------------------------------------

class Degree
{
public:
  this(float value)
  {
    mDegValue = value;
  }

  this(Radian radians)
  {
    
    mDegValue = radians.valueDegrees();
  }  

  float
  valueDegrees()
  {
    return mDegValue;
  }

  float
  valueRadians()
  {
    return mDegValue * PI / 180;
  }
  
  Degree opNeg()
  {
    mDegValue = -mDegValue;
    return this;
  }
private:
  float mDegValue;
};
