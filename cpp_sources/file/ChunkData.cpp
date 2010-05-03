//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "ChunkCommon.hpp"
#include "ChunkData.hpp"

#include "main/Platform.hpp"

// --------------------------------------------------------------------------------

ChunkData::ChunkData(void* memory, size_t size)
  : mData(static_cast<int8u*>(memory)), mSize(size), mEndIterator(mData, mSize, mSize), mFileName("")
{
}


ChunkData::~ChunkData()
{
  if(mFileName != "")
    rPlatform()->releaseFileMapping(mFileName);
}



// --------------------------------------------------------------------------------



bool
ChunkData::isSignatureValid() const
{
  
  if(mSize < CHUNK_SIGNATURE_SIZE) return false;
  for(int i = 0; i < CHUNK_SIGNATURE_SIZE; ++i)
    if(mData[i] != CHUNK_SIGNATURE[i]) return false;  

  return true;
}

// --------------------------------------------------------------------------------

bool
ChunkData::checkAll()
{
  if(isSignatureValid()) {
    ChunkIterator iChunk = begin();
    bool valid = true;
    size_t totalChunkSize = CHUNK_SIGNATURE_SIZE;
    while(iChunk != end() && iChunk.getChunkType() != "FEND") {
      if(!iChunk.isValid() || !iChunk.checkCRC()) {
        valid = false;
        break;
      }
      totalChunkSize += iChunk.getChunkSize() + CHUNK_META_SIZE;
      ++iChunk;
    }    
    if(valid) {
      if(iChunk.getChunkType() == "FEND" && iChunk.checkCRC()) {
        totalChunkSize += iChunk.getChunkSize() + CHUNK_META_SIZE;
        ++iChunk;
        if(iChunk == end() && totalChunkSize == mSize) {
          return true;
        }
      }
    }
  }
  
  ostringstream ossError;
  ossError << "ChunkData::checkAll(): corrupted ChunkData";
  logEngineError(ossError.str());
  return false;
}


// --------------------------------------------------------------------------------


ChunkIterator::ChunkIterator(int8u* data, size_t fileSize, size_t filePos)
  : mData(data), mSize(fileSize), mChunkPos(filePos), mChunkSize(INT_MAX), mChunkReadPos(0)
{
  if(mData != NULL)
    readChunkSize();
  else
    mChunkSize = 0;
}
  

// --------------------------------------------------------------------------------

void
ChunkIterator::readChunkSize()
{
  mChunkSize = *(reinterpret_cast<size_t*>(mData + mChunkPos));
}

int8u*
ChunkIterator::getReadPosition()
{
  return mData + mChunkPos + CHUNK_HEADER_SIZE + mChunkReadPos;
}

size_t
ChunkIterator::getChunkSize()
{
  return mChunkSize;
}  
  
string
ChunkIterator::getChunkType()
{
  char* fourCC = reinterpret_cast<char*>(mData + mChunkPos + 4);
  return string(fourCC, 4);
}

unsigned long
ChunkIterator::getCRC()
{
  unsigned long* pCRC = reinterpret_cast<unsigned long*>(mData + mChunkPos + CHUNK_HEADER_SIZE + mChunkSize);
  return *pCRC;
}

  
// --------------------------------------------------------------------------------

size_t
ChunkIterator::read(void* buffer, size_t bytes)
{
  if((bytes + mChunkReadPos) >= mChunkSize)
    bytes = mChunkSize - mChunkReadPos;
  memcpy(buffer, getReadPosition(), bytes);
  mChunkReadPos += bytes;
  return bytes;
}  

// --------------------------------------------------------------------------------

