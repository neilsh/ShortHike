//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef FILE_CHUNK_STREAM_HPP
#define FILE_CHUNK_STREAM_HPP

// Used for Chunk based output. This is a simple stream based interface.
class ChunkIterator;

extern unsigned long update_crc(unsigned long crc, const unsigned char *buf, int len);

class ChunkStream
{
public:
  ChunkStream(string fileName);
  ~ChunkStream();

  void close();

  bool openChunk(string fourCC);
  void closeChunk();

  void write(const void* input, size_t bytesToWrite);
  void writeCompressedRaw(const void* input, size_t bytesToWrite);
  bool writeCompressedDeflate(const void* input, size_t bytesToWrite, int level = 9);
  bool writeCompressedJPEG2000(const void* input, int width, int height, float rate = 0.1);
  void writeString(const string& value);
  void copyChunk(const ChunkIterator& iChunk);

  template <class C> void write(const C& input)
  {
    size_t bytesToWrite = sizeof(C);
    write(static_cast<const void*>(&input), bytesToWrite);    
  }
private:
  ChunkStream& operator=(const ChunkStream&) {}

  ofstream* mStream;

  unsigned long mRunningCRC;
  streamoff mLastChunkStart;
};


#endif
