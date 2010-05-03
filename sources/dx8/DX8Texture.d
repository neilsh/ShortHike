//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Texture;
private import DX8RenderSystem;
private import d3d8;
private import std.c.string;

class DX8Texture : public Texture
{

  this(DX8RenderSystem renderSystem, bool mipMaps = false)
  {
    super();
    mHasMipMaps = mipMaps;
    mRenderSystem = renderSystem;
    //   mTextureSurface = null;
  }

  ~this()
  {
    cleanup();
  }

  // --------------------------------------------------------------------------------

  void
  cleanup()
  {
    //   if(mTextureSurface != null)
    //     mTextureSurface->Release();
    //   mTextureSurface = null;
  }


  // --------------------------------------------------------------------------------



  bool
  flush()
  {
    HRESULT hr;
    if(!super.flush()) return false;
  
    cleanup();

    //   // First create the mipmaps
    //   int mipWidth = mWidth;
    //   int mipHeight = mHeight;
    //   vector<int8u*> mipMaps;
  
    //   if(!createMipMaps(static_cast<int8u*>(mData), mipWidth, mipHeight, mipMaps)) {
    //     logEngineError("flush(): Unable to create mip maps");
    //     return false;
    //   } 

    IDirect3DDevice8 device = mRenderSystem.getDirect3DDevice();
    if(mHasMipMaps) {
      assert(CHECK_FAIL("Not implemented"));
//       hr = device.CreateTexture(mWidth, mHeight, 0u, 0u, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mTextureSurface);
//       int numMipMaps = mTextureSurface.GetLevelCount();
//       ubyte[][] mipMaps = createMipMaps(mData, mWidth, mHeight, numMipMaps);
//       for(int iLevel = 0; iLevel < numMipMaps; ++iLevel) {
        
//       }
    }  
    // pure load version
    else {      
      hr = device.CreateTexture(mWidth, mHeight, 1u, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTextureSurface);
      D3DLOCKED_RECT rect;
      hr = mTextureSurface.LockRect(0, &rect, null, D3DLOCK_NOSYSLOCK);
      if(rect.Pitch == mWidth * 4) {
        memcpy(rect.pBits, mData, mWidth * mHeight * 4);
      }
      else {
        assert(CHECK_FAIL("Not implemented"));
      }      
      hr = mTextureSurface.UnlockRect(0);
    }
    
    //   // Now allocate surface for maps
    //   DDSURFACEDESC2 surfaceDesc;
    //   ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
    //   surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
    //   surfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE | DDSD_MIPMAPCOUNT;
    //   surfaceDesc.dwMipMapCount = mipMaps.size();
    //   surfaceDesc.ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
    //   surfaceDesc.dwWidth = mWidth;
    //   surfaceDesc.dwHeight = mHeight;
  
    //   if(mRenderSystem->isHardwareAccelerated())
    //     surfaceDesc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    //   else
    //     surfaceDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    //   // The standard Alpha format seems to be argb
    //   DDPIXELFORMAT pixelFormat;
    //   ZeroMemory(&pixelFormat, sizeof(pixelFormat));
    //   pixelFormat.dwSize = sizeof(pixelFormat);
    //   pixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
    //   pixelFormat.dwRGBBitCount = 32;
    //   pixelFormat.dwRBitMask = 0x00ff0000;
    //   pixelFormat.dwGBitMask = 0x0000ff00;
    //   pixelFormat.dwBBitMask = 0x000000ff;
    //   pixelFormat.dwRGBAlphaBitMask = 0xff000000;
  
    //   surfaceDesc.ddpfPixelFormat = pixelFormat;

    //   IDirectDraw7* directDraw = mRenderSystem->getDirectDraw();

    //   hr = directDraw->CreateSurface(&surfaceDesc, &mTextureSurface, null);  

    //   IDirectDrawSurface7* currentLevel = mTextureSurface;
    //   IDirectDrawSurface7* nextLevel = null;
    //   DDSCAPS2 surfaceCaps;
    //   ZeroMemory(&surfaceCaps, sizeof(surfaceCaps));
    //   surfaceCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
    //   currentLevel->AddRef();


    
    //   for(vector<int8u*>::iterator iMap = mipMaps.begin(); iMap != mipMaps.end(); ++iMap) {
    //     ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC2));
    //     surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
    //     hr = currentLevel->Lock(null, &surfaceDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_NOSYSLOCK, null);

    //     int8u* sourcePtr = *iMap;
    //     int8u* destPtr = static_cast<int8u*>(surfaceDesc.lpSurface);
    //     int pixelCount = mipWidth * mipHeight;
    //     {
    //       memcpy(destPtr, sourcePtr, pixelCount * 4);
    // //       for(int iPixel = 0; iPixel < pixelCount; ++iPixel) {
    // //         int8u r = *sourcePtr++;
    // //         int8u g = *sourcePtr++;
    // //         int8u b = *sourcePtr++;
    // //         int8u a = *sourcePtr++;
    // //         *destPtr++ = b;
    // //         *destPtr++ = g;
    // //         *destPtr++ = r;
    // //         *destPtr++ = a;
    // //       }
    //     }
    //     hr = mTextureSurface->Unlock(null);
    //     HRESULT invalidobject = DDERR_INVALIDOBJECT;
    //     HRESULT invalidparams = DDERR_INVALIDPARAMS;
    //     hr = currentLevel->GetAttachedSurface(&surfaceCaps, &nextLevel);
    //     currentLevel->Release();

    //     currentLevel = nextLevel;
    //     mipWidth /= 2;
    //     mipHeight /= 2;
    //   }

    //   // Delete temporary mipmaps
    //   for(vector<int8u*>::iterator iMap = mipMaps.begin() + 1; iMap != mipMaps.end(); ++iMap) {
    //     delete[] (*iMap);
    //   }
  
    return true;
  }


  DX8RenderSystem mRenderSystem;
  IDirect3DTexture8 mTextureSurface;
  bool mHasMipMaps;
}

