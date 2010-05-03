// Test to draw some simple triangles in Direct3D 8.0, in D!
// by Sean L Palmer (seanpalmer@verizon.com)
// This code is released without any warranty.  Use at your own risk.

import win32api;
import d3d8;
import d3d8caps;
import d3d8types;

import time;
import stream;
import string;
import c.stdio;
import vector;
import math;

private:

const float PI = 3.141592f;//math.PI;
HINSTANCE hinst;
HWND hwnd;
IDirect3D8 d3d;
IDirect3DDevice8 dev;
int bitspixel;
int w,h;

alias vector.Vector2 Vc2;
alias vector.Vector3 Vc3;

alias uint Color4b;

Color4b RGBA(ubyte r,ubyte g,ubyte b,ubyte a)
{
  return D3DCOLOR_RGBA(r,g,b,a);
}

enum
{  // texture id's
  //TexTex1,
  TexTotal // number of textures
}

const char[][] texnames =
[
  //"Tex1.bmp",
];

const IDirect3DTexture8[] textures =
[
  //null,
];

void d3dErrorMsg(char[] s,HRESULT code)
{
  printf("%s Direct3D error %d", s, code);
  assert(false);
}

bit CheckError(HRESULT code)
{
  if (code!=S_OK)
  {
    d3dErrorMsg("Direct3D Error:",code);
    return true;
  }
  return false;
}

bit Win32ErrorMsg(char* s)
{
  DWORD err=GetLastError();
  char buf[512];
  sprintf(buf,"%s error code 0x%08x",s,err);
  MessageBox(GetFocus(), buf, "TestD3D", MB_OK); 
  assert(false);
  return false;
}

bit DirectXErrorMsg(HRESULT hr,char* s)
{
  char buf[512];
  sprintf(buf,"%s HRESULT 0x%08x (%d)",s,hr,hr&0xFFFF);
  MessageBox(GetFocus(), buf, "TestD3D", MB_OK); 
  assert(false);
  return false;
}

bit DXErr(HRESULT hr, char[] s)
{
  if (FAILED(hr))
  {
    DirectXErrorMsg(hr,s);
    return true;
  }
  return false;
}

bit DXErr(HRESULT hr)
{
  if (FAILED(hr))
  {
    DirectXErrorMsg(hr,"");
    return true;
  }
  return false;
}

// load a D3D texture from a bmp file
bit InitTexture(int slot)
{
  IDirect3DTexture8 texsurf;
  if (texnames[slot])
  {
    // load texture from file
    //Msg(Fmt("InitTexture %d %s\n",slot,texnames[slot]));
    //DXErr(D3DXCreateTextureFromFile(dev,texnames[slot],&texsurf),"D3DXCreateTextureFromFile:");
  }
  else
  {
    // create render target texture same size and format as the front buffer
    DXErr(dev.CreateTexture(w,h,1,D3DUSAGE_RENDERTARGET,D3DFORMAT.D3DFMT_A8R8G8B8,D3DPOOL.D3DPOOL_DEFAULT,&texsurf),"CreateTexture");
    // clear it?
  }
  if (!texsurf) return false;
  textures[slot] = texsurf;
  return true;
}

void DoneTexture(int slot)
{
  // Release the material that was created earlier.
  if (textures[slot])
  {
    textures[slot].Release();
    textures[slot] = null;
  }
}


