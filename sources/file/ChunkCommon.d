//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import ChunkStream;
private import ChunkData;
private import CacheManager;

private import std.mmfile;

const uint CHUNK_SIGNATURE_SIZE = 8;
const uint CHUNK_LENGTH_SIZE = 4;
const uint CHUNK_TYPE_SIZE = 4;
const uint CHUNK_FOOTER_SIZE = 4;
const uint CHUNK_HEADER_SIZE = CHUNK_LENGTH_SIZE + CHUNK_TYPE_SIZE;
const uint CHUNK_META_SIZE = CHUNK_HEADER_SIZE + CHUNK_FOOTER_SIZE;

const ubyte CHUNK_SIGNATURE[CHUNK_SIGNATURE_SIZE] = [0x94, 0x53, 0x48, 0x44, 0x0d, 0x0a, 0x1a, 0x0a];

// ================================================================================

class TestChunkCommon : public TestFixture
{
  //   TEST_FIXTURE_START(TestChunkCommon);
  //   TEST_CASE(write_read_chunk);
  //   TEST_CASE(speed_raw);
  //   TEST_CASE(speed_compression_zlib);
  //   TEST_CASE(speed_jpg2000);
  //   TEST_FIXTURE_END();


  this()
  {
    mFixtureName = "TestChunkCommon";
    addTest("write_read_chunk", &write_read_chunk);
  }

  static this()
  {
    addTestFixture(new TestChunkCommon());
  }


  void
  write_read_chunk()
  {
    // Write deflate test
    char[] fileName = "testdata/TestChunkCommon/write_read_chunk.shd";

    ChunkStream cStream = new ChunkStream(fileName);
    cStream.openChunk("FOOC");
    cStream.write(0x01020304);
    cStream.write(0x05060708);
    cStream.write(0x090a0b0c);
    cStream.closeChunk();

    cStream.openChunk("BARC");
    cStream.write(-0.1f);
    cStream.write(1.0f);
    cStream.closeChunk();

    cStream.openChunk("BAZC");
    cStream.writeString("foo bar baz");
    cStream.closeChunk();
    cStream.close();

    // 
//     ifstream inFile(fileName.c_str(), ios_base::in | ios_base::binary);
//     inFile.seekg(0, ios::end);
//     streampos fileSize = inFile.tellg();
//     inFile.seekg(0, ios::beg);
//     char* data = new char[fileSize];
//     inFile.read(data, fileSize);


//     ChunkData cData(reinterpret_cast<int8u*>(data), fileSize);
    
    MmFile chunkFile = new MmFile(fileName);
    ChunkData cData = new ChunkData(cast(ubyte[])chunkFile[]);

    assert(CHECK("ChunkData: Valid signature", cData.isSignatureValid()));
    ChunkIterator iChunk = cData.begin();
    assert(CHECK("Chunk1: Valid iterator", iChunk.isValid()));
    assert(CHECK_EQUAL("Chunk1: Valid type", "FOOC", iChunk.getChunkType()));
    assert(CHECK_EQUAL("Chunk1: Valid size", 12u, iChunk.getChunkSize()));
    assert(CHECK("Chunk1: Valid CRC", iChunk.checkCRC()));
    iChunk++;
    assert(CHECK("Chunk2: Valid iterator", iChunk.isValid()));
    assert(CHECK_EQUAL("Chunk2: Valid type", "BARC", iChunk.getChunkType()));
    assert(CHECK_EQUAL("Chunk2: Valid size", 8u, iChunk.getChunkSize()));
    float chunk2Float1;
    float chunk2Float2;
    iChunk.read(chunk2Float1);
    iChunk.read(chunk2Float2);
    assert(CHECK_EQUAL("Chunk2: Float 1", -0.1f, chunk2Float1));
    assert(CHECK_EQUAL("Chunk2: Float 2", 1.0f, chunk2Float2));
    assert(CHECK("Chunk2: Valid CRC", iChunk.checkCRC()));
    iChunk++;
    assert(CHECK("Chunk3: Valid iterator", iChunk.isValid()));
    assert(CHECK_EQUAL("Chunk3: Valid type", "BAZC", iChunk.getChunkType()));
    assert(CHECK_EQUAL("Chunk3: Valid size", 15u, iChunk.getChunkSize()));
    char[] chunk3String;
    iChunk.readString(chunk3String);
    assert(CHECK_EQUAL("Chunk3: String read correct", "foo bar baz", chunk3String));
    assert(CHECK("Chunk3: Valid CRC", iChunk.checkCRC()));
    iChunk++;
    assert(CHECK("FEND: Valid iterator", iChunk.isValid()));
    assert(CHECK_EQUAL("FEND: Valid type", "FEND", iChunk.getChunkType()));
    assert(CHECK("FEND: Valid CRC", iChunk.checkCRC()));
    iChunk++;
    assert(CHECK("End: Invalid iterator", iChunk.isValid() == false));
    assert(CHECK("End: Equal to end iterator", iChunk == cData.end()));
    delete chunkFile;
  }

