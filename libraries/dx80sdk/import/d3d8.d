/*==========================================================================;
 *
 *
 *  File:   d3d8.d
 *  Content:    Direct3D include file
 *
 ****************************************************************************/

enum { DIRECT3D_VERSION        = 0x0800 }

// include this file content only if compiling for DX8 interfaces


/* This identifier is passed to Direct3DCreate8 in order to ensure that an
 * application was built against the correct header files. This number is
 * incremented whenever a header (or other) change would require applications
 * to be rebuilt. If the version doesn't match, Direct3DCreate8 will fail.
 * (The number itself has no meaning.)*/

enum { D3D_SDK_VERSION = 120 }

private import std.c.stdlib;
private import std.c.windows.windows;
private import std.c.windows.com;

//import c.stdlib;
// import com;

//#define COM_NO_WINDOWS_H
//#include <objbase.h>

// import win32api;

HRESULT MAKE_HRESULT(int sev, int fac, int code)
{
	return cast(HRESULT)((sev << 31) | (fac << 16) | code);
}

typedef void* HMONITOR;

//#define D3DAPI WINAPI

/*
 * Interface IID's  (link with dxguid.lib)
 */
//#if defined( _WIN32 ) && !defined( _NO_COM)

/* IID_IDirect3D8 */
/* {1DD9E8DA-1C77-4d40-B0CF-98FEFDFF9512} */
extern (Windows) {
//   extern IID IID_IDirect3D8;
  const GUID IID_IDirect3D8 = { 0x1dd9e8da, 0x1c77, 0x4d40, [0xb0, 0xcf, 0x98, 0xfe, 0xfd, 0xff, 0x95, 0x12]};

/* IID_IDirect3DDevice8 */
/* {7385E5DF-8FE8-41D5-86B6-D7B48547B6CF} */
//  extern IID IID_IDirect3DDevice8;
const GUID IID_IDirect3DDevice8 = {0x7385e5df, 0x8fe8, 0x41d5, [0x86, 0xb6, 0xd7, 0xb4, 0x85, 0x47, 0xb6, 0xcf]};

/* IID_IDirect3DResource8 */
/* {1B36BB7B-09B7-410a-B445-7D1430D7B33F} */
//  extern IID IID_IDirect3DResource8;
const GUID IID_IDirect3DResource8 = {0x1b36bb7b, 0x9b7, 0x410a, [0xb4, 0x45, 0x7d, 0x14, 0x30, 0xd7, 0xb3, 0x3f]};

/* IID_IDirect3DBaseTexture8 */
/* {B4211CFA-51B9-4a9f-AB78-DB99B2BB678E} */
//  extern IID IID_IDirect3DBaseTexture8;
const GUID IID_IDirect3DBaseTexture8 = {0xb4211cfa, 0x51b9, 0x4a9f, [0xab, 0x78, 0xdb, 0x99, 0xb2, 0xbb, 0x67, 0x8e]};

/* IID_IDirect3DTexture8 */
/* {E4CDD575-2866-4f01-B12E-7EECE1EC9358} */
//  extern IID IID_IDirect3DTexture8;
const GUID IID_IDirect3DTexture8 = {0xe4cdd575, 0x2866, 0x4f01, [0xb1, 0x2e, 0x7e, 0xec, 0xe1, 0xec, 0x93, 0x58]};

/* IID_IDirect3DCubeTexture8 */
/* {3EE5B968-2ACA-4c34-8BB5-7E0C3D19B750} */
//  extern IID IID_IDirect3DCubeTexture8;
const GUID IID_IDirect3DCubeTexture8 = {0x3ee5b968, 0x2aca, 0x4c34, [0x8b, 0xb5, 0x7e, 0x0c, 0x3d, 0x19, 0xb7, 0x50]};

/* IID_IDirect3DVolumeTexture8 */
/* {4B8AAAFA-140F-42ba-9131-597EAFAA2EAD} */
//  extern IID IID_IDirect3DVolumeTexture8;
const GUID IID_IDirect3DVolumeTexture8 = {0x4b8aaafa, 0x140f, 0x42ba, [0x91, 0x31, 0x59, 0x7e, 0xaf, 0xaa, 0x2e, 0xad]};

/* IID_IDirect3DVertexBuffer8 */
/* {8AEEEAC7-05F9-44d4-B591-000B0DF1CB95} */
//  extern IID IID_IDirect3DVertexBuffer8;
const GUID IID_IDirect3DVertexBuffer8 = {0x8aeeeac7, 0x05f9, 0x44d4, [0xb5, 0x91, 0x00, 0x0b, 0x0d, 0xf1, 0xcb, 0x95]};

/* IID_IDirect3DIndexBuffer8 */
/* {0E689C9A-053D-44a0-9D92-DB0E3D750F86} */
//  extern IID IID_IDirect3DIndexBuffer8;
const GUID IID_IDirect3DIndexBuffer8 = {0x0e689c9a, 0x053d, 0x44a0, [0x9d, 0x92, 0xdb, 0x0e, 0x3d, 0x75, 0x0f, 0x86]};

/* IID_IDirect3DSurface8 */
/* {B96EEBCA-B326-4ea5-882F-2FF5BAE021DD} */
//  extern IID IID_IDirect3DSurface8;
const GUID IID_IDirect3DSurface8 = {0xb96eebca, 0xb326, 0x4ea5, [0x88, 0x2f, 0x2f, 0xf5, 0xba, 0xe0, 0x21, 0xdd]};

/* IID_IDirect3DVolume8 */
/* {BD7349F5-14F1-42e4-9C79-972380DB40C0} */
//  extern IID IID_IDirect3DVolume8;
const GUID IID_IDirect3DVolume8 = {0xbd7349f5, 0x14f1, 0x42e4, [0x9c, 0x79, 0x97, 0x23, 0x80, 0xdb, 0x40, 0xc0]};

/* IID_IDirect3DSwapChain8 */
/* {928C088B-76B9-4C6B-A536-A590853876CD} */
//  extern IID IID_IDirect3DSwapChain8;
const GUID IID_IDirect3DSwapChain8 = {0x928c088b, 0x76b9, 0x4c6b, [0xa5, 0x36, 0xa5, 0x90, 0x85, 0x38, 0x76, 0xcd]};
};

