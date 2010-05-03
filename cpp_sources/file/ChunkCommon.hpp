//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef FILE_CHUNK_COMMON_HPP
#define FILE_CHUNK_COMMON_HPP

const size_t CHUNK_SIGNATURE_SIZE = 8;
const size_t CHUNK_LENGTH_SIZE = 4;
const size_t CHUNK_TYPE_SIZE = 4;
const size_t CHUNK_FOOTER_SIZE = 4;
const size_t CHUNK_HEADER_SIZE = CHUNK_LENGTH_SIZE + CHUNK_TYPE_SIZE;
const size_t CHUNK_META_SIZE = CHUNK_HEADER_SIZE + CHUNK_FOOTER_SIZE;

const int8u CHUNK_SIGNATURE[CHUNK_SIGNATURE_SIZE] = {
  0x94, 0x53, 0x48, 0x44, 0x0d, 0x0a, 0x1a, 0x0a
};  


extern unsigned long update_crc(unsigned long crc, const unsigned char *buf, int len);
extern unsigned long crc(unsigned char *buf, int len);

enum CompressionType {
  COMPRESSION_RAW = 0,
  COMPRESSION_DEFLATE = 1,
  COMPRESSION_JPEG2000 = 2
};
 

class TestChunkCommon : public TestFixture
{
public:
  TEST_FIXTURE_START(TestChunkCommon);
  TEST_CASE(write_read_chunk);
  TEST_CASE(speed_raw);
  TEST_CASE(speed_compression_zlib);
  TEST_CASE(speed_jpg2000);
  TEST_FIXTURE_END();

  void write_read_chunk();
  void speed_raw();
  void speed_compression_zlib();
  void speed_jpg2000();
};

#endif
