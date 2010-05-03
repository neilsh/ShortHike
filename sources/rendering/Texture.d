//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import ChunkData;
private import CacheManager;
private import ChunkStream;
private import ChunkCommon;
private import LoadImage;
private import RenderSystem;

class Texture
{
public:
  ~this()
  {
  }
  

  // --------------------------------------------------------------------------------  

  bool
  load(ChunkIterator begin, ChunkIterator end, out ChunkIterator curr)
  {
    curr = begin;
    if(curr == end || curr.getChunkType() != "IRGB") return false;

    curr.read(mWidth);
    curr.read(mHeight);

    mData.length = mWidth * mHeight * 4;
    curr.readCompressed(mData);
  
    flush();
    curr++;
  
    return true;
  }


  // --------------------------------------------------------------------------------

  bool
  save(ChunkStream stream, CompressionType compression = CompressionType.COMPRESSION_DEFLATE)
  {
    stream.openChunk("IRGB");
    stream.write(mWidth);
    stream.write(mHeight);
    if(CompressionType.COMPRESSION_DEFLATE == compression)
      stream.writeCompressedDeflate(mData, mWidth * mHeight * 4);
    else if(CompressionType.COMPRESSION_JPEG2000 == compression)
      stream.writeCompressedJPEG2000(mData.ptr, mWidth, mHeight, 0.15f);
    else
      stream.writeCompressedRaw(mData, mWidth * mHeight * 4);
    stream.closeChunk();
    return true;
  }
  

  // --------------------------------------------------------------------------------

  //   bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  //   bool save(ChunkStream* chunkStream, CompressionType compression = COMPRESSION_DEFLATE);

  bool
  flush()
  {
    return true;
  }

  // --------------------------------------------------------------------------------
  
  void
  loadSolid(ubyte r, ubyte g, ubyte b, ubyte a)
  {
    setSize(64, 64);
    ubyte* destPtr = mData;
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
  loadCheckerBoard(int squareSize = 16)
  {
    setSize(256, 256);
    ubyte* destPtr = mData;
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
  loadFromTGA(char[] fileName)
  {
    assert(CHECK_FAIL("Not implemented"));
    assert(false);
    //     if(loadImageTGA(fileName, mData, mWidth, mHeight))
    //       flush();
  }

  void
  loadFromPNG(char[] fileName)
  {
    if(loadImagePNG(fileName, mData, mWidth, mHeight))
      flush();
  }

  // --------------------------------------------------------------------------------

  int getWidth()
  {
    return mWidth;
  }

  int getHeight()
  {
    return mHeight;
  }

  int getRowPitch()
  {
    return mWidth * 4;
  }

  void setPixel(int x, int y, Color c) 
  {
    ubyte r = cast(ubyte)(c.r * 255);
    ubyte g = cast(ubyte)(c.g * 255);
    ubyte b = cast(ubyte)(c.b * 255);
    ubyte a = cast(ubyte)(c.a * 255);
    setPixel(x, y, r, g, b, a);
  }
  
  void setPixel(int x, int y, ubyte r, ubyte g, ubyte b, ubyte a)
  {
    if(x < 0 || x >= mWidth || y < 0 || y >= mHeight)
      return;
    ubyte* dataPtr = mData.ptr + (x + y * mWidth) * 4;
    *dataPtr++ = b;
    *dataPtr++ = g;
    *dataPtr++ = r;
    *dataPtr++ = a;   
  }
  

  // --------------------------------------------------------------------------------

  void setSize(int w, int h)
  {
    mWidth = w;
    mHeight = h;
    mData.length = mWidth * mHeight * 4;
    mData[] = 0;
  }
  

  int mWidth;
  int mHeight;

  ubyte[] mData;
protected:
  this()
  {
    mWidth = 0;
    mHeight = 0;
  }
}