//#endif

//interface IDirect3D8;
//interface IDirect3DDevice8;

//interface IDirect3DResource8;
//interface IDirect3DBaseTexture8;
//interface IDirect3DTexture8;
//interface IDirect3DVolumeTexture8;
//interface IDirect3DCubeTexture8;

//interface IDirect3DVertexBuffer8;
//interface IDirect3DIndexBuffer8;

//interface IDirect3DSurface8;
//interface IDirect3DVolume8;

//interface IDirect3DSwapChain8;


import d3d8types;
import d3d8caps;


extern (Windows) {

/*
 * DLL Function for creating a Direct3D8 object. This object supports
 * enumeration and allows the creation of Direct3DDevice8 objects.
 * Pass the value of the constant D3D_SDK_VERSION to this function, so
 * that the run-time can validate that your application was compiled
 * against the right headers.
 */

IDirect3D8 Direct3DCreate8(UINT SDKVersion);


/*
 * Direct3D interfaces
 */






interface IDirect3D8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3D8 methods ***/
    HRESULT RegisterSoftwareDevice(void* pInitializeFunction);
    UINT GetAdapterCount();
    HRESULT GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier);
    UINT GetAdapterModeCount(UINT Adapter);
    HRESULT EnumAdapterModes(UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode);
    HRESULT GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode);
    HRESULT CheckDeviceType(UINT Adapter,D3DDEVTYPE CheckType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
    HRESULT CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
    HRESULT CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType);
    HRESULT CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
    HRESULT GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps);
    HMONITOR GetAdapterMonitor(UINT Adapter);
    HRESULT CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice8* ppReturnedDeviceInterface);
};

alias IDirect3D8 LPDIRECT3D8, PDIRECT3D8;

struct RGNDATA;
struct PALETTEENTRY;