  // --------------------------------------------------------------------------------
  // lEVEL 3
  // Uncompressed time: 741 ms
  // Compressed time: 2117 ms
  //
  // Level 6 
  // Uncompressed time: 683 ms
  // Compressed time: 2025 ms



  // const uint MESH_DATA_BYTES = 149730020;
  // const uint IMAGE_DATA_BYTES = 157289420;

  // const string MESH_FILE_NAME = "testdata/TestChunkCommon/MeshData.dat";
  // const string MESH_COMPRESSED_FILE_NAME = "testdata/TestChunkCommon/MeshData.zip";

  // void
  // compressMeshData()
  // {
  //   // Compress MeshData.dat to MeshData.zip
  //   uint sourceLen;
  //   void* source = rPlatform().aquireFileMapping(MESH_FILE_NAME, sourceLen);

  //   unsigned long bufferLen = static_cast<unsigned long>(sourceLen * 1.02f);
  //   char* compressed = new char[bufferLen];
  //   unsigned long compressedLen = bufferLen;
  //   logEngineInfo("Compressing..");
  //   if(Z_OK != compress2(reinterpret_cast<Bytef*>(compressed), &compressedLen, static_cast<const Bytef*>(source), sourceLen, 9))
  //     assert(CHECK_FAIL("Error in compression"));
  //   logEngineInfo("Releasing..");
  //   rPlatform().releaseFileMapping(MESH_FILE_NAME);  
  //   logEngineInfo("Saving..");
  //   ofstream file(MESH_COMPRESSED_FILE_NAME.c_str(), ios_base::binary | ios_base::trunc);
  //   file.write(compressed, compressedLen);
  //   file.close();  
  //   logEngineInfo("Done..");
  //   delete[] compressed;
  // }


  // void
  // unCompressMeshData()
  // {
  //   char* dest = new char[MESH_DATA_BYTES];
  //   // --------------------------------------------------------------------------------
  //   // Read from disk
  //   Timer* rawTimer = rPlatform().createTimer();
  //   rawTimer.start();

  //   uint rawLen;
  //   void* raw = rPlatform().aquireFileMapping(MESH_FILE_NAME, rawLen);
  //   memcpy(dest, raw, rawLen);

  //   int ms = rawTimer.check(1000);
  //   ostringstream ossInfo;
  //   ossInfo << "Uncompressed time: " << ms << " ms";
  //   logEngineInfo(ossInfo.str());

  //   // --------------------------------------------------------------------------------
  //   // Read compressed
  //   Timer* zipTimer = rPlatform().createTimer();
  //   zipTimer.start();

  //   uint sourceLen;
  //   void* source = rPlatform().aquireFileMapping(MESH_COMPRESSED_FILE_NAME, sourceLen);

  //   unsigned long destLen = MESH_DATA_BYTES;
  //   if(Z_OK != uncompress(reinterpret_cast<Bytef*>(dest), &destLen, static_cast<Bytef*>(source),  sourceLen))
  //     assert(CHECK_FAIL("Error in decompression"));
  //   rPlatform().releaseFileMapping(MESH_COMPRESSED_FILE_NAME);

