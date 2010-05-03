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
// This vector struct assumes a w = 1.

private import Common;
private import std.math;
private import Matrix;
private import Quaternion;


// --------------------------------------------------------------------------------

Vector
createVector(float _x, float _y, float _z)
{
  Vector v;
  v.x = _x;
  v.y = _y;
  v.z = _z;
  return v;
}
  

struct Vector
{
  // Check layout
  static assert(Vector.sizeof == 12);            

  // --------------------------------------------------------------------------------

  float
  opIndex(int index)
  {
    return v[index];
  }

  float
  opIndexAssign(float value, int index)
  {
    return v[index] = value;
  }

  // --------------------------------------------------------------------------------

  bool
  opEquals(Vector rhs)
  {
    return cast(bool)((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
  }

  int
  opCmp(Vector rhs)
  {
    if(*this == rhs) return 0;
    if(x < rhs.x) return -1;
    if(y < rhs.y) return -1;
    if(z < rhs.z) return -1;
    return +1;
  }


// --------------------------------------------------------------------------------

  Vector
  opAddAssign(Vector rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vector
  opSubAssign(Vector rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;  
  }
  

  // --------------------------------------------------------------------------------

  Vector
  opNeg()
  {
    Vector negated;
    negated.x = -x;
    negated.y = -y;
    negated.z = -z;
    return negated;
  }

  Vector
  opMul(float scalar)
  {
    Vector scaled;
    scaled.x = x * scalar;
    scaled.y = y * scalar;
    scaled.z = z * scalar;
    return scaled;
  }

  // --------------------------------------------------------------------------------

  Vector
  opAdd(Vector rhs)
  {
    return createVector(x + rhs.x, y + rhs.y, z + rhs.z);
  }
  
  Vector
  opSub(Vector rhs)
  {
    return createVector(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  Vector
  opMulAssign(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  Vector
  opDivAssign(float scalar)
  {
    float invScalar = 1 / scalar;
    return *this *= invScalar;
  }


  // --------------------------------------------------------------------------------

  Vector
  opMul(Matrix rhs)
  {
    Vector result;
    for(int iCol = 0; iCol < 3; ++iCol) {
      result[iCol] = 0;
      for(int iMid = 0; iMid < 3; ++iMid) {
        result.v[iCol] += v[iMid] * rhs[iMid, iCol];
      }
      result.v[iCol] += rhs[3, iCol];
    }
    if(rhs._14 != 0 || rhs._24 != 0 || rhs._34 != 0 || rhs._44 != 1.0f) {
      // FIXME: not tested yet
      float tempW = 0;
      for(int iMid = 0; iMid < 3; ++iMid) {
        tempW += v[iMid] * rhs[iMid, 3];
      }
      tempW += rhs[3, 3];
      for(int iCol = 0; iCol < 3; ++iCol) {
        result.v[iCol] *= tempW;
      }
//       CHECK_FAIL("Not tested");
    }
  
    return result;
  }

  Vector
  opMul(Quaternion rhs)
  {
    Matrix rotation = createMatrix(rhs);
    return *this * rotation;
  }

  // --------------------------------------------------------------------------------

  void
  normalize()
  {
    *this /= length();
  }


  float
  length()
  {
    return sqrt(squaredLength());
  }
  
  float
  squaredLength()
  {
    return x * x + y * y + z * z;
  }
  
  float
  dotProduct(Vector rhs)
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }
  
  Vector
  crossProduct(Vector rhs)
  {
    // #define CROSS(dest,v,v2) \
    //           dest[0]=v[1]*v2[2]-v[2]*v2[1]; \
    //           dest[1]=v[2]*v2[0]-v[0]*v2[2]; \
    //           dest[2]=v[0]*v2[1]-v[1]*v2[0];
    return createVector(v[1] * rhs.v[2] - v[2] * rhs.v[1],
                        v[2] * rhs.v[0] - v[0] * rhs.v[2],
                        v[0] * rhs.v[1] - v[1] * rhs.v[0]);
    
  }

  // --------------------------------------------------------------------------------

  char[]
  toString()
  {
    char[] string = "v:(";
    for(int iCol = 0; iCol < 3; ++iCol) {
      string ~= std.string.toString(v[iCol]);
      if(iCol < 2)
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
      float x, y, z;
    };
    float[3] v;
  };

  // --------------------------------------------------------------------------------

  unittest
  {
  }
  
}

// --------------------------------------------------------------------------------

const Vector ZERO = {v:[0, 0, 0]};
const Vector ORIGIN = {v:[0, 0, 0]};
const Vector UNIT_X = {v:[1, 0, 0]};
const Vector UNIT_Y = {v:[0, 1, 0]};
const Vector UNIT_Z = {v:[0, 0, 1]};

