//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DX7Common.hpp"
#include "DX7Mesh.hpp"
#include "DX7RenderSystem.hpp"

#pragma warning(disable: 4189)

// --------------------------------------------------------------------------------

DX7Mesh::DX7Mesh(DX7RenderSystem* renderSystem, bool collisionSupport)
  : Mesh(collisionSupport), mRenderSystem(renderSystem), mVertexBuffer(NULL), mIndexBuffer(NULL)
{
}

DX7Mesh::~DX7Mesh()
{
  cleanup();
}

// --------------------------------------------------------------------------------

void
DX7Mesh::cleanup()
{
  if(NULL != mVertexBuffer)
    mVertexBuffer->Release();
  if(NULL != mIndexBuffer)
    delete[] mIndexBuffer;
  mVertexCount = 0;
  mIndexCount = 0;
  mVertexBuffer = NULL;
  mIndexBuffer = NULL;
}

// --------------------------------------------------------------------------------

void
DX7Mesh::flushInternal()
{
  cleanup();

  if(D3DMAXNUMVERTICES < mVertexData.size() || D3DMAXNUMVERTICES < mIndexData.size())
    CHECK_FAIL("The vertex or index count is too high");

  if(mVertexData.size() == 0) {
    logEngineWarn("Trying to activate 0 sized mesh");
    return;
  }
  
  IDirect3D7* direct3D = mRenderSystem->getDirect3D();

  D3DVERTEXBUFFERDESC bufferDesc;
  bufferDesc.dwSize = sizeof(bufferDesc);
  if(mRenderSystem->isHardwareAccelerated())
    bufferDesc.dwCaps = D3DVBCAPS_WRITEONLY;
  else
    bufferDesc.dwCaps = D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY;

  bufferDesc.dwFVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
  bufferDesc.dwNumVertices = mVertexData.size();  
  
  HRESULT hr = direct3D->CreateVertexBuffer(&bufferDesc, &mVertexBuffer, 0);
  CHECK("Failed VB creation", SUCCEEDED(hr));

  float* bufferData;
  hr = mVertexBuffer->Lock(DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY | DDLOCK_DISCARDCONTENTS,
                           reinterpret_cast<void**>(&bufferData), NULL);
  for(VertexData::const_iterator iVertex = mVertexData.begin(); iVertex != mVertexData.end(); ++iVertex) {
    Vertex vertex = *iVertex;
    *bufferData++ = vertex.position.x;
    *bufferData++ = vertex.position.y;
    *bufferData++ = vertex.position.z;
    *bufferData++ = vertex.normal.x;
    *bufferData++ = vertex.normal.y;
    *bufferData++ = vertex.normal.z;
    *bufferData++ = vertex.u;
    *bufferData++ = 1 - vertex.v;
  }
  hr = mVertexBuffer->Unlock();

  // Didn't do much good and might hinder restarting the device
  //   hr = mVertexBuffer->Optimize(mRenderSystem->getDirect3DDevice(), 0);
  mVertexCount = mVertexData.size();

  mIndexBuffer = new WORD[mIndexData.size()];
  WORD* indexPtr = mIndexBuffer;
  for(IndexData::const_iterator iIndex = mIndexData.begin(); iIndex != mIndexData.end(); ++iIndex) {
    unsigned int index = *iIndex;
    WORD indexWord = static_cast<WORD>(index);
    if(D3DMAXNUMVERTICES < index)
      CHECK_FAIL("Index count too high");
    if(index < 0 || index >= mVertexCount)
      CHECK_FAIL("Invalid index");
    *indexPtr++ = static_cast<WORD>(index);
  }  
  mIndexCount = mIndexData.size();
}