interface IDirect3DDevice8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DDevice8 methods ***/
    HRESULT TestCooperativeLevel();
    UINT GetAvailableTextureMem();
    HRESULT ResourceManagerDiscardBytes(DWORD Bytes);
    HRESULT GetDirect3D(IDirect3D8* ppD3D8);
    HRESULT GetDeviceCaps(D3DCAPS8* pCaps);
    HRESULT GetDisplayMode(D3DDISPLAYMODE* pMode);
    HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
    HRESULT SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface8 pCursorBitmap);
    HRESULT SetCursorPosition(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags);
    BOOL ShowCursor(BOOL bShow);
    HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain8* pSwapChain);
    HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    HRESULT Present(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
    HRESULT GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8* ppBackBuffer);
    HRESULT GetRasterStatus(D3DRASTER_STATUS* pRasterStatus);
    HRESULT SetGammaRamp(DWORD Flags,D3DGAMMARAMP* pRamp);
    HRESULT GetGammaRamp(D3DGAMMARAMP* pRamp);
    HRESULT CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture8* ppTexture);
    HRESULT CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture8* ppVolumeTexture);
    HRESULT CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture8* ppCubeTexture);
    HRESULT CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer8* ppVertexBuffer);
    HRESULT CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer8* ppIndexBuffer);
    HRESULT CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,BOOL Lockable,IDirect3DSurface8* ppSurface);
    HRESULT CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,IDirect3DSurface8* ppSurface);
    HRESULT CreateImageSurface(UINT Width,UINT Height,D3DFORMAT Format,IDirect3DSurface8* ppSurface);
    HRESULT CopyRects(IDirect3DSurface8 pSourceSurface,RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8 pDestinationSurface,POINT* pDestPointsArray);
    HRESULT UpdateTexture(IDirect3DBaseTexture8 pSourceTexture,IDirect3DBaseTexture8 pDestinationTexture);
    HRESULT GetFrontBuffer(IDirect3DSurface8 pDestSurface);
    HRESULT SetRenderTarget(IDirect3DSurface8 pRenderTarget,IDirect3DSurface8 pNewZStencil);
    HRESULT GetRenderTarget(IDirect3DSurface8* ppRenderTarget);
    HRESULT GetDepthStencilSurface(IDirect3DSurface8* ppZStencilSurface);
    HRESULT BeginScene();
    HRESULT EndScene();
    HRESULT Clear(DWORD Count,D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
    HRESULT SetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    HRESULT GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE,D3DMATRIX*);
    HRESULT SetViewport(D3DVIEWPORT8* pViewport);
    HRESULT GetViewport(D3DVIEWPORT8* pViewport);
    HRESULT SetMaterial(D3DMATERIAL8* pMaterial);
    HRESULT GetMaterial(D3DMATERIAL8* pMaterial);
    HRESULT SetLight(DWORD Index,D3DLIGHT8*);
    HRESULT GetLight(DWORD Index,D3DLIGHT8*);
    HRESULT LightEnable(DWORD Index,BOOL Enable);
    HRESULT GetLightEnable(DWORD Index,BOOL* pEnable);
    HRESULT SetClipPlane(DWORD Index,float* pPlane);
    HRESULT GetClipPlane(DWORD Index,float* pPlane);
    HRESULT SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
    HRESULT GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue);
    HRESULT BeginStateBlock();
    HRESULT EndStateBlock(DWORD* pToken);
    HRESULT ApplyStateBlock(DWORD Token);
    HRESULT CaptureStateBlock(DWORD Token);
    HRESULT DeleteStateBlock(DWORD Token);
    HRESULT CreateStateBlock(D3DSTATEBLOCKTYPE Type,DWORD* pToken);
    HRESULT SetClipStatus(D3DCLIPSTATUS8* pClipStatus);
    HRESULT GetClipStatus(D3DCLIPSTATUS8* pClipStatus);
    HRESULT GetTexture(DWORD Stage,IDirect3DBaseTexture8* ppTexture);
    HRESULT SetTexture(DWORD Stage,IDirect3DBaseTexture8 pTexture);
    HRESULT GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
    HRESULT SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
    HRESULT ValidateDevice(DWORD* pNumPasses);
    HRESULT GetInfo(DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize);
    HRESULT SetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
    HRESULT GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
    HRESULT SetCurrentTexturePalette(UINT PaletteNumber);
    HRESULT GetCurrentTexturePalette(UINT *PaletteNumber);
    HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
    HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount);
    HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,D3DFORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer8 pDestBuffer,DWORD Flags);
    HRESULT CreateVertexShader(DWORD* pDeclaration,DWORD* pFunction,DWORD* pHandle,DWORD Usage);
    HRESULT SetVertexShader(DWORD Handle);
    HRESULT GetVertexShader(DWORD* pHandle);
    HRESULT DeleteVertexShader(DWORD Handle);
    HRESULT SetVertexShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT GetVertexShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT GetVertexShaderDeclaration(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT GetVertexShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer8 pStreamData,UINT Stride);
    HRESULT GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer8* ppStreamData,UINT* pStride);
    HRESULT SetIndices(IDirect3DIndexBuffer8 pIndexData,UINT BaseVertexIndex);
    HRESULT GetIndices(IDirect3DIndexBuffer8* ppIndexData,UINT* pBaseVertexIndex);
    HRESULT CreatePixelShader(DWORD* pFunction,DWORD* pHandle);
    HRESULT SetPixelShader(DWORD Handle);
    HRESULT GetPixelShader(DWORD* pHandle);
    HRESULT DeletePixelShader(DWORD Handle);
    HRESULT SetPixelShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT GetPixelShaderConstant(DWORD Register,void* pConstantData,DWORD ConstantCount);
    HRESULT GetPixelShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData);
    HRESULT DrawRectPatch(UINT Handle,float* pNumSegs,D3DRECTPATCH_INFO* pRectPatchInfo);
    HRESULT DrawTriPatch(UINT Handle,float* pNumSegs,D3DTRIPATCH_INFO* pTriPatchInfo);
    HRESULT DeletePatch(UINT Handle);
};

