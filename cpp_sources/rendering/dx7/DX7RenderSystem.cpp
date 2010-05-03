//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DX7Common.hpp"

#include "DX7RenderSystem.hpp"
#include "rendering/Surface.hpp"
#include "DX7Mesh.hpp"
#include "DX7Texture.hpp"
#include "DX7Surface.hpp"
#include "rendering/Camera.hpp"

//--------------------------------------------------------------------------------

const GUID* GUID_RGB_DEVICE = &IID_IDirect3DRGBDevice;
const GUID* GUID_HAL_DEVICE = &IID_IDirect3DHALDevice;
const GUID* GUID_TnLHAL_DEVICE = &IID_IDirect3DTnLHalDevice;

//--------------------------------------------------------------------------------

#pragma warning(disable: 4189)
#pragma warning(disable: 4101)

bool
createDX7RenderSystem(HWND hWindow, RenderSystem** outRenderSystem)
{
  *outRenderSystem = NULL;
  
  DX7RenderSystem* renderSystem = new DX7RenderSystem();
  if(renderSystem->initialize()) {
    if(renderSystem->initializeDevice(hWindow, GUID_TnLHAL_DEVICE) ||
       renderSystem->initializeDevice(hWindow, GUID_HAL_DEVICE) ||
       renderSystem->initializeDevice(hWindow, GUID_RGB_DEVICE)) {
//     if(renderSystem->initializeDevice(hWindow, GUID_HAL_DEVICE)) {
      *outRenderSystem = renderSystem;
      return true;
    }
  }
  delete renderSystem;
  return false;
}

D3DMATRIX makeD3DMatrix(const Matrix& matrix)
{
  D3DMATRIX d3dMatrix;
  for (unsigned row = 0; row < 4; ++row)
    for (unsigned col = 0; col < 4; ++col)
      d3dMatrix.m[row][col] = matrix[row][col];

  return d3dMatrix;
}



// --------------------------------------------------------------------------------

DX7RenderSystem::DX7RenderSystem()
  : mDirectDraw(NULL), mDirect3D(NULL),
    mPrimarySurface(NULL), mBackBuffer(NULL), mZBuffer(NULL), mDirect3DDevice(NULL),
    mCurrentMesh(NULL), mFrameTriangles(0)
{
}

DX7RenderSystem::~DX7RenderSystem()
{
  if(NULL != mDirect3D)
    mDirect3D->Release();
  if(NULL != mDirectDraw)
    mDirectDraw->Release();
}


//--------------------------------------------------------------------------------

bool
DX7RenderSystem::initialize()
{
  HRESULT hr;

  // Create the IDirectDraw interface. The first parameter is the GUID,
  // which is allowed to be NULL. If there are more than one DirectDraw
  // drivers on the system, a NULL guid requests the primary driver. For 
  // non-GDI hardware cards like the 3DFX and PowerVR, the guid would need
  // to be explicity specified . (Note: these guids are normally obtained
  // from enumeration, which is convered in a subsequent tutorial.)
  hr = DirectDrawCreateEx( NULL, (VOID**)&mDirectDraw, IID_IDirectDraw7, NULL );
  if(FAILED(hr)) {
    
    return false;
  }
  
  // Query DirectDraw for access to Direct3D
  hr = mDirectDraw->QueryInterface( IID_IDirect3D7, (VOID**)&mDirect3D );
  if(FAILED(hr)) {
    return false;
  }  

  return true;
}


//-----------------------------------------------------------------------------
// Name: EnumZBufferCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
	// For this tutorial, we are only interested in z-buffers, so ignore any
	// other formats (e.g. DDPF_STENCILBUFFER) that get enumerated. An app
	// could also check the depth of the z-buffer (16-bit, etc,) and make a
	// choice based on that, as well. For this tutorial, we'll take the first
	// one we get.
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

        // Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
    }

    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}

bool
DX7RenderSystem::initializeDevice(HWND hWindow, const GUID* deviceGUID)
{
  HRESULT hr;
  mDeviceGUID = deviceGUID;
  
  // FIXME: Support for software rendering
//   const GUID* pDeviceGUID = &IID_IDirect3DHALDevice;
//   &IID_IDirect3DRGBDevice;
  

  // Set the Windows cooperative level. This is where we tell the system
  // whether wew will be rendering in fullscreen mode or in a window. Note
  // that some hardware (non-GDI) may not be able to render into a window.
  // The flag DDSCL_NORMAL specifies windowed mode. Using fullscreen mode
  // is the topic of a subsequent tutorial. The DDSCL_FPUSETUP flag is a 
  // hint to DirectX to optimize floating points calculations. See the docs
  // for more info on this. Note: this call could fail if another application
  // already controls a fullscreen, exclusive mode.
  hr = mDirectDraw->SetCooperativeLevel( hWindow, DDSCL_NORMAL );
//   if( FAILED( hr ) )
//     return hr;

  // Initialize a surface description structure for the primary surface. The
  // primary surface represents the entire display, with dimensions and a
  // pixel format of the display. Therefore, none of that information needs
  // to be specified in order to create the primary surface.
  DDSURFACEDESC2 ddsd;
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  
  // Create the primary surface.
  hr = mDirectDraw->CreateSurface( &ddsd, &mPrimarySurface, NULL );
//   if( FAILED( hr ) )
//     return hr;

  // Create a clipper object which handles all our clipping for cases when
  // our window is partially obscured by other windows. This is not needed
  // for apps running in fullscreen mode.
  LPDIRECTDRAWCLIPPER pcClipper;
  hr = mDirectDraw->CreateClipper( 0, &pcClipper, NULL );
//   if( FAILED( hr ) )
//     return hr;

  // Associate the clipper with our window. Note that, afterwards, the
  // clipper is internally referenced by the primary surface, so it is safe
  // to release our local reference to it.
  pcClipper->SetHWnd( 0, hWindow );
  mPrimarySurface->SetClipper( pcClipper );
  pcClipper->Release();
  
  // Setup a surface description to create a backbuffer. This is an
  // offscreen plain surface with dimensions equal to our window size.
  // The DDSCAPS_3DDEVICE is needed so we can later query this surface
  // for an IDirect3DDevice interface.
  ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

  // Set the dimensions of the backbuffer. Note that if our window changes
  // size, we need to destroy this surface and create a new one.
  GetClientRect( hWindow, &mScreenRect );
  ClientToScreen( hWindow, (POINT*)&mScreenRect.left );
  ClientToScreen( hWindow, (POINT*)&mScreenRect.right );
  ddsd.dwWidth  = mScreenRect.right - mScreenRect.left;
  ddsd.dwHeight = mScreenRect.bottom - mScreenRect.top;

  // Create the backbuffer. The most likely reason for failure is running
  // out of video memory. (A more sophisticated app should handle this.)
  hr = mDirectDraw->CreateSurface( &ddsd, &mBackBuffer, NULL );
  //   if( FAILED( hr ) )
//     return hr;


  // Enumerate the various z-buffer formats, finding a DDPIXELFORMAT
  // to use to create the z-buffer surface.
  DDPIXELFORMAT ddpfZBuffer;
  mDirect3D->EnumZBufferFormats( *mDeviceGUID, 
                              EnumZBufferCallback, (VOID*)&ddpfZBuffer );
  
  // If we found a good zbuffer format, then the dwSize field will be
  // properly set during enumeration. Else, we have a problem and will exit.
//   if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
//     return E_FAIL;
  
  // Get z-buffer dimensions from the render target
  // Setup the surface desc for the z-buffer.
  ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
  ddsd.dwWidth        = mScreenRect.right - mScreenRect.left;
  ddsd.dwHeight       = mScreenRect.bottom - mScreenRect.top;
  memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );
  
  // For hardware devices, the z-buffer should be in video memory. For
  // software devices, create the z-buffer in system memory
  if( IsEqualIID(*mDeviceGUID, *GUID_HAL_DEVICE) || IsEqualIID(*mDeviceGUID, *GUID_TnLHAL_DEVICE) )
    ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
  else
    ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
  
  // Create and attach a z-buffer. Real apps should be able to handle an
  // error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this 
  // tutorial, though, we are simply going to exit ungracefully.
  hr = mDirectDraw->CreateSurface( &ddsd, &mZBuffer, NULL );
