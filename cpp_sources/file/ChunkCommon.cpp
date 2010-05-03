//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "ChunkCommon.hpp"
#include "ChunkStream.hpp"
#include "ChunkData.hpp"

#include "main/Platform.hpp"
#include "main/Timer.hpp"

// Table of CRCs of all 8-bit messages.
static unsigned long crc_table[256];
   
// Flag: has the table been computed? Initially false.
static int crc_table_computed = 0;
   
// Make the table for a fast CRC.
void make_crc_table(void)
{
  unsigned long c;
  int n, k;
  
  for (n = 0; n < 256; n++) {
    c = (unsigned long) n;
    for (k = 0; k < 8; k++) {
      if (c & 1)
        c = 0xedb88320L ^ (c >> 1);
      else
        c = c >> 1;
    }
    crc_table[n] = c;
  }
  crc_table_computed = 1;
}
   
// Update a running CRC with the bytes buf[0..len-1]--the CRC
// should be initialized to all 1's, and the transmitted value
// is the 1's complement of the final running CRC (see the
// crc() routine below)).
   
unsigned long update_crc(unsigned long crc, const unsigned char *buf,
                         int len)
{
  unsigned long c = crc;
  int n;
  
  if (!crc_table_computed)
    make_crc_table();
  for (n = 0; n < len; n++) {
    c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  }
  return c;
}
   
// Return the CRC of the bytes buf[0..len-1].
unsigned long crc(unsigned char *buf, int len)
{
  return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}


// ================================================================================


TEST_FIXTURE_DEFINITION("unit/file/ChunkCommon", TestChunkCommon);


void
TestChunkCommon::write_read_chunk()
{
  // Write deflate test
  string fileName = "testdata/TestChunkCommon/write_read_chunk.shd";

  ChunkStream* cStream = new ChunkStream(fileName.c_str());
  cStream->openChunk("FOOC");
  cStream->write(0x01020304);
  cStream->write(0x05060708);
  cStream->write(0x090a0b0c);
  cStream->closeChunk();

  cStream->openChunk("BARC");
  cStream->write(-0.1f);
  cStream->write(1.0f);
  cStream->closeChunk();

  cStream->openChunk("BAZC");
  cStream->writeString("foo bar baz");
  cStream->closeChunk();
  cStream->close();


  ifstream inFile(fileName.c_str(), ios_base::in | ios_base::binary);
  inFile.seekg(0, ios::end);
  streampos fileSize = inFile.tellg();
  inFile.seekg(0, ios::beg);
  char* data = new char[fileSize];
  inFile.read(data, fileSize);

  ChunkData cData(reinterpret_cast<int8u*>(data), fileSize);
  CHECK("ChunkData: Valid signature", cData.isSignatureValid());
  ChunkIterator iChunk = cData.begin();
  CHECK("Chunk1: Valid iterator", iChunk.isValid());
  CHECK_EQUAL("Chunk1: Valid type", string, "FOOC", iChunk.getChunkType());
  CHECK_EQUAL("Chunk1: Valid size", int, 12, iChunk.getChunkSize());
  CHECK("Chunk1: Valid CRC", iChunk.checkCRC());
  ++iChunk;
  CHECK("Chunk2: Valid iterator", iChunk.isValid());
  CHECK_EQUAL("Chunk2: Valid type", string, "BARC", iChunk.getChunkType());
  CHECK_EQUAL("Chunk2: Valid size", int, 8, iChunk.getChunkSize());
  float chunk2Float1;
  float chunk2Float2;
  iChunk.read(chunk2Float1);
  iChunk.read(chunk2Float2);
  CHECK_EQUAL("Chunk2: Float 1", float, -0.1f, chunk2Float1);
  CHECK_EQUAL("Chunk2: Float 2", float, 1.0f, chunk2Float2);
  CHECK("Chunk2: Valid CRC", iChunk.checkCRC());
  ++iChunk;
  CHECK("Chunk3: Valid iterator", iChunk.isValid());
  CHECK_EQUAL("Chunk3: Valid type", string, "BAZC", iChunk.getChunkType());
  CHECK_EQUAL("Chunk3: Valid size", int, 15, iChunk.getChunkSize());
  string chunk3String;
  iChunk.readString(chunk3String);
  CHECK_EQUAL("Chunk3: String read correct", string, string("foo bar baz"), chunk3String);
  CHECK("Chunk3: Valid CRC", iChunk.checkCRC());
  ++iChunk;
  CHECK("FEND: Valid iterator", iChunk.isValid());
  CHECK_EQUAL("FEND: Valid type", string, "FEND", iChunk.getChunkType());
  CHECK("FEND: Valid CRC", iChunk.checkCRC());
  ++iChunk;
  CHECK("End: Invalid iterator", iChunk.isValid() == false);
  CHECK("End: Equal to end iterator", iChunk == cData.end());

  delete data;
}

// --------------------------------------------------------------------------------
// lEVEL 3
// Uncompressed time: 741 ms
// Compressed time: 2117 ms
//
// Level 6 
// Uncompressed time: 683 ms
// Compressed time: 2025 ms



const size_t MESH_DATA_BYTES = 149730020;
const size_t IMAGE_DATA_BYTES = 157289420;

const string MESH_FILE_NAME = "testdata/TestChunkCommon/MeshData.dat";
const string MESH_COMPRESSED_FILE_NAME = "testdata/TestChunkCommon/MeshData.zip";

