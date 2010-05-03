//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DX7Common.hpp"
#include "DX7Surface.hpp"
#include "DX7RenderSystem.hpp"

#pragma warning(disable: 4189)


// --------------------------------------------------------------------------------

DX7Surface::DX7Surface(DX7RenderSystem* renderSystem, int width, int height)
  : mRenderSystem(renderSystem), Surface(width, height), mSurface(NULL)
{
}

DX7Surface::~DX7Surface()
{
  cleanup();
}

// --------------------------------------------------------------------------------

void
DX7Surface::cleanup()
{
  if(mSurface != NULL)
    mSurface->Release();
  mSurface = NULL;
}


// --------------------------------------------------------------------------------

void
DX7Surface::flush()
{
  HRESULT hr;
  Surface::flush();
  
  using agg::int8u;

  cleanup();

  DDSURFACEDESC2 surfaceDesc;
  ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
  surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  surfaceDesc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH| DDSD_PIXELFORMAT;
  surfaceDesc.dwWidth = mWidth;
  surfaceDesc.dwHeight = mHeight;
  
  if(mRenderSystem->isHardwareAccelerated())
    surfaceDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  else
    surfaceDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

//   // The standard Alpha format seems to be argb
  DDPIXELFORMAT pixelFormat;
  ZeroMemory(&pixelFormat, sizeof(pixelFormat));
  pixelFormat.dwSize = sizeof(pixelFormat);

  pixelFormat.dwFlags = DDPF_RGB;
  pixelFormat.dwRGBBitCount = 32;
  pixelFormat.dwRBitMask = 0x00ff0000;
  pixelFormat.dwGBitMask = 0x0000ff00;
  pixelFormat.dwBBitMask = 0x000000ff;  
  surfaceDesc.ddpfPixelFormat = pixelFormat;

  IDirectDraw7* directDraw = mRenderSystem->getDirectDraw();

  hr = directDraw->CreateSurface(&surfaceDesc, &mSurface, NULL);  
  if(DD_OK != hr) {
    reportDD7Error(hr, __FILE__, __LINE__);
    return;
  }

  ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
  surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  hr = mSurface->GetSurfaceDesc(&surfaceDesc);
  if(DD_OK != hr) {
    reportDD7Error(hr, __FILE__, __LINE__);
    return;
  }

  // Set the color key to black
  DDCOLORKEY colorKey;
  colorKey.dwColorSpaceLowValue = colorKey.dwColorSpaceHighValue = 0x00000000;
  hr = mSurface->SetColorKey(DDCKEY_SRCBLT, &colorKey);
  if(DD_OK != hr) {
    reportDD7Error(hr, __FILE__, __LINE__);
    return;
  }
  
  ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
  surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  hr = mSurface->Lock(NULL, &surfaceDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_NOSYSLOCK, NULL);
  if(DD_OK != hr) {
    reportDD7Error(hr, __FILE__, __LINE__);
    return;
  }

  int8u* sourcePtr = static_cast<int8u*>(getData());
  int8u* destPtr = static_cast<int8u*>(surfaceDesc.lpSurface);
  for(int iLine = 0; iLine < mHeight; ++iLine) {
    int8u* linePtr = destPtr;
    for(int iPixel = 0; iPixel < mWidth; ++iPixel) {
      int8u b = *sourcePtr++;
      int8u g = *sourcePtr++;
      int8u r = *sourcePtr++;
      int8u a = *sourcePtr++;
      *linePtr++ = b;
      *linePtr++ = g;
      *linePtr++ = r;
      *linePtr++ = a;
    }
    destPtr += surfaceDesc.lPitch;
  }
  
  hr = mSurface->Unlock(NULL);
  if(DD_OK != hr) {
    reportDD7Error(hr, __FILE__, __LINE__);
    return;
  }
}




