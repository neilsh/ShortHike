//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef UTIL_MATH_INLINE_HPP
#define UTIL_MATH_INLINE_HPP


template<class NumClass>
inline NumClass clamp(NumClass original, NumClass min, NumClass max)
{
  if(original < min) return min;
  if(original > max) return max;
  return original;
}


inline float frand()
{
  return (float)rand()/(float)RAND_MAX;
}


inline float fnrand()
{
  return (frand() - 0.5f) * 2.0f;
}  

#endif
