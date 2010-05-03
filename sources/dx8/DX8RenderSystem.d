//--------------------------------------------------------------------------------
//
// ShortHike.com
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


private import Common;
private import Mesh;
private import RenderSystem;
private import Surface;
private import Texture;

private import std.c.windows.windows;
private import std.c.windows.com;
private import std.c.string;
private import d3d8;

private import DX8Mesh;
private import DX8Texture;
private import DX8Util;

//--------------------------------------------------------------------------------

extern (Windows) 
{
  export HWND GetDesktopWindow();

  struct MONITORINFO {
    DWORD  cbSize; 
    RECT   rcMonitor; 
    RECT   rcWork; 
    DWORD  dwFlags; 
  }

  BOOL GetMonitorInfoA(HMONITOR hMonitor,  // handle to display monitor
                       MONITORINFO* lpmi  // display monitor information 
                       );
}

//--------------------------------------------------------------------------------

bool
createDX8RenderSystem(WNDPROC globalMessageProcedure, out RenderSystem outRenderSystem)
{
  DX8RenderSystem renderSystem = new DX8RenderSystem();
  // Start by initializing Direct3D8
  if(renderSystem.initialize()) {

    UINT adapter = D3DADAPTER_DEFAULT;

    MONITORINFO monitorInfo;
    ZeroMemory(&monitorInfo, monitorInfo.sizeof);
    monitorInfo.cbSize = monitorInfo.sizeof;    
    GetMonitorInfoA(renderSystem.getDirect3D.GetAdapterMonitor(adapter), &monitorInfo);
    RECT windowSize = monitorInfo.rcMonitor;

    // Now create the required window
    int windowWidth;
    int windowHeight;
    bool fullScreen;
  
    //     MONITORENUMPROC enumProc = cast(MONITORENUMPROC)&MonitorInfoEnumProc;
    //     EnumDisplayMonitors(null, null, enumProc, 0);
  
    HINSTANCE hInst = GetModuleHandleA(null);
    WNDCLASS windowClass;
    windowClass.lpszClassName = "ShortHike";
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = globalMessageProcedure;
    windowClass.hInstance = hInst;
    windowClass.hIcon = LoadIconA(hInst, "SHORTHIKEICON");
    windowClass.hCursor = LoadCursorA(cast(HINSTANCE) null, IDC_ARROW);
    windowClass.hbrBackground = cast(HBRUSH) (COLOR_WINDOW + 1);
    windowClass.lpszMenuName = null;
    windowClass.cbClsExtra = windowClass.cbWndExtra = 0;
    RegisterClassA(&windowClass);

    //     //   applicationWindow;
    //     //   WNDCLASSA windowClass;
    //     //   ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
  
    //     //   windowClass.cbSize = WNDCLASSA.sizeof;
    //     //   windowClass.style = CS_CLASSDC;
    //     //   windowClass.lpfnWndProc = cast(WNDPROC)&globalMessageProcedure;
    //     //   windowClass.hInstance = GetModuleHandleA(null);
    //     //   windowClass.hIcon = LoadIconA(gCurrentInstance, "SHORTHIKEICON");
    //     //   windowClass.hCursor = LoadCursorA(null, IDC_ARROW);
    //     //   windowClass.lpszClassName = "ShortHike";

    //     //   RegisterClassA(&windowClass);

    // FIXME: Read from config files.
    //     int displayMonitor = 0;
    //     displayMonitor = clamp(displayMonitor, 0, cast(int)(gMonitorBounds.length - 1));
    //     windowSize = gMonitorBounds[displayMonitor];

    windowWidth = windowSize.right - windowSize.left;
    windowHeight = windowSize.bottom - windowSize.top;
    // Force smaller window
    //     windowSize.left = 20;
    //     windowSize.top = 20;
    //     windowWidth = 1024;
    //     windowHeight = 768;
    //  Force smaller window
    //       windowSize.left = 50;
    //       windowSize.top = 50;
    //       windowWidth = 800;
    //       windowHeight = 600;
    fullScreen = false;
  
    DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    HWND hWindow = CreateWindowA( "ShortHike", "ShortHike", 
                                  windowStyle, windowSize.left, windowSize.top, windowWidth, windowHeight,
                                  GetDesktopWindow(), null, windowClass.hInstance, null);

    ShowWindow(hWindow, SW_SHOWNORMAL);
    //   UpdateWindow(gApplicationWindow);

    //     logInfo() << " - Trying DX8RenderSystem" << endl;




    if(renderSystem.initializeDevice(hWindow, adapter)) {
      outRenderSystem = renderSystem;
      return true;
    }
  }
  delete renderSystem;
  return false;
}


