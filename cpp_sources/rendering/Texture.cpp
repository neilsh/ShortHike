//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Texture.hpp"
#include "LoadImage.hpp"

#include "file/ChunkStream.hpp"
#include "file/ChunkData.hpp"

// --------------------------------------------------------------------------------

Texture::Texture()
  : mData(NULL), mWidth(0), mHeight(0)
{
}

Texture::~Texture()
{
  if(NULL != mData)
    delete[] mData;
}


// --------------------------------------------------------------------------------

#pragma warning(disable: 4100)

bool
Texture::load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr)
{
  curr = begin;
  if(curr == end || curr.getChunkType() != "IRGB") return false;

  curr.read(mWidth);
  curr.read(mHeight);
  delete[] mData;
  mData = new agg::int8u[mWidth * mHeight * 4];
  curr.readCompressed(mData, mWidth * mHeight * 4);
  
  flush();
  ++curr;
  
  return true;
}


// --------------------------------------------------------------------------------

bool
Texture::save(ChunkStream* stream, CompressionType compression)
{
  stream->openChunk("IRGB");
  stream->write(mWidth);
  stream->write(mHeight);
  if(COMPRESSION_DEFLATE == compression)
    stream->writeCompressedDeflate(mData, mWidth * mHeight * 4);
  else if(COMPRESSION_JPEG2000 == compression)
    stream->writeCompressedJPEG2000(mData, mWidth, mHeight, 0.15f);
  else
    stream->writeCompressedRaw(mData, mWidth * mHeight * 4);
  stream->closeChunk();
  return true;
}


// --------------------------------------------------------------------------------


bool
Texture::flush()
{
  return true;
}

// --------------------------------------------------------------------------------

void
Texture::loadSolid(int r, int g, int b, int a)
{
  if(NULL != mData)
    delete[] mData;

  mWidth = 64;
  mHeight = 64;

  mData = new agg::int8u[mWidth * mHeight * 4];
  agg::int8u* destPtr = static_cast<agg::int8u*>(getData());
  for(int x = 0; x < mWidth; ++x) {
    for(int y = 0; y < mHeight; ++y) {
        *destPtr++ = b;
        *destPtr++ = g;
        *destPtr++ = r;
        *destPtr++ = a;
    }
  }
  flush();
}

// --------------------------------------------------------------------------------

void
Texture::loadCheckerBoard(int squareSize)
{
  if(NULL != mData)
    delete[] mData;
  mWidth = 256;
  mHeight = 256;
  mData = new agg::int8u[mWidth * mHeight * 4];
  agg::int8u* destPtr = static_cast<agg::int8u*>(getData());
  for(int x = 0; x < mWidth; ++x) {
    for(int y = 0; y < mHeight; ++y) {
      int value = ((x / squareSize) + (y / squareSize)) % 2;
      if(0 == value) {
        *destPtr++ = 0x3f;
        *destPtr++ = 0x3f;
        *destPtr++ = 0x3f;
        *destPtr++ = 0xff;
      }
      else {
        *destPtr++ = 0xcf;
        *destPtr++ = 0xcf;
        *destPtr++ = 0xcf;
        *destPtr++ = 0xff;
      }
    }
  }
  flush();
}

// --------------------------------------------------------------------------------

void
Texture::loadFromTGA(string fileName)
{
  if(loadImageTGA(fileName, reinterpret_cast<agg::int8u**>(&mData), &mWidth, &mHeight))
    flush();
}

void
Texture::loadFromPNG(string fileName)
{
  if(loadImagePNG(fileName, reinterpret_cast<agg::int8u**>(&mData), &mWidth, &mHeight))
    flush();
}

