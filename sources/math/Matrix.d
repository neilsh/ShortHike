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
// --------------------------------------------------------------------------------

private import std.math;
private import Math;
private import Quaternion;
private import Vector;
private import std.string;
private import Log;

// --------------------------------------------------------------------------------

Matrix
createMatrix(float v11, float v12, float v13, float v14,
             float v21, float v22, float v23, float v24,
             float v31, float v32, float v33, float v34,
             float v41, float v42, float v43, float v44)
{
  Matrix m;
  m._11 = v11;
  m._12 = v12;
  m._13 = v13;
  m._14 = v14;
  m._21 = v21;
  m._22 = v22;
  m._23 = v23;
  m._24 = v24;
  m._31 = v31;
  m._32 = v32;
  m._33 = v33;
  m._34 = v34;
  m._41 = v41;
  m._42 = v42;
  m._43 = v43;
  m._44 = v44;
  return m;
}

//--------------------------------------------------------------------------------
// Creates a translation matrix

Matrix
createMatrix(Vector position)
{
  Matrix m;
  m.setIdentity();
  m.setTranslation(position);
  return m;
}

//--------------------------------------------------------------------------------
// Creates a rotation matrix

Matrix
createMatrix(Quaternion orientation)
{
  Matrix m;
  m.setIdentity();
  m.setOrientation(orientation);
  return m;
}

//--------------------------------------------------------------------------------
// Creates a compound rotation and translation matrix

Matrix
createMatrix(Vector position, Quaternion orientation)
{
  Matrix m;
  m.setIdentity();
  m.setTranslation(position);
  m.setOrientation(orientation);
  return m;
}

//--------------------------------------------------------------------------------
// Creates a pure scaling matrix

Matrix
createMatrix(float scaling)
{
  Matrix m;
  m.setIdentity();
  m.setScaling(scaling);
  return m;
}


// --------------------------------------------------------------------------------

struct Matrix
{
public:
  static assert(Matrix.sizeof == 64);  
  
  void
  setTranslation(Vector position)
  {
    _41 = position.x;
    _42 = position.y;
    _43 = position.z;
  }

  void
  setScaling(Vector scaleAxis)
  {
    _11 = scaleAxis.x;
    _22 = scaleAxis.y;
    _33 = scaleAxis.z;
  }

  void
  setScaling(float uniformScale)
  {
    _11 = uniformScale;
    _22 = uniformScale;
    _33 = uniformScale;
  }

  void
  setIdentity()
  {
    _11 = 1;
    _12 = 0;
    _13 = 0;
    _14 = 0;
    
    _21 = 0;
    _22 = 1;
    _23 = 0;
    _24 = 0;
    
    _31 = 0;
    _32 = 0;
    _33 = 1;
    _34 = 0;
    
    _41 = 0;
    _42 = 0;
    _43 = 0;
    _44 = 1;
  }


  //--------------------------------------------------------------------------------
  
  Vector
  getPosition()
  {
    return Vector.createVector(_41, _42, _43);
  }
  
  Vector
  getScaling()
  {
    return Vector.createVector(_11, _22, _33);
  }


  //----------------------------------------------------------------------------

  void
  setOrientation(Quaternion orientation)
  {
    float fTx  = 2.0f * orientation[1];
    float fTy  = 2.0f * orientation[2];
    float fTz  = 2.0f * orientation[3];
    float fTwx = fTx * orientation[0];
    float fTwy = fTy * orientation[0];
    float fTwz = fTz * orientation[0];
    float fTxx = fTx * orientation[1];
    float fTxy = fTy * orientation[1];
    float fTxz = fTz * orientation[1];
    float fTyy = fTy * orientation[2];
    float fTyz = fTz * orientation[2];
    float fTzz = fTz * orientation[3];
    
    // Original Wild Magic code
    //   m[0][0] = 1.0f - (fTyy + fTzz);
    //   m[0][1] = fTxy - fTwz;
    //   m[0][2] = fTxz + fTwy;
    //   m[1][0] = fTxy + fTwz;
    //   m[1][1] = 1.0f - (fTxx + fTzz);
    //   m[1][2] = fTyz - fTwx;
    //   m[2][0] = fTxz - fTwy;
    //   m[2][1] = fTyz + fTwx;
    //   m[2][2] = 1.0f - (fTxx + fTyy);

    // This is transpose in relation to the original code..
    m[0][0] = 1.0f - (fTyy + fTzz);
    m[1][0] = fTxy - fTwz;
    m[2][0] = fTxz + fTwy;
    m[0][1] = fTxy + fTwz;
    m[1][1] = 1.0f - (fTxx + fTzz);
    m[2][1] = fTyz - fTwx;
    m[0][2] = fTxz - fTwy;
    m[1][2] = fTyz + fTwx;
    m[2][2] = 1.0f - (fTxx + fTyy);
  }


