/*==========================================================================;
 *
 *  Copyright (C) 1995-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       d3d8caps.h
 *  Content:    Direct3D capabilities include file
 *
 ***************************************************************************/

//enum { DIRECT3D_VERSION        = 0x0800 }

private import std.c.windows.windows;
// import win32api;
private import d3d8types;

// include this file content only if compiling for DX8 interfaces

align(4)://#pragma pack(4)

struct D3DCAPS8
{
    /* Device Info */
    D3DDEVTYPE  DeviceType;
    UINT    AdapterOrdinal;

    /* Caps from DX7 Draw */
    DWORD   Caps;
    DWORD   Caps2;
    DWORD   Caps3;
    DWORD   PresentationIntervals;

    /* Cursor Caps */
    DWORD   CursorCaps;

    /* 3D Device Caps */
    DWORD   DevCaps;

    DWORD   PrimitiveMiscCaps;
    DWORD   RasterCaps;
    DWORD   ZCmpCaps;
    DWORD   SrcBlendCaps;
    DWORD   DestBlendCaps;
    DWORD   AlphaCmpCaps;
    DWORD   ShadeCaps;
    DWORD   TextureCaps;
    DWORD   TextureFilterCaps;          // D3DPTFILTERCAPS for IDirect3DTexture8's
    DWORD   CubeTextureFilterCaps;      // D3DPTFILTERCAPS for IDirect3DCubeTexture8's
    DWORD   VolumeTextureFilterCaps;    // D3DPTFILTERCAPS for IDirect3DVolumeTexture8's
    DWORD   TextureAddressCaps;         // D3DPTADDRESSCAPS for IDirect3DTexture8's
    DWORD   VolumeTextureAddressCaps;   // D3DPTADDRESSCAPS for IDirect3DVolumeTexture8's

    DWORD   LineCaps;                   // D3DLINECAPS

    DWORD   MaxTextureWidth, MaxTextureHeight;
    DWORD   MaxVolumeExtent;

    DWORD   MaxTextureRepeat;
    DWORD   MaxTextureAspectRatio;
    DWORD   MaxAnisotropy;
    float   MaxVertexW;

    float   GuardBandLeft;
    float   GuardBandTop;
    float   GuardBandRight;
    float   GuardBandBottom;

    float   ExtentsAdjust;
    DWORD   StencilCaps;

    DWORD   FVFCaps;
    DWORD   TextureOpCaps;
    DWORD   MaxTextureBlendStages;
    DWORD   MaxSimultaneousTextures;

    DWORD   VertexProcessingCaps;
    DWORD   MaxActiveLights;
    DWORD   MaxUserClipPlanes;
    DWORD   MaxVertexBlendMatrices;
    DWORD   MaxVertexBlendMatrixIndex;

    float   MaxPointSize;

    DWORD   MaxPrimitiveCount;          // max number of primitives per DrawPrimitive call
    DWORD   MaxVertexIndex;
    DWORD   MaxStreams;
    DWORD   MaxStreamStride;            // max stride for SetStreamSource

    DWORD   VertexShaderVersion;
    DWORD   MaxVertexShaderConst;       // number of vertex shader constant registers

    DWORD   PixelShaderVersion;
    float   MaxPixelShaderValue;        // max value of pixel shader arithmetic component

}

//#pragma pack()

alias D3DCAPS8 _D3DCAPS8;

//
// BIT DEFINES FOR D3DCAPS8 DWORD MEMBERS
//

//
// Caps
//
const int D3DCAPS_READ_SCANLINE           = 0x00020000;

//
// Caps2
//
const int D3DCAPS2_NO2DDURING3DSCENE      = 0x00000002;
const int D3DCAPS2_FULLSCREENGAMMA        = 0x00020000;
const int D3DCAPS2_CANRENDERWINDOWED      = 0x00080000;
const int D3DCAPS2_CANCALIBRATEGAMMA      = 0x00100000;
const int D3DCAPS2_RESERVED               = 0x02000000;

//
// Caps3
//
const uint D3DCAPS3_RESERVED               = 0x8000001f;