// --------------------------------------------------------------------------------


class DX8RenderSystem : public RenderSystem
{
public:
  this()
  {
    //     mDirectDraw = null;
    //     mDirect3D = null;
    //     mPrimarySurface = null;
    //     mBackBuffer = null;
    //     mZBuffer = null;
    //     mDirect3DDevice = null;
    //     mCurrentMesh = null;
    //     mFrameTriangles = 0;

  }

  ~this()
  {
    //   if(!(null is mDirect3D))
    //     mDirect3D.Release();
    //   if(!(null is mDirectDraw))
    //     mDirectDraw.Release();
  }


  //--------------------------------------------------------------------------------

  bool
  initialize()
  {
    HRESULT hr;
    logInfo() << horizontal_rule << "DX8RenderSystem: Diagnostics" << endl;
    mDirect3D = Direct3DCreate8(D3D_SDK_VERSION); 
    assert(CHECK("Valid root", mDirect3D != null));
    
    int adapterCount = mDirect3D.GetAdapterCount();
    for(int iAdapter = 0; iAdapter < adapterCount; ++iAdapter) {
      D3DADAPTER_IDENTIFIER8 adapterIdentifier;
      mDirect3D.GetAdapterIdentifier(iAdapter, D3DENUM_NO_WHQL_LEVEL, &adapterIdentifier);
      logInfo << " Adapter " << iAdapter << " : " << adapterIdentifier.Description[0 .. strlen(adapterIdentifier.Description)]
                << " (" << adapterIdentifier.Driver[0 .. strlen(adapterIdentifier.Driver)] << ")";
      if(iAdapter == D3DADAPTER_DEFAULT)
        logInfo << " [DEFAULT]";
      logInfo << endl;    
    }
    return true;
  }

  //-----------------------------------------------------------------------------

  IDirect3D8 getDirect3D() {return mDirect3D;}
  IDirect3DDevice8 getDirect3DDevice() {return mDirect3DDevice;}

  //-----------------------------------------------------------------------------

  bool
  isDepthFormatExisting(D3DFORMAT DepthFormat, D3DFORMAT AdapterFormat, UINT adapter)
  {
    HRESULT hr = mDirect3D.CheckDeviceFormat(adapter, D3DDEVTYPE.D3DDEVTYPE_HAL, AdapterFormat,
                                             D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE.D3DRTYPE_SURFACE, DepthFormat);
    return cast(bool)(SUCCEEDED(hr));
  }


  D3DFORMAT
  findDepthFormat(D3DFORMAT displayMode, UINT adapter)
  {
    D3DFORMAT depthFormat = D3DFMT_UNKNOWN;
    if(isDepthFormatExisting(D3DFMT_D32, displayMode, adapter)) {
      depthFormat = D3DFMT_D32;
    }
    else if(isDepthFormatExisting(D3DFMT_D24S8, displayMode, adapter)) {
      depthFormat = D3DFMT_D24S8;
    }
    else if(isDepthFormatExisting(D3DFMT_D24X8, displayMode, adapter)) {
      depthFormat = D3DFMT_D24X8;
    }
    else if(isDepthFormatExisting(D3DFMT_D24X4S4, displayMode, adapter)) {
      depthFormat = D3DFMT_D24X4S4;
    }
    else if(isDepthFormatExisting(D3DFMT_D15S1, displayMode, adapter)) {
      depthFormat = D3DFMT_D15S1;
    }
    else if(isDepthFormatExisting(D3DFMT_D16, displayMode, adapter)) {
      depthFormat = D3DFMT_D16;
    }
    else {
      assert(CHECK_FAIL("No valid depth/stencil buffer found"));
    }
    return depthFormat;
  }


  //-----------------------------------------------------------------------------