alias IDirect3DDevice8 LPDIRECT3DDEVICE8, PDIRECT3DDEVICE8;



interface IDirect3DSwapChain8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DSwapChain8 methods ***/
    HRESULT Present(RECT* pSourceRect,RECT* pDestRect,HWND hDestWindowOverride,RGNDATA* pDirtyRegion);
    HRESULT GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8* ppBackBuffer);
};

alias IDirect3DSwapChain8 LPDIRECT3DSWAPCHAIN8, PDIRECT3DSWAPCHAIN8;


interface IDirect3DResource8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DResource8 methods ***/
    HRESULT GetDevice(IDirect3DDevice8* ppDevice);
    HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
    HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
    HRESULT FreePrivateData(GUID refguid);
    DWORD SetPriority(DWORD PriorityNew);
    DWORD GetPriority();
    HRESULT PreLoad();
    D3DRESOURCETYPE GetType();
};

alias IDirect3DResource8 LPDIRECT3DRESOURCE8, PDIRECT3DRESOURCE8;


interface IDirect3DBaseTexture8 : public IDirect3DResource8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DResource8 methods ***/
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
    DWORD SetLOD(DWORD LODNew);
    DWORD GetLOD();
    DWORD GetLevelCount();
};

alias IDirect3DBaseTexture8 LPDIRECT3DBASETEXTURE8, PDIRECT3DBASETEXTURE8;




interface IDirect3DTexture8 : public IDirect3DBaseTexture8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DBaseTexture8 methods ***/
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
//     DWORD SetLOD(DWORD LODNew);
//     DWORD GetLOD();
//     DWORD GetLevelCount();
    HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
    HRESULT GetSurfaceLevel(UINT Level,IDirect3DSurface8* ppSurfaceLevel);
    HRESULT LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,RECT* pRect,DWORD Flags);
    HRESULT UnlockRect(UINT Level);
    HRESULT AddDirtyRect(RECT* pDirtyRect);
};

alias IDirect3DTexture8 LPDIRECT3DTEXTURE8, PDIRECT3DTEXTURE8;




interface IDirect3DVolumeTexture8 : public IDirect3DBaseTexture8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DBaseTexture8 methods ***/
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
//     DWORD SetLOD(DWORD LODNew);
//     DWORD GetLOD();
//     DWORD GetLevelCount();
    HRESULT GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc);
    HRESULT GetVolumeLevel(UINT Level,IDirect3DVolume8* ppVolumeLevel);
    HRESULT LockBox(UINT Level,D3DLOCKED_BOX* pLockedVolume,D3DBOX* pBox,DWORD Flags);
    HRESULT UnlockBox(UINT Level);
    HRESULT AddDirtyBox(D3DBOX* pDirtyBox);
};

alias IDirect3DVolumeTexture8 LPDIRECT3DVOLUMETEXTURE8, PDIRECT3DVOLUMETEXTURE8;