void UseMaterial(int slot, float r,float g,float b, float a, bit additive)
{
  D3DMATERIAL8 mtrl;
  mtrl.Diffuse.r = mtrl.Ambient.r = r;
  mtrl.Diffuse.g = mtrl.Ambient.g = g;
  mtrl.Diffuse.b = mtrl.Ambient.b = b;
  mtrl.Diffuse.a = mtrl.Ambient.a = a;
  mtrl.Specular.r = mtrl.Specular.g = mtrl.Specular.b = 1.0F;
  mtrl.Specular.a = 1.0F;
  mtrl.Power=10.0F;
  mtrl.Emissive.r = mtrl.Emissive.g = mtrl.Emissive.b = 0.0F;
  mtrl.Emissive.a = 0.0F;
  DXErr(dev.SetMaterial(&mtrl),"SetMaterial");

  if (additive)
  {
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ALPHABLENDENABLE, true));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_SRCBLEND, D3DBLEND.D3DBLEND_SRCALPHA));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_DESTBLEND, D3DBLEND.D3DBLEND_ONE));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ZWRITEENABLE, false));
  }
  else if (a<=0.999F)
  {
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ALPHABLENDENABLE, true));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_SRCBLEND, D3DBLEND.D3DBLEND_SRCALPHA));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_DESTBLEND, D3DBLEND.D3DBLEND_INVSRCALPHA));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ZWRITEENABLE, false));
  }
  else
  {
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ALPHABLENDENABLE, false));
    DXErr(dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ZWRITEENABLE, true));
  }

  if (slot<0)
  {
    DXErr(dev.SetTexture(0,null),"SetTexture:");
  }
  else
  {
    //DXErr(dev.SetTexture(0,textures[slot]),"SetTexture:");
    DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_MIPFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_LINEAR));
    DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_MINFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_ANISOTROPIC));
    DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_MAGFILTER, D3DTEXTUREFILTERTYPE.D3DTEXF_ANISOTROPIC));
    version(none)
    {
     // dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_MAXMIPLEVEL,2); // 0
     DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_MIPMAPLODBIAS,*(DWORD*)&lodbias));
    }

    //dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2); // ||D3DTTFF_PROJECTED);
    //dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    //dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_ADDRESSU,D3DTEXTUREADDRESS.D3DTADDRESS_WRAP);
    //dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_ADDRESSV,D3DTEXTUREADDRESS.D3DTADDRESS_WRAP);
    DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_ADDRESSU,D3DTEXTUREADDRESS.D3DTADDRESS_CLAMP));
    DXErr(dev.SetTextureStageState(0,D3DTEXTURESTAGESTATETYPE.D3DTSS_ADDRESSV,D3DTEXTUREADDRESS.D3DTADDRESS_CLAMP));
  }
}

struct D3DSVERTEX
{
  Vc3 pos;
  Color4b diffuse;
  const uint fvf = D3DFVF_XYZ|D3DFVF_DIFFUSE;
};

void DrawTri(float x,float y,float z,float radius,float radians,Color4b color)
{
  D3DSVERTEX verts[3];

  // Data for the geometry of the triangle. 
  verts[0].pos.d[0]=x+sin(radians)*radius;
  verts[0].pos.d[1]=y+cos(radians)*radius;
  verts[0].pos.d[2]=z;
  verts[0].diffuse=color;
  
  verts[1].pos.d[0]=x+sin(radians+PI*2/3)*radius;
  verts[1].pos.d[1]=y+cos(radians+PI*2/3)*radius;
  verts[1].pos.d[2]=z;
  verts[1].diffuse=color;
  
  verts[2].pos.d[0]=x+sin(radians+PI*4/3)*radius;
  verts[2].pos.d[1]=y+cos(radians+PI*4/3)*radius;
  verts[2].pos.d[2]=z;
  verts[2].diffuse=color;

  DXErr(dev.SetVertexShader(D3DSVERTEX.fvf),"SetVertexShader");

  // Draw the triangle using a DrawPrimitive() call.
  DXErr(dev.DrawPrimitiveUP(D3DPRIMITIVETYPE.D3DPT_TRIANGLELIST,
                     1, verts, D3DSVERTEX.size),"DrawPrimitive:");
}

void SetupProjection()
{
  D3DMATRIX mat;
  memset(&mat,0,mat.size);
  mat._11 = 1.0f;
  mat._22 =-1.0f;
  mat._33 = 1.0f;
  mat._44 = 1.0f;
  DXErr(dev.SetTransform(D3DTRANSFORMSTATETYPE.D3DTS_PROJECTION, &mat),
    "SetTransform(PROJ):");
}

void SetupCamera()
{
  D3DMATRIX mat;
  memset(&mat,0,mat.size);
  mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;

  // The world matrix controls the position and orientation of the polygons
  // in world space. Use it to position objects.  All our objects are in world
  // coordinates already so we leave this identity.
  DXErr(dev.SetTransform(D3DTRANSFORMSTATETYPE.D3DTS_WORLD, &mat),
    "SetTransform(WORLD):");

  // The view matrix defines the position and orientation of the camera.
  // Here, we are just moving it back along the z-axis by 2 units.
  // matView._43 = 2.0f;
  DXErr(dev.SetTransform(D3DTRANSFORMSTATETYPE.D3DTS_VIEW, &mat),
    "SetTransform(VIEW):");
}