  //--------------------------------------------------------------------------------
  
  Quaternion
  getOrientation()
  {
    static int[3] mINext =  [ 1, 2, 0 ];

    Quaternion orientation;
    float trace = m[0][0] + m[1][1] + m[2][2];
    
    if ( trace > 0.0f ) {
      // |w| > 1/2, may as well choose w > 1/2
      float root = sqrt(trace + 1.0f);  // 2w
      orientation[0] = 0.5f * root;
      root = 0.5f / root;  // 1/(4w)
      orientation[1] = (m[1][2] - m[2][1]) * root;
      orientation[2] = (m[2][0] - m[0][2]) * root;
      orientation[3] = (m[0][1] - m[1][0]) * root;
    }
    else {
      // |w| <= 1/2
      int i = 0;
      if ( m[1][1] > m[0][0] )
        i = 1;
      if ( m[2][2] > m[i][i] )
        i = 2;
      int j = mINext[i];
      int k = mINext[j];
      
      float root = sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0f);
      float realPart[3];
      realPart[i] = 0.5f * root;
      root = 0.5f / root;
      orientation[0] = (m[j][k] - m[k][j]) * root;
      realPart[j] = (m[i][j] + m[j][i]) * root;
      realPart[k] = (m[i][k] + m[k][i]) * root;
      
      orientation[1] = realPart[0];
      orientation[2] = realPart[1];
      orientation[3] = realPart[2];
    }
    
    return orientation;
  }

// This is the original version. I needed to transpose all matrix references
// Quaternion
// getOrientation() const
// {
//   static int mINext[3] =  { 1, 2, 0 };

//   Quaternion orientation;
//   float trace = m[0][0] + m[1][1] + m[2][2];

//   if ( trace > 0.0f ) {
//     // |w| > 1/2, may as well choose w > 1/2
//     float root = sqrt(trace + 1.0f);  // 2w
//     orientation[0] = 0.5f * root;
//     root = 0.5f / root;  // 1/(4w)
//     orientation[1] = (m[2][1] - m[1][2]) * root;
//     orientation[2] = (m[0][2] - m[2][0]) * root;
//     orientation[3] = (m[1][0] - m[0][1]) * root;
//   }
//   else {
//     // |w| <= 1/2
//     int i = 0;
//     if ( m[1][1] > m[0][0] )
//       i = 1;
//     if ( m[2][2] > m[i][i] )
//       i = 2;
//     int j = mINext[i];
//     int k = mINext[j];
    
//     float root = sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0f);
//     float realPart[3];
//     realPart[i] = 0.5f * root;
//     root = 0.5f / root;
//     orientation[0] = (m[k][j] - m[j][k]) * root;
//     realPart[j] = (m[j][i] + m[i][j]) * root;
//     realPart[k] = (m[k][i] + m[i][k]) * root;
    
//     orientation[1] = realPart[0];
//     orientation[2] = realPart[1];
//     orientation[3] = realPart[2];
//   }

//   return orientation;
// }

//--------------------------------------------------------------------------------
  
  Matrix
  getRotationMatrix()
  {
    Matrix result;
    result.setIdentity();
    for(int iRow = 0; iRow < 3; ++iRow)
      for(int iCol = 0; iCol < 3; ++iCol)
      result[iRow,iCol] = m[iRow][iCol];
    return result;
  }



  //--------------------------------------------------------------------------------
  void
  transposeRotation()
  {
    float t12 = _21;
    float t13 = _31;
    float t23 = _32;
    _21 = _12;
    _31 = _13;
    _32 = _23;
    _12 = t12;
    _13 = t13;
    _23 = t23;
  }

  //--------------------------------------------------------------------------------
  
//     float fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
//     float fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
//     float fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
//     float fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
//     float fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
//     float fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
//     float fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
//     float fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
//     float fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
//     float fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
//     float fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
//     float fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];