bool
ChunkIterator::readCompressed(void* buffer, size_t bytes)
{
  int type;
  read(type);
  if(COMPRESSION_RAW == type) {
    size_t dataSize;
    read(dataSize);
    if(dataSize != bytes) {
      logEngineError("Mismatch between expected bytes and actual");
      return false;
    }
    read(buffer, bytes);
  }
  else if(COMPRESSION_DEFLATE == type) {
    uLongf dataSize;
    read(dataSize);
    if(dataSize != bytes) {
      logEngineError("Mismatch between expected bytes and actual");
      return false;
    }
    size_t compressedSize;
    read(compressedSize);
    if((compressedSize + mChunkReadPos) > mChunkSize) {
      logEngineError("Malformed file: Not enough compressed data");
      return false;
    }
    uncompress(static_cast<Bytef*>(buffer), &dataSize, getReadPosition(), compressedSize);
    if(dataSize != bytes) {
      logEngineError("Wrong number of compressed bytes read");
      return false;
    }
    mChunkReadPos += compressedSize;
    return true;
  }
  else if(COMPRESSION_JPEG2000 == type) {
    size_t originalSize;
    size_t compressedSize;

    read(originalSize);
    if(originalSize != bytes) {
      logEngineError("JP2: Mismatch between expected bytes and actual");
      return false;
    }
    read(compressedSize);
    if((compressedSize + mChunkReadPos) > mChunkSize) {
      logEngineError("JP2: Malformed file: Not enough compressed data");
      return false;
    }    
    jas_stream_t* jasStream = jas_stream_memopen(reinterpret_cast<char*>(getReadPosition()), compressedSize);
    jas_image_t* jasImage = jp2_decode(jasStream, "");
    jas_stream_close(jasStream);

    // decode stuff out
    int width = jas_image_width(jasImage);
    int height = jas_image_height(jasImage);
    if((width * height * 4) != static_cast<int>(originalSize)) {
      jas_image_destroy(jasImage);
      logEngineError("JP2: Malformed file: width/height don't match data size");
      return false;
    }
    
    const int COMPONENT_NUMBER = 4;
    int8u* imageData = static_cast<int8u*>(buffer);
    jas_matrix_t* rowData = jas_matrix_create(1, width);
    for(int iComponent = 0; iComponent < COMPONENT_NUMBER; ++iComponent) {
      for(int y = 0; y < height; ++y) {
        jas_image_readcmpt(jasImage, iComponent, 0, y, width, 1, rowData);
        for(int x = 0; x < width; ++x) {
          imageData[(y * width + x) * 4 + iComponent] = jas_matrix_get(rowData, 0, x);
        }
      }
    }
    jas_image_destroy(jasImage);
    mChunkReadPos += compressedSize;
    return true;
  }
  else {
    logEngineError("Unsupported compression type");
    return false;
  }
  return true;
}


// --------------------------------------------------------------------------------

bool
ChunkIterator::readString(string &oString)
{
  unsigned int stringLength;
  if(read(stringLength)) {
    oString.resize(stringLength);
    return read(&(oString[0]), stringLength) == stringLength;
  }
  return false;
}

// --------------------------------------------------------------------------------

ChunkIterator&
ChunkIterator::operator++()
{
  mChunkPos += getChunkSize() + CHUNK_HEADER_SIZE + CHUNK_FOOTER_SIZE;
  if(mChunkPos > mSize) {
    logEngineError("ChunkData: Error in chunk file. Chunk pointing past end of file.");
    mChunkPos = mSize;
  }
  
  readChunkSize();
  mChunkReadPos = 0;
  return *this;
}

bool
ChunkIterator::operator==(const ChunkIterator& rhs)
{
  if(NULL == mData && NULL == rhs.mData) return true;
  return mData == rhs.mData && mChunkPos == rhs.mChunkPos;
}

bool
ChunkIterator::operator!=(const ChunkIterator& rhs)
{
  if(NULL == mData && NULL == rhs.mData) return false;
  return mData != rhs.mData || mChunkPos != rhs.mChunkPos;
}

// --------------------------------------------------------------------------------

bool
ChunkIterator::isValid()
{
  size_t chunkEnd = mChunkPos + mChunkSize + CHUNK_META_SIZE;
  return mChunkPos >= 0 && chunkEnd <= mSize;    
}


bool
ChunkIterator::checkCRC()
{
  unsigned long dataCRC = crc(reinterpret_cast<unsigned char*>(mData + mChunkPos + CHUNK_LENGTH_SIZE), CHUNK_TYPE_SIZE + mChunkSize);
  return dataCRC == getCRC();
}
