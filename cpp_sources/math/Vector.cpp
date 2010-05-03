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
// This vector class assumes a w = 1.

#include "Common.hpp"
#include "Vector.hpp"

// --------------------------------------------------------------------------------

Vector::Vector(float _x, float _y, float _z)
  : x(_x), y(_y), z(_z)
{
}


// --------------------------------------------------------------------------------

const bool
Vector::operator==(const Vector& rhs) const
{
  return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

const bool
Vector::operator!=(const Vector& rhs) const
{
  return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

const bool
Vector::operator<(const Vector& rhs) const
{
  if(x != rhs.x) return x < rhs.x;
  if(y != rhs.y) return y < rhs.y;
  return z < rhs.z;
}


// --------------------------------------------------------------------------------

const Vector
Vector::operator+=(const Vector& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

const Vector
Vector::operator-=(const Vector& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;  
}


// --------------------------------------------------------------------------------

const Vector
Vector::operator-() const
{
  Vector negated;
  negated.x = -x;
  negated.y = -y;
  negated.z = -z;
  return negated;
}

const Vector
Vector::operator*(float scalar) const
{
  Vector scaled;
  scaled.x = x * scalar;
  scaled.y = y * scalar;
  scaled.z = z * scalar;
  return scaled;
}



// --------------------------------------------------------------------------------

const Vector Vector::ZERO(0.0f, 0.0f, 0.0f);
const Vector Vector::ORIGIN(0.0f, 0.0f, 0.0f);
const Vector Vector::UNIT_X(1.0f, 0.0f, 0.0f);
const Vector Vector::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vector Vector::UNIT_Z(0.0f, 0.0f, 1.0f);

namespace Wm3
{
//   template<> const Vector4<float> Vector4<float>::ZERO(0.0f,0.0f,0.0f,0.0f);
//   template<> const Vector4<float> Vector4<float>::UNIT_X(1.0f,0.0f,0.0f,0.0f);
//   template<> const Vector4<float> Vector4<float>::UNIT_Y(0.0f,1.0f,0.0f,0.0f);
//   template<> const Vector4<float> Vector4<float>::UNIT_Z(0.0f,0.0f,1.0f,0.0f);
//   template<> const Vector4<float> Vector4<float>::UNIT_W(0.0f,0.0f,0.0f,1.0f);
  
//   template<> const Vector4<double> Vector4<double>::ZERO(0.0,0.0,0.0,0.0);
//   template<> const Vector4<double> Vector4<double>::UNIT_X(1.0,0.0,0.0,0.0);
//   template<> const Vector4<double> Vector4<double>::UNIT_Y(0.0,1.0,0.0,0.0);
//   template<> const Vector4<double> Vector4<double>::UNIT_Z(0.0,0.0,1.0,0.0);
//   template<> const Vector4<double> Vector4<double>::UNIT_W(0.0,0.0,0.0,1.0);
}

// --------------------------------------------------------------------------------

const Vector
Vector::operator+(const Vector& rhs) const
{
  return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
}

const Vector
Vector::operator-(const Vector& rhs) const
{
  return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
}

const Vector
Vector::operator*=(float scalar)
{
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

const Vector
Vector::operator/=(float scalar)
{
  float invScalar = 1 / scalar;
  return (*this *= invScalar);
}


// --------------------------------------------------------------------------------

const Vector
Vector::operator*(const Matrix& rhs) const
{
  Vector result;
  for(int iCol = 0; iCol < 3; ++iCol) {
    result[iCol] = 0;
    for(int iMid = 0; iMid < 3; ++iMid) {
      result[iCol] += v[iMid] * rhs[iMid][iCol];
    }
    result[iCol] += rhs[3][iCol];
  }
  if(rhs._14 != 0 || rhs._24 != 0 || rhs._34 != 0 || rhs._44 != 1.0f) {
    // FIXME: not tested yet
    float tempW = 0;
    for(int iMid = 0; iMid < 3; ++iMid) {
      tempW += v[iMid] * rhs[iMid][3];
    }
    tempW += rhs[3][3];
    for(int iCol = 0; iCol < 3; ++iCol) {
      result[iCol] *= tempW;
    }
  }
  
  return result;
}

const Vector
Vector::operator*(const Quaternion& rhs) const
{
  Matrix rotation(rhs);
  return *this * rotation;
}

// --------------------------------------------------------------------------------

void
Vector::normalize()
{
  *this /= length();
}


float
Vector::length() const
{
  return sqrt(squaredLength());
}

float
Vector::squaredLength() const
{
  return x * x + y * y + z * z;
}

float
Vector::dotProduct(const Vector& rhs) const
{
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector
Vector::crossProduct(const Vector& rhs) const
{
// #define CROSS(dest,v,v2) \
//           dest[0]=v[1]*v2[2]-v[2]*v2[1]; \
//           dest[1]=v[2]*v2[0]-v[0]*v2[2]; \
//           dest[2]=v[0]*v2[1]-v[1]*v2[0];
  return Vector(v[1] * rhs.v[2] - v[2] * rhs.v[1],
                v[2] * rhs.v[0] - v[0] * rhs.v[2],
                v[0] * rhs.v[1] - v[1] * rhs.v[0]);
  
}


// --------------------------------------------------------------------------------

// float
// Vector::operator[](size_t iRow) const
// {
//   return v[iRow];
// }

// float&
// Vector::operator[](size_t iRow)
// {
//   return v[iRow];
// }




// --------------------------------------------------------------------------------










namespace Wm3 
{

  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>::Vector4 ()
  {
    // uninitialized for performance in array construction
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>::Vector4 (Real fX, Real fY, Real fZ, Real fW)
  {
    m_afTuple[0] = fX;
    m_afTuple[1] = fY;
    m_afTuple[2] = fZ;
    m_afTuple[3] = fW;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>::Vector4 (const Vector4& rkV)
  {
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>::operator const Real* () const
  {
    return m_afTuple;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>::operator Real* ()
  {
    return m_afTuple;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::operator[] (int i) const
  {
    assert( 0 <= i && i <= 3 );
    return m_afTuple[i];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real& Vector4<Real>::operator[] (int i)
  {
    assert( 0 <= i && i <= 3 );
    return m_afTuple[i];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::X () const
  {
    return m_afTuple[0];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real& Vector4<Real>::X ()
  {
    return m_afTuple[0];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::Y () const
  {
    return m_afTuple[1];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real& Vector4<Real>::Y ()
  {
    return m_afTuple[1];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::Z () const
  {
    return m_afTuple[2];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real& Vector4<Real>::Z ()
  {
    return m_afTuple[2];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::W () const
  {
    return m_afTuple[3];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real& Vector4<Real>::W ()
  {
    return m_afTuple[3];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>& Vector4<Real>::operator= (const Vector4& rkV)
  {
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
    return *this;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  int Vector4<Real>::CompareArrays (const Vector4& rkV) const
  {
    return memcmp(m_afTuple,rkV.m_afTuple,4*sizeof(Real));
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator== (const Vector4& rkV) const
  {
    return CompareArrays(rkV) == 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator!= (const Vector4& rkV) const
  {
    return CompareArrays(rkV) != 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator< (const Vector4& rkV) const
  {
    return CompareArrays(rkV) < 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator<= (const Vector4& rkV) const
  {
    return CompareArrays(rkV) <= 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator> (const Vector4& rkV) const
  {
    return CompareArrays(rkV) > 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  bool Vector4<Real>::operator>= (const Vector4& rkV) const
  {
    return CompareArrays(rkV) >= 0;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> Vector4<Real>::operator+ (const Vector4& rkV) const
  {
    return Vector4(
                   m_afTuple[0]+rkV.m_afTuple[0],
                   m_afTuple[1]+rkV.m_afTuple[1],
                   m_afTuple[2]+rkV.m_afTuple[2],
                   m_afTuple[3]+rkV.m_afTuple[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> Vector4<Real>::operator- (const Vector4& rkV) const
  {
    return Vector4(
                   m_afTuple[0]-rkV.m_afTuple[0],
                   m_afTuple[1]-rkV.m_afTuple[1],
                   m_afTuple[2]-rkV.m_afTuple[2],
                   m_afTuple[3]-rkV.m_afTuple[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> Vector4<Real>::operator* (Real fScalar) const
  {
    return Vector4(
                   fScalar*m_afTuple[0],
                   fScalar*m_afTuple[1],
                   fScalar*m_afTuple[2],
                   fScalar*m_afTuple[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> Vector4<Real>::operator/ (Real fScalar) const
  {
    Vector4 kQuot;

    if ( fScalar != (Real)0.0 )
      {
        Real fInvScalar = ((Real)1.0)/fScalar;
        kQuot.m_afTuple[0] = fInvScalar*m_afTuple[0];
        kQuot.m_afTuple[1] = fInvScalar*m_afTuple[1];
        kQuot.m_afTuple[2] = fInvScalar*m_afTuple[2];
        kQuot.m_afTuple[3] = fInvScalar*m_afTuple[3];
      }
    else
      {
        kQuot.m_afTuple[0] = Math<Real>::MAX_REAL;
        kQuot.m_afTuple[1] = Math<Real>::MAX_REAL;
        kQuot.m_afTuple[2] = Math<Real>::MAX_REAL;
        kQuot.m_afTuple[3] = Math<Real>::MAX_REAL;
      }

    return kQuot;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> Vector4<Real>::operator- () const
  {
    return Vector4(
                   -m_afTuple[0],
                   -m_afTuple[1],
                   -m_afTuple[2],
                   -m_afTuple[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real> operator* (Real fScalar, const Vector4<Real>& rkV)
  {
    return Vector4<Real>(
                         fScalar*rkV[0],
                         fScalar*rkV[1],
                         fScalar*rkV[2],
                         fScalar*rkV[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>& Vector4<Real>::operator+= (const Vector4& rkV)
  {
    m_afTuple[0] += rkV.m_afTuple[0];
    m_afTuple[1] += rkV.m_afTuple[1];
    m_afTuple[2] += rkV.m_afTuple[2];
    m_afTuple[3] += rkV.m_afTuple[3];
    return *this;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>& Vector4<Real>::operator-= (const Vector4& rkV)
  {
    m_afTuple[0] -= rkV.m_afTuple[0];
    m_afTuple[1] -= rkV.m_afTuple[1];
    m_afTuple[2] -= rkV.m_afTuple[2];
    m_afTuple[3] -= rkV.m_afTuple[3];
    return *this;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>& Vector4<Real>::operator*= (Real fScalar)
  {
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    m_afTuple[3] *= fScalar;
    return *this;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Vector4<Real>& Vector4<Real>::operator/= (Real fScalar)
  {
    if ( fScalar != (Real)0.0 )
      {
        Real fInvScalar = ((Real)1.0)/fScalar;
        m_afTuple[0] *= fInvScalar;
        m_afTuple[1] *= fInvScalar;
        m_afTuple[2] *= fInvScalar;
        m_afTuple[3] *= fInvScalar;
      }
    else
      {
        m_afTuple[0] = Math<Real>::MAX_REAL;
        m_afTuple[1] = Math<Real>::MAX_REAL;
        m_afTuple[2] = Math<Real>::MAX_REAL;
        m_afTuple[3] = Math<Real>::MAX_REAL;
      }

    return *this;
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::Length () const
  {
    return Math<Real>::Sqrt(
                            m_afTuple[0]*m_afTuple[0] +
                            m_afTuple[1]*m_afTuple[1] +
                            m_afTuple[2]*m_afTuple[2] +
                            m_afTuple[3]*m_afTuple[3]);
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::SquaredLength () const
  {
    return
      m_afTuple[0]*m_afTuple[0] +
      m_afTuple[1]*m_afTuple[1] +
      m_afTuple[2]*m_afTuple[2] +
      m_afTuple[3]*m_afTuple[3];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::Dot (const Vector4& rkV) const
  {
    return
      m_afTuple[0]*rkV.m_afTuple[0] +
      m_afTuple[1]*rkV.m_afTuple[1] +
      m_afTuple[2]*rkV.m_afTuple[2] +
      m_afTuple[3]*rkV.m_afTuple[3];
  }
  //----------------------------------------------------------------------------
  template <class Real>
  Real Vector4<Real>::Normalize ()
  {
    Real fLength = Length();

    if ( fLength > Math<Real>::ZERO_TOLERANCE )
      {
        Real fInvLength = ((Real)1.0)/fLength;
        m_afTuple[0] *= fInvLength;
        m_afTuple[1] *= fInvLength;
        m_afTuple[2] *= fInvLength;
        m_afTuple[3] *= fInvLength;
      }
    else
      {
        fLength = (Real)0.0;
        m_afTuple[0] = (Real)0.0;
        m_afTuple[1] = (Real)0.0;
        m_afTuple[2] = (Real)0.0;
        m_afTuple[3] = (Real)0.0;
      }

    return fLength;
  }
  //----------------------------------------------------------------------------
 
}

