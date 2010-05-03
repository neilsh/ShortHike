//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// This class is only created from files through CacheManager

private import Common;
private import ChunkCommon;
private import std.zlib;
private import std.stream;
private import std.c.string;

class ChunkData
{

  this(ubyte[] memory)
  {
    mData = memory;
    mEndIterator = new ChunkIterator(mData, mData.length);
  }

  this(char[] fileName) 
  {
    File file = new File(fileName, FileMode.In);
    mData.length = cast(uint)file.size();
    file.readExact(mData.ptr, mData.length);
    file.close();
    mEndIterator = new ChunkIterator(mData, mData.length);
  }
  

  ~this()
  {
  }

  ChunkIterator begin()
  {
    return new ChunkIterator(mData, 8);
  }

  ChunkIterator
  end()
  {
    return mEndIterator;
  }
  

  // --------------------------------------------------------------------------------


  bool
  isSignatureValid()
  {
    if(mData.length < CHUNK_SIGNATURE_SIZE) return false;
    for(int i = 0; i < CHUNK_SIGNATURE_SIZE; ++i)
      if(mData[i] != CHUNK_SIGNATURE[i]) return false;  
    
    return true;
  }

  // --------------------------------------------------------------------------------

  bool
  checkAll()
  {
    if(isSignatureValid()) {
      ChunkIterator iChunk = begin();
      bool valid = true;
      uint totalChunkSize = CHUNK_SIGNATURE_SIZE;
      while(iChunk != end() && iChunk.getChunkType() != "FEND") {
        if(!iChunk.isValid() || !iChunk.checkCRC()) {
          valid = false;
          break;
        }
        totalChunkSize += iChunk.getChunkSize() + CHUNK_META_SIZE;
        iChunk++;
      }    
      if(valid) {
        if(iChunk.getChunkType() == "FEND" && iChunk.checkCRC()) {
          totalChunkSize += iChunk.getChunkSize() + CHUNK_META_SIZE;
          iChunk++;
          if(iChunk == end() && totalChunkSize == mData.length) {
            return true;
          }
        }
      }
    }
  
    logError << "ChunkData::checkAll(): corrupted ChunkData" << endl;
    return false;
  }

private:
  ubyte[] mData;
  
  ChunkIterator mEndIterator;
};


// ================================================================================


class ChunkIterator
{

  this(ubyte[] data = null, uint filePos = 0)
  {
    mData = data;
    mChunkPos = filePos;
    mChunkReadPos = 0;

    if(mData !is null)
      readChunkSize();
    else
      mChunkSize = 0;
  }
  

  // --------------------------------------------------------------------------------

  void
  readChunkSize()
  {
    mChunkSize = *(cast(uint*)(mData.ptr + mChunkPos));
  }


  ubyte*
  getReadPosition()
  {
    return mData.ptr + mChunkPos + CHUNK_HEADER_SIZE + mChunkReadPos;
  }

  uint
  getChunkSize()
  {
    return mChunkSize;
  }  
  
  char[]
  getChunkType()
  {
    char[4] fourCC = (cast(char*)(mData.ptr + mChunkPos + 4))[0 .. 4];
    return fourCC.dup;
  }


  uint
  getCRC()
  {
    uint* pCRC = cast(uint*)(mData.ptr + mChunkPos + CHUNK_HEADER_SIZE + mChunkSize);
    return *pCRC;
  }

  
  // --------------------------------------------------------------------------------

  uint
  readExact(ubyte* buffer, uint bytes)
  {
    if((bytes + mChunkReadPos) >= mChunkSize)
      bytes = mChunkSize - mChunkReadPos;
    memcpy(buffer, getReadPosition(), bytes);
    mChunkReadPos += bytes;
    return bytes;
  }  

  // --------------------------------------------------------------------------------

