//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DX7Common.hpp"
#include "DX7Texture.hpp"
#include "DX7RenderSystem.hpp"
#include "rendering/LoadImage.hpp"

#pragma warning(disable: 4189)


// --------------------------------------------------------------------------------

DX7Texture::DX7Texture(DX7RenderSystem* renderSystem)
  : mRenderSystem(renderSystem), Texture(), mTextureSurface(NULL)
{
}

DX7Texture::~DX7Texture()
{
  cleanup();
}

// --------------------------------------------------------------------------------

void
DX7Texture::cleanup()
{
  if(mTextureSurface != NULL)
    mTextureSurface->Release();
  mTextureSurface = NULL;
}


// --------------------------------------------------------------------------------



bool
DX7Texture::flush()
{
  HRESULT hr;
  if(!Texture::flush()) return false;
  
  using agg::int8u;

  cleanup();

  // First create the mipmaps
  int mipWidth = mWidth;
  int mipHeight = mHeight;
  vector<int8u*> mipMaps;
  
  if(!createMipMaps(static_cast<int8u*>(mData), mipWidth, mipHeight, mipMaps)) {
    logEngineError("DX7Texture::flush(): Unable to create mip maps");
    return false;
  } 

  // Now allocate surface for maps
  DDSURFACEDESC2 surfaceDesc;
  ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
  surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
  surfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE | DDSD_MIPMAPCOUNT;
  surfaceDesc.dwMipMapCount = mipMaps.size();
  surfaceDesc.ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
  surfaceDesc.dwWidth = mWidth;
  surfaceDesc.dwHeight = mHeight;
  
  if(mRenderSystem->isHardwareAccelerated())
    surfaceDesc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
  else
    surfaceDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

  // The standard Alpha format seems to be argb
  DDPIXELFORMAT pixelFormat;
  ZeroMemory(&pixelFormat, sizeof(pixelFormat));
  pixelFormat.dwSize = sizeof(pixelFormat);
  pixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
  pixelFormat.dwRGBBitCount = 32;
  pixelFormat.dwRBitMask = 0x00ff0000;
  pixelFormat.dwGBitMask = 0x0000ff00;
  pixelFormat.dwBBitMask = 0x000000ff;
  pixelFormat.dwRGBAlphaBitMask = 0xff000000;
  
  surfaceDesc.ddpfPixelFormat = pixelFormat;

  IDirectDraw7* directDraw = mRenderSystem->getDirectDraw();

  hr = directDraw->CreateSurface(&surfaceDesc, &mTextureSurface, NULL);  

  IDirectDrawSurface7* currentLevel = mTextureSurface;
  IDirectDrawSurface7* nextLevel = NULL;
  DDSCAPS2 surfaceCaps;
  ZeroMemory(&surfaceCaps, sizeof(surfaceCaps));
  surfaceCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
  currentLevel->AddRef();
  for(vector<int8u*>::iterator iMap = mipMaps.begin(); iMap != mipMaps.end(); ++iMap) {
    ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
    surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
    hr = currentLevel->Lock(NULL, &surfaceDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_NOSYSLOCK, NULL);

    int8u* sourcePtr = *iMap;
    int8u* destPtr = static_cast<int8u*>(surfaceDesc.lpSurface);
    int pixelCount = mipWidth * mipHeight;
    {
      memcpy(destPtr, sourcePtr, pixelCount * 4);
//       for(int iPixel = 0; iPixel < pixelCount; ++iPixel) {
//         int8u r = *sourcePtr++;
//         int8u g = *sourcePtr++;
//         int8u b = *sourcePtr++;
//         int8u a = *sourcePtr++;
//         *destPtr++ = b;
//         *destPtr++ = g;
//         *destPtr++ = r;
//         *destPtr++ = a;
//       }
    }
    hr = mTextureSurface->Unlock(NULL);
    HRESULT invalidobject = DDERR_INVALIDOBJECT;
    HRESULT invalidparams = DDERR_INVALIDPARAMS;
    hr = currentLevel->GetAttachedSurface(&surfaceCaps, &nextLevel);
    currentLevel->Release();

    currentLevel = nextLevel;
    mipWidth /= 2;
    mipHeight /= 2;
  }

  // Delete temporary mipmaps
  for(vector<int8u*>::iterator iMap = mipMaps.begin() + 1; iMap != mipMaps.end(); ++iMap) {
    delete[] (*iMap);
  }
  
  return true;
}