//   if(FAILED(hr))
//     return hr;
  
  // Attach the z-buffer to the back buffer.
  hr = mBackBuffer->AddAttachedSurface( mZBuffer );
//   if(FAILED(hr))
//     return hr;


  // Before creating the device, check that we are NOT in a palettized
  // display. That case will cause CreateDevice() to fail, since this simple 
  // tutorial does not bother with palettes.
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
  mDirectDraw->GetDisplayMode( &ddsd );
//   if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
//     return DDERR_INVALIDMODE;

  // Create the device. The device is created off of our back buffer, which
  // becomes the render target for the newly created device. Note that the
  // z-buffer must be created BEFORE the device

  if( FAILED( hr = mDirect3D->CreateDevice(*mDeviceGUID, mBackBuffer, &mDirect3DDevice ))) {
    cleanUpDevice();
    return false;
  }

  // Create the viewport
  DWORD dwRenderWidth  = mScreenRect.right - mScreenRect.left;
  DWORD dwRenderHeight = mScreenRect.bottom - mScreenRect.top;
  D3DVIEWPORT7 vp = { 0, 0, dwRenderWidth, dwRenderHeight, 0.0f, 1.0f };
  hr = mDirect3DDevice->SetViewport( &vp );
//   if( FAILED( hr ) )
//     return hr;

  // Finish by setting up our scene
  //   return App_InitDeviceObjects( mDirect3DDevice );}

//   DDPIXELFORMAT pixelFormat;
//   pixelFormat.dwSize = sizeof(pixelFormat);
//   hr = mBackBuffer->GetPixelFormat(&pixelFormat);

  D3DDEVICEDESC7 deviceCaps;
  mDirect3DDevice->GetCaps(&deviceCaps);

  // Set up the default render state
  if(mDeviceGUID == GUID_TnLHAL_DEVICE) {
    mDirect3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
    mDirect3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
    mDirect3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFG_LINEAR);
    // Enable specular only for modules closer to the player. The hit is too big ..
//     mDirect3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);
  }
  else if(mDeviceGUID == GUID_HAL_DEVICE) {
    mDirect3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
    mDirect3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
  }
  else if(mDeviceGUID == GUID_RGB_DEVICE) {
    mDirect3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
  }
  else {
    CHECK_FAIL("Unknown device GUID");
  }
  

  // FIXME: Aquire Mesh resources

  return true;
}

// --------------------------------------------------------------------------------

void
DX7RenderSystem::cleanUpDevice()
{
  // FIXME: Release Mesh resources

  if(NULL != mDirect3DDevice)
    mDirect3DDevice->Release();
  if(NULL != mZBuffer)
    mZBuffer->Release();
  if(NULL != mBackBuffer)
    mBackBuffer->Release();
  if(NULL != mPrimarySurface)
    mPrimarySurface->Release();
  
  mPrimarySurface = NULL;
  mBackBuffer = NULL;
  mZBuffer = NULL;
  mDirect3DDevice = NULL;
  
  mCurrentMesh = NULL;
}


// --------------------------------------------------------------------------------
// Resource management

Mesh*
DX7RenderSystem::createMesh(bool collisionsupport)
{
  return new DX7Mesh(this, collisionsupport);
}

void
DX7RenderSystem::destroyMesh(Mesh* mesh)
{
  DX7Mesh* dx7Mesh = static_cast<DX7Mesh*>(mesh);
  delete dx7Mesh;
}

Texture*
DX7RenderSystem::createTexture()
{
  return new DX7Texture(this);
}

void
DX7RenderSystem::destroyTexture(Texture* texture)
{
  DX7Texture* dx7Texture = static_cast<DX7Texture*>(texture);
  delete dx7Texture;
}

Surface*
DX7RenderSystem::createSurface(int width, int height)
{
  return new DX7Surface(this, width, height);
}

void
DX7RenderSystem::destroySurface(Surface* surface)
{
  delete surface;
}

Camera*
DX7RenderSystem::createCamera()
{
  return new Camera();
}

void
DX7RenderSystem::destroyCamera(Camera* camera)
{
  delete camera;
}

// --------------------------------------------------------------------------------
int
DX7RenderSystem::getWidth()
{
  return mScreenRect.right - mScreenRect.left;  
}

int
DX7RenderSystem::getHeight()
{
  return mScreenRect.bottom - mScreenRect.top;
}


// --------------------------------------------------------------------------------
// Scene management

bool
DX7RenderSystem::beginScene()
{
  if( NULL == mPrimarySurface ) return false;

  // Do some smart stuff like check for DDERR_WRONGMODE and restore stuff when neccessary
  if( FAILED( mDirectDraw->TestCooperativeLevel() ) )
    return false;

  // Force the video hardware to synchronize with us. Otherwise on light loads the CPU will
  // run away.
  DDSURFACEDESC2 backBufferDesc;
  ZeroMemory(&backBufferDesc, sizeof(backBufferDesc));
  backBufferDesc.dwSize = sizeof(backBufferDesc);  
  HRESULT hr = mBackBuffer->Lock(NULL, &backBufferDesc, DDLOCK_READONLY, NULL);
  hr = mBackBuffer->Unlock(NULL);

  // Standard begin scene work
  hr = mDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
  hr = mDirect3DDevice->BeginScene();
  hr = mDirect3DDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x7f7f7f7f);

  // Set the transform matrices. Direct3D uses three independant matrices:
  // the world matrix, the view matrix, and the projection matrix. For
  // convienence, we are first setting up an identity matrix.
  D3DMATRIX mat;
  mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
  mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
  mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
  mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
  
  // The world matrix controls the position and orientation of the polygons
  // in world space. We'll use it later to spin the triangle.
  D3DMATRIX matWorld = mat;
  mDirect3DDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
  
  // The view matrix defines the position and orientation of the camera.
  // Here, we are just moving it back along the z-axis by 10 units.
  D3DMATRIX matView = mat;
  matView._43 = 90.0f;
  mDirect3DDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );
  
  // The projection matrix defines how the 3D scene is "projected" onto the
  // 2D render target (the backbuffer surface). Refer to the docs for more
  // info about projection matrices.
  D3DMATRIX matProj = mat;
  matProj._11 =  2.0f;
  matProj._22 =  2.0f;
  matProj._34 =  1.0f;
  matProj._43 = -1.0f;
  matProj._44 =  0.0f;
  mDirect3DDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
  
  // Enable z-buffering.
  mDirect3DDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );
  
  // Set the material as yellow. We're setting the ambient color here
  // since this tutorial only uses ambient lighting. For apps that use real
  // lights, the diffuse and specular values should be set. (In addition, the
  // polygons' vertices need normals for true lighting.)
  D3DMATERIAL7 mtrl;
  ZeroMemory( &mtrl, sizeof(mtrl) );
  mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 1.0f;
  mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
  mDirect3DDevice->SetMaterial( &mtrl );

  // Set up the light. Note: to be friendly for cards that can do hardware
  // transform-and-lighting (TnL), we should (but we don't) check the caps
  // to see if the device can support directional lights. 
  D3DLIGHT7 light;
  ZeroMemory( &light, sizeof(D3DLIGHT7) );
  light.dltType       =  D3DLIGHT_DIRECTIONAL;
  light.dcvDiffuse.r  =  1.0f;
  light.dcvDiffuse.g  =  1.0f;
  light.dcvDiffuse.b  =  1.0f;
  light.dvDirection.x =  1.0f;
  light.dvDirection.y = -1.0f;
  light.dvDirection.z =  1.0f;
  mDirect3DDevice->SetLight( 0, &light );
  mDirect3DDevice->LightEnable( 0, TRUE );
  mDirect3DDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );

  setColor(1, 1, 1);
  
  mFrameTriangles = 0;

  return true;
}

