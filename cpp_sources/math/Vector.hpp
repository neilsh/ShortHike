//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

class Matrix;
class Quaternion; 

#pragma warning(disable: 4201)

class Vector
{
public:
  Vector(float _x = 0, float _y = 0, float _z = 0);

  const bool operator==(const Vector& rhs) const;
  const bool operator!=(const Vector& rhs) const;
  const bool operator<(const Vector& rhs) const;

  const Vector operator+(const Vector& rhs) const;
  const Vector operator-(const Vector& rhs) const;
  const Vector operator+=(const Vector& rhs);
  const Vector operator-=(const Vector& rhs);
  const Vector operator-() const;
  
  const Vector operator*(const Matrix& rhs) const;
  const Vector operator*(const Quaternion& rhs) const;
  const Vector operator*(float scalar) const;
  const Vector operator*=(float scalar);
  const Vector operator/=(float scalar);

  void normalize();

  float length() const;
  float squaredLength() const;
  float dotProduct(const Vector& rhs) const;
  Vector crossProduct(const Vector& rhs) const;

  inline float operator[](size_t iRow) const {return v[iRow];}
  inline float& operator[](size_t iRow) {return v[iRow];}
  
  static const Vector ZERO;
  static const Vector ORIGIN;
  static const Vector UNIT_X;
  static const Vector UNIT_Y;
  static const Vector UNIT_Z;
  static const Vector UNIT_W;
  
  // --------------------------------------------------------------------------------
  // The data itself

  union 
  {
    struct
    {
      float x, y, z;
    };
    float v[3];
  };
};



// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

namespace Wm3
{

template <class Real>
class Vector4
{
public:
    // construction
    Vector4 ();  // uninitialized
    Vector4 (Real fX, Real fY, Real fZ, Real fW);
    Vector4 (const Vector4& rkV);

    // coordinate access
    operator const Real* () const;
    operator Real* ();
    Real operator[] (int i) const;
    Real& operator[] (int i);
    Real X () const;
    Real& X ();
    Real Y () const;
    Real& Y ();
    Real Z () const;
    Real& Z ();
    Real W () const;
    Real& W ();

    // assignment
    Vector4& operator= (const Vector4& rkV);

    // comparison
    bool operator== (const Vector4& rkV) const;
    bool operator!= (const Vector4& rkV) const;
    bool operator<  (const Vector4& rkV) const;
    bool operator<= (const Vector4& rkV) const;
    bool operator>  (const Vector4& rkV) const;
    bool operator>= (const Vector4& rkV) const;

    // arithmetic operations
    Vector4 operator+ (const Vector4& rkV) const;
    Vector4 operator- (const Vector4& rkV) const;
    Vector4 operator* (Real fScalar) const;
    Vector4 operator/ (Real fScalar) const;
    Vector4 operator- () const;

    // arithmetic updates
    Vector4& operator+= (const Vector4& rkV);
    Vector4& operator-= (const Vector4& rkV);
    Vector4& operator*= (Real fScalar);
    Vector4& operator/= (Real fScalar);

    // vector operations
    Real Length () const;
    Real SquaredLength () const;
    Real Dot (const Vector4& rkV) const;
    Real Normalize ();

    // special vectors
//     static const Vector4 ZERO;
//     static const Vector4 UNIT_X;
//     static const Vector4 UNIT_Y;
//     static const Vector4 UNIT_Z;
//     static const Vector4 UNIT_W;

private:
    // support for comparisons
    int CompareArrays (const Vector4& rkV) const;

    Real m_afTuple[4];
};

template <class Real>
Vector4<Real> operator* (Real fScalar, const Vector4<Real>& rkV);


typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;

}


#endif