void
compressMeshData()
{
  // Compress MeshData.dat to MeshData.zip
  size_t sourceLen;
  void* source = rPlatform()->aquireFileMapping(MESH_FILE_NAME, sourceLen);

  unsigned long bufferLen = static_cast<unsigned long>(sourceLen * 1.02f);
  char* compressed = new char[bufferLen];
  unsigned long compressedLen = bufferLen;
  logEngineInfo("Compressing..");
  if(Z_OK != compress2(reinterpret_cast<Bytef*>(compressed), &compressedLen, static_cast<const Bytef*>(source), sourceLen, 9))
    CHECK_FAIL("Error in compression");
  logEngineInfo("Releasing..");
  rPlatform()->releaseFileMapping(MESH_FILE_NAME);  
  logEngineInfo("Saving..");
  ofstream file(MESH_COMPRESSED_FILE_NAME.c_str(), ios_base::binary | ios_base::trunc);
  file.write(compressed, compressedLen);
  file.close();  
  logEngineInfo("Done..");
  delete[] compressed;
}


void
unCompressMeshData()
{
  char* dest = new char[MESH_DATA_BYTES];
  // --------------------------------------------------------------------------------
  // Read from disk
  Timer* rawTimer = rPlatform()->createTimer();
  rawTimer->start();

  size_t rawLen;
  void* raw = rPlatform()->aquireFileMapping(MESH_FILE_NAME, rawLen);
  memcpy(dest, raw, rawLen);

  int ms = rawTimer->check(1000);
  ostringstream ossInfo;
  ossInfo << "Uncompressed time: " << ms << " ms";
  logEngineInfo(ossInfo.str());

  // --------------------------------------------------------------------------------
  // Read compressed
  Timer* zipTimer = rPlatform()->createTimer();
  zipTimer->start();

  size_t sourceLen;
  void* source = rPlatform()->aquireFileMapping(MESH_COMPRESSED_FILE_NAME, sourceLen);

  unsigned long destLen = MESH_DATA_BYTES;
  if(Z_OK != uncompress(reinterpret_cast<Bytef*>(dest), &destLen, static_cast<Bytef*>(source),  sourceLen))
    CHECK_FAIL("Error in decompression");
  rPlatform()->releaseFileMapping(MESH_COMPRESSED_FILE_NAME);

  int zipMS = zipTimer->check(1000);
  ostringstream ossZip;
  ossZip << "Compressed time: " << zipMS << " ms";
  logEngineInfo(ossZip.str());

  // --------------------------------------------------------------------------------

  delete[] dest;
}


// --------------------------------------------------------------------------------

void
TestChunkCommon::speed_raw()
{
  Timer* zipTimer = rPlatform()->createTimer();
  zipTimer->start();

  size_t rawLen;
  char* raw = static_cast<char*>(rPlatform()->aquireFileMapping(MESH_FILE_NAME, rawLen));
  int aCount = 0;
  for(size_t iByte = 0; iByte < rawLen; ++iByte) {
    if(raw[iByte] == 'a') {
      aCount++;
    }
  }  

  int rawMS = zipTimer->check(1000);
  ostringstream ossRaw;
  ossRaw << "Raw time: " << rawMS << " ms";
  logEngineInfo(ossRaw.str());
}


// --------------------------------------------------------------------------------
// Raw:
// Raw time: 955 ms
//
// Level 3: 27%
// Level 6:
// Deflate time: 32599 ms
// Inflate time: 1983 ms
// Level 9: 26%
// Deflate time: 66115 ms
// Inflate time: 1921 ms
// Inflate and read time: 3860 ms

void
TestChunkCommon::speed_compression_zlib()
{
  const string CHUNK_FILE_NAME = "testdata/TestChunkCommon/MeshData.shd";

  Timer* zipTimer = rPlatform()->createTimer();
  zipTimer->start();

  size_t rawLen;
  void* raw = rPlatform()->aquireFileMapping(MESH_FILE_NAME, rawLen);

  ChunkStream* cStream = new ChunkStream(CHUNK_FILE_NAME);
  cStream->openChunk("VRTX");
  cStream->writeCompressedDeflate(raw, rawLen, 9);
  cStream->closeChunk();
  cStream->close();  
  delete cStream;

  int deflateMS = zipTimer->check(1000);
  ostringstream ossDeflate;
  ossDeflate << "Deflate time: " << deflateMS << " ms";
  logEngineInfo(ossDeflate.str());
  

  size_t shdLen;
  void* shdData = rPlatform()->aquireFileMapping(CHUNK_FILE_NAME, shdLen);
  ChunkData cData(shdData, shdLen);

  CHECK("ChunkData: Valid signature", cData.isSignatureValid());
  ChunkIterator iChunk = cData.begin();
  CHECK("Chunk1: Valid iterator", iChunk.isValid());
  CHECK_EQUAL("Chunk1: Valid type", string, "VRTX", iChunk.getChunkType());
  CHECK("Chunk1: Valid CRC", iChunk.checkCRC());

  char* dest = new char[MESH_DATA_BYTES];

  zipTimer->start();  
  iChunk.readCompressed(dest, MESH_DATA_BYTES);
  
  for(int iByte = 0; iByte < MESH_DATA_BYTES; ++iByte) {
    if(static_cast<char*>(raw)[iByte] != dest[iByte]) {
      CHECK_FAIL("Corrupted data");
    }    
  }

  int inflateMS = zipTimer->check(1000);
  ostringstream ossInflate;
  ossInflate << "Inflate time: " << inflateMS << " ms";
  logEngineInfo(ossInflate.str());
  
  delete[] dest;
  rPlatform()->releaseFileMapping(CHUNK_FILE_NAME);
  rPlatform()->releaseFileMapping(MESH_FILE_NAME);
}

// --------------------------------------------------------------------------------



void
TestChunkCommon::speed_jpg2000()
{


}



