//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_DX7_DX7COMMON_HPP
#define RENDERING_DX7_DX7COMMON_HPP

#define DIRECT3D_VERSION 0x0700
#define D3D_OVERLOADS

#include "ddraw.h"
#include "d3d.h"

extern const GUID* GUID_RGB_DEVICE;
extern const GUID* GUID_HAL_DEVICE;
extern const GUID* GUID_TnLHAL_DEVICE;

void reportDD7Error(HRESULT hr, string file, long line);
void reportD3D7Error(HRESULT hr, string file, long line);

#endif