void
DX7RenderSystem::endScene()
{
  if( NULL == mPrimarySurface ) return;
  HRESULT hr = mDirect3DDevice->EndScene();
  
  // We are in windowed mode, so perform a blit from the backbuffer to the
  // correct position on the primary surface
  //   Check for DDERR_SURFACELOST to restore our surfaces
  hr = mPrimarySurface->Blt( &mScreenRect, mBackBuffer, NULL, DDBLT_WAIT, NULL );
}

// --------------------------------------------------------------------------------

void
DX7RenderSystem::setProjectionMatrix(Camera* camera)
{
  float theta(camera->mFieldOfViewY * 0.5);
  float heightScale = 1 / tanf(theta);
  float widthScale = heightScale / camera->mAspectRatio;

  float depthScale = camera->mFarClipDistance / ( camera->mFarClipDistance - camera->mNearClipDistance );
  float depthShift = -depthScale * camera->mNearClipDistance;

  Matrix projectionMatrix = Matrix::ZERO;
  projectionMatrix[0][0] = widthScale;
  projectionMatrix[1][1] = heightScale;  
  projectionMatrix[2][2] = depthScale;
  projectionMatrix[2][3] = 1.0f;
  projectionMatrix[3][2] = depthShift;

  D3DMATRIX d3dmat = makeD3DMatrix(projectionMatrix);
  HRESULT hr = mDirect3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &d3dmat);
}

void
DX7RenderSystem::setViewMatrix(const Matrix& viewMatrix)
{
  D3DMATRIX d3dMatrix = makeD3DMatrix(viewMatrix);
  HRESULT hr;
  hr = mDirect3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &d3dMatrix);
}


// --------------------------------------------------------------------------------

// Optimization: Use same format for surfaces and target

void
DX7RenderSystem::renderSurface(Surface* surface, int xpos, int ypos)
{
  DX7Surface* dx7Surface = static_cast<DX7Surface*>(surface);
  if(dx7Surface->mSurface == NULL) return;

  RECT destRect;
  destRect.left = xpos;
  destRect.top = ypos;
  destRect.right = xpos + dx7Surface->getWidth();
  destRect.bottom = ypos + dx7Surface->getHeight();

  HRESULT hr = mBackBuffer->Blt(&destRect, dx7Surface->mSurface, NULL, DDBLT_KEYSRC, NULL);
}

// --------------------------------------------------------------------------------
// Lighting

void
DX7RenderSystem::setSunLight(Vector, Color)
{
}

void
DX7RenderSystem::setPlanetLight(Vector, Color)
{
}

void
DX7RenderSystem::enableLighting(bool enabled)
{
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, enabled);
}

void
DX7RenderSystem::enableSunLight(bool enabled)
{
  mDirect3DDevice->LightEnable(0, enabled);
}

void
DX7RenderSystem::enablePlanetLight(bool enabled)
{
  mDirect3DDevice->LightEnable(1, enabled);
}

bool
DX7RenderSystem::getCapsLighting()
{
  return true;
}


// --------------------------------------------------------------------------------
// State and mesh rendering  


void
DX7RenderSystem::enableZBuffer(bool enabled)
{
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, enabled);
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, enabled);
}

void
DX7RenderSystem::enableAlphaBlending(bool enabled)
{
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, enabled);
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
  mDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

bool
DX7RenderSystem::getCapsAlphaBlending()
{
  D3DDEVICEDESC7 caps;
  mDirect3DDevice->GetCaps(&caps);
  return 0 != (caps.dwDevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX);
}

// --------------------------------------------------------------------------------

void
DX7RenderSystem::setMesh(Mesh* mesh, bool wireframe, bool cull)
{
  DX7Mesh* dx7Mesh = static_cast<DX7Mesh*>(mesh);
  mCurrentMesh = dx7Mesh;
  if(mCurrentMesh == NULL) return;
  if(mCurrentMesh->mVertexCount == 0 || mCurrentMesh->mIndexCount == 0 ||
     mCurrentMesh->mVertexBuffer == NULL || mCurrentMesh->mIndexBuffer == NULL) {
    mCurrentMesh = NULL;
    return;
  }
  
  if(wireframe) {
    mDirect3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);    
  }
  else {
    mDirect3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
  }  

  if(cull) {
    mDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);    
  }
  else {
    mDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
  }  
}

void
DX7RenderSystem::renderMesh(const Matrix& matrix)
{
  if(mCurrentMesh == NULL || mCurrentMesh->mVertexBuffer == NULL || mCurrentMesh->mIndexBuffer == NULL) return;

  mFrameTriangles += mCurrentMesh->mIndexCount / 3;
  
  D3DMATRIX d3dMatrix = makeD3DMatrix(matrix);
  HRESULT hr;
  hr = mDirect3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dMatrix);
  hr = mDirect3DDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, mCurrentMesh->mVertexBuffer, 0, mCurrentMesh->mVertexCount,
                                        mCurrentMesh->mIndexBuffer, mCurrentMesh->mIndexCount, 0);
}

// --------------------------------------------------------------------------------
// Materials

void
DX7RenderSystem::setColorTexture(Texture* texture)
{
  if(mDeviceGUID == GUID_RGB_DEVICE) {
    return;
  }

  HRESULT hr;
  if(texture != NULL) {
    DX7Texture* dx7Texture = static_cast<DX7Texture*>(texture);
    hr = mDirect3DDevice->SetTexture(0, dx7Texture->mTextureSurface);
  }
  else {
    enableColorTexture(false);
  }  
}

void
DX7RenderSystem::setColorTextureFunction(ColorTextureFunction)
{
  // Not supported other than transparency
}

void
DX7RenderSystem::enableColorTexture(bool enabled)
{
  if(!enabled) {
    mDirect3DDevice->SetTexture(0, NULL);
  }  
}


void
DX7RenderSystem::setColor(float r, float g, float b)
{
  D3DMATERIAL7 mtrl;
  ZeroMemory( &mtrl, sizeof(mtrl) );
  mtrl.diffuse.r = mtrl.ambient.r = r;
  mtrl.diffuse.g = mtrl.ambient.g = g;
  mtrl.diffuse.b = mtrl.ambient.b = b;
  mDirect3DDevice->SetMaterial( &mtrl );
}



bool
DX7RenderSystem::getCapsColorTexture()
{
  if(mDeviceGUID == GUID_RGB_DEVICE)
    return false;
  else
    return true;
}


bool
DX7RenderSystem::getCapsSpecularFunction()
{
  return false;
}


// --------------------------------------------------------------------------------
// Not supported by DX 7
  
void
DX7RenderSystem::enableVertexColor(bool)
{
}

bool
DX7RenderSystem::getCapsVertexColor()
{
  return false;
}


void
DX7RenderSystem::setStructureTexture(Texture* )
{
}

void
DX7RenderSystem::enableStructureTexture(bool)
{
}

bool
DX7RenderSystem::getCapsStructureTexture()
{
  return false;
}


void
DX7RenderSystem::setDecalTexture(Texture* )
{
}