//     float fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
//     if ( Math<float>::FAbs(fDet) <= Math<float>::ZERO_TOLERANCE )
//         return Matrix4<float>::ZERO;

//     Matrix4 kInv;
//     kInv[0][0] = + m_afEntry[ 5]*fB5 - m_afEntry[ 6]*fB4 + m_afEntry[ 7]*fB3;
//     kInv[1][0] = - m_afEntry[ 4]*fB5 + m_afEntry[ 6]*fB2 - m_afEntry[ 7]*fB1;
//     kInv[2][0] = + m_afEntry[ 4]*fB4 - m_afEntry[ 5]*fB2 + m_afEntry[ 7]*fB0;
//     kInv[3][0] = - m_afEntry[ 4]*fB3 + m_afEntry[ 5]*fB1 - m_afEntry[ 6]*fB0;
//     kInv[0][1] = - m_afEntry[ 1]*fB5 + m_afEntry[ 2]*fB4 - m_afEntry[ 3]*fB3;
//     kInv[1][1] = + m_afEntry[ 0]*fB5 - m_afEntry[ 2]*fB2 + m_afEntry[ 3]*fB1;
//     kInv[2][1] = - m_afEntry[ 0]*fB4 + m_afEntry[ 1]*fB2 - m_afEntry[ 3]*fB0;
//     kInv[3][1] = + m_afEntry[ 0]*fB3 - m_afEntry[ 1]*fB1 + m_afEntry[ 2]*fB0;
//     kInv[0][2] = + m_afEntry[13]*fA5 - m_afEntry[14]*fA4 + m_afEntry[15]*fA3;
//     kInv[1][2] = - m_afEntry[12]*fA5 + m_afEntry[14]*fA2 - m_afEntry[15]*fA1;
//     kInv[2][2] = + m_afEntry[12]*fA4 - m_afEntry[13]*fA2 + m_afEntry[15]*fA0;
//     kInv[3][2] = - m_afEntry[12]*fA3 + m_afEntry[13]*fA1 - m_afEntry[14]*fA0;
//     kInv[0][3] = - m_afEntry[ 9]*fA5 + m_afEntry[10]*fA4 - m_afEntry[11]*fA3;
//     kInv[1][3] = + m_afEntry[ 8]*fA5 - m_afEntry[10]*fA2 + m_afEntry[11]*fA1;
//     kInv[2][3] = - m_afEntry[ 8]*fA4 + m_afEntry[ 9]*fA2 - m_afEntry[11]*fA0;
//     kInv[3][3] = + m_afEntry[ 8]*fA3 - m_afEntry[ 9]*fA1 + m_afEntry[10]*fA0;

//     float fInvDet = ((float)1.0)/fDet;
//     for (int iRow = 0; iRow < 4; iRow++)
//     {
//         for (int iCol = 0; iCol < 4; iCol++)
//             kInv[iRow][iCol] *= fInvDet;
//     }

