//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Mesh;
private import DX8RenderSystem;

private import std.c.windows.windows;
private import d3d8;


const DWORD MESH_FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | 0;
                        // D3DFVF_TEXCOORDSIZE2(0));



class DX8Mesh : public Mesh
{

  
  // --------------------------------------------------------------------------------

  this(DX8RenderSystem renderSystem, bool collisionSupport)
  {
    super(collisionSupport);
    mRenderSystem = renderSystem;
    mVertexBuffer = null;
    mIndexBuffer = null;
    mVertexCount = 0;
    mIndexCount = 0;
  }

  ~this()
  {
    cleanup();
  }

  // --------------------------------------------------------------------------------

  void
  cleanup()
  {
    if(mVertexBuffer !is null)
      mVertexBuffer.Release();
    if(mIndexBuffer !is null)
      mIndexBuffer.Release();
    mVertexBuffer = null;
    mIndexBuffer = null;
    mVertexCount = 0;
    mIndexCount = 0;
  }

  // --------------------------------------------------------------------------------

  void
  flushInternal()
  {
    cleanup();

//     if(D3DMAXNUMVERTICES < mVertexData.length || D3DMAXNUMVERTICES < mIndexData.length)
//       assert(CHECK_FAIL("The vertex or index count is too high"));
    if(mVertexData.length == 0) {
      logWarn() << "Trying to flush 0 sized mesh" << endLog;
      return;
    }
  
    IDirect3DDevice8 device = mRenderSystem.getDirect3DDevice();

    mStride = 8 * 4;
    
    HRESULT hr;
    hr = device.CreateVertexBuffer(mStride * mVertexData.length, cast(uint)D3DUSAGE_WRITEONLY,
                                           MESH_FVF, cast(uint)D3DPOOL_DEFAULT, &mVertexBuffer);
    assert(CHECK("Debug: check", hr == D3D_OK));

    hr = device.CreateIndexBuffer(mIndexData.length * DWORD.sizeof, cast(uint)D3DUSAGE_WRITEONLY, cast(uint)D3DFMT_INDEX32,
                                  cast(uint)D3DPOOL_DEFAULT, &mIndexBuffer);
    assert(CHECK("Debug: check", hr == D3D_OK));

    ubyte* testPtr;
    hr = mVertexBuffer.Lock(0u, 0u, &testPtr, cast(uint)D3DLOCK_NOSYSLOCK);
    float* vertexPtr = cast(float*)testPtr;
    
    for(int iVertex = 0; iVertex < mVertexData.length; ++iVertex) {
      Vertex vertex = mVertexData[iVertex];
      *vertexPtr++ = vertex.position.x;
      *vertexPtr++ = vertex.position.y;
      *vertexPtr++ = vertex.position.z;
      *vertexPtr++ = vertex.normal.x;
      *vertexPtr++ = vertex.normal.y;
      *vertexPtr++ = vertex.normal.z;
      *vertexPtr++ = vertex.u;
      *vertexPtr++ = 1 - vertex.v;
    }
    hr = mVertexBuffer.Unlock();
    
    DWORD* indexPtr;
    hr = mIndexBuffer.Lock(0u, 0u, cast(ubyte**)&indexPtr, cast(uint)D3DLOCK_NOSYSLOCK);
    for(int iIndex = 0; iIndex < mIndexData.length; ++iIndex) {
       *indexPtr++ = mIndexData[iIndex];
    }
    hr = mIndexBuffer.Unlock();

    mIndexCount = mIndexData.length;
    mVertexCount = mVertexData.length;
  }

  // --------------------------------------------------------------------------------

  DX8RenderSystem mRenderSystem;

  IDirect3DVertexBuffer8 mVertexBuffer;
  IDirect3DIndexBuffer8 mIndexBuffer;

  DWORD mVertexCount;
  DWORD mIndexCount;
  DWORD mStride;
}

