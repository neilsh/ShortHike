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

#include "Common.hpp"
#include "Math.hpp"

const float Math::EPSILON = FLT_EPSILON;
const float Math::ZERO_TOLERANCE = 1e-06f;
const float Math::MAX_REAL = FLT_MAX;
const float Math::PI = (float)(4.0*atan(1.0));
const float Math::TWO_PI = 2.0f*Math::PI;
const float Math::HALF_PI = 0.5f*Math::PI;
const float Math::INV_PI = 1.0f/Math::PI;
const float Math::INV_TWO_PI = 1.0f/Math::TWO_PI;
const float Math::DEG_TO_RAD = Math::PI/180.0f;
const float Math::RAD_TO_DEG = 180.0f/Math::PI;

namespace Wm3
{
template<> const float Math<float>::EPSILON = FLT_EPSILON;
template<> const float Math<float>::ZERO_TOLERANCE = 1e-06f;
template<> const float Math<float>::MAX_REAL = FLT_MAX;
template<> const float Math<float>::PI = (float)(4.0*atan(1.0));
template<> const float Math<float>::TWO_PI = 2.0f*Math<float>::PI;
template<> const float Math<float>::HALF_PI = 0.5f*Math<float>::PI;
template<> const float Math<float>::INV_PI = 1.0f/Math<float>::PI;
template<> const float Math<float>::INV_TWO_PI = 1.0f/Math<float>::TWO_PI;
template<> const float Math<float>::DEG_TO_RAD = Math<float>::PI/180.0f;
template<> const float Math<float>::RAD_TO_DEG = 180.0f/Math<float>::PI;

template<> const double Math<double>::EPSILON = DBL_EPSILON;
template<> const double Math<double>::ZERO_TOLERANCE = 1e-08;
template<> const double Math<double>::MAX_REAL = DBL_MAX;
template<> const double Math<double>::PI = 4.0*atan(1.0);
template<> const double Math<double>::TWO_PI = 2.0*Math<double>::PI;
template<> const double Math<double>::HALF_PI = 0.5*Math<double>::PI;
template<> const double Math<double>::INV_PI = 1.0/Math<double>::PI;
template<> const double Math<double>::INV_TWO_PI = 1.0/Math<double>::TWO_PI;
template<> const double Math<double>::DEG_TO_RAD = Math<double>::PI/180.0;
template<> const double Math<double>::RAD_TO_DEG = 180.0/Math<double>::PI;

//----------------------------------------------------------------------------
template <>
float Math<float>::FastInvSqrt (float fValue)
{
    float fHalf = 0.5f*fValue;
    int i  = *(int*)&fValue;
    i = 0x5f3759df - (i >> 1);
    fValue = *(float*)&i;
    fValue = fValue*(1.5f - fHalf*fValue*fValue);
    return fValue;
}
//----------------------------------------------------------------------------
// template <>
// double Math<double>::FastInvSqrt (double dValue)
// {
//     double dHalf = 0.5*dValue;
//     Integer64 i  = *(Integer64*)&dValue;
// #if defined(WM3_USING_VC70) || defined(WM3_USING_VC6)
//     i = 0x5fe6ec85e7de30da - (i >> 1);
// #else
//     i = 0x5fe6ec85e7de30daLL - (i >> 1);
// #endif
//     dValue = *(double*)&i;
//     dValue = dValue*(1.5 - dHalf*dValue*dValue);
//     return dValue;
// }
//----------------------------------------------------------------------------
 
}