  //   int zipMS = zipTimer.check(1000);
  //   ostringstream ossZip;
  //   ossZip << "Compressed time: " << zipMS << " ms";
  //   logEngineInfo(ossZip.str());

  //   // --------------------------------------------------------------------------------

  //   delete[] dest;
  // }


  // // --------------------------------------------------------------------------------

  // void
  // TestChunkCommon::speed_raw()
  // {
  //   Timer* zipTimer = rPlatform().createTimer();
  //   zipTimer.start();

  //   uint rawLen;
  //   char* raw = static_cast<char*>(rPlatform().aquireFileMapping(MESH_FILE_NAME, rawLen));
  //   int aCount = 0;
  //   for(uint iByte = 0; iByte < rawLen; ++iByte) {
  //     if(raw[iByte] == 'a') {
  //       aCount++;
  //     }
  //   }  

  //   int rawMS = zipTimer.check(1000);
  //   ostringstream ossRaw;
  //   ossRaw << "Raw time: " << rawMS << " ms";
  //   logEngineInfo(ossRaw.str());
  // }


  // // --------------------------------------------------------------------------------
  // // Raw:
  // // Raw time: 955 ms
  // //
  // // Level 3: 27%
  // // Level 6:
  // // Deflate time: 32599 ms
  // // Inflate time: 1983 ms
  // // Level 9: 26%
  // // Deflate time: 66115 ms
  // // Inflate time: 1921 ms
  // // Inflate and read time: 3860 ms

  // void
  // TestChunkCommon::speed_compression_zlib()
  // {
  //   const string CHUNK_FILE_NAME = "testdata/TestChunkCommon/MeshData.shd";

  //   Timer* zipTimer = rPlatform().createTimer();
  //   zipTimer.start();

  //   uint rawLen;
  //   void* raw = rPlatform().aquireFileMapping(MESH_FILE_NAME, rawLen);

  //   ChunkStream* cStream = new ChunkStream(CHUNK_FILE_NAME);
  //   cStream.openChunk("VRTX");
  //   cStream.writeCompressedDeflate(raw, rawLen, 9);
  //   cStream.closeChunk();
  //   cStream.close();  
  //   delete cStream;

  //   int deflateMS = zipTimer.check(1000);
  //   ostringstream ossDeflate;
  //   ossDeflate << "Deflate time: " << deflateMS << " ms";
  //   logEngineInfo(ossDeflate.str());
  

  //   uint shdLen;
  //   void* shdData = rPlatform().aquireFileMapping(CHUNK_FILE_NAME, shdLen);
  //   ChunkData cData(shdData, shdLen);

  //   assert(CHECK("ChunkData: Valid signature", cData.isSignatureValid()));
  //   ChunkIterator iChunk = cData.begin();
  //   assert(CHECK("Chunk1: Valid iterator", iChunk.isValid()));
  //   assert(CHECK_EQUAL("Chunk1: Valid type", string, "VRTX", iChunk.getChunkType()));
  //   assert(CHECK("Chunk1: Valid CRC", iChunk.checkCRC()));

  //   char* dest = new char[MESH_DATA_BYTES];

  //   zipTimer.start();  
  //   iChunk.readCompressed(dest, MESH_DATA_BYTES);
  
  //   for(int iByte = 0; iByte < MESH_DATA_BYTES; ++iByte) {
  //     if(static_cast<char*>(raw)[iByte] != dest[iByte]) {
  //       assert(CHECK_FAIL("Corrupted data"));
  //     }    
  //   }

  //   int inflateMS = zipTimer.check(1000);
  //   ostringstream ossInflate;
  //   ossInflate << "Inflate time: " << inflateMS << " ms";
  //   logEngineInfo(ossInflate.str());
  
  //   delete[] dest;
  //   rPlatform().releaseFileMapping(CHUNK_FILE_NAME);
  //   rPlatform().releaseFileMapping(MESH_FILE_NAME);
  // }

  // // --------------------------------------------------------------------------------



  // void
  // TestChunkCommon::speed_jpg2000()
  // {


  // }

}



