//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_LIBRARIES_HPP
#define SHORTHIKE_LIBRARIES_HPP

#pragma warning(push, 3)

// This should be moved down to EntryWin32
#define VC_EXTRALEAN
#define WIN32_MEAN_AND_LEAN
#include "windows.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <iomanip>
#include <limits>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <list>
#include <fstream>
#include <queue>

using namespace std;

//--------------------------------------------------------------------------------
// Freetype

#ifdef ENABLE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

//--------------------------------------------------------------------------------
// FMOD

#ifdef ENABLE_FMOD
#include "fmod.h"
#endif

//--------------------------------------------------------------------------------
// AntiGrain

#ifdef ENABLE_ANTIGRAIN

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_renderer_scanline.h"
#include "agg_pixfmt_rgba.h"
#include "agg_gamma_lut.h"
#include "agg_ellipse.h"
#include "agg_rounded_rect.h"
#include "agg_conv_stroke.h"
#include "agg_renderer_primitives.h"
#include "agg_renderer_markers.h"
#include "agg_font_freetype.h"

typedef agg::rgba8 ColorType;
#endif

//--------------------------------------------------------------------------------
// Zlib

#ifdef ENABLE_ZLIB
#include "zlib.h"
#endif


//--------------------------------------------------------------------------------
// PNG

#ifdef ENABLE_PNG
#include "png.h"
#endif

//--------------------------------------------------------------------------------
// JASPER

#ifdef ENABLE_JASPER
#include "jasper/jasper.h"
#endif


//--------------------------------------------------------------------------------
// ColDet

#ifdef ENABLE_COLDET
#include "coldet.h"
#endif


#pragma warning(pop)


#endif