void Render()
{
  static float m = 0;
  m += PI*2/700;
  UseMaterial(-1,1.0f,1.0f,1.0f,1.0f,false);
  float n = 1.0f;
  for (float r = 0.2; r < 0.9f; r += 0.1f)
  {
	n = -n;
    for (float a = m; a < m+PI*2; a += PI*2/30)
	  DrawTri(sin(a*n)*r, cos(a*n)*r, 0.75f, 0.03f, -a*n, RGBA(255,cast(int)(r*255),cast(int)((1.0f-r)*255),255));
  }
  //DrawTri(0.15F,0.0F,0.75F, 0.75F, RGBA(255,255,255,255));
  //UseMaterial(-1, 1.0F,0.0F,1.0F,1.0f,false);
  //DrawTri(-0.15F,0.0F,1.0F, 0.75F, RGBA(255,0,255,255));
}

bit InitRender()
{
  if (!hwnd || !dev)
    return false;
  // clear color and depth buffers
  DXErr(dev.Clear(0, null, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER/*|D3DCLEAR_STENCIL*/,
       RGBA(0,64,0,255), 1.0F, 0),"Clear:");

  // Begin the scene
  if (DXErr(dev.BeginScene(),"BeginScene:"))
    return false;

  return true;
}

void DoneRender()
{
  if (dev)
  {
    DXErr(dev.EndScene(),"EndScene:");
    DXErr(dev.Present(null,null,null,null),"Present:");
  }
}


bit InitDemo()
{
  printf("InitDemo\n");
  for (int i=0; i<TexTotal; ++i)
  {
    printf("init texture %d\n",i);
    InitTexture(i);
  }
  printf("setting d3d states\n");
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_AMBIENT, RGBA(255,255,255,255));

  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_FILLMODE, D3DFILL_SOLID);
version(none)
{
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_COLORVERTEX, TRUE);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_DIFFUSEMATERIALSOURCE,  D3DMCS_MATERIAL);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_DIFFUSEMATERIALSOURCE,  D3DMCS_COLOR1);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_AMBIENTMATERIALSOURCE,  D3DMCS_COLOR1);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);

  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_LIGHTING, true);
}
else
{
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_LIGHTING, false);
}
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_CULLMODE, D3DCULL.D3DCULL_NONE);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_CULLMODE, D3DCULL.D3DCULL_CW);
  //dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_CULLMODE, D3DCULL.D3DCULL_CCW);
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ZENABLE, D3DZBUFFERTYPE.D3DZB_TRUE);
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ALPHATESTENABLE, false);
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_ALPHABLENDENABLE, false);
  dev.SetRenderState(D3DRENDERSTATETYPE.D3DRS_FOGCOLOR,RGBA(0,0,0,255));

  /*
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_COLOROP, D3DTEXTUREOP.D3DTOP_MODULATE);
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_COLORARG1, D3DTA_TEXTURE);
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_ALPHAOP, D3DTEXTUREOP.D3DTOP_MODULATE);
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  dev.SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE.D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  dev.SetTextureStageState(1, D3DTEXTURESTAGESTATETYPE.D3DTSS_COLOROP, D3DTEXTUREOP.D3DTOP_DISABLE);
  dev.SetTextureStageState(1, D3DTEXTURESTAGESTATETYPE.D3DTSS_ALPHAOP, D3DTEXTUREOP.D3DTOP_DISABLE);
  */
  return true;
}

void DoneDemo()
{
  printf("DoneDemo\n");
  for (int i=TexTotal; --i>=0; )
    DoneTexture(i);
}

void Resized(int width,int height)
{
  // if (!dev) return;
  if (width<0 || height<0)
  {
    if (hwnd)
    {
      RECT r;
      GetClientRect(hwnd,&r);
      width = r.right;
      height= r.bottom;
    }
  }
  // set viewport to cover the window
  w=width; h=height;
  //dev->SetViewport(0,0,w,h);
}