interface IDirect3DCubeTexture8 : public IDirect3DBaseTexture8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DBaseTexture8 methods ***/
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
//     DWORD SetLOD(DWORD LODNew);
//     DWORD GetLOD();
//     DWORD GetLevelCount();
    HRESULT GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
    HRESULT GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface8* ppCubeMapSurface);
    HRESULT LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,RECT* pRect,DWORD Flags);
    HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level);
    HRESULT AddDirtyRect(D3DCUBEMAP_FACES FaceType,RECT* pDirtyRect);
};

alias IDirect3DCubeTexture8 LPDIRECT3DCUBETEXTURE8, PDIRECT3DCUBETEXTURE8;




interface IDirect3DVertexBuffer8 : public IDirect3DResource8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DResource8 methods ***/
  // Crap. These don't need to be redefined..
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
    HRESULT Lock(UINT OffsetToLock,UINT SizeToLock,BYTE** ppbData,DWORD Flags);
    HRESULT Unlock();
    HRESULT GetDesc(D3DVERTEXBUFFER_DESC *pDesc);
};

alias IDirect3DVertexBuffer8 LPDIRECT3DVERTEXBUFFER8, PDIRECT3DVERTEXBUFFER8;


interface IDirect3DIndexBuffer8 : public IDirect3DResource8
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DResource8 methods ***/
//     HRESULT GetDevice(IDirect3DDevice8* ppDevice);
//     HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
//     HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
//     HRESULT FreePrivateData(GUID refguid);
//     DWORD SetPriority(DWORD PriorityNew);
//     DWORD GetPriority();
//     HRESULT PreLoad();
//     D3DRESOURCETYPE GetType();
    HRESULT Lock(UINT OffsetToLock,UINT SizeToLock,BYTE** ppbData,DWORD Flags);
    HRESULT Unlock();
    HRESULT GetDesc(D3DINDEXBUFFER_DESC *pDesc);
};

alias IDirect3DIndexBuffer8 LPDIRECT3DINDEXBUFFER8, PDIRECT3DINDEXBUFFER8;



interface IDirect3DSurface8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DSurface8 methods ***/
    HRESULT GetDevice(IDirect3DDevice8* ppDevice);
    HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
    HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
    HRESULT FreePrivateData(GUID refguid);
    HRESULT GetContainer(IID riid,void** ppContainer);
    HRESULT GetDesc(D3DSURFACE_DESC *pDesc);
    HRESULT LockRect(D3DLOCKED_RECT* pLockedRect,RECT* pRect,DWORD Flags);
    HRESULT UnlockRect();
};

alias IDirect3DSurface8 LPDIRECT3DSURFACE8, PDIRECT3DSURFACE8;


interface IDirect3DVolume8 : public IUnknown
{
    /*** IUnknown methods ***/
    //HRESULT QueryInterface(IID* riid, void** pvObject);
    //ULONG AddRef();
    //ULONG Release();

    /*** IDirect3DVolume8 methods ***/
    HRESULT GetDevice(IDirect3DDevice8* ppDevice);
    HRESULT SetPrivateData(GUID refguid,void* pData,DWORD SizeOfData,DWORD Flags);
    HRESULT GetPrivateData(GUID refguid,void* pData,DWORD* pSizeOfData);
    HRESULT FreePrivateData(GUID refguid);
    HRESULT GetContainer(IID riid,void** ppContainer);
    HRESULT GetDesc(D3DVOLUME_DESC *pDesc);
    HRESULT LockBox(D3DLOCKED_BOX * pLockedVolume,D3DBOX* pBox,DWORD Flags);
    HRESULT UnlockBox();
};

alias IDirect3DVolume8 LPDIRECT3DVOLUME8, PDIRECT3DVOLUME8;


/****************************************************************************
 * Flags for SetPrivateData method on all D3D8 interfaces
 *
 * The passed pointer is an IUnknown ptr. The SizeOfData argument to SetPrivateData
 * must be set to sizeof(IUnknown*). Direct3D will call AddRef through this
 * pointer and Release when the private data is destroyed. The data will be
 * destroyed when another SetPrivateData with the same GUID is set, when
 * FreePrivateData is called, or when the D3D8 object is freed.
 ****************************************************************************/
