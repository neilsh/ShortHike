//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_SURFACE_HPP
#define RENDERING_SURFACE_HPP

// Surface data is always in RGBA format

class DX7RenderSystem;
class ChunkStream;
class ChunkData;
class ChunkIterator;

class Surface
{
  friend class DX7RenderSystem;
public:
  virtual ~Surface();

  bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  bool save(ChunkStream* chunkStream);

  virtual void flush();
  
  void* getData() const {return mData;}
  int getWidth() const {return mWidth;}
  int getHeight() const {return mHeight;}
  int getRowPitch() const {return mWidth * 4;}

  void loadFromTGA(string fileName);
  void loadFromPNG(string fileName);
  void loadCheckerBoard(int squareSize = 8);
protected:
  Surface(int width, int height);

  int mWidth;
  int mHeight;

  void* mData;
};


#endif
