//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_TEXTURE_HPP
#define RENDERING_TEXTURE_HPP

// Texture data is always in RGBA format

#include "file/ChunkCommon.hpp"

class DX7RenderSystem;
class ChunkStream;
class ChunkData;
class ChunkIterator;

class Texture
{
  friend class DX7RenderSystem;
public:
  virtual ~Texture();

  bool load(string fileName);
  bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  bool save(ChunkStream* chunkStream, CompressionType compression = COMPRESSION_DEFLATE);

  virtual bool flush();
  
  void loadCheckerBoard(int squareSize = 16);
  void loadSolid(int r, int g, int b, int a);
  void loadFromTGA(string fileName);
  void loadFromPNG(string fileName);

  void* getData() const {return mData;}
  int getWidth() const {return mWidth;}
  int getHeight() const {return mHeight;}
  int getRowPitch() const {return mWidth * 4;}
protected:
  Texture();

  int mWidth;
  int mHeight;

  void* mData;
  vector<void*> mMipMaps;
};


#endif
