//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_COMMON_HPP
#define SHORTHIKE_COMMON_HPP

//--------------------------------------------------------------------------------
// Disable float conversions

#pragma warning(disable: 4305)
#pragma warning(disable: 4244)


//--------------------------------------------------------------------------------
// Common include files

#include "Libraries.hpp"

#include "Types.hpp"
#include "main/Root.hpp"
#include "profiler/prof.h"
#include "util/TestFixture.hpp"
#include "util/Exceptions.hpp"
#include "util/Log.hpp"

#include "math/Math.hpp"
#include "math/Angle.hpp"
#include "math/Vector.hpp"
#include "math/Matrix.hpp"
#include "math/Quaternion.hpp"
#include "math/Ray.hpp"
#include "math/AxisAlignedBox.hpp"

//#include "math/Vector_inline.hpp"
//#include "math/Matrix_inline.hpp"
//#include "math/Quaternion_inline.hpp"
#include "math/Math_inline.hpp"
#include "util/math_inline.hpp"

#endif