//
// PresentationIntervals
//
const int D3DPRESENT_INTERVAL_DEFAULT     = 0x00000000;
const int D3DPRESENT_INTERVAL_ONE         = 0x00000001;
const int D3DPRESENT_INTERVAL_TWO         = 0x00000002;
const int D3DPRESENT_INTERVAL_THREE       = 0x00000004;
const int D3DPRESENT_INTERVAL_FOUR        = 0x00000008;
const uint D3DPRESENT_INTERVAL_IMMEDIATE   = 0x80000000;

//
// CursorCaps
//
// Driver supports HW color cursor in at least hi-res modes(height >=400)
const int D3DCURSORCAPS_COLOR             = 0x00000001L;
// Driver supports HW cursor also in low-res modes(height < 400)
const int D3DCURSORCAPS_LOWRES            = 0x00000002L;

//
// DevCaps
//
const int D3DDEVCAPS_EXECUTESYSTEMMEMORY  = 0x00000010L; /* Device can use execute buffers from system memory */
const int D3DDEVCAPS_EXECUTEVIDEOMEMORY   = 0x00000020L; /* Device can use execute buffers from video memory */
const int D3DDEVCAPS_TLVERTEXSYSTEMMEMORY = 0x00000040L; /* Device can use TL buffers from system memory */
const int D3DDEVCAPS_TLVERTEXVIDEOMEMORY  = 0x00000080L; /* Device can use TL buffers from video memory */
const int D3DDEVCAPS_TEXTURESYSTEMMEMORY  = 0x00000100L; /* Device can texture from system memory */
const int D3DDEVCAPS_TEXTUREVIDEOMEMORY   = 0x00000200L; /* Device can texture from device memory */
const int D3DDEVCAPS_DRAWPRIMTLVERTEX     = 0x00000400L; /* Device can draw TLVERTEX primitives */
const int D3DDEVCAPS_CANRENDERAFTERFLIP   = 0x00000800L; /* Device can render without waiting for flip to complete */
const int D3DDEVCAPS_TEXTURENONLOCALVIDMEM = 0x00001000L; /* Device can texture from nonlocal video memory */
const int D3DDEVCAPS_DRAWPRIMITIVES2      = 0x00002000L; /* Device can support DrawPrimitives2 */
const int D3DDEVCAPS_SEPARATETEXTUREMEMORIES = 0x00004000L; /* Device is texturing from separate memory pools */
const int D3DDEVCAPS_DRAWPRIMITIVES2EX    = 0x00008000L; /* Device can support Extended DrawPrimitives2 i.e. DX7 compliant driver*/
const int D3DDEVCAPS_HWTRANSFORMANDLIGHT  = 0x00010000L; /* Device can support transformation and lighting in hardware and DRAWPRIMITIVES2EX must be also */
const int D3DDEVCAPS_CANBLTSYSTONONLOCAL  = 0x00020000L; /* Device supports a Tex Blt from system memory to non-local vidmem */
const int D3DDEVCAPS_HWRASTERIZATION      = 0x00080000L; /* Device has HW acceleration for rasterization */
const int D3DDEVCAPS_PUREDEVICE           = 0x00100000L; /* Device supports D3DCREATE_PUREDEVICE */
const int D3DDEVCAPS_QUINTICRTPATCHES     = 0x00200000L; /* Device supports quintic Beziers and BSplines */
const int D3DDEVCAPS_RTPATCHES            = 0x00400000L; /* Device supports Rect and Tri patches */
const int D3DDEVCAPS_RTPATCHHANDLEZERO    = 0x00800000L; /* Indicates that RT Patches may be drawn efficiently using handle 0 */
const int D3DDEVCAPS_NPATCHES             = 0x01000000L; /* Device supports N-Patches */