bit InitD3D()
{
  printf("Initializing driver\n");
  dev = null;
  assert(D3DMATRIX.size == 64);
  assert(D3DVECTOR.size == 12);

  // This call typically takes around 3.0 seconds to complete.
  d3d = Direct3DCreate8(D3D_SDK_VERSION);
  assert(d3d);

  // Get the current desktop display mode, so we can set up a back
  // buffer of the same format
  D3DDISPLAYMODE d3ddm;
  DXErr(d3d.GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm),"GetAdapterDisplayMode");

  D3DPRESENT_PARAMETERS pp;
  debug
  {
    pp.Windowed = true;
    pp.BackBufferCount = 1;
    //pp.SwapEffect = D3DSWAPEFFECT.D3DSWAPEFFECT_DISCARD;
    pp.SwapEffect = D3DSWAPEFFECT.D3DSWAPEFFECT_COPY_VSYNC;
    pp.EnableAutoDepthStencil = true;
    pp.BackBufferFormat = d3ddm.Format;
    pp.AutoDepthStencilFormat = (d3ddm.Format == D3DFORMAT.D3DFMT_X8R8G8B8) 
									? D3DFORMAT.D3DFMT_D24X8 //D3DFORMAT.D3DFMT_D24S8 
									: D3DFORMAT.D3DFMT_D16;
  }
  else
  {
    pp.Windowed = false;
    pp.BackBufferWidth = w;
    pp.BackBufferHeight = h;
    pp.BackBufferFormat = D3DFORMAT.D3DFMT_R5G6B5;
    pp.BackBufferCount = 1;
    pp.SwapEffect = D3DSWAPEFFECT.D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT.D3DSWAPEFFECT_DISCARD;
    pp.EnableAutoDepthStencil = true;
    pp.AutoDepthStencilFormat = D3DFORMAT.D3DFMT_D16;
    pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;// D3DPRESENT_RATE_UNLIMITED;// 85;
    pp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  }

  // I really should use ID3DDevice8::EnumAdapterModes to determine valid values

  UINT adapter = D3DADAPTER_DEFAULT;
  D3DDEVTYPE devtype = D3DDEVTYPE.D3DDEVTYPE_HAL;

  DXErr(d3d.CheckDeviceType(adapter,devtype,pp.BackBufferFormat,pp.BackBufferFormat,pp.Windowed),
        "CheckDeviceType");
  DXErr(d3d.CheckDeviceFormat(adapter,devtype,pp.BackBufferFormat,D3DUSAGE_RENDERTARGET,
        D3DRESOURCETYPE.D3DRTYPE_SURFACE,pp.BackBufferFormat),"CheckDeviceFormat");

  // Create the D3DDevice. Here we are using the default adapter (most
  // systems only have one).
  // This call typically takes around 1.0 seconds to complete.
  if (DXErr(d3d.CreateDevice(adapter, devtype, GetFocus(),
                              D3DCREATE_HARDWARE_VERTEXPROCESSING,
                              //D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                              &pp, &dev),"CreateDevice"))
    return false;
  assert(dev);

  D3DDEVICE_CREATION_PARAMETERS dcp;
  DXErr(dev.GetCreationParameters(&dcp),"GetCreationParameters");

  debug
  {
    D3DADAPTER_IDENTIFIER8 dddi;
    if (!DXErr(d3d.GetAdapterIdentifier(dcp.AdapterOrdinal/*D3DADAPTER_DEFAULT*/,D3DENUM_NO_WHQL_LEVEL,&dddi),"GetAdapterIdentifier:"))
    {
      printf("Using device: %c%c%c%c%c\n",dddi.Description[0],dddi.Description[1],dddi.Description[2],dddi.Description[3],dddi.Description[4]);
    }
  }

  DXErr(dev.TestCooperativeLevel(),"preliminary TestCooperativeLevel");

  SetupProjection();
  SetupCamera();
  // Resized(-1,-1);

  printf("Initialized driver\n");
  return true;
}

void DoneD3D()
{
  printf("Deinitializing driver...\n");

  // Release the D3D objects used by the app

  // Do a safe check for releasing the D3DDEVICE. RefCount should be zero.
  if (dev)
    if (0 < dev.Release())
      return;
  dev=null;

  // Do a safe check for releasing DDRAW. RefCount should be zero.
  if (d3d)
    if (0 < d3d.Release())
      return;
  d3d=null;

  printf("Deinitialized driver\n");
}

bit ProcessEvents()
{
  MSG msg;
  while (PeekMessage(&msg, cast(HWND) null, 0, 0, PM_NOREMOVE))
  {
    if (!GetMessage(&msg, cast(HWND) null, 0, 0))
      return false;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return true;
}

extern(Windows)
int WindowProc(HWND mhwnd, uint msg, int wp, int lp)
{
  if (mhwnd==hwnd || msg==WM_CREATE)
  switch (msg)
  {
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0; // don't allow DefWndProc to post WM_DESTROY
    case WM_KEYUP:
      if (VK_ESCAPE == wp)
      {
        PostMessage(mhwnd,WM_CLOSE,0,0);
        return 0;
      }
      break;
    case WM_SYSCOMMAND:
    {
      // disable screen saver
      if ((wp & 0xFFF0) == SC_SCREENSAVE ||    // disable screensaver
          (wp & 0xFFF0) == SC_MONITORPOWER ||  // disable monitor suspend
          (wp & 0xFFF0) == SC_KEYMENU)         // disable ALT key menu
        return 0;
      break;
    }
    case WM_SYSCHAR:
      if (wp==VK_F10) // disable app menu
        return 0;
      break;
    case WM_PAINT:
      ValidateRect(hwnd,null);
      return 0;  // we'll paint ourselves elsewhere
    case WM_SIZE:
      if (wp!=SIZE_MINIMIZED)
        Resized(LOWORD(lp),HIWORD(lp));
      break;
    default:
      break;
  }
  return DefWindowProc(mhwnd, msg, wp, lp);
}

bit InitWindow()
{
  printf("Creating window\n");
  const char* classname="TestD3DClass";
  const char* windowtitle="TestD3D";
  DWORD style= WS_VISIBLE |
      WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
      WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
      WS_POPUPWINDOW | WS_THICKFRAME | WS_CAPTION;
  WNDCLASSA wc;
  wc.style =
    CS_OWNDC |
    CS_DBLCLKS |
    CS_BYTEALIGNCLIENT |
    CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = (WNDPROC)&WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hinst;
  wc.hIcon = LoadIcon(null, IDI_APPLICATION);
  wc.hCursor = LoadCursor(null, IDC_CROSS);
  wc.hbrBackground = null;
  // Stock brushes cause some spurious GDI32 exceptions
  // wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName = null;
  wc.lpszClassName = classname;
  if (!RegisterClass(&wc))
  {
    assert(false);
    return false;
  }
  RECT r;
  r.left=r.top=0;
  r.right=w; r.bottom=h;
  AdjustWindowRect(&r,style,false); // assume no menu bar
  hwnd = CreateWindowEx(0,classname,windowtitle,style,
      0,0,r.right-r.left,r.bottom-r.top,
      null,null,hinst,null);
  if (!hwnd)
  {
    assert(false);
    return false;
  }
  return true;
}

void DoneWindow()
{
  if (hwnd)
  {
    printf("Destroying window\n");
    DestroyWindow(hwnd);
    hwnd=null;
  }
}

// adding "-L/exet:nt/su:windows" to DMD command line gets rid of the console window
int main(char[][] args)
{
  hinst=GetModuleHandle(null);
  w=640; h=480;
  if (InitWindow())
  {
    if (InitD3D())
    {
      if (InitDemo())
      {
        printf("running\n");
        int starttime,endtime;
        int nframes;
        time.time(&starttime);
        do
        {
          ++nframes;
          InitRender();
          Render();
          DoneRender();
        } while (ProcessEvents());
        time.time(&endtime);
        printf("did %d frames in %d seconds, that's %f FPS\n",nframes,endtime-starttime,cast(double)nframes/(endtime-starttime));
        printf("closing\n");
        DoneDemo();
      }
      DoneD3D();
    }
    DoneWindow();
  }
  return 0;
}