void
DX7RenderSystem::enableDecalTexture(bool)
{
}

bool
DX7RenderSystem::getCapsDecalTexture()
{
  return false;
}

// --------------------------------------------------------------------------------

void
reportDD7Error(HRESULT hr, string file, long line)
{
  ostringstream ossError;
  ossError << file << "(" << line << ") : DD7 ";
  string errDesc;
  if(DD_OK == hr) errDesc = "DD_OK: The request completed successfully. ";
  else if(DDERR_ALREADYINITIALIZED == hr) errDesc = "DDERR_ALREADYINITIALIZED: The object has already been initialized. ";
  else if(DDERR_BLTFASTCANTCLIP == hr) errDesc = "DDERR_BLTFASTCANTCLIP: A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method. ";
  else if(DDERR_CANNOTATTACHSURFACE == hr) errDesc = "DDERR_CANNOTATTACHSURFACE: A surface cannot be attached to another requested surface. ";
  else if(DDERR_CANNOTDETACHSURFACE == hr) errDesc = "DDERR_CANNOTDETACHSURFACE: A surface cannot be detached from another requested surface. ";
  else if(DDERR_CANTCREATEDC == hr) errDesc = "DDERR_CANTCREATEDC: Windows cannot create any more device contexts (DCs), or a DC has requested a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC). ";
  else if(DDERR_CANTDUPLICATE == hr) errDesc = "DDERR_CANTDUPLICATE: Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated. ";
  else if(DDERR_CANTLOCKSURFACE == hr) errDesc = "DDERR_CANTLOCKSURFACE: Access to this surface is refused because an attempt was made to lock the primary surface without DCI support. ";
  else if(DDERR_CANTPAGELOCK == hr) errDesc = "DDERR_CANTPAGELOCK: An attempt to page-lock a surface failed. Page lock does not work on a display-memory surface or an emulated primary surface. ";
  else if(DDERR_CANTPAGEUNLOCK == hr) errDesc = "DDERR_CANTPAGEUNLOCK: An attempt to page-unlock a surface failed. Page unlock does not work on a display-memory surface or an emulated primary surface. ";
  else if(DDERR_CLIPPERISUSINGHWND == hr) errDesc = "DDERR_CLIPPERISUSINGHWND: An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle. ";
  else if(DDERR_COLORKEYNOTSET == hr) errDesc = "DDERR_COLORKEYNOTSET: No source color key is specified for this operation. ";
  else if(DDERR_CURRENTLYNOTAVAIL == hr) errDesc = "DDERR_CURRENTLYNOTAVAIL: No support is currently available. ";
  else if(DDERR_DDSCAPSCOMPLEXREQUIRED == hr) errDesc = "DDERR_DDSCAPSCOMPLEXREQUIRED: New for DirectX 7.0. The surface requires the DDSCAPS_COMPLEX flag. ";
  else if(DDERR_DCALREADYCREATED == hr) errDesc = "DDERR_DCALREADYCREATED: A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface. ";
  else if(DDERR_DEVICEDOESNTOWNSURFACE == hr) errDesc = "DDERR_DEVICEDOESNTOWNSURFACE: Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device. ";
  else if(DDERR_DIRECTDRAWALREADYCREATED == hr) errDesc = "DDERR_DIRECTDRAWALREADYCREATED: A DirectDraw object representing this driver has already been created for this process. ";
  else if(DDERR_EXCEPTION == hr) errDesc = "DDERR_EXCEPTION: An exception was encountered while performing the requested operation. ";
  else if(DDERR_EXCLUSIVEMODEALREADYSET == hr) errDesc = "DDERR_EXCLUSIVEMODEALREADYSET: An attempt was made to set the cooperative level when it was already set to exclusive. ";
  else if(DDERR_EXPIRED == hr) errDesc = "DDERR_EXPIRED: The data has expired and is therefore no longer valid. ";
  else if(DDERR_GENERIC == hr) errDesc = "DDERR_GENERIC: There is an undefined error condition. ";
  else if(DDERR_HEIGHTALIGN == hr) errDesc = "DDERR_HEIGHTALIGN: The height of the provided rectangle is not a multiple of the required alignment. ";
  else if(DDERR_HWNDALREADYSET == hr) errDesc = "DDERR_HWNDALREADYSET: The DirectDraw cooperative-level window handle has already been set. It cannot be reset while the process has surfaces or palettes created. ";
  else if(DDERR_HWNDSUBCLASSED == hr) errDesc = "DDERR_HWNDSUBCLASSED: DirectDraw is prevented from restoring state because the DirectDraw cooperative-level window handle has been subclassed. ";
  else if(DDERR_IMPLICITLYCREATED == hr) errDesc = "DDERR_IMPLICITLYCREATED: The surface cannot be restored because it is an implicitly created surface. ";
  else if(DDERR_INCOMPATIBLEPRIMARY == hr) errDesc = "DDERR_INCOMPATIBLEPRIMARY: The primary surface creation request does not match the existing primary surface. ";
  else if(DDERR_INVALIDCAPS == hr) errDesc = "DDERR_INVALIDCAPS: One or more of the capability bits passed to the callback function are incorrect. ";
  else if(DDERR_INVALIDCLIPLIST == hr) errDesc = "DDERR_INVALIDCLIPLIST: DirectDraw does not support the provided clip list. ";
  else if(DDERR_INVALIDDIRECTDRAWGUID == hr) errDesc = "DDERR_INVALIDDIRECTDRAWGUID: The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier. ";
  else if(DDERR_INVALIDMODE == hr) errDesc = "DDERR_INVALIDMODE: DirectDraw does not support the requested mode. ";
  else if(DDERR_INVALIDOBJECT == hr) errDesc = "DDERR_INVALIDOBJECT: DirectDraw received a pointer that was an invalid DirectDraw object. ";
  else if(DDERR_INVALIDPARAMS == hr) errDesc = "DDERR_INVALIDPARAMS: One or more of the parameters passed to the method are incorrect. ";
  else if(DDERR_INVALIDPIXELFORMAT == hr) errDesc = "DDERR_INVALIDPIXELFORMAT: The pixel format was invalid as specified. ";
  else if(DDERR_INVALIDPOSITION == hr) errDesc = "DDERR_INVALIDPOSITION: The position of the overlay on the destination is no longer legal. ";
  else if(DDERR_INVALIDRECT == hr) errDesc = "DDERR_INVALIDRECT: The provided rectangle was invalid. ";
  else if(DDERR_INVALIDSTREAM == hr) errDesc = "DDERR_INVALIDSTREAM: The specified stream contains invalid data. ";
  else if(DDERR_INVALIDSURFACETYPE == hr) errDesc = "DDERR_INVALIDSURFACETYPE: The surface was of the wrong type. ";
  else if(DDERR_LOCKEDSURFACES == hr) errDesc = "DDERR_LOCKEDSURFACES: One or more surfaces are locked, causing the failure of the requested operation. ";
  else if(DDERR_MOREDATA == hr) errDesc = "DDERR_MOREDATA: There is more data available than the specified buffer size can hold. ";
  else if(DDERR_NEWMODE == hr) errDesc = "DDERR_NEWMODE: New for DirectX 7.0. When IDirectDraw7::StartModeTest is called with the DDSMT_ISTESTREQUIRED flag, it may return this value to denote that some or all of the resolutions can and should be tested. IDirectDraw7::EvaluateMode returns this value to indicate that the test has switched to a new display mode. ";
  else if(DDERR_NO3D == hr) errDesc = "DDERR_NO3D: No 3-D hardware or emulation is present. ";
  else if(DDERR_NOALPHAHW == hr) errDesc = "DDERR_NOALPHAHW: No alpha-acceleration hardware is present or available, causing the failure of the requested operation. ";
  else if(DDERR_NOBLTHW == hr) errDesc = "DDERR_NOBLTHW: No blitter hardware is present. ";
  else if(DDERR_NOCLIPLIST == hr) errDesc = "DDERR_NOCLIPLIST: No clip list is available. ";
  else if(DDERR_NOCLIPPERATTACHED == hr) errDesc = "DDERR_NOCLIPPERATTACHED: No DirectDrawClipper object is attached to the surface object. ";
  else if(DDERR_NOCOLORCONVHW == hr) errDesc = "DDERR_NOCOLORCONVHW: No color-conversion hardware is present or available. ";
  else if(DDERR_NOCOLORKEY == hr) errDesc = "DDERR_NOCOLORKEY: The surface does not currently have a color key. ";
  else if(DDERR_NOCOLORKEYHW == hr) errDesc = "DDERR_NOCOLORKEYHW: There is no hardware support for the destination color key. ";
  else if(DDERR_NOCOOPERATIVELEVELSET == hr) errDesc = "DDERR_NOCOOPERATIVELEVELSET: A create function was called without the IDirectDraw7::SetCooperativeLevel method. ";
  else if(DDERR_NODC == hr) errDesc = "DDERR_NODC: No device context (DC) has ever been created for this surface. ";
  else if(DDERR_NODDROPSHW == hr) errDesc = "DDERR_NODDROPSHW: No DirectDraw raster-operation (ROP) hardware is available. ";
  else if(DDERR_NODIRECTDRAWHW == hr) errDesc = "DDERR_NODIRECTDRAWHW: Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. ";
  else if(DDERR_NODIRECTDRAWSUPPORT == hr) errDesc = "DDERR_NODIRECTDRAWSUPPORT: DirectDraw support is not possible with the current display driver. ";
  else if(DDERR_NODRIVERSUPPORT == hr) errDesc = "DDERR_NODRIVERSUPPORT: New for DirectX 7.0. Testing cannot proceed because the display adapter driver does not enumerate refresh rates. ";
  else if(DDERR_NOEMULATION == hr) errDesc = "DDERR_NOEMULATION: Software emulation is not available. ";
  else if(DDERR_NOEXCLUSIVEMODE == hr) errDesc = "DDERR_NOEXCLUSIVEMODE: The operation requires the application to have exclusive mode, but the application does not have exclusive mode. ";
  else if(DDERR_NOFLIPHW == hr) errDesc = "DDERR_NOFLIPHW: Flipping visible surfaces is not supported. ";
  else if(DDERR_NOFOCUSWINDOW == hr) errDesc = "DDERR_NOFOCUSWINDOW: An attempt was made to create or set a device window without first setting the focus window. ";
  else if(DDERR_NOGDI == hr) errDesc = "DDERR_NOGDI: No GDI is present. ";
  else if(DDERR_NOHWND == hr) errDesc = "DDERR_NOHWND: Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle. ";
  else if(DDERR_NOMIPMAPHW == hr) errDesc = "DDERR_NOMIPMAPHW: No mipmap-capable texture mapping hardware is present or available. ";
  else if(DDERR_NOMIRRORHW == hr) errDesc = "DDERR_NOMIRRORHW: No mirroring hardware is present or available. ";
  else if(DDERR_NOMONITORINFORMATION == hr) errDesc = "DDERR_NOMONITORINFORMATION: New for DirectX 7.0. Testing cannot proceed because the monitor has no associated EDID data. ";
  else if(DDERR_NONONLOCALVIDMEM == hr) errDesc = "DDERR_NONONLOCALVIDMEM: An attempt was made to allocate nonlocal video memory from a device that does not support nonlocal video memory. ";
  else if(DDERR_NOOPTIMIZEHW == hr) errDesc = "DDERR_NOOPTIMIZEHW: The device does not support optimized surfaces. ";
  else if(DDERR_NOOVERLAYDEST == hr) errDesc = "DDERR_NOOVERLAYDEST: The IDirectDrawSurface7::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface7::UpdateOverlay method has not been called on to establish as a destination. ";
  else if(DDERR_NOOVERLAYHW == hr) errDesc = "DDERR_NOOVERLAYHW: No overlay hardware is present or available. ";
  else if(DDERR_NOPALETTEATTACHED == hr) errDesc = "DDERR_NOPALETTEATTACHED: No palette object is attached to this surface. ";
  else if(DDERR_NOPALETTEHW == hr) errDesc = "DDERR_NOPALETTEHW: There is no hardware support for 16- or 256-color palettes. ";
  else if(DDERR_NORASTEROPHW == hr) errDesc = "DDERR_NORASTEROPHW: No appropriate raster-operation hardware is present or available. ";
  else if(DDERR_NOROTATIONHW == hr) errDesc = "DDERR_NOROTATIONHW: No rotation hardware is present or available. ";
  else if(DDERR_NOSTEREOHARDWARE == hr) errDesc = "DDERR_NOSTEREOHARDWARE: There is no stereo hardware present or available. ";
  else if(DDERR_NOSTRETCHHW == hr) errDesc = "DDERR_NOSTRETCHHW: There is no hardware support for stretching. ";
  else if(DDERR_NOSURFACELEFT == hr) errDesc = "DDERR_NOSURFACELEFT: There is no hardware present that supports stereo surfaces. ";
  else if(DDERR_NOT4BITCOLOR == hr) errDesc = "DDERR_NOT4BITCOLOR: The DirectDrawSurface object is not using a 4-bit color palette, and the requested operation requires a 4-bit color palette. ";
  else if(DDERR_NOT4BITCOLORINDEX == hr) errDesc = "DDERR_NOT4BITCOLORINDEX: The DirectDrawSurface object is not using a 4-bit color index palette, and the requested operation requires a 4-bit color index palette. ";
  else if(DDERR_NOT8BITCOLOR == hr) errDesc = "DDERR_NOT8BITCOLOR: The DirectDrawSurface object is not using an 8-bit color palette, and the requested operation requires an 8-bit color palette. ";
  else if(DDERR_NOTAOVERLAYSURFACE == hr) errDesc = "DDERR_NOTAOVERLAYSURFACE: An overlay component is called for a nonoverlay surface. ";
  else if(DDERR_NOTEXTUREHW == hr) errDesc = "DDERR_NOTEXTUREHW: The operation cannot be carried out because no texture-mapping hardware is present or available. ";
  else if(DDERR_NOTFLIPPABLE == hr) errDesc = "DDERR_NOTFLIPPABLE: An attempt was made to flip a surface that cannot be flipped. ";
  else if(DDERR_NOTFOUND == hr) errDesc = "DDERR_NOTFOUND: The requested item was not found. ";
  else if(DDERR_NOTINITIALIZED == hr) errDesc = "DDERR_NOTINITIALIZED: An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized. ";
  else if(DDERR_NOTLOADED == hr) errDesc = "DDERR_NOTLOADED: The surface is an optimized surface, but it has not yet been allocated any memory. ";
  else if(DDERR_NOTLOCKED == hr) errDesc = "DDERR_NOTLOCKED: An attempt was made to unlock a surface that was not locked. ";
  else if(DDERR_NOTPAGELOCKED == hr) errDesc = "DDERR_NOTPAGELOCKED: An attempt was made to page-unlock a surface with no outstanding page locks. ";
  else if(DDERR_NOTPALETTIZED == hr) errDesc = "DDERR_NOTPALETTIZED: The surface being used is not a palette-based surface. ";
  else if(DDERR_NOVSYNCHW == hr) errDesc = "DDERR_NOVSYNCHW: There is no hardware support for vertical blank synchronized operations. ";
  else if(DDERR_NOZBUFFERHW == hr) errDesc = "DDERR_NOZBUFFERHW: The operation to create a z-buffer in display memory or to perform a blit, using a z-buffer cannot be carried out because there is no hardware support for z-buffers. ";
  else if(DDERR_NOZOVERLAYHW == hr) errDesc = "DDERR_NOZOVERLAYHW: The overlay surfaces cannot be z-layered, based on the z-order because the hardware does not support z-ordering of overlays. ";
  else if(DDERR_OUTOFCAPS == hr) errDesc = "DDERR_OUTOFCAPS: The hardware needed for the requested operation has already been allocated. ";
  else if(DDERR_OUTOFMEMORY == hr) errDesc = "DDERR_OUTOFMEMORY: DirectDraw does not have enough memory to perform the operation. ";
  else if(DDERR_OUTOFVIDEOMEMORY == hr) errDesc = "DDERR_OUTOFVIDEOMEMORY: DirectDraw does not have enough display memory to perform the operation. ";
  else if(DDERR_OVERLAPPINGRECTS == hr) errDesc = "DDERR_OVERLAPPINGRECTS: The source and destination rectangles are on the same surface and overlap each other. ";
  else if(DDERR_OVERLAYCANTCLIP == hr) errDesc = "DDERR_OVERLAYCANTCLIP: The hardware does not support clipped overlays. ";
  else if(DDERR_OVERLAYCOLORKEYONLYONEACTIVE == hr) errDesc = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE: An attempt was made to have more than one color key active on an overlay. ";
  else if(DDERR_OVERLAYNOTVISIBLE == hr) errDesc = "DDERR_OVERLAYNOTVISIBLE: The IDirectDrawSurface7::GetOverlayPosition method was called on a hidden overlay. ";
  else if(DDERR_PALETTEBUSY == hr) errDesc = "DDERR_PALETTEBUSY: Access to this palette is refused because the palette is locked by another thread. ";
  else if(DDERR_PRIMARYSURFACEALREADYEXISTS == hr) errDesc = "DDERR_PRIMARYSURFACEALREADYEXISTS: This process has already created a primary surface. ";
  else if(DDERR_REGIONTOOSMALL == hr) errDesc = "DDERR_REGIONTOOSMALL: The region passed to the IDirectDrawClipper::GetClipList method is too small. ";
  else if(DDERR_SURFACEALREADYATTACHED == hr) errDesc = "DDERR_SURFACEALREADYATTACHED: An attempt was made to attach a surface to another surface to which it is already attached. ";
  else if(DDERR_SURFACEALREADYDEPENDENT == hr) errDesc = "DDERR_SURFACEALREADYDEPENDENT: An attempt was made to make a surface a dependency of another surface on which it is already dependent. ";
  else if(DDERR_SURFACEBUSY == hr) errDesc = "DDERR_SURFACEBUSY: Access to the surface is refused because the surface is locked by another thread. ";
  else if(DDERR_SURFACEISOBSCURED == hr) errDesc = "DDERR_SURFACEISOBSCURED: Access to the surface is refused because the surface is obscured. ";
  else if(DDERR_SURFACELOST == hr) errDesc = "DDERR_SURFACELOST: Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it. ";
  else if(DDERR_SURFACENOTATTACHED == hr) errDesc = "DDERR_SURFACENOTATTACHED: The requested surface is not attached. ";
  else if(DDERR_TESTFINISHED == hr) errDesc = "DDERR_TESTFINISHED: New for DirectX 7.0. When returned by the IDirectDraw7::StartModeTest method, this value means that no test could be initiated because all the resolutions chosen for testing already have refresh rate information in the registry. When returned by IDirectDraw7::EvaluateMode, the value means that DirectDraw has completed a refresh rate test. ";
  else if(DDERR_TOOBIGHEIGHT == hr) errDesc = "DDERR_TOOBIGHEIGHT: The height requested by DirectDraw is too large. ";
  else if(DDERR_TOOBIGSIZE == hr) errDesc = "DDERR_TOOBIGSIZE: The size requested by DirectDraw is too large. However, the individual height and width are valid sizes. ";
  else if(DDERR_TOOBIGWIDTH == hr) errDesc = "DDERR_TOOBIGWIDTH: The width requested by DirectDraw is too large. ";
  else if(DDERR_UNSUPPORTED == hr) errDesc = "DDERR_UNSUPPORTED: The operation is not supported. ";
  else if(DDERR_UNSUPPORTEDFORMAT == hr) errDesc = "DDERR_UNSUPPORTEDFORMAT: The pixel format requested is not supported by DirectDraw. ";
  else if(DDERR_UNSUPPORTEDMASK == hr) errDesc = "DDERR_UNSUPPORTEDMASK: The bitmask in the pixel format requested is not supported by DirectDraw. ";
  else if(DDERR_UNSUPPORTEDMODE == hr) errDesc = "DDERR_UNSUPPORTEDMODE: The display is currently in an unsupported mode. ";
  else if(DDERR_VERTICALBLANKINPROGRESS == hr) errDesc = "DDERR_VERTICALBLANKINPROGRESS: A vertical blank is in progress. ";
  else if(DDERR_VIDEONOTACTIVE == hr) errDesc = "DDERR_VIDEONOTACTIVE: The video port is not active. ";
  else if(DDERR_WASSTILLDRAWING == hr) errDesc = "DDERR_WASSTILLDRAWING: The previous blit operation that is transferring information to or from this surface is incomplete. ";
  else if(DDERR_WRONGMODE == hr) errDesc = "DDERR_WRONGMODE: This surface cannot be restored because it was created in a different mode. ";
  else if(DDERR_XALIGN == hr) errDesc = "DDERR_XALIGN: The provided rectangle was not horizontally aligned on a required boundary. ";

  ossError << errDesc;
  logEngineError(ossError.str());
}