  bool
  initializeDevice(HWND hWindow, UINT adapter)
  {
//     IDirect3DDevice8* newScreenDevice;
     D3DCAPS8 deviceCaps;
     HRESULT hr = mDirect3D.GetDeviceCaps(adapter, D3DDEVTYPE.D3DDEVTYPE_HAL, &deviceCaps);
     assert(CHECK("Hardware acceleration", hr == D3D_OK));
     logInfo << " Hardware acceleration    : true" << endl;
//     assert(CHECK("Enquiring about Device capabilities",
//           mDirect3D.GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &deviceCaps) == D3D_OK);

//     if(fullScreen) {
//       Log::info("Creating FULLSCREEN mode device");
//       vector<ScreenMode> screenModes = findSuitableScreenModes(maximumWidth, maximumRefresh);

//       Log::info("\nTrying to initialize D3DDevice ...");

//       for(vector<ScreenMode>::const_iterator modeI = screenModes.begin(); modeI != screenModes.end(); ++modeI) {
//         ScreenMode candidateMode = *modeI;
//         Log::info("Candidate mode:");
//         Log::info(candidateMode.toString());
        
//         ZeroMemory(&mPresentParameters, sizeof(mPresentParameters));
//         mPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
//         mPresentParameters.Windowed = FALSE;
//         mPresentParameters.hDeviceWindow = window;      
//         mPresentParameters.EnableAutoDepthStencil = TRUE;
//         mPresentParameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
//         mPresentParameters.FullScreen_RefreshRateInHz = candidateMode.refresh;
//         mPresentParameters.BackBufferWidth = candidateMode.width;
//         mPresentParameters.BackBufferHeight = candidateMode.height;
//         mPresentParameters.BackBufferFormat = candidateMode.displayFormat;
//         mPresentParameters.BackBufferCount = 1;
//         mPresentParameters.AutoDepthStencilFormat = candidateMode.depthFormat;
//         if(antialiasing) {
//           mPresentParameters.MultiSampleType = findMultisampleType(candidateMode);
//           ostringstream multisampleStream;
//           multisampleStream << "Multisampling enabled: " << mPresentParameters.MultiSampleType << " samples";
//           Log::info(multisampleStream.str());
//         }
//         else
//           mPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;



//         HRESULT createDeviceResult =
//           mDirect3D.CreateDevice(adapter, D3DDEVTYPE_HAL, window,
//                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
//                                      &mPresentParameters, &newScreenDevice);
//         if(createDeviceResult == D3D_OK) {
//           Log::info("D3D device created successfully!");
//           return newScreenDevice;
//         }
        
//         Log::info(hresultToString(createDeviceResult));
//       }

//     }

//     Log::info("Creating WINDOWED mode device");
     CHECK("Checking for windowed mode support in device", (deviceCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) != 0);
     logInfo << " Windowed mode support    : true" << endl;

     logInfo << " Non square textures      : ";
     if(deviceCaps.Caps2 & D3DPTEXTURECAPS_SQUAREONLY == 0)
       logInfo << "FALSE" << endl;
     else
       logInfo << "true" << endl;
     
     logInfo << " Non pow2 textures        : ";
     if(deviceCaps.Caps2 & D3DPTEXTURECAPS_POW2 == 0)
       logInfo << "FALSE" << endl;
     else
       logInfo << "true" << endl;
     
     logInfo << " Max texture size         : " << deviceCaps.MaxTextureWidth << "x" << deviceCaps.MaxTextureHeight << endl;
     logInfo << " Max texture aspect ratio : " << deviceCaps.MaxTextureAspectRatio << endl;

     D3DDISPLAYMODE currentScreenMode;
     CHECK("Finding current screen mode",
           mDirect3D.GetAdapterDisplayMode( adapter, &currentScreenMode) == D3D_OK);

     logInfo << " Desktop display          : " << currentScreenMode.Width << "x" << currentScreenMode.Height << " "
       << currentScreenMode.RefreshRate << "Hz " << formatToString(currentScreenMode.Format) << endl;
     //DirectXRenderEngine::formatToString(mode.Format);
//     Log::info("Desktop display mode:");
//     Log::info(modeToString(currentScreenMode));

     D3DFORMAT depthFormat = findDepthFormat(currentScreenMode.Format, adapter);
     logInfo << " Depth/Stencil mode       : " << formatToString(depthFormat) << endl;
//     Log::info("Depth/Stencil mode selected:");
//     Log::info(DirectXRenderEngine::formatToString(depthFormat));
  
     ZeroMemory(cast(byte*)&mPresentParameters, mPresentParameters.sizeof);
     mPresentParameters.Windowed = TRUE;
     mPresentParameters.SwapEffect = D3DSWAPEFFECT.D3DSWAPEFFECT_DISCARD;
     mPresentParameters.BackBufferFormat = currentScreenMode.Format;
     mPresentParameters.EnableAutoDepthStencil = TRUE;
     mPresentParameters.AutoDepthStencilFormat = depthFormat;      
     mPresentParameters.MultiSampleType = D3DMULTISAMPLE_TYPE.D3DMULTISAMPLE_NONE;
     
     CHECK("Creating windowed Direct3D device",
           mDirect3D.CreateDevice(adapter, D3DDEVTYPE.D3DDEVTYPE_HAL, hWindow,
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &mPresentParameters, &mDirect3DDevice ) == D3D_OK);
     logInfo << " Texture memory available : " << mDirect3DDevice.GetAvailableTextureMem()/1000000 << "MB" << endl;
     logInfo() << horizontal_rule;

     RECT cRect;
     GetClientRect(hWindow, &cRect);
     mWidth = cRect.right - cRect.left;
     mHeight = cRect.bottom - cRect.top;

    // This mesh is used for all 2D rendering
    {
      mTextureMesh = cast(DX8Mesh)createMesh(false);
      Vertex v0;
      v0.position = createVector(0, 0, 0);
      v0.normal = Vector.UNIT_Z;
      v0.u = 0;
      v0.v = 1;
      mTextureMesh.mVertexData ~= v0;
      Vertex v1;
      v1.position = createVector(1, 0, 0);
      v1.normal = Vector.UNIT_Z;
      v1.u = 1;
      v1.v = 1;
      mTextureMesh.mVertexData ~= v1;
      Vertex v2;
      v2.position = createVector(0, 1, 0);
      v2.normal = Vector.UNIT_Z;
      v2.u = 0;
      v2.v = 0;
      mTextureMesh.mVertexData ~= v2;
      Vertex v3;
      v3.position = createVector(1, 1, 0);
      v3.normal = Vector.UNIT_Z;
      v3.u = 1;
      v3.v = 0;
      mTextureMesh.mVertexData ~= v3;
      mTextureMesh.mIndexData ~= 0;
      mTextureMesh.mIndexData ~= 1;
      mTextureMesh.mIndexData ~= 2;
      mTextureMesh.mIndexData ~= 1;
      mTextureMesh.mIndexData ~= 3;
      mTextureMesh.mIndexData ~= 2;
      mTextureMesh.flush();
    }

     // This shoulb be in beg/end
    return true;
  }