//
// PrimitiveMiscCaps
//
const int D3DPMISCCAPS_MASKZ              = 0x00000002L;
const int D3DPMISCCAPS_LINEPATTERNREP     = 0x00000004L;
const int D3DPMISCCAPS_CULLNONE           = 0x00000010L;
const int D3DPMISCCAPS_CULLCW             = 0x00000020L;
const int D3DPMISCCAPS_CULLCCW            = 0x00000040L;
const int D3DPMISCCAPS_COLORWRITEENABLE   = 0x00000080L;
const int D3DPMISCCAPS_CLIPPLANESCALEDPOINTS = 0x00000100L; /* Device correctly clips scaled points to clip planes */
const int D3DPMISCCAPS_CLIPTLVERTS        = 0x00000200L; /* device will clip post-transformed vertex primitives */
const int D3DPMISCCAPS_TSSARGTEMP         = 0x00000400L; /* device supports D3DTA_TEMP for temporary register */
const int D3DPMISCCAPS_BLENDOP            = 0x00000800L; /* device supports D3DRS_BLENDOP */

//
// LineCaps
//
const int D3DLINECAPS_TEXTURE             = 0x00000001L;
const int D3DLINECAPS_ZTEST               = 0x00000002L;
const int D3DLINECAPS_BLEND               = 0x00000004L;
const int D3DLINECAPS_ALPHACMP            = 0x00000008L;
const int D3DLINECAPS_FOG                 = 0x00000010L;

//
// RasterCaps
//
const int D3DPRASTERCAPS_DITHER           = 0x00000001L;
const int D3DPRASTERCAPS_PAT              = 0x00000008L;
const int D3DPRASTERCAPS_ZTEST            = 0x00000010L;
const int D3DPRASTERCAPS_FOGVERTEX        = 0x00000080L;
const int D3DPRASTERCAPS_FOGTABLE         = 0x00000100L;
const int D3DPRASTERCAPS_ANTIALIASEDGES   = 0x00001000L;
const int D3DPRASTERCAPS_MIPMAPLODBIAS    = 0x00002000L;
const int D3DPRASTERCAPS_ZBIAS            = 0x00004000L;
const int D3DPRASTERCAPS_ZBUFFERLESSHSR   = 0x00008000L;
const int D3DPRASTERCAPS_FOGRANGE         = 0x00010000L;
const int D3DPRASTERCAPS_ANISOTROPY       = 0x00020000L;
const int D3DPRASTERCAPS_WBUFFER          = 0x00040000L;
const int D3DPRASTERCAPS_WFOG             = 0x00100000L;
const int D3DPRASTERCAPS_ZFOG             = 0x00200000L;
const int D3DPRASTERCAPS_COLORPERSPECTIVE = 0x00400000L; /* Device iterates colors perspective correct */
const int D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE  = 0x00800000L;

//
// ZCmpCaps, AlphaCmpCaps
//
const int D3DPCMPCAPS_NEVER               = 0x00000001L;
const int D3DPCMPCAPS_LESS                = 0x00000002L;
const int D3DPCMPCAPS_EQUAL               = 0x00000004L;
const int D3DPCMPCAPS_LESSEQUAL           = 0x00000008L;
const int D3DPCMPCAPS_GREATER             = 0x00000010L;
const int D3DPCMPCAPS_NOTEQUAL            = 0x00000020L;
const int D3DPCMPCAPS_GREATEREQUAL        = 0x00000040L;
const int D3DPCMPCAPS_ALWAYS              = 0x00000080L;

//
// SourceBlendCaps, DestBlendCaps
//
const int D3DPBLENDCAPS_ZERO              = 0x00000001L;
const int D3DPBLENDCAPS_ONE               = 0x00000002L;
const int D3DPBLENDCAPS_SRCCOLOR          = 0x00000004L;
const int D3DPBLENDCAPS_INVSRCCOLOR       = 0x00000008L;
const int D3DPBLENDCAPS_SRCALPHA          = 0x00000010L;
const int D3DPBLENDCAPS_INVSRCALPHA       = 0x00000020L;
const int D3DPBLENDCAPS_DESTALPHA         = 0x00000040L;
const int D3DPBLENDCAPS_INVDESTALPHA      = 0x00000080L;
const int D3DPBLENDCAPS_DESTCOLOR         = 0x00000100L;
const int D3DPBLENDCAPS_INVDESTCOLOR      = 0x00000200L;
const int D3DPBLENDCAPS_SRCALPHASAT       = 0x00000400L;
const int D3DPBLENDCAPS_BOTHSRCALPHA      = 0x00000800L;
const int D3DPBLENDCAPS_BOTHINVSRCALPHA   = 0x00001000L;