void
reportD3D7Error(HRESULT hr, string file, long line)
{
  ostringstream ossError;
  ossError << file << "(" << line << ") : D3D7 ";
  string errDesc;

  if(D3D_OK == hr) errDesc = "D3D_OK: No error occurred. ";
  else if(D3DERR_BADMAJORVERSION == hr) errDesc = "D3DERR_BADMAJORVERSION: The service that you requested is unavailable in this major version of DirectX. (A major version denotes a primary release, such as DirectX 6.0.) ";
  else if(D3DERR_BADMINORVERSION == hr) errDesc = "D3DERR_BADMINORVERSION: The service that you requested is available in this major version of DirectX, but not in this minor version. Get the latest version of the component run time from Microsoft. (A minor version denotes a secondary release, such as DirectX 6.1.) ";
  else if(D3DERR_COLORKEYATTACHED == hr) errDesc = "D3DERR_COLORKEYATTACHED: The application attempted to create a texture with a surface that uses a color key for transparency. ";
  else if(D3DERR_CONFLICTINGTEXTUREFILTER == hr) errDesc = "D3DERR_CONFLICTINGTEXTUREFILTER: The current texture filters cannot be used together. ";
  else if(D3DERR_CONFLICTINGTEXTUREPALETTE == hr) errDesc = "D3DERR_CONFLICTINGTEXTUREPALETTE: The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palettized textures simultaneously enabled also share the same palette. ";
  else if(D3DERR_CONFLICTINGRENDERSTATE == hr) errDesc = "D3DERR_CONFLICTINGRENDERSTATE: The currently set render states cannot be used together. ";
  else if(D3DERR_DEVICEAGGREGATED == hr) errDesc = "D3DERR_DEVICEAGGREGATED: The IDirect3DDevice7::SetRenderTarget method was called on a device that was retrieved from the render target surface. ";
  else if(D3DERR_EXECUTE_CLIPPED_FAILED == hr) errDesc = "D3DERR_EXECUTE_CLIPPED_FAILED: The execute buffer could not be clipped during execution. ";
  else if(D3DERR_EXECUTE_CREATE_FAILED == hr) errDesc = "D3DERR_EXECUTE_CREATE_FAILED: The execute buffer could not be created. This typically occurs when no memory is available to allocate the execute buffer. ";
  else if(D3DERR_EXECUTE_DESTROY_FAILED == hr) errDesc = "D3DERR_EXECUTE_DESTROY_FAILED: The memory for the execute buffer could not be deallocated. ";
  else if(D3DERR_EXECUTE_FAILED == hr) errDesc = "D3DERR_EXECUTE_FAILED: The contents of the execute buffer are invalid and cannot be executed. ";
  else if(D3DERR_EXECUTE_LOCK_FAILED == hr) errDesc = "D3DERR_EXECUTE_LOCK_FAILED: The execute buffer could not be locked. ";
  else if(D3DERR_EXECUTE_LOCKED == hr) errDesc = "D3DERR_EXECUTE_LOCKED: The operation requested by the application could not be completed because the execute buffer is locked. ";
  else if(D3DERR_EXECUTE_NOT_LOCKED == hr) errDesc = "D3DERR_EXECUTE_NOT_LOCKED: The execute buffer could not be unlocked because it is not currently locked. ";
  else if(D3DERR_EXECUTE_UNLOCK_FAILED == hr) errDesc = "D3DERR_EXECUTE_UNLOCK_FAILED: The execute buffer could not be unlocked. ";
  else if(D3DERR_INBEGIN == hr) errDesc = "D3DERR_INBEGIN: The requested operation cannot be completed while scene rendering is taking place. Try again after the scene is completed and the IDirect3DDevice7::EndScene method is called. ";
  else if(D3DERR_INBEGINSTATEBLOCK == hr) errDesc = "D3DERR_INBEGINSTATEBLOCK: The operation cannot be completed while recording states for a state block. Complete recording by calling the IDirect3DDevice7::EndStateBlock method, and try again. ";
  else if(D3DERR_INITFAILED == hr) errDesc = "D3DERR_INITFAILED: A rendering device could not be created because the new device could not be initialized. ";
  else if(D3DERR_INVALID_DEVICE == hr) errDesc = "D3DERR_INVALID_DEVICE: The requested device type is not valid. ";
  else if(D3DERR_INVALIDCURRENTVIEWPORT == hr) errDesc = "D3DERR_INVALIDCURRENTVIEWPORT: The currently selected viewport is not valid. ";
  else if(D3DERR_INVALIDMATRIX == hr) errDesc = "D3DERR_INVALIDMATRIX: The requested operation could not be completed because the combination of the currently set world, view, and projection matrices is invalid (the determinant of the combined matrix is 0). ";
  else if(D3DERR_INVALIDPALETTE == hr) errDesc = "D3DERR_INVALIDPALETTE: The palette associated with a surface is invalid. ";
  else if(D3DERR_INVALIDPRIMITIVETYPE == hr) errDesc = "D3DERR_INVALIDPRIMITIVETYPE: The primitive type specified by the application is invalid. ";
  else if(D3DERR_INVALIDRAMPTEXTURE == hr) errDesc = "D3DERR_INVALIDRAMPTEXTURE: Ramp mode is being used, and the texture handle in the current material does not match the current texture handle that is set as a render state. ";
  else if(D3DERR_INVALIDSTATEBLOCK == hr) errDesc = "D3DERR_INVALIDSTATEBLOCK: The state block handle is invalid. ";
  else if(D3DERR_INVALIDVERTEXFORMAT == hr) errDesc = "D3DERR_INVALIDVERTEXFORMAT: The combination of flexible vertex format flags specified by the application is not valid. ";
  else if(D3DERR_INVALIDVERTEXTYPE == hr) errDesc = "D3DERR_INVALIDVERTEXTYPE: The vertex type specified by the application is invalid. ";
  else if(D3DERR_LIGHT_SET_FAILED == hr) errDesc = "D3DERR_LIGHT_SET_FAILED: The attempt to set lighting parameters for a light object failed. ";
  else if(D3DERR_LIGHTHASVIEWPORT == hr) errDesc = "D3DERR_LIGHTHASVIEWPORT: The requested operation failed because the light object is associated with another viewport. ";
  else if(D3DERR_LIGHTNOTINTHISVIEWPORT == hr) errDesc = "D3DERR_LIGHTNOTINTHISVIEWPORT: The requested operation failed because the light object has not been associated with this viewport. ";
  else if(D3DERR_MATERIAL_CREATE_FAILED == hr) errDesc = "D3DERR_MATERIAL_CREATE_FAILED: The material could not be created. This typically occurs when no memory is available to allocate for the material. ";
  else if(D3DERR_MATERIAL_DESTROY_FAILED == hr) errDesc = "D3DERR_MATERIAL_DESTROY_FAILED: The memory for the material could not be deallocated. ";
  else if(D3DERR_MATERIAL_GETDATA_FAILED == hr) errDesc = "D3DERR_MATERIAL_GETDATA_FAILED: The material parameters could not be retrieved. ";
  else if(D3DERR_MATERIAL_SETDATA_FAILED == hr) errDesc = "D3DERR_MATERIAL_SETDATA_FAILED: The material parameters could not be set. ";
  else if(D3DERR_MATRIX_CREATE_FAILED == hr) errDesc = "D3DERR_MATRIX_CREATE_FAILED: The matrix could not be created. This can occur when no memory is available to allocate for the matrix. ";
  else if(D3DERR_MATRIX_DESTROY_FAILED == hr) errDesc = "D3DERR_MATRIX_DESTROY_FAILED: The memory for the matrix could not be deallocated. ";
  else if(D3DERR_MATRIX_GETDATA_FAILED == hr) errDesc = "D3DERR_MATRIX_GETDATA_FAILED: The matrix data could not be retrieved. This can occur when the matrix was not created by the current device. ";
  else if(D3DERR_MATRIX_SETDATA_FAILED == hr) errDesc = "D3DERR_MATRIX_SETDATA_FAILED: The matrix data could not be set. This can occur when the matrix was not created by the current device. ";
  else if(D3DERR_NOCURRENTVIEWPORT == hr) errDesc = "D3DERR_NOCURRENTVIEWPORT: The viewport parameters could not be retrieved because none have been set. ";
  else if(D3DERR_NOTINBEGIN == hr) errDesc = "D3DERR_NOTINBEGIN: The requested rendering operation could not be completed because scene rendering has not begun. Call IDirect3DDevice7::BeginScene to begin rendering, and try again. ";
  else if(D3DERR_NOTINBEGINSTATEBLOCK == hr) errDesc = "D3DERR_NOTINBEGINSTATEBLOCK: The requested operation could not be completed because it is only valid while recording a state block. Call the IDirect3DDevice7::BeginStateBlock method, and try again. ";
  else if(D3DERR_NOVIEWPORTS == hr) errDesc = "D3DERR_NOVIEWPORTS: The requested operation failed because the device currently has no viewports associated with it. ";
  else if(D3DERR_SCENE_BEGIN_FAILED == hr) errDesc = "D3DERR_SCENE_BEGIN_FAILED: Scene rendering could not begin. ";
  else if(D3DERR_SCENE_END_FAILED == hr) errDesc = "D3DERR_SCENE_END_FAILED: Scene rendering could not be completed. ";
  else if(D3DERR_SCENE_IN_SCENE == hr) errDesc = "D3DERR_SCENE_IN_SCENE: Scene rendering could not begin because a previous scene was not completed by a call to the IDirect3DDevice7::EndScene method. ";
  else if(D3DERR_SCENE_NOT_IN_SCENE == hr) errDesc = "D3DERR_SCENE_NOT_IN_SCENE: Scene rendering could not be completed because a scene was not started by a previous call to the IDirect3DDevice7::BeginScene method. ";
  else if(D3DERR_SETVIEWPORTDATA_FAILED == hr) errDesc = "D3DERR_SETVIEWPORTDATA_FAILED: The viewport parameters could not be set. ";
  else if(D3DERR_STENCILBUFFER_NOTPRESENT == hr) errDesc = "D3DERR_STENCILBUFFER_NOTPRESENT: The requested stencil buffer operation could not be completed because there is no stencil buffer attached to the render target surface. ";
  else if(D3DERR_SURFACENOTINVIDMEM == hr) errDesc = "D3DERR_SURFACENOTINVIDMEM: The device could not be created because the render target surface is not located in video memory. (Hardware-accelerated devices require video-memory render target surfaces.) ";
  else if(D3DERR_TEXTURE_BADSIZE == hr) errDesc = "D3DERR_TEXTURE_BADSIZE: The dimensions of a current texture are invalid. This can occur when an application attempts to use a texture that has dimensions that are not a power of 2 with a device that requires them. ";
  else if(D3DERR_TEXTURE_CREATE_FAILED == hr) errDesc = "D3DERR_TEXTURE_CREATE_FAILED: The texture handle for the texture could not be retrieved from the driver. ";
  else if(D3DERR_TEXTURE_DESTROY_FAILED == hr) errDesc = "D3DERR_TEXTURE_DESTROY_FAILED: The device was unable to deallocate the texture memory. ";
  else if(D3DERR_TEXTURE_GETSURF_FAILED == hr) errDesc = "D3DERR_TEXTURE_GETSURF_FAILED: The DirectDraw surface used to create the texture could not be retrieved. ";
  else if(D3DERR_TEXTURE_LOAD_FAILED == hr) errDesc = "D3DERR_TEXTURE_LOAD_FAILED: The texture could not be loaded. ";
  else if(D3DERR_TEXTURE_LOCK_FAILED == hr) errDesc = "D3DERR_TEXTURE_LOCK_FAILED: The texture could not be locked. ";
  else if(D3DERR_TEXTURE_LOCKED == hr) errDesc = "D3DERR_TEXTURE_LOCKED: The requested operation could not be completed because the texture surface is currently locked. ";
  else if(D3DERR_TEXTURE_NO_SUPPORT == hr) errDesc = "D3DERR_TEXTURE_NO_SUPPORT: The device does not support texture mapping. ";
  else if(D3DERR_TEXTURE_NOT_LOCKED == hr) errDesc = "D3DERR_TEXTURE_NOT_LOCKED: The requested operation could not be completed because the texture surface is not locked. ";
  else if(D3DERR_TEXTURE_SWAP_FAILED == hr) errDesc = "D3DERR_TEXTURE_SWAP_FAILED: The texture handles could not be swapped. ";
  else if(D3DERR_TEXTURE_UNLOCK_FAILED == hr) errDesc = "D3DERR_TEXTURE_UNLOCK_FAILED: The texture surface could not be unlocked. ";
  else if(D3DERR_TOOMANYOPERATIONS == hr) errDesc = "D3DERR_TOOMANYOPERATIONS: The application is requesting more texture-filtering operations than the device supports. ";
  else if(D3DERR_TOOMANYPRIMITIVES == hr) errDesc = "D3DERR_TOOMANYPRIMITIVES: The device is unable to render the provided number of primitives in a single pass. ";
  else if(D3DERR_UNSUPPORTEDALPHAARG == hr) errDesc = "D3DERR_UNSUPPORTEDALPHAARG: The device does not support one of the specified texture-blending arguments for the alpha channel. ";
  else if(D3DERR_UNSUPPORTEDALPHAOPERATION == hr) errDesc = "D3DERR_UNSUPPORTEDALPHAOPERATION: The device does not support one of the specified texture-blending operations for the alpha channel. ";
  else if(D3DERR_UNSUPPORTEDCOLORARG == hr) errDesc = "D3DERR_UNSUPPORTEDCOLORARG: The device does not support one of the specified texture-blending arguments for color values. ";
  else if(D3DERR_UNSUPPORTEDCOLOROPERATION == hr) errDesc = "D3DERR_UNSUPPORTEDCOLOROPERATION: The device does not support one of the specified texture-blending operations for color values. ";
  else if(D3DERR_UNSUPPORTEDFACTORVALUE == hr) errDesc = "D3DERR_UNSUPPORTEDFACTORVALUE: The specified texture factor value is not supported by the device. ";
  else if(D3DERR_UNSUPPORTEDTEXTUREFILTER == hr) errDesc = "D3DERR_UNSUPPORTEDTEXTUREFILTER: The specified texture filter is not supported by the device. ";
  else if(D3DERR_VBUF_CREATE_FAILED == hr) errDesc = "D3DERR_VBUF_CREATE_FAILED: The vertex buffer could not be created. This can happen when there is insufficient memory to allocate a vertex buffer. ";
  else if(D3DERR_VERTEXBUFFERLOCKED == hr) errDesc = "D3DERR_VERTEXBUFFERLOCKED: The requested operation could not be completed because the vertex buffer is locked. ";
  else if(D3DERR_VERTEXBUFFEROPTIMIZED == hr) errDesc = "D3DERR_VERTEXBUFFEROPTIMIZED: The requested operation could not be completed because the vertex buffer is optimized. (The contents of optimized vertex buffers are driver-specific and considered private.) ";
  else if(D3DERR_VERTEXBUFFERUNLOCKFAILED == hr) errDesc = "D3DERR_VERTEXBUFFERUNLOCKFAILED: The vertex buffer could not be unlocked because the vertex buffer memory was overrun. Be sure that your application does not write beyond the size of the vertex buffer. ";
  else if(D3DERR_VIEWPORTDATANOTSET == hr) errDesc = "D3DERR_VIEWPORTDATANOTSET: The requested operation could not be completed because viewport parameters have not yet been set. Set the viewport parameters by calling the IDirect3DDevice7::SetViewport method, and try again. ";
  else if(D3DERR_VIEWPORTHASNODEVICE == hr) errDesc = "D3DERR_VIEWPORTHASNODEVICE: This value is used only by the IDirect3DDevice3 interface and its predecessors. For the IDirect3DDevice7 interface, this error value is not used. The requested operation could not be completed because the viewport has not yet been associated with a device. Associate the viewport with a rendering device by calling the IDirect3DDevice3::AddViewport method, and try again.";
  else if(D3DERR_WRONGTEXTUREFORMAT == hr) errDesc = "D3DERR_WRONGTEXTUREFORMAT: The pixel format of the texture surface is not valid. ";
  else if(D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY == hr) errDesc = "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: The requested operation could not be completed because the specified device requires system-memory depth-buffer surfaces. (Software rendering devices require system-memory depth buffers.) ";
  else if(D3DERR_ZBUFF_NEEDS_VIDEOMEMORY == hr) errDesc = "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: The requested operation could not be completed because the specified device requires video-memory depth-buffer surfaces. (Hardware-accelerated devices require video-memory depth buffers.) ";
  else if(D3DERR_ZBUFFER_NOTPRESENT == hr) errDesc = "D3DERR_ZBUFFER_NOTPRESENT: The requested operation could not be completed because the render target surface does not have an attached depth buffer. ";

  ossError << errDesc;
  logEngineError(ossError.str());
}


  