const int D3DSPD_IUNKNOWN                        = 0x00000001L;

/****************************************************************************
 *
 * Parameter for IDirect3D8 Enum and GetCaps8 functions to get the info for
 * the current mode only.
 *
 ****************************************************************************/

const int D3DCURRENT_DISPLAY_MODE                = 0x00EFFFFFL;

/****************************************************************************
 *
 * Flags for IDirect3D8::CreateDevice's BehaviorFlags
 *
 ****************************************************************************/

const int D3DCREATE_FPU_PRESERVE                 = 0x00000002L;
const int D3DCREATE_MULTITHREADED                = 0x00000004L;

const int D3DCREATE_PUREDEVICE                   = 0x00000010L;
const int D3DCREATE_SOFTWARE_VERTEXPROCESSING    = 0x00000020L;
const int D3DCREATE_HARDWARE_VERTEXPROCESSING    = 0x00000040L;
const int D3DCREATE_MIXED_VERTEXPROCESSING       = 0x00000080L;


/****************************************************************************
 *
 * Parameter for IDirect3D8::CreateDevice's iAdapter
 *
 ****************************************************************************/

const int D3DADAPTER_DEFAULT                    = 0;

/****************************************************************************
 *
 * Flags for IDirect3D8::EnumAdapters
 *
 ****************************************************************************/

const int D3DENUM_NO_WHQL_LEVEL                  = 0x00000002L;

/****************************************************************************
 *
 * Maximum number of back-buffers supported in DX8
 *
 ****************************************************************************/

const int D3DPRESENT_BACK_BUFFERS_MAX            = 3L;

/****************************************************************************
 *
 * Flags for IDirect3DDevice8::SetGammaRamp
 *
 ****************************************************************************/

const int D3DSGR_NO_CALIBRATION                 = 0x00000000L;
const int D3DSGR_CALIBRATE                      = 0x00000001L;

/****************************************************************************
 *
 * Flags for IDirect3DDevice8::SetCursorPosition
 *
 ****************************************************************************/

const int D3DCURSOR_IMMEDIATE_UPDATE            = 0x00000001L;

/****************************************************************************
 *
 * Flags for DrawPrimitive/DrawIndexedPrimitive
 *   Also valid for Begin/BeginIndexed
 *   Also valid for VertexBuffer::CreateVertexBuffer
 ****************************************************************************/


/*
 *  DirectDraw error codes
 */
const int _FACD3D = 0x876;
int MAKE_D3DHRESULT(int code) { return MAKE_HRESULT( 1, _FACD3D, code ); }

/*
 * Direct3D Errors
 */
const int D3D_OK                             = S_OK;

const int D3DERR_WRONGTEXTUREFORMAT              = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2072);
const int D3DERR_UNSUPPORTEDCOLOROPERATION       = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2073);
const int D3DERR_UNSUPPORTEDCOLORARG             = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2074);
const int D3DERR_UNSUPPORTEDALPHAOPERATION       = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2075);
const int D3DERR_UNSUPPORTEDALPHAARG             = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2076);
const int D3DERR_TOOMANYOPERATIONS               = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2077);
const int D3DERR_CONFLICTINGTEXTUREFILTER        = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2078);
const int D3DERR_UNSUPPORTEDFACTORVALUE          = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2079);
const int D3DERR_CONFLICTINGRENDERSTATE          = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2081);
const int D3DERR_UNSUPPORTEDTEXTUREFILTER        = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2082);
const int D3DERR_CONFLICTINGTEXTUREPALETTE       = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2086);
const int D3DERR_DRIVERINTERNALERROR             = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2087);

const int D3DERR_NOTFOUND                        = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2150);
const int D3DERR_MOREDATA                        = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2151);
const int D3DERR_DEVICELOST                      = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2152);
const int D3DERR_DEVICENOTRESET                  = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2153);
const int D3DERR_NOTAVAILABLE                    = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2154);
const int D3DERR_OUTOFVIDEOMEMORY                = cast(HRESULT)((1 << 31) | (0x876 << 16) |  380);
const int D3DERR_INVALIDDEVICE                   = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2155);
const int D3DERR_INVALIDCALL                     = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2156);
const int D3DERR_DRIVERINVALIDCALL               = cast(HRESULT)((1 << 31) | (0x876 << 16) | 2157);

};


