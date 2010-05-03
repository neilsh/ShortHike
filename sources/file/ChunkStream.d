//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import ChunkCommon;
private import ChunkData;
private import std.stream;
private import std.zlib;

class ChunkStream
{


// --------------------------------------------------------------------------------

this(char[] fileName)
{
  mStream = new File(fileName, FileMode.OutNew);
  mLastChunkStart = uint.max;

  // Start by writing the signature
  mStream.writeExact(cast(void*)CHUNK_SIGNATURE, CHUNK_SIGNATURE_SIZE);
}


~this()
{  
  close();
}


// --------------------------------------------------------------------------------

void
close()
{
  if(mStream is null) return;

  openChunk("FEND");
  closeChunk();
  mStream.close();
  mStream = null;
}


// --------------------------------------------------------------------------------

bool
openChunk(char[] fourCC)
{
  if(mStream is null) {
    logError << "ChunkStream::openChunk(): trying to open Chunk in closed ChunkStream" << endl;
    return false;
  }  
  if(fourCC.length != 4) {
    logError << "ChunkStream::openChunk(): incorrect size of fourCC" << endl;
    return false;
  }
  
  mLastChunkStart = mStream.position();
  
  mRunningCRC = 0u;
  // Start by writing out the lenght as 0. We'll update it once all is written.
  uint zeroSize = 0;
  mStream.writeExact(cast(void*)&zeroSize, 4);
  
  void* pFourCC = fourCC;
  mRunningCRC = adler32(mRunningCRC, pFourCC[0 .. 4]);
  mStream.writeExact(fourCC, 4);
  return true;
}

void
closeChunk()
{
  // find size of chunk data only
  uint bytesWritten = cast(uint)(mStream.position() - mLastChunkStart) - CHUNK_HEADER_SIZE;
  
  // insert the correct size
  mStream.seekSet(cast(long)mLastChunkStart);
  mStream.writeExact(cast(void*)&bytesWritten, 4);
  
  // skip back to the end and write out the CRC, not part of size
  mStream.seekEnd(0);

  // FIXME: Not needed?
//   mRunningCRC = mRunningCRC ^ 0xffffffffL;
  mStream.writeExact(cast(void*)&mRunningCRC, 4);
}

// --------------------------------------------------------------------------------

void
writeExact(void[] input)
{
  mStream.writeExact(input.ptr, input.length);
}


void
writeExact(void* input, uint bytesToWrite)
{
  mStream.writeExact(input, bytesToWrite);
  mRunningCRC = adler32(mRunningCRC, input[0 .. bytesToWrite]);
}

// --------------------------------------------------------------------------------

void
writeCompressedRaw(void* input, uint bytesToWrite)
{
  write(cast(uint)(CompressionType.COMPRESSION_RAW));
  write(bytesToWrite);
  writeExact(input, bytesToWrite);
}

// --------------------------------------------------------------------------------

bool
writeCompressedDeflate(void* input, uint bytesToWrite, int level = 9)
{
  assert(CHECK("Valid compression level", level >= 0 && level <= 9));

  write(cast(uint)(CompressionType.COMPRESSION_DEFLATE));
  write(bytesToWrite);

  void[] buffer = compress(input[0 .. bytesToWrite], level);

  write(buffer.length);
  writeExact(buffer);
  return true;
}


// --------------------------------------------------------------------------------

bool
writeCompressedJPEG2000(void* input, int width, int height, float rate)
{
  assert(CHECK_FAIL("JPEG2000 Compression not implemented"));
  
//   uint originalSize = width * height * 4;
//   write<int>(COMPRESSION_JPEG2000);
//   write(originalSize);
  
//   const int COMPONENT_NUMBER = 4;
//   jas_image_cmptparm_t componentParam[COMPONENT_NUMBER];
//   for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
//     ZeroMemory(componentParam + iComponent, sizeof(jas_image_cmptparm_t));
//     componentParam[iComponent].tlx = 0;
//     componentParam[iComponent].tly = 0;
//     componentParam[iComponent].hstep = 1;
//     componentParam[iComponent].vstep = 1;
//     componentParam[iComponent].width = width;
//     componentParam[iComponent].height = height;
//     componentParam[iComponent].prec = 8;
//     componentParam[iComponent].sgnd = false;
//   }
  
//   jas_image_t* jasImage = jas_image_create(COMPONENT_NUMBER, componentParam, JAS_CLRSPC_SRGB);
  
//   const int8u* imageData = static_cast<const int8u*>(input);
//   jas_matrix_t* rowData = jas_matrix_create(1, width);
//   for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
//     for(int y = 0; y < height; ++y) {
//       for(int x = 0; x < width; ++x) {
//         jas_matrix_set(rowData, 0, x, imageData[(y * width + x) * 4 + iComponent]);
//       }
//       jas_image_writecmpt(jasImage, iComponent, 0, y, width, 1, rowData);
//     }
//   }
//   ostringstream ossParams;
//   ossParams << "rate=" << rate;
//   char stringBuf[100];
//   strncpy(stringBuf, ossParams.str().c_str(), 100);

//   jas_stream_t* jasStream = jas_stream_tmpfile();
//   jp2_encode(jasImage, jasStream, stringBuf);
//   jas_image_destroy(jasImage);
  
//   assert(CHECK("Can seek in temporary", jas_stream_isseekable(jasStream) == 1));
//   uint compressedSize = jas_stream_length(jasStream);
//   write(compressedSize);

//   jas_stream_rewind(jasStream);
//   char* data = new char[compressedSize];
//   jas_stream_read(jasStream, data, compressedSize);
//   write(data, compressedSize);
//   delete[] data;
//   jas_stream_close(jasStream);

  return false;
}




// --------------------------------------------------------------------------------

void
writeString(char[] value)
{
  write(value.length);
  writeExact(cast(byte*)&value[0], value.length);
}


// --------------------------------------------------------------------------------

  void write(byte x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(ubyte x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(short x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(ushort x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(int x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(uint x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(long x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(ulong x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(float x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(double x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(real x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(ifloat x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(idouble x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(ireal x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(cfloat x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(cdouble x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(creal x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(char x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(wchar x) { writeExact(cast(byte*)&x, x.sizeof); }
  void write(dchar x) { writeExact(cast(byte*)&x, x.sizeof); }


// --------------------------------------------------------------------------------

// void
// copyChunk(ChunkIterator iChunk)
// {
//   write(iChunk.mData + iChunk.mChunkPos, iChunk.mChunkSize + CHUNK_META_SIZE);
// }


private:
  File mStream;
  uint mRunningCRC;
  ulong mLastChunkStart;
}
