//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.
//--------------------------------------------------------------------------------

private import std.math;
private import Math;
private import Angle;
private import Matrix;
private import Vector;
private import Log;

// --------------------------------------------------------------------------------

Quaternion
createQuaternion(float _w, float _x, float _y, float _z)
{
  Quaternion q;
  q.w = _w;
  q.x = _x;
  q.y = _y;
  q.z = _z;
  return q;
}

Quaternion
createQuaternion(Vector axis, float angle)
{
  // Assert: axis is unit length
  // The quaternion representing the rotation is
  //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
  Quaternion* q = new Quaternion;
  float halfAngle = 0.5f * angle;
  float halfSin = sin(halfAngle);
  q.w = cos(halfAngle);
  q.x = halfSin * axis.x;
  q.y = halfSin * axis.y;
  q.z = halfSin * axis.z;
  return *q;
}




struct Quaternion
{
public:
  static assert(Quaternion.sizeof == 16);

  // --------------------------------------------------------------------------------

  float
  opIndex(int index)
  {
    return q[index];
  }

  float
  opIndexAssign(float value, int index)
  {
    return q[index] = value;
  }


  // --------------------------------------------------------------------------------
  

  float
  length()
  {
    return sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
  }

  // --------------------------------------------------------------------------------

  void
  normalize()
  {
    float fLength = length();
  
    if(fLength > Math.ZERO_TOLERANCE ) {
      float fInvLength = 1.0f / fLength;
      q[0] *= fInvLength;
      q[1] *= fInvLength;
      q[2] *= fInvLength;
      q[3] *= fInvLength;
    }
    else {
      q[0] = 0.0f;
      q[1] = 0.0f;
      q[2] = 0.0f;
      q[3] = 0.0f;
    }
  }

  // --------------------------------------------------------------------------------

  char[]
  toString()
  {
    char[] string = "q:(";
    for(int iCol = 0; iCol < 4; ++iCol) {
      string ~= std.string.toString(q[iCol]);
      if(iCol < 3)
        string ~= ", ";
    }
    string ~= ")";
    return string;
  }

  // --------------------------------------------------------------------------------
  // The data itself

  union 
  {
    struct
    {
      float w, x, y, z;
    }
    float q[4];
  }
}

Quaternion
opMul(Quaternion lhs, Quaternion rhs)
{
    Quaternion product;

    product[0] = lhs[0]*rhs[0] - lhs[1]*rhs[1] - lhs[2]*rhs[2] - lhs[3]*rhs[3];
    product[1] = lhs[0]*rhs[1] + lhs[1]*rhs[0] + lhs[2]*rhs[3] - lhs[3]*rhs[2];
    product[2] = lhs[0]*rhs[2] + lhs[2]*rhs[0] + lhs[3]*rhs[1] - lhs[1]*rhs[3];
    product[3] = lhs[0]*rhs[3] + lhs[3]*rhs[0] + lhs[1]*rhs[2] - lhs[2]*rhs[1];

    return product;
}

// --------------------------------------------------------------------------------
// Constant values

const Quaternion IDENTITY = {q:[1, 0, 0, 0]};
const Quaternion ZERO = {q:[0, 0, 0, 0]};
  
