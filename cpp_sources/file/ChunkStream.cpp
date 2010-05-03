//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "ChunkStream.hpp"
#include "ChunkData.hpp"
#include "ChunkCommon.hpp"


// --------------------------------------------------------------------------------

ChunkStream::ChunkStream(string fileName)
  : mStream(new ofstream(fileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc)), mLastChunkStart(INT_MAX)
{
  mStream->write(reinterpret_cast<const char*>(CHUNK_SIGNATURE), CHUNK_SIGNATURE_SIZE);
}


ChunkStream::~ChunkStream()
{  
  close();
}


// --------------------------------------------------------------------------------

void
ChunkStream::close()
{
  if(NULL == mStream) return;

  openChunk("FEND");
  closeChunk();
  mStream->close();
  mStream = NULL;
}


// --------------------------------------------------------------------------------

bool
ChunkStream::openChunk(string fourCC)
{
  if(NULL == mStream) {
    logEngineError("ChunkStream::openChunk(): trying to open Chunk in closed ChunkStream");
    return false;
  }  
  if(fourCC.size() != 4) {
    logEngineError("ChunkStream::openChunk(): incorrect size of fourCC");
    return false;
  }
  
  mLastChunkStart = mStream->tellp();
  
  mRunningCRC = 0xffffffffL;
  size_t zeroSize = 0;
  mStream->write(reinterpret_cast<char*>(&zeroSize), 4);
  
  unsigned char cFourCC[4];
  cFourCC[0] = fourCC[0];
  cFourCC[1] = fourCC[1];
  cFourCC[2] = fourCC[2];
  cFourCC[3] = fourCC[3];

  mRunningCRC = update_crc(mRunningCRC, cFourCC, 4);
  mStream->write(fourCC.c_str(), 4);
  return true;
}

void
ChunkStream::closeChunk()
{
  // find size of chunk data only
  size_t bytesWritten = mStream->tellp() - mLastChunkStart - static_cast<streampos>(CHUNK_HEADER_SIZE);
  
  // insert the correct size
  mStream->seekp(mLastChunkStart);
  mStream->write(reinterpret_cast<char*>(&bytesWritten), 4);
  
  // skip back to the end and write out the CRC, not part of size
  mStream->seekp(0, ios::end);
  mRunningCRC = mRunningCRC ^ 0xffffffffL;
  mStream->write(reinterpret_cast<char*>(&mRunningCRC), 4);
}

// --------------------------------------------------------------------------------

void
ChunkStream::write(const void* input, size_t bytesToWrite)
{
  const char* data = static_cast<const char*>(input);
  mStream->write(data, bytesToWrite);
  const unsigned char* crcData = static_cast<const unsigned char*>(input);
  mRunningCRC = update_crc(mRunningCRC, crcData, bytesToWrite);
}

// --------------------------------------------------------------------------------

void
ChunkStream::writeCompressedRaw(const void* input, size_t bytesToWrite)
{
  write<int>(COMPRESSION_RAW);
  write(bytesToWrite);
  write(input, bytesToWrite);
}

// --------------------------------------------------------------------------------

bool
ChunkStream::writeCompressedDeflate(const void* input, size_t bytesToWrite, int level)
{
  CHECK("Valid compression level", level >= 0 && level <= 9);

  write<int>(COMPRESSION_DEFLATE);
  write(bytesToWrite);

  size_t bufferSize = static_cast<size_t>(bytesToWrite * 1.2f + 12.0f);
  uLongf bytesWritten = bufferSize;
  Bytef* buffer = new Bytef[bufferSize];
  if(Z_OK != compress2(buffer, &bytesWritten, const_cast<Bytef*>(static_cast<const Bytef*>(input)), bytesToWrite, level)) {
    logEngineError("Error in deflating data");
    return false;
  }
  write(bytesWritten);
  write(buffer, bytesWritten);  
  delete[] buffer;
  return true;
}


// --------------------------------------------------------------------------------

bool
ChunkStream::writeCompressedJPEG2000(const void* input, int width, int height, float rate)
{
  size_t originalSize = width * height * 4;
  write<int>(COMPRESSION_JPEG2000);
  write(originalSize);
  
  const int COMPONENT_NUMBER = 4;
  jas_image_cmptparm_t componentParam[COMPONENT_NUMBER];
  for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
    ZeroMemory(componentParam + iComponent, sizeof(jas_image_cmptparm_t));
    componentParam[iComponent].tlx = 0;
    componentParam[iComponent].tly = 0;
    componentParam[iComponent].hstep = 1;
    componentParam[iComponent].vstep = 1;
    componentParam[iComponent].width = width;
    componentParam[iComponent].height = height;
    componentParam[iComponent].prec = 8;
    componentParam[iComponent].sgnd = false;
  }
  
  jas_image_t* jasImage = jas_image_create(COMPONENT_NUMBER, componentParam, JAS_CLRSPC_SRGB);
  
  const int8u* imageData = static_cast<const int8u*>(input);
  jas_matrix_t* rowData = jas_matrix_create(1, width);
  for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
    for(int y = 0; y < height; ++y) {
      for(int x = 0; x < width; ++x) {
        jas_matrix_set(rowData, 0, x, imageData[(y * width + x) * 4 + iComponent]);
      }
      jas_image_writecmpt(jasImage, iComponent, 0, y, width, 1, rowData);
    }
  }
  ostringstream ossParams;
  ossParams << "rate=" << rate;
  char stringBuf[100];
  strncpy(stringBuf, ossParams.str().c_str(), 100);

  jas_stream_t* jasStream = jas_stream_tmpfile();
  jp2_encode(jasImage, jasStream, stringBuf);
  jas_image_destroy(jasImage);
  
  CHECK("Can seek in temporary", jas_stream_isseekable(jasStream) == 1);
  size_t compressedSize = jas_stream_length(jasStream);
  write(compressedSize);

  jas_stream_rewind(jasStream);
  char* data = new char[compressedSize];
  jas_stream_read(jasStream, data, compressedSize);
  write(data, compressedSize);
  delete[] data;
  jas_stream_close(jasStream);

  return true;
}




// --------------------------------------------------------------------------------

void
ChunkStream::writeString(const string& value)
{
  unsigned int stringLenght = value.size();
  write(stringLenght);
  write(&value[0], stringLenght);
}

// --------------------------------------------------------------------------------

void
ChunkStream::copyChunk(const ChunkIterator& iChunk)
{
  write(iChunk.mData + iChunk.mChunkPos, iChunk.mChunkSize + CHUNK_META_SIZE);
}

