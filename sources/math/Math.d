//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import std.random;

float radFromDeg(float deg)
{
  return deg * PI / 180;
}


float clamp(float value, float min, float max)
{
  if(value < min) value = min;
  if(value > max) value = max;
  return value;  
}

int clamp(int value, int min, int max)
{
  if(value < min) value = min;
  if(value > max) value = max;
  return value;  
}


float frand()
{
  return cast(float)rand()/cast(float)uint.max;
}


float fnrand()
{
  return (frand() - 0.5f) * 2.0f;
}  

float max(float lhs, float rhs)
{
  if(lhs > rhs)
    return lhs;
  else
    return rhs;
}


export float EPSILON = 0.000001;
export float ZERO_TOLERANCE = 1e-06f;