//
// ShadeCaps
//
const int D3DPSHADECAPS_COLORGOURAUDRGB       = 0x00000008L;
const int D3DPSHADECAPS_SPECULARGOURAUDRGB    = 0x00000200L;
const int D3DPSHADECAPS_ALPHAGOURAUDBLEND     = 0x00004000L;
const int D3DPSHADECAPS_FOGGOURAUD            = 0x00080000L;

//
// TextureCaps
//
const int D3DPTEXTURECAPS_PERSPECTIVE         = 0x00000001L; /* Perspective-correct texturing is supported */
const int D3DPTEXTURECAPS_POW2                = 0x00000002L; /* Power-of-2 texture dimensions are required - applies to non-Cube/Volume textures only. */
const int D3DPTEXTURECAPS_ALPHA               = 0x00000004L; /* Alpha in texture pixels is supported */
const int D3DPTEXTURECAPS_SQUAREONLY          = 0x00000020L; /* Only square textures are supported */
const int D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE = 0x00000040L; /* Texture indices are not scaled by the texture size prior to interpolation */
const int D3DPTEXTURECAPS_ALPHAPALETTE        = 0x00000080L; /* Device can draw alpha from texture palettes */
// Device can use non-POW2 textures if:
//  1) D3DTEXTURE_ADDRESS is set to CLAMP for this texture's stage
//  2) D3DRS_WRAP(N) is zero for this texture's coordinates
//  3) mip mapping is not enabled (use magnification filter only)
const int D3DPTEXTURECAPS_NONPOW2CONDITIONAL  = 0x00000100L;
const int D3DPTEXTURECAPS_PROJECTED           = 0x00000400L; /* Device can do D3DTTFF_PROJECTED */
const int D3DPTEXTURECAPS_CUBEMAP             = 0x00000800L; /* Device can do cubemap textures */
const int D3DPTEXTURECAPS_VOLUMEMAP           = 0x00002000L; /* Device can do volume textures */
const int D3DPTEXTURECAPS_MIPMAP              = 0x00004000L; /* Device can do mipmapped textures */
const int D3DPTEXTURECAPS_MIPVOLUMEMAP        = 0x00008000L; /* Device can do mipmapped volume textures */
const int D3DPTEXTURECAPS_MIPCUBEMAP          = 0x00010000L; /* Device can do mipmapped cube maps */
const int D3DPTEXTURECAPS_CUBEMAP_POW2        = 0x00020000L; /* Device requires that cubemaps be power-of-2 dimension */
const int D3DPTEXTURECAPS_VOLUMEMAP_POW2      = 0x00040000L; /* Device requires that volume maps be power-of-2 dimension */

//
// TextureFilterCaps
//
const int D3DPTFILTERCAPS_MINFPOINT           = 0x00000100L; /* Min Filter */
const int D3DPTFILTERCAPS_MINFLINEAR          = 0x00000200L;
const int D3DPTFILTERCAPS_MINFANISOTROPIC     = 0x00000400L;
const int D3DPTFILTERCAPS_MIPFPOINT           = 0x00010000L; /* Mip Filter */
const int D3DPTFILTERCAPS_MIPFLINEAR          = 0x00020000L;
const int D3DPTFILTERCAPS_MAGFPOINT           = 0x01000000L; /* Mag Filter */
const int D3DPTFILTERCAPS_MAGFLINEAR          = 0x02000000L;
const int D3DPTFILTERCAPS_MAGFANISOTROPIC     = 0x04000000L;
const int D3DPTFILTERCAPS_MAGFAFLATCUBIC      = 0x08000000L;
const int D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC   = 0x10000000L;

//
// TextureAddressCaps
//
const int D3DPTADDRESSCAPS_WRAP           = 0x00000001L;
const int D3DPTADDRESSCAPS_MIRROR         = 0x00000002L;
const int D3DPTADDRESSCAPS_CLAMP          = 0x00000004L;
const int D3DPTADDRESSCAPS_BORDER         = 0x00000008L;
const int D3DPTADDRESSCAPS_INDEPENDENTUV  = 0x00000010L;
const int D3DPTADDRESSCAPS_MIRRORONCE     = 0x00000020L;