//     return kInv;

  Matrix
  inverse()
  {
    float fA0 = mRaw[ 0]*mRaw[ 5] - mRaw[ 1]*mRaw[ 4];
    float fA1 = mRaw[ 0]*mRaw[ 6] - mRaw[ 2]*mRaw[ 4];
    float fA2 = mRaw[ 0]*mRaw[ 7] - mRaw[ 3]*mRaw[ 4];
    float fA3 = mRaw[ 1]*mRaw[ 6] - mRaw[ 2]*mRaw[ 5];
    float fA4 = mRaw[ 1]*mRaw[ 7] - mRaw[ 3]*mRaw[ 5];
    float fA5 = mRaw[ 2]*mRaw[ 7] - mRaw[ 3]*mRaw[ 6];
    float fB0 = mRaw[ 8]*mRaw[13] - mRaw[ 9]*mRaw[12];
    float fB1 = mRaw[ 8]*mRaw[14] - mRaw[10]*mRaw[12];
    float fB2 = mRaw[ 8]*mRaw[15] - mRaw[11]*mRaw[12];
    float fB3 = mRaw[ 9]*mRaw[14] - mRaw[10]*mRaw[13];
    float fB4 = mRaw[ 9]*mRaw[15] - mRaw[11]*mRaw[13];
    float fB5 = mRaw[10]*mRaw[15] - mRaw[11]*mRaw[14];
    
    float fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
    if(fabs(fDet) <= Math.ZERO_TOLERANCE)
      return ZERO;
    
    Matrix kInv;
    kInv[0, 0] = + mRaw[ 5]*fB5 - mRaw[ 6]*fB4 + mRaw[ 7]*fB3;
    kInv[1, 0] = - mRaw[ 4]*fB5 + mRaw[ 6]*fB2 - mRaw[ 7]*fB1;
    kInv[2, 0] = + mRaw[ 4]*fB4 - mRaw[ 5]*fB2 + mRaw[ 7]*fB0;
    kInv[3, 0] = - mRaw[ 4]*fB3 + mRaw[ 5]*fB1 - mRaw[ 6]*fB0;
    kInv[0, 1] = - mRaw[ 1]*fB5 + mRaw[ 2]*fB4 - mRaw[ 3]*fB3;
    kInv[1, 1] = + mRaw[ 0]*fB5 - mRaw[ 2]*fB2 + mRaw[ 3]*fB1;
    kInv[2, 1] = - mRaw[ 0]*fB4 + mRaw[ 1]*fB2 - mRaw[ 3]*fB0;
    kInv[3, 1] = + mRaw[ 0]*fB3 - mRaw[ 1]*fB1 + mRaw[ 2]*fB0;
    kInv[0, 2] = + mRaw[13]*fA5 - mRaw[14]*fA4 + mRaw[15]*fA3;
    kInv[1, 2] = - mRaw[12]*fA5 + mRaw[14]*fA2 - mRaw[15]*fA1;
    kInv[2, 2] = + mRaw[12]*fA4 - mRaw[13]*fA2 + mRaw[15]*fA0;
    kInv[3, 2] = - mRaw[12]*fA3 + mRaw[13]*fA1 - mRaw[14]*fA0;
    kInv[0, 3] = - mRaw[ 9]*fA5 + mRaw[10]*fA4 - mRaw[11]*fA3;
    kInv[1, 3] = + mRaw[ 8]*fA5 - mRaw[10]*fA2 + mRaw[11]*fA1;
    kInv[2, 3] = - mRaw[ 8]*fA4 + mRaw[ 9]*fA2 - mRaw[11]*fA0;
    kInv[3, 3] = + mRaw[ 8]*fA3 - mRaw[ 9]*fA1 + mRaw[10]*fA0;
    
    
    float fInvDet = 1.0f/fDet;
    for (int iRow = 0; iRow < 4; iRow++)
      for (int iCol = 0; iCol < 4; iCol++)
      kInv[iRow, iCol] = kInv[iRow, iCol] * fInvDet;
    
    return kInv;
  }

  //--------------------------------------------------------------------------------


  Matrix
  opMul(Matrix rhs)
  {
    Matrix product;
    for(int iRow = 0; iRow < 4; iRow++) {
      for(int iCol = 0; iCol < 4; iCol++) {
        product[iRow, iCol] = 0;
        for(int iMid = 0; iMid < 4; iMid++) {
          product[iRow, iCol] = product[iRow, iCol] + m[iRow][iMid] * rhs[iMid, iCol];
        }
      }
    }
    return product;
  }



//--------------------------------------------------------------------------------

  float
  opIndex(int row, int column)
  {
    return m[row][column];
  }

  float
  opIndexAssign(float value, int row, int column)
  {
    return m[row][column] = value;
  }

  // --------------------------------------------------------------------------------

  char[]
  toString()
  {
    char[] string = "m:";
    for(int iRow = 0; iRow < 4; ++iRow) {
      for(int iCol = 0; iCol < 4; ++iCol) {
        string ~= std.string.toString(m[iRow][iCol]) ~ " ";
      }
      string ~= "\n";
    }
    return string;
  }
  
  // --------------------------------------------------------------------------------
  // The data itself
  
  union 
  {
    struct
    {
      float _11, _12, _13, _14;
      float _21, _22, _23, _24;
      float _31, _32, _33, _34;
      float _41, _42, _43, _44;
    }
    float mRaw[16];
    float[4][4] m;
  }
}

// --------------------------------------------------------------------------------
  
const Matrix ZERO = { mRaw:[0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0]};
const Matrix IDENTITY = { mRaw:[1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1]};