  bool
  readCompressed(inout ubyte[] buffer)
  {
    uint bytes = buffer.length;
    int type;
    read(type);
    if(CompressionType.COMPRESSION_RAW == type) {
      uint dataSize;
      read(dataSize);
      if(dataSize != bytes) {
        logError << "Compressed mismatch between expected " << bytes << " and actual " << dataSize << endl;
        return false;
      }
      readExact(buffer, bytes);
    }
    else if(CompressionType.COMPRESSION_DEFLATE == type) {
      uint dataSize;
      read(dataSize);
      if(dataSize != bytes) {
        logError << "Compressed mismatch between expected " << bytes << " and actual " << dataSize << endl;
        return false;
      }
      uint compressedSize;
      read(compressedSize);
      if((compressedSize + mChunkReadPos) > mChunkSize) {
        logError << "Malformed file: Not enough compressed data" << endl;
        return false;
      }
      buffer[] = cast(ubyte[])uncompress(getReadPosition()[0 .. compressedSize]);
      if(buffer.length != bytes) {
        logError << "Wrong number of compressed bytes read" << endl;
        return false;
      }
      mChunkReadPos += compressedSize;
      return true;
    }
    else if(CompressionType.COMPRESSION_JPEG2000 == type) {
      assert(CHECK_FAIL("JP2 encoding not implemented"));
//       uint originalSize;
//       uint compressedSize;

//       read(originalSize);
//       if(originalSize != bytes) {
//         logError << "JP2: Mismatch between expected bytes and actual" << endl;
//         return false;
//       }
//       read(compressedSize);
//       if((compressedSize + mChunkReadPos) > mChunkSize) {
//         logError << "JP2: Malformed file: Not enough compressed data" << endl;
//         return false;
//       }    
//       jas_stream_t* jasStream = jas_stream_memopen(reinterpret_cast<char*>(getReadPosition()), compressedSize);
//       jas_image_t* jasImage = jp2_decode(jasStream, "");
//       jas_stream_close(jasStream);

//       // decode stuff out
//       int width = jas_image_width(jasImage);
//       int height = jas_image_height(jasImage);
//       if((width * height * 4) != static_cast<int>(originalSize)) {
//         jas_image_destroy(jasImage);
//         logEngineError("JP2: Malformed file: width/height don't match data size");
//         return false;
//       }
    
//       const int COMPONENT_NUMBER = 4;
//       int8u* imageData = static_cast<int8u*>(buffer);
//       jas_matrix_t* rowData = jas_matrix_create(1, width);
//       for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
//         for(int y = 0; y < height; ++y) {
//           jas_image_readcmpt(jasImage, iComponent, 0, y, width, 1, rowData);
//           for(int x = 0; x < width; ++x) {
//             imageData[(y * width + x) * 4 + iComponent] = jas_matrix_get(rowData, 0, x);
//           }
//         }
//       }
//       jas_image_destroy(jasImage);
//       mChunkReadPos += compressedSize;
      return false;
    }
    else {
      logError << "Unsupported compression type" << endl;
      return false;
    }
    return true;
  }


  // --------------------------------------------------------------------------------

  bool
  readString(out char[] oString)
  {
    uint stringLength;
    if(read(stringLength)) {
      oString.length = stringLength;
      return cast(bit)(readExact(cast(ubyte*)&(oString[0]), stringLength) == stringLength);
    }
    return false;
  }

  bool read(out byte x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out ubyte x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out short x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out ushort x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out int x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out uint x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out long x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out ulong x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out float x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out double x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out real x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out ifloat x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out idouble x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out ireal x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out cfloat x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out cdouble x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out creal x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out char x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out wchar x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }
  bool read(out dchar x) { return cast(bool)(x.sizeof == readExact(cast(ubyte*)&x, x.sizeof)); }  

  // --------------------------------------------------------------------------------

  ChunkIterator
  opPostInc()
  {
    mChunkPos += getChunkSize() + CHUNK_HEADER_SIZE + CHUNK_FOOTER_SIZE;
    if(mChunkPos > mData.length) {
      logError << "ChunkData: Error in chunk file. Chunk pointing past end of file." << endl;
      mChunkPos = mData.length;
    }
  
    readChunkSize();
    mChunkReadPos = 0;
    return this;
  }

  bool
  opEquals(ChunkIterator rhs)
  {
    if(null == mData && null == rhs.mData) return true;
    ubyte* ptr1 = mData.ptr;
    ubyte* ptr2 = rhs.mData.ptr;
    uint pos1 = mChunkPos;
    uint pos2 = rhs.mChunkPos;
    if(mData.ptr != rhs.mData.ptr)
      return false;
    if(mChunkPos != rhs.mChunkPos)
      return false;
    return true;
  }

  // --------------------------------------------------------------------------------

  bool
  isValid()
  {
    uint chunkEnd = mChunkPos + mChunkSize + CHUNK_META_SIZE;
    return cast(bool)(mChunkPos >= 0 && chunkEnd <= mData.length);
  }


  bool
  checkCRC()
  {
    ulong dataCRC = adler32(0u, (mData.ptr + mChunkPos + CHUNK_LENGTH_SIZE)[0 .. CHUNK_TYPE_SIZE + mChunkSize]);
    ulong fileCRC = getCRC();
    return cast(bool)(dataCRC == fileCRC);
  }

  ChunkIterator
  copy()
  {
    ChunkIterator newIter = new ChunkIterator(mData);
    newIter.mChunkPos = mChunkPos;
    newIter.mChunkSize = mChunkSize;
    newIter.mChunkReadPos = mChunkReadPos;
    return newIter;
  }
  

private:
  ubyte[] mData;

  uint mChunkPos;
  uint mChunkSize;  
  uint mChunkReadPos;
}