//
// StencilCaps
//
const int D3DSTENCILCAPS_KEEP             = 0x00000001L;
const int D3DSTENCILCAPS_ZERO             = 0x00000002L;
const int D3DSTENCILCAPS_REPLACE          = 0x00000004L;
const int D3DSTENCILCAPS_INCRSAT          = 0x00000008L;
const int D3DSTENCILCAPS_DECRSAT          = 0x00000010L;
const int D3DSTENCILCAPS_INVERT           = 0x00000020L;
const int D3DSTENCILCAPS_INCR             = 0x00000040L;
const int D3DSTENCILCAPS_DECR             = 0x00000080L;

//
// TextureOpCaps
//
const int D3DTEXOPCAPS_DISABLE                    = 0x00000001L;
const int D3DTEXOPCAPS_SELECTARG1                 = 0x00000002L;
const int D3DTEXOPCAPS_SELECTARG2                 = 0x00000004L;
const int D3DTEXOPCAPS_MODULATE                   = 0x00000008L;
const int D3DTEXOPCAPS_MODULATE2X                 = 0x00000010L;
const int D3DTEXOPCAPS_MODULATE4X                 = 0x00000020L;
const int D3DTEXOPCAPS_ADD                        = 0x00000040L;
const int D3DTEXOPCAPS_ADDSIGNED                  = 0x00000080L;
const int D3DTEXOPCAPS_ADDSIGNED2X                = 0x00000100L;
const int D3DTEXOPCAPS_SUBTRACT                   = 0x00000200L;
const int D3DTEXOPCAPS_ADDSMOOTH                  = 0x00000400L;
const int D3DTEXOPCAPS_BLENDDIFFUSEALPHA          = 0x00000800L;
const int D3DTEXOPCAPS_BLENDTEXTUREALPHA          = 0x00001000L;
const int D3DTEXOPCAPS_BLENDFACTORALPHA           = 0x00002000L;
const int D3DTEXOPCAPS_BLENDTEXTUREALPHAPM        = 0x00004000L;
const int D3DTEXOPCAPS_BLENDCURRENTALPHA          = 0x00008000L;
const int D3DTEXOPCAPS_PREMODULATE                = 0x00010000L;
const int D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR     = 0x00020000L;
const int D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA     = 0x00040000L;
const int D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR  = 0x00080000L;
const int D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA  = 0x00100000L;
const int D3DTEXOPCAPS_BUMPENVMAP                 = 0x00200000L;
const int D3DTEXOPCAPS_BUMPENVMAPLUMINANCE        = 0x00400000L;
const int D3DTEXOPCAPS_DOTPRODUCT3                = 0x00800000L;
const int D3DTEXOPCAPS_MULTIPLYADD                = 0x01000000L;
const int D3DTEXOPCAPS_LERP                       = 0x02000000L;

//
// FVFCaps
//
const int D3DFVFCAPS_TEXCOORDCOUNTMASK    = 0x0000ffffL; /* mask for texture coordinate count field */
const int D3DFVFCAPS_DONOTSTRIPELEMENTS   = 0x00080000L; /* Device prefers that vertex elements not be stripped */
const int D3DFVFCAPS_PSIZE                = 0x00100000L; /* Device can receive point size */

//
// VertexProcessingCaps
//
const int D3DVTXPCAPS_TEXGEN              = 0x00000001L; /* device can do texgen */
const int D3DVTXPCAPS_MATERIALSOURCE7     = 0x00000002L; /* device can do DX7-level colormaterialsource ops */
const int D3DVTXPCAPS_DIRECTIONALLIGHTS   = 0x00000008L; /* device can do directional lights */
const int D3DVTXPCAPS_POSITIONALLIGHTS    = 0x00000010L; /* device can do positional lights (includes point and spot) */
const int D3DVTXPCAPS_LOCALVIEWER         = 0x00000020L; /* device can do local viewer */
const int D3DVTXPCAPS_TWEENING            = 0x00000040L; /* device can do vertex tweening */
const int D3DVTXPCAPS_NO_VSDT_UBYTE4      = 0x00000080L; /* device does not support D3DVSDT_UBYTE4 */