  // --------------------------------------------------------------------------------

  void
  cleanUpDevice()
  {
    // FIXME: Release Mesh resources

    //   if(null != mDirect3DDevice)
    //     mDirect3DDevice.Release();
    //   if(null != mZBuffer)
    //     mZBuffer.Release();
    //   if(null != mBackBuffer)
    //     mBackBuffer.Release();
    //   if(null != mPrimarySurface)
    //     mPrimarySurface.Release();
  
    //   mPrimarySurface = null;
    //   mBackBuffer = null;
    //   mZBuffer = null;
    //   mDirect3DDevice = null;
  
    //   mCurrentMesh = null;
  }


  // --------------------------------------------------------------------------------
  // Resource management

  override Mesh
  createMesh(bool collisionsupport)
  {
    return new DX8Mesh(this, collisionsupport);
  }
  
  override Texture
  createTexture()
  {
    return new DX8Texture(this);
  }

  override Camera
  createCamera()
  {
    return new Camera();
  }


  // --------------------------------------------------------------------------------
  override int
  getWidth()
  {
    return mWidth;
  }

  override int
  getHeight()
  {
    return mHeight;
  }


  // --------------------------------------------------------------------------------
  // Scene management

  override bool
  beginScene()
  {
    //   // Force the video hardware to synchronize with us. Otherwise on light loads the CPU will
    //   // run away.
    //   DDSURFACEDESC2 backBufferDesc;
    //   ZeroMemory(&backBufferDesc, sizeof(backBufferDesc));
    //   backBufferDesc.dwSize = sizeof(backBufferDesc);  
    //   HRESULT hr = mBackBuffer.Lock(null, &backBufferDesc, DDLOCK_READONLY, null);
    //   hr = mBackBuffer.Unlock(null);

    HRESULT hr;
    // Standard begin scene work
    hr = mDirect3DDevice.Clear(0, null, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
    hr = mDirect3DDevice.BeginScene();
    
    // Load a basic working state
    hr = mDirect3DDevice.SetRenderState(D3DRS_AMBIENT, 0x7f7f7f7f);
    hr = mDirect3DDevice.SetTransform(D3DTS_WORLD, cast(D3DMATRIX*)&Matrix.IDENTITY);
    Matrix view = createMatrix(createQuaternion(Vector.UNIT_Y, 0.7f)) *
      createMatrix(createVector(0, 0, 90));
    hr = mDirect3DDevice.SetTransform(D3DTS_VIEW, cast(D3DMATRIX*)&view);
    float nearPlane = 1.5f;
    float farPlane = 10000.0f;
    float q = farPlane/(farPlane-nearPlane);
    Matrix projection = Matrix.ZERO;
    projection[0, 0] = 2;
    projection[1, 1] = 2;
    projection[2, 2] = q;
    projection[3, 2] = -q*nearPlane;
    projection[2, 3] = 1;
    hr = mDirect3DDevice.SetTransform(D3DTS_PROJECTION, cast(D3DMATRIX*)&projection);
    hr = mDirect3DDevice.SetRenderState( D3DRS_ZENABLE, TRUE );
  
    // FIXME: Do this using standard state functions
    mDirect3DDevice.LightEnable(0, TRUE);
    mDirect3DDevice.SetRenderState(D3DRS_LIGHTING, TRUE );
    mDirect3DDevice.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_MAGFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_LINEAR);
    mDirect3DDevice.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_MINFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_LINEAR);
    mDirect3DDevice.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_MIPFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_LINEAR);

    mFrameTriangles = 0;

    return true;
  }

  override void
  endScene()
  {
    mDirect3DDevice.EndScene();
    mDirect3DDevice.Present( null, null, null, null );
  }

  // --------------------------------------------------------------------------------

  override void
  setView(Matrix projectionMatrix, Matrix viewMatrix)
  {
    //   float theta = camera.mFieldOfViewY * 0.5;
    //   float heightScale = 1 / tanf(theta);
    //   float widthScale = heightScale / camera.mAspectRatio;

    //   float depthScale = camera.mFarClipDistance / ( camera.mFarClipDistance - camera.mNearClipDistance );
    //   float depthShift = -depthScale * camera.mNearClipDistance;

    //   Matrix projectionMatrix = Matrix.ZERO;
    //   projectionMatrix[0][0] = widthScale;
    //   projectionMatrix[1][1] = heightScale;  
    //   projectionMatrix[2][2] = depthScale;
    //   projectionMatrix[2][3] = 1.0f;
    //   projectionMatrix[3][2] = depthShift;

    //   D3DMATRIX d3dmat = makeD3DMatrix(projectionMatrix);
  
    HRESULT hr = mDirect3DDevice.SetTransform(D3DTS_PROJECTION, cast(D3DMATRIX*)&projectionMatrix); 
    hr = mDirect3DDevice.SetTransform(D3DTS_VIEW, cast(D3DMATRIX*)&viewMatrix);
  }

  override void
  setViewScreen()
  {
    mDirect3DDevice.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  }

  // --------------------------------------------------------------------------------

  // Optimization: Use same format for surfaces and target

  void
  renderTexture(Texture texture, Rect screenRect, Rect textureRect, Matrix transform)
  {
    DX8Texture dx8Texture = cast(DX8Texture)texture;
    HRESULT hr;
    hr = mDirect3DDevice.SetTexture(0, dx8Texture.mTextureSurface);
    hr = mDirect3DDevice.SetRenderState(D3DRS_WRAP0, 0);
    setViewScreen();

    Matrix projectionMatrix = Matrix.IDENTITY;

    Matrix screenScale = Matrix.IDENTITY;
    //   screenScale.setScaling(Vector( 2.0f / mWidth, 2.0f / mHeight, 1));
    screenScale.setScaling(createVector(2.0f / mWidth, 2.0f / mHeight, 1));
    Matrix screenShift = createMatrix(createVector(-1.0f, -1.0f, 0));
    Matrix viewMatrix = screenScale * screenShift;
    
    Matrix surfaceScale = Matrix.IDENTITY;
    surfaceScale.setScaling(createVector(screenRect.w, -screenRect.h, 1));
    
    Matrix surfaceShift = createMatrix(createVector(cast(float)screenRect.x + 0.5f, mHeight - cast(float)screenRect.y + 0.5f, 0));
    Matrix worldMatrix = surfaceScale * surfaceShift;
    
    Matrix textureScale = Matrix.IDENTITY;
    float textureWidth = texture.getWidth();
    float textureHeight = texture.getHeight();
    textureScale.setScaling(createVector(textureRect.w / textureWidth, textureRect.h / textureHeight, 1));
    Matrix textureShift = Matrix.IDENTITY;
    textureShift._31 = textureRect.x / textureWidth;
    textureShift._32 = textureRect.y / textureHeight;
    Matrix textureMatrix = textureScale * textureShift;
    
    hr = mDirect3DDevice.SetTransform(D3DTS_PROJECTION, cast(D3DMATRIX*)&projectionMatrix);
    hr = mDirect3DDevice.SetTransform(D3DTS_VIEW, cast(D3DMATRIX*)&viewMatrix);
    hr = mDirect3DDevice.SetTransform(D3DTS_WORLD, cast(D3DMATRIX*)&worldMatrix);
    hr = mDirect3DDevice.SetTransform(D3DTS_TEXTURE0, cast(D3DMATRIX*)&textureMatrix);
    
    hr = mDirect3DDevice.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    hr = mDirect3DDevice.SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    hr = mDirect3DDevice.SetRenderState(D3DRS_SRCBLEND, D3DBLEND.D3DBLEND_SRCALPHA);
    hr = mDirect3DDevice.SetRenderState(D3DRS_DESTBLEND, D3DBLEND.D3DBLEND_INVSRCALPHA);
    
    hr = mDirect3DDevice.SetRenderState( D3DRS_LIGHTING, FALSE);
    hr = mDirect3DDevice.SetTextureStageState( 0, D3DTEXTURESTAGESTATETYPE.D3DTSS_TEXTURETRANSFORMFLAGS,
                                               D3DTEXTURETRANSFORMFLAGS.D3DTTFF_COUNT2);
    
    D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, mtrl.sizeof );
    mtrl.Diffuse.r = mtrl.Diffuse.g = mtrl.Diffuse.b = 1.0f;
    mtrl.Ambient.r = mtrl.Ambient.g = mtrl.Ambient.b = 1.0f;
    hr = mDirect3DDevice.SetMaterial( &mtrl );
    
    mFrameTriangles += mTextureMesh.mIndexCount / 3;

    hr = mDirect3DDevice.SetStreamSource(0, mTextureMesh.mVertexBuffer, mTextureMesh.mStride);
    hr = mDirect3DDevice.SetIndices(mTextureMesh.mIndexBuffer, 0);
    hr = mDirect3DDevice.SetVertexShader(DX8Mesh.MESH_FVF);  
    hr = mDirect3DDevice.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, mTextureMesh.mVertexCount, 0, mTextureMesh.mIndexCount / 3);
    hr = mDirect3DDevice.SetTextureStageState( 0, D3DTEXTURESTAGESTATETYPE.D3DTSS_TEXTURETRANSFORMFLAGS,
                                               D3DTEXTURETRANSFORMFLAGS.D3DTTFF_DISABLE);
  }


  // --------------------------------------------------------------------------------
  // Lighting

  override void
  enableLighting(LightEnv env)
  {
    env.primaryDirection.normalize();
    
    D3DLIGHT8 light0;
    ZeroMemory( &light0, D3DLIGHT8.sizeof );
    light0.Type = D3DLIGHTTYPE.D3DLIGHT_DIRECTIONAL;
    light0.Diffuse.r = env.primaryColor.r;
    light0.Diffuse.g = env.primaryColor.g;
    light0.Diffuse.b = env.primaryColor.b;
    light0.Specular.r = env.primaryColor.r;
    light0.Specular.g = env.primaryColor.g;
    light0.Specular.b = env.primaryColor.b;    
    light0.Direction.x = env.primaryDirection.x;
    light0.Direction.y = env.primaryDirection.y;
    light0.Direction.z = env.primaryDirection.z;
    mDirect3DDevice.SetLight(0, &light0);
    mDirect3DDevice.LightEnable(0, TRUE );

    D3DLIGHT8 light1;
    ZeroMemory( &light1, D3DLIGHT8.sizeof );
    light1.Type = D3DLIGHTTYPE.D3DLIGHT_DIRECTIONAL;
    light1.Diffuse.r = env.secondaryColor.r;
    light1.Diffuse.g = env.secondaryColor.g;
    light1.Diffuse.b = env.secondaryColor.b;
    light1.Specular.r = 0;
    light1.Specular.g = 0;
    light1.Specular.b = 0;    
    light1.Direction.x = env.secondaryDirection.x;
    light1.Direction.y = env.secondaryDirection.y;
    light1.Direction.z = env.secondaryDirection.z;
    mDirect3DDevice.SetLight(1, &light1);
    mDirect3DDevice.LightEnable(1, TRUE );

    mDirect3DDevice.SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    mDirect3DDevice.SetRenderState(D3DRS_LIGHTING, TRUE);
  }

  override void
  disableLighting()
  {
    mDirect3DDevice.LightEnable(0, FALSE);
    mDirect3DDevice.LightEnable(1, FALSE);
    mDirect3DDevice.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    mDirect3DDevice.SetRenderState(D3DRS_LIGHTING, FALSE);
  }

  void enableSpecular(bool enabled)
  {
    mDirect3DDevice.SetRenderState(D3DRS_SPECULARENABLE, enabled);
  }
  

  // --------------------------------------------------------------------------------
  // State and mesh rendering  


  override void
  enableZBuffer(bool enabled)
  {
    mDirect3DDevice.SetRenderState(D3DRS_ZENABLE, enabled);
    mDirect3DDevice.SetRenderState(D3DRS_ZWRITEENABLE, enabled);
  }

  override void
  enableAlphaBlending(bool enabled)
  {
    mDirect3DDevice.SetRenderState(D3DRS_ALPHABLENDENABLE, enabled);
    mDirect3DDevice.SetRenderState(D3DRS_SRCBLEND, D3DBLEND.D3DBLEND_SRCALPHA);
    mDirect3DDevice.SetRenderState(D3DRS_DESTBLEND, D3DBLEND.D3DBLEND_INVSRCALPHA);
  }

  // --------------------------------------------------------------------------------

  override void
  setMesh(Mesh mesh, bool wireframe, bool cull)
  {
    mCurrentMesh = cast(DX8Mesh)mesh;
    if(mCurrentMesh is null) return;

//     if(mCurrentMesh.mVertexCount == 0 || mCurrentMesh.mIndexCount == 0 ||
//        mCurrentMesh.mVertexBuffer == null || mCurrentMesh.mIndexBuffer == null) {
//       mCurrentMesh = null;
//       return;
//     }
  
    if(wireframe) {
      mDirect3DDevice.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);    
    }
    else {
      mDirect3DDevice.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }  
    
    if(cull) {
      mDirect3DDevice.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);    
    }
    else {
      mDirect3DDevice.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }  

    HRESULT hr = mDirect3DDevice.SetStreamSource(0, mCurrentMesh.mVertexBuffer, mCurrentMesh.mStride);
    hr = mDirect3DDevice.SetIndices(mCurrentMesh.mIndexBuffer, 0);
    hr = mDirect3DDevice.SetVertexShader(DX8Mesh.MESH_FVF);
  }

  override void
  renderMesh(Matrix matrix)
  {
    if(mCurrentMesh == null || mCurrentMesh.mVertexBuffer == null || mCurrentMesh.mIndexBuffer == null) return;

    mFrameTriangles += mCurrentMesh.mIndexCount / 3;
  
    HRESULT hr = mDirect3DDevice.SetTransform(D3DTS_WORLD, cast(D3DMATRIX*)&matrix);
    hr = mDirect3DDevice.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, mCurrentMesh.mVertexCount, 0, mCurrentMesh.mIndexCount / 3);
  }

  // --------------------------------------------------------------------------------
  // Materials

  override void
  setMaterialTexture(Texture texture)
  {
    DX8Texture dx8Texture = cast(DX8Texture)texture;
    HRESULT hr = mDirect3DDevice.SetTexture(0, dx8Texture.mTextureSurface);

    static Color diffuse = {r:1, g:1, b:1, a:1};
    static Color specular = {r:.5, g:.5, b:.5, a:1};
    static Color ambient = {r:0.5, g:0.5, b:0.5, a:1};
        
    D3DMATERIAL8 material;
    ZeroMemory(cast(byte*)&material, material.sizeof);
    material.Diffuse = cast(D3DCOLORVALUE)diffuse;
    material.Specular = cast(D3DCOLORVALUE)specular;
    material.Ambient = cast(D3DCOLORVALUE)ambient;
    material.Power = 20.0;
    hr = mDirect3DDevice.SetMaterial(&material);
  }

  override void
  setMaterialColor(Color diffuse, Color specular, Color ambient)
  {
    D3DMATERIAL8 material;
    ZeroMemory(cast(byte*)&material, material.sizeof);
    material.Diffuse = cast(D3DCOLORVALUE)diffuse;
    material.Specular = cast(D3DCOLORVALUE)specular;
    material.Ambient = cast(D3DCOLORVALUE)ambient;
    material.Power = 0.0;
    HRESULT hr = mDirect3DDevice.SetMaterial(&material);
    hr = mDirect3DDevice.SetTexture(0, null);
  }


  // --------------------------------------------------------------------------------

  char[]
  formatToString(D3DFORMAT format)
  {
    if(format == D3DFMT_R8G8B8)
      return "D3DFMT_R8G8B8: 24-bit RGB pixel format with 8 bits per channel.";
    else if(format == D3DFMT_A8R8G8B8)
      return "D3DFMT_A8R8G8B8: 32-bit ARGB pixel format with alpha, using 8 bits per channel. ";
    else if(format == D3DFMT_X8R8G8B8)
      return "D3DFMT_X8R8G8B8: 32-bit RGB pixel format, where 8 bits are reserved for each color. ";
    else if(format == D3DFMT_R5G6B5)
      return "D3DFMT_R5G6B5: 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. ";
    else if(format == D3DFMT_X1R5G5B5)
      return "D3DFMT_X1R5G5B5: 16-bit pixel format where 5 bits are reserved for each color. ";
    else if(format == D3DFMT_A1R5G5B5)
      return "D3DFMT_A1R5G5B5: 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. ";
    else if(format == D3DFMT_A4R4G4B4)
      return "D3DFMT_A4R4G4B4: 16-bit ARGB pixel format with 4 bits for each channel. ";
    else if(format == D3DFMT_A8)
      return "D3DFMT_A8: 8-bit alpha only. ";
    else if(format == D3DFMT_R3G3B2)
      return "D3DFMT_R3G3B2: 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. ";
    else if(format == D3DFMT_A8R3G3B2)
      return "D3DFMT_A8R3G3B2: 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. ";
    else if(format == D3DFMT_X4R4G4B4)
      return "D3DFMT_X4R4G4B4: 16-bit RGB pixel format using 4 bits for each color. ";
    else if(format == D3DFMT_A8P8)
      return "D3DFMT_A8P8: 8-bit color indexed with 8 bits of alpha. ";
    else if(format == D3DFMT_P8)
      return "D3DFMT_P8: 8-bit color indexed. ";
    else if(format == D3DFMT_L8)
      return "D3DFMT_L8: 8-bit luminance only. ";
    else if(format == D3DFMT_A8L8)
      return "D3DFMT_A8L8: 16-bit using 8 bits each for alpha and luminance. ";
    else if(format == D3DFMT_A4L4)
      return "D3DFMT_A4L4: 8-bit using 4 bits each for alpha and luminance. ";
    else if(format == D3DFMT_V8U8)
      return "D3DFMT_V8U8: 16-bit bump-map format using 8 bits each for u and v data. ";
    else if(format == D3DFMT_Q8W8V8U8)
      return "D3DFMT_Q8W8V8U8: 32-bit bump-map format using 8 bits for each channel. ";
    else if(format == D3DFMT_V16U16)
      return "D3DFMT_V16U16: 32-bit bump-map format using 16 bits for each channel. ";
    else if(format == D3DFMT_W11V11U10)
      return "D3DFMT_W11V11U10: 32-bit bump-map format using 11 bits each for w and v, and 10 bits for u. ";
    else if(format == D3DFMT_L6V5U5)
      return "D3DFMT_L6V5U5: 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for u and v. ";
    else if(format == D3DFMT_X8L8V8U8)
      return "D3DFMT_X8L8V8U8: 32-bit bump-map format with luminance using 8 bits for each channel. ";
//     else if(format == D3DFMT_DXT1)
//       return "D3DFMT_DXT1: DXT1 compression texture format ";
//     else if(format == D3DFMT_DXT2)
//       return "D3DFMT_DXT2: DXT2 compression texture format ";
//     else if(format == D3DFMT_DXT3)
//       return "D3DFMT_DXT3: DXT3 compression texture format ";
//     else if(format == D3DFMT_DXT4)
//       return "D3DFMT_DXT4: DXT4 compression texture format ";
//     else if(format == D3DFMT_DXT5)
//       return "D3DFMT_DXT5: DXT5 compression texture format ";
    else if(format == D3DFMT_VERTEXDATA)
      return "D3DFMT_VERTEXDATA: Describes a vertex buffer surface. ";
    else if(format == D3DFMT_INDEX16)
      return "D3DFMT_INDEX16: 16-bit index buffer bit depth. ";
    else if(format == D3DFMT_INDEX32)
      return "D3DFMT_INDEX32: 32-bit index buffer bit depth. ";
    else if(format == D3DFMT_D16)
      return "D3DFMT_D16: LOCKABLE 16-bit z-buffer bit depth. This is an application-lockable surface format. ";
    else if(format == D3DFMT_D32)
      return "D3DFMT_D32: 32-bit z-buffer bit depth. ";
    else if(format == D3DFMT_D15S1)
      return "D3DFMT_D15S1: 16-bit z-buffer bit depth where 15 bits are reserved for the depth channel and 1 bit is reserved for the stencil channel. ";
    else if(format == D3DFMT_D24S8)
      return "D3DFMT_D24S8: 32-bit z-buffer bit depth using 24 bits for the depth channel and 8 bits for the stencil channel. ";
    else if(format == D3DFMT_D16)
      return "D3DFMT_D16: 16-bit z-buffer bit depth. ";
    else if(format == D3DFMT_D24X8)
      return "D3DFMT_D24X8: 32-bit z-buffer bit depth using 24 bits for the depth channel. ";
    else if(format == D3DFMT_D24X4S4)
      return "D3DFMT_D24X4S4: 32-bit z-buffer bit depth using 24 bits for the depth channel and 4 bits for the stencil channel. ";
    else if(format == D3DFMT_UNKNOWN)
      return "D3DFMT_UNKNOWN: Surface format is unknown. ";
    return "ERROR: format not recognized";
  }


private:
  IDirect3D8 mDirect3D;
  IDirect3DDevice8 mDirect3DDevice;  
  D3DPRESENT_PARAMETERS mPresentParameters;
  int mWidth;
  int mHeight;

  DX8Mesh mCurrentMesh;
  DX8Mesh mTextureMesh;
  int mFrameTriangles;
}



