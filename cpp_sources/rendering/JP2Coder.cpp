//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "JP2Coder.hpp"

// #include <jasper/jasper.h>

// #pragma warning(push, 3)
// #pragma warning(disable: 4706)
// #include "NCSFile.h"
// #pragma warning(pop)

//--------------------------------------------------------------------------------

// const int IMAGE_BANDS = 4;


// class CompressWorker : public CNCSFile
// {
// public:
//   CompressWorker(void* inputData, int width, int height, string outputFile)
//     : CNCSFile(), inputBuffer(static_cast<unsigned char*>(inputData)), imageWidth(width), imageHeight(height)
//   {
//     // Fill out the band information structure
//     NCSFileBandInfo bandInfo[IMAGE_BANDS];
//     bandInfo[0].nBits = 8;
//     bandInfo[0].bSigned = FALSE;
//     bandInfo[0].szDesc = "Red";

//     bandInfo[1].nBits = 8;
//     bandInfo[1].bSigned = FALSE;
//     bandInfo[1].szDesc = "Green";

//     bandInfo[2].nBits = 8;
//     bandInfo[2].bSigned = FALSE;
//     bandInfo[2].szDesc = "Blue";

//     bandInfo[3].nBits = 8;
//     bandInfo[3].bSigned = FALSE;
//     bandInfo[3].szDesc = "Alpha";

//     // Fill out the file information structure
//     NCSFileViewFileInfoEx fileInfo;
//     fileInfo.nSizeX = width;
//     fileInfo.nSizeY = height;
//     fileInfo.nBands = IMAGE_BANDS;
//     fileInfo.nCompressionRate = 20;
//     fileInfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
//     fileInfo.fCellIncrementX = 1000;
//     fileInfo.fCellIncrementY = 1000;
//     fileInfo.fOriginX = 0;
//     fileInfo.fOriginY = 0;
//     fileInfo.szDatum = "RAW";
//     fileInfo.szProjection = "RAW";

//     fileInfo.fCWRotationDegrees = 0;
//     fileInfo.eColorSpace = NCSCS_sRGB;
//     fileInfo.eCellType = NCSCT_UINT8;
//     fileInfo.pBands = bandInfo;

//     CNCSError error = SetFileInfo(fileInfo);    
//     string errorMessage = error.GetErrorMessage();

//     // Open file for output
//     char* fileNameC = new char[1000];
//     strncpy(fileNameC, outputFile.c_str(), 1000);
//     NCSError errorCode = Open(fileNameC, false, true);
//     if(errorCode != NCS_SUCCESS) {
//       string errorMessage = string(FormatErrorText(errorCode));
//       cout << "NCS: " << errorMessage << endl;
//     }
//     delete[] fileNameC;

//     // Open the input file
// //     input = CreateFile(inputFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
// //     inputBuffer = new unsigned char[imageWidth * IMAGE_BANDS];
//   }
  
//   ~CompressWorker()
//   {
//     delete[] inputBuffer;
//   }
  


//   virtual CNCSError
//   WriteReadLine(UINT32 nextLine, void **ppInputArray)
//   {
// //     LONGLONG offset = nextLine;
// //     offset *= imageWidth;
// //     offset *= IMAGE_BANDS;
    
// //     LARGE_INTEGER offsetStruct;
// //     offsetStruct.QuadPart = offset;

// //     SetFilePointerEx(input, offsetStruct, NULL, FILE_BEGIN);

// //     DWORD bytesToRead = imageWidth * IMAGE_BANDS;
// //     DWORD actualBytesRead;
// //     ReadFile(input, inputBuffer, bytesToRead, &actualBytesRead, NULL);

//     unsigned char* bandPointer[IMAGE_BANDS];
//     for(int band = 0; band < IMAGE_BANDS; ++band) {
//       bandPointer[band] = static_cast<unsigned char*>(ppInputArray[band]);
//     }    

//     long offset = nextLine * imageWidth * IMAGE_BANDS;
//     unsigned char* dataPointer = inputBuffer + offset;
//     for(int xpos = 0; xpos < imageWidth; ++xpos) {
//       for(int band = 0; band < IMAGE_BANDS; ++band)
//         *bandPointer[band]++ = *dataPointer++;
//     }

//     return NCS_SUCCESS;
//   }
  


// //   virtual void
// //   WriteStatus(UINT32 currentLine)
// //   {
// //     if(currentLine % (imageHeight / 100) == 0) {
// //       ostringstream status;
// //       status << "Processing: " << (currentLine * 100 / imageHeight) << "% complete" << endl;
// //       OutputDebugString(status.str().c_str());
// //     }    
// //   }
  

// private:
//   int imageWidth;
//   int imageHeight;
//   unsigned char* inputBuffer;
// };


// takes pure RGBA data

// bool
// compressJP2(void* data, int width, int height, string outputFile)
// {
//   jas_stream_t* in = jas_stream_memopen(char *data, width * height * 4);
//   if (jas_image_encode(image, out, cmdopts->outfmt, cmdopts->outopts)) {
//     fprintf(stderr, "error: cannot encode image\n");
//     exit(EXIT_FAILURE);
//   }
//   jas_stream_flush(out);

// zzz
//   return true;
// }


// --------------------------------------------------------------------------------

// static int8u* chunkDataData;
// static size_t chunkDataSize;
// static size_t chunkDataPos;

// NCSError
// chunkDataOpen(char*, void** clientdata)
// {
//   *clientdata = NULL;
// }

// NCSError
// chunkDataClose(void*)
// {
// }

// NCSError
// chunkDataRead(void*, void* buffer, UINT32 length)
// {
//   memcpy(buffer, chunkDataData + chunkDataPos, length);
// }

// NCSError
// chunkDataSeek(void*, UINT64 offset)
// {
//   CHECK_FAIL("chunkDataSeek not implemented");
// }


// NCSError
// chunkDataTell(void*, UINT64 *offset)
// {
//   CHECK_FAIL("chunkDataTell not implemented");
// }

// bool
// decompressJP2(ChunkIterator iChunk, void*& data, int& oWidth, int& oHeight)
// {
//   NCSecwSetIOCallbacks(rgbaOpen, rgbaClose, rgbaRead, rgbaSeek, rgbaTell);
// }



//--------------------------------------------------------------------------------
// Initializing the compressor screen

// INT WINAPI WinMain( HINSTANCE , HINSTANCE, LPSTR , INT )
// {
//   int imageWidth = 43200;
//   int imageHeight = 21600;
//   string inputFile = "earth_diffuse_1km_shaded_RGB.raw";
//   string outputFile = "earth_diffuse_1km_shaded.ecw";

//   cout << "Input file: " << inputFile << " Output file: " << outputFile << endl;

//   CNCSFile::SetKeySize();
 
//   CompressWorker worker(inputFile, outputFile, imageWidth, imageHeight);
//   CNCSError error = worker.Write();
//   string errorMessage = error.GetErrorMessage();
// }
