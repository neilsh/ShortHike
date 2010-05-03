//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "LoadImage.hpp"

// --------------------------------------------------------------------------------

bool
isPowerOfTwo(int size)
{
  const int LARGEST_POWER_OF_TWO = 1 << 28;
  int base = 1;
  while(base < LARGEST_POWER_OF_TWO) {
    if(size == base) return true;
    if(size < base) return false;
    base = base * 2;
  }
  return false;
}

bool
createMipMaps(int8u* originalMap, int width, int height, vector<int8u*>& oMipMaps)
{  
  if(!(isPowerOfTwo(width) && isPowerOfTwo(height))) {
    ostringstream ossError;
    ossError << "createMipMap: Only power of 2 textures supported: " << width << "x" << height;
    logEngineError(ossError.str());
    return false;
  }

  oMipMaps.push_back(originalMap);
  
  int prevWidth = width;
  int prevHeight = height;
  int8u* prevMap = originalMap;
  while(prevWidth > 1 && prevHeight > 1) {
    int currWidth = prevWidth / 2;
    int currHeight = prevHeight / 2;
    int8u* currMap = new int8u[currWidth * currHeight * 4];
    oMipMaps.push_back(currMap);
    int8u* iMap = currMap;
    int8u* ul = prevMap;
    int8u* ur = prevMap + 4;
    int8u* ll = prevMap + (prevWidth * 4);
    int8u* lr = prevMap + (prevWidth * 4) + 4;

    for(int y = 0; y < currHeight; y++) {
      for(int x = 0; x < currWidth; x++) {
        *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
        *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
        *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
        *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
        ul += 4;
        ur += 4;
        ll += 4;
        lr += 4;
      }
      ul += prevWidth * 4;
      ur += prevWidth * 4;
      ll += prevWidth * 4;
      lr += prevWidth * 4;
    }

    prevWidth = currWidth;
    prevHeight = currHeight;
    prevMap = currMap;
  }
  return true;
}

// --------------------------------------------------------------------------------
// Targa loading


#define IMG_OK              0x1
#define IMG_ERR_NO_FILE     0x2
#define IMG_ERR_MEM_FAIL    0x4
#define IMG_ERR_BAD_FORMAT  0x8
#define IMG_ERR_UNSUPPORTED 0x40

class TGAImg
{
public:
  TGAImg();
  ~TGAImg();
  int Load(const char* szFilename);
  int GetBPP();
  int GetWidth();
  int GetHeight();
  unsigned char* GetImg();       // Return a pointer to image data
  unsigned char* GetPalette();   // Return a pointer to VGA palette

private:
  short int iWidth,iHeight,iBPP;
  unsigned long lImageSize;
  char bEnc;
  unsigned char *pImage, *pPalette, *pData;
   
  // Internal workers
  int ReadHeader();
  int LoadRawData();
  int LoadTgaRLEData();
  int LoadTgaPalette();
  void BGRtoRGB();
  void FlipImg();
};


TGAImg::TGAImg()
{ 
  pImage=pPalette=pData=NULL;
  iWidth=iHeight=iBPP=bEnc=0;
  lImageSize=0;
}


TGAImg::~TGAImg()
{
  if(pImage)
    {
      delete [] pImage;
      pImage=NULL;
    }

  if(pPalette)
    {
      delete [] pPalette;
      pPalette=NULL;
    }

  if(pData)
    {
      delete [] pData;
      pData=NULL;
    }
}


int TGAImg::Load(const char* szFilename)
{
  using namespace std;
  ifstream fIn;
  unsigned long ulSize;
  int iRet;

  // Clear out any existing image and palette
  if(pImage)
    {
      delete [] pImage;
      pImage=NULL;
    }

  if(pPalette)
    {
      delete [] pPalette;
      pPalette=NULL;
    }

  // Open the specified file
  fIn.open(szFilename,ios::binary);
    
  if(fIn==NULL)
    return IMG_ERR_NO_FILE;

  // Get file size
  fIn.seekg(0,ios_base::end);
  ulSize=fIn.tellg();
  fIn.seekg(0,ios_base::beg);

  // Allocate some space
  // Check and clear pDat, just in case
  if(pData)
    delete [] pData; 

  pData=new unsigned char[ulSize];

  if(pData==NULL)
    {
      fIn.close();
      return IMG_ERR_MEM_FAIL;
    }

  // Read the file into memory
  fIn.read((char*)pData,ulSize);

  fIn.close();

  // Process the header
  iRet=ReadHeader();

  if(iRet!=IMG_OK)
    return iRet;

  switch(bEnc)
    {
    case 1: // Raw Indexed
      {
        // Check filesize against header values
        if((lImageSize+18+pData[0]+768)>ulSize)
          return IMG_ERR_BAD_FORMAT;

        // Double check image type field
        if(pData[1]!=1)
          return IMG_ERR_BAD_FORMAT;

        // Load image data
        iRet=LoadRawData();
        if(iRet!=IMG_OK)
          return iRet;

        // Load palette
        iRet=LoadTgaPalette();
        if(iRet!=IMG_OK)
          return iRet;

        break;
      }

    case 2: // Raw RGB
      {
        // Check filesize against header values
        if((lImageSize+18+pData[0])>ulSize)
          return IMG_ERR_BAD_FORMAT;

        // Double check image type field
        if(pData[1]!=0)
          return IMG_ERR_BAD_FORMAT;

        // Load image data
        iRet=LoadRawData();
        if(iRet!=IMG_OK)
          return iRet;

        BGRtoRGB(); // Convert to RGB
        break;
      }

    case 9: // RLE Indexed
      {
        // Double check image type field
        if(pData[1]!=1)
          return IMG_ERR_BAD_FORMAT;

        // Load image data
        iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
          return iRet;

        // Load palette
        iRet=LoadTgaPalette();
        if(iRet!=IMG_OK)
          return iRet;

        break;
      }
 
    case 10: // RLE RGB
      {
        // Double check image type field
        if(pData[1]!=0)
          return IMG_ERR_BAD_FORMAT;

        // Load image data
        iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
          return iRet;

        BGRtoRGB(); // Convert to RGB
        break;
      }

    default:
      return IMG_ERR_UNSUPPORTED;
    }

  // Check flip bit
  if((pData[17] & 0x20)==0) 
    FlipImg();

  // Release file memory
  delete [] pData;
  pData=NULL;

  return IMG_OK;
}


int TGAImg::ReadHeader() // Examine the header and populate our class attributes
{
  short ColMapStart,ColMapLen;
  short x1,y1,x2,y2;

  if(pData==NULL)
    return IMG_ERR_NO_FILE;

  if(pData[1]>1)    // 0 (RGB) and 1 (Indexed) are the only types we know about
    return IMG_ERR_UNSUPPORTED;

  bEnc=pData[2];     // Encoding flag  1 = Raw indexed image
  //                2 = Raw RGB
  //                3 = Raw greyscale
  //                9 = RLE indexed
  //               10 = RLE RGB
  //               11 = RLE greyscale
  //               32 & 33 Other compression, indexed

  if(bEnc>11)       // We don't want 32 or 33
    return IMG_ERR_UNSUPPORTED;


  // Get palette info
  memcpy(&ColMapStart,&pData[3],2);
  memcpy(&ColMapLen,&pData[5],2);

  // Reject indexed images if not a VGA palette (256 entries with 24 bits per entry)
  if(pData[1]==1) // Indexed
    {
      if(ColMapStart!=0 || ColMapLen!=256 || pData[7]!=24)
        return IMG_ERR_UNSUPPORTED;
    }

  // Get image window and produce width & height values
  memcpy(&x1,&pData[8],2);
  memcpy(&y1,&pData[10],2);
  memcpy(&x2,&pData[12],2);
  memcpy(&y2,&pData[14],2);

  iWidth=(x2-x1);
  iHeight=(y2-y1);

  if(iWidth<1 || iHeight<1)
    return IMG_ERR_BAD_FORMAT;

  // Bits per Pixel
  iBPP=pData[16];

  // Check flip / interleave byte
  if(pData[17]>32) // Interleaved data
    return IMG_ERR_UNSUPPORTED;

  // Calculate image size
  lImageSize=(iWidth * iHeight * (iBPP/8));

  return IMG_OK;
}


int TGAImg::LoadRawData() // Load uncompressed image data
{
  short iOffset;
 
  if(pImage) // Clear old data if present
    delete [] pImage;

  pImage=new unsigned char[lImageSize];

  if(pImage==NULL)
    return IMG_ERR_MEM_FAIL;

  iOffset=pData[0]+18; // Add header to ident field size

  if(pData[1]==1) // Indexed images
    iOffset+=768;  // Add palette offset

  memcpy(pImage,&pData[iOffset],lImageSize);

  return IMG_OK;
}


int TGAImg::LoadTgaRLEData() // Load RLE compressed image data
{
  short iOffset,iPixelSize;
  unsigned char *pCur;
  unsigned long Index=0;
  unsigned char bLength,bLoop;

  // Calculate offset to image data
  iOffset=pData[0]+18;

  // Add palette offset for indexed images
  if(pData[1]==1)
    iOffset+=768; 

  // Get pixel size in bytes
  iPixelSize=iBPP/8;

  // Set our pointer to the beginning of the image data
  pCur=&pData[iOffset];

  // Allocate space for the image data
  if(pImage!=NULL)
    delete [] pImage;

  pImage=new unsigned char[lImageSize];

  if(pImage==NULL)
    return IMG_ERR_MEM_FAIL;

  // Decode
  while(Index<lImageSize) 
    {
      if(*pCur & 0x80) // Run length chunk (High bit = 1)
        {
          bLength=*pCur-127; // Get run length
          pCur++;            // Move to pixel data  

          // Repeat the next pixel bLength times
          for(bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize)
            memcpy(&pImage[Index],pCur,iPixelSize);
  
          pCur+=iPixelSize; // Move to the next descriptor chunk
        }
      else // Raw chunk
        {
          bLength=*pCur+1; // Get run length
          pCur++;          // Move to pixel data

          // Write the next bLength pixels directly
          for(bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize,pCur+=iPixelSize)
            memcpy(&pImage[Index],pCur,iPixelSize);
        }
    }
 
  return IMG_OK;
}


int TGAImg::LoadTgaPalette() // Load a 256 color palette
{
  unsigned char bTemp;
  short iIndex,iPalPtr;
  
  // Delete old palette if present
  if(pPalette)
    {
      delete [] pPalette;
      pPalette=NULL;
    }

  // Create space for new palette
  pPalette=new unsigned char[768];

  if(pPalette==NULL)
    return IMG_ERR_MEM_FAIL;

  // VGA palette is the 768 bytes following the header
  memcpy(pPalette,&pData[pData[0]+18],768);

  // Palette entries are BGR ordered so we have to convert to RGB
  for(iIndex=0,iPalPtr=0;iIndex!=256;++iIndex,iPalPtr+=3)
    {
      bTemp=pPalette[iPalPtr];               // Get Blue value
      pPalette[iPalPtr]=pPalette[iPalPtr+2]; // Copy Red to Blue
      pPalette[iPalPtr+2]=bTemp;             // Replace Blue at the end
    }

  return IMG_OK;
}


void TGAImg::BGRtoRGB() // Convert BGR to RGB (or back again)
{
  unsigned long Index,nPixels;
  unsigned char *bCur;
  unsigned char bTemp;
  short iPixelSize;

  // Set ptr to start of image
  bCur=pImage;

  // Calc number of pixels
  nPixels=iWidth*iHeight;

  // Get pixel size in bytes
  iPixelSize=iBPP/8;

  for(Index=0;Index!=nPixels;Index++)  // For each pixel
    {
      bTemp=*bCur;      // Get Blue value
      *bCur=*(bCur+2);  // Swap red value into first position
      *(bCur+2)=bTemp;  // Write back blue to last position

      bCur+=iPixelSize; // Jump to next pixel
    }

}


void TGAImg::FlipImg() // Flips the image vertically (Why store images upside down?)
{
  unsigned char bTemp;
  unsigned char *pLine1, *pLine2;
  int iLineLen,iIndex;
 
  iLineLen=iWidth*(iBPP/8);
  pLine1=pImage;
  pLine2=&pImage[iLineLen * (iHeight - 1)];

  for( ;pLine1<pLine2;pLine2-=(iLineLen*2))
    {
      for(iIndex=0;iIndex!=iLineLen;pLine1++,pLine2++,iIndex++)
        {
          bTemp=*pLine1;
          *pLine1=*pLine2;
          *pLine2=bTemp;       
        }
    } 

}


int TGAImg::GetBPP() 
{
  return iBPP;
}


int TGAImg::GetWidth()
{
  return iWidth;
}


int TGAImg::GetHeight()
{
  return iHeight;
}


unsigned char* TGAImg::GetImg()
{
  return pImage;
}


unsigned char* TGAImg::GetPalette()
{
  return pPalette;
}


bool
loadImageTGA(string fileName, agg::int8u** oData, int* oWidth, int* oHeight)
{
  TGAImg tgaImage = TGAImg();

  int result = tgaImage.Load(fileName.c_str());
  if(result != IMG_OK) {
    ostringstream ossError;
    ossError << "TGA loader: Error: ";
    if(IMG_ERR_NO_FILE == result)
      ossError << "No file (IMG_ERR_NO_FILE)";
    else if(IMG_ERR_MEM_FAIL == result)
      ossError << "Memory failure (IMG_ERR_MEM_FAIL)";
    else if(IMG_ERR_BAD_FORMAT == result)
      ossError << "Bad format (IMG_ERR_BAD_FORMAT)";
    else if(IMG_ERR_UNSUPPORTED == result)
      ossError << "Unsupported image type (IMG_ERR_UNSUPPORTED)";
    logEngineError(ossError.str());
    return false;
  }

  if(tgaImage.GetBPP() != 32) {
    ostringstream ossError;
    ossError << "TGA loader: 32bit support only. " << tgaImage.GetBPP() << "bit not supported.";
    logEngineError(ossError.str());
    return false;
  }
  if(tgaImage.GetWidth() != *oWidth || tgaImage.GetHeight() != *oHeight) {
    if(NULL != *oData) delete[] *oData;
    *oWidth = 0;
    *oHeight = 0;  

    *oData = new agg::int8u[tgaImage.GetWidth() * tgaImage.GetHeight() * 4];
  }
  
  *oWidth = tgaImage.GetWidth();
  *oHeight = tgaImage.GetHeight();  

  agg::int8u* srcData = tgaImage.GetImg();
  agg::int8u* dstData = *oData;
  int pixelCount = (*oWidth) * (*oHeight);
  for(int iPixel = 0; iPixel < pixelCount; ++iPixel) {
    int8u r = *srcData++;
    int8u g = *srcData++;
    int8u b = *srcData++;
    int8u a = *srcData++;
    *dstData++ = b;
    *dstData++ = g;
    *dstData++ = r;
    *dstData++ = a;
  }

  return true;
}



// --------------------------------------------------------------------------------
// PNG loading


bool
loadImagePNG(string fileName, agg::int8u** oData, int* oWidth, int* oHeight)
{
   png_structp png_ptr;
   png_infop info_ptr;
   unsigned int sig_read = 0;
   FILE *fp;

   if ((fp = fopen(fileName.c_str(), "rb")) == NULL)
      return (ERROR);

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      fclose(fp);
      return (ERROR);
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      return (ERROR);
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */

//    if (setjmp(png_jmpbuf(png_ptr)))
//    {
//       /* Free all of the memory associated with the png_ptr and info_ptr */
//       png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
//       fclose(fp);
//       /* If we get here, we had a problem reading the file */
//       return (ERROR);
//    }

   /* One of the following I/O initialization methods is REQUIRED */

   /* Set up the input control if you are using standard C streams */
   png_init_io(png_ptr, fp);

   /* If we have already read some of the signature */
   png_set_sig_bytes(png_ptr, sig_read);

   /*
    * If you have enough memory to read in the entire image at once,
    * and you need to specify only transforms that can be controlled
    * with one of the PNG_TRANSFORM_* bits (this presently excludes
    * dithering, filling, setting background, and doing gamma
    * adjustment), then you can read the entire image (including
    * pixels) into the info structure with this call:
    */
   png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY | PNG_TRANSFORM_PACKING, png_voidp_NULL);
   
   int imageWidth = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
   int imageHeight = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
   int colorType = png_get_color_type(png_ptr, info_ptr);
   if(!(colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_RGB_ALPHA)) {
     logEngineError("PNG loader: Only 24bit and 32bit images supported.");
     return false;
   }
   
   if(imageWidth != *oWidth || imageHeight != *oHeight) {
     if(NULL != *oData) delete[] *oData;
     *oWidth = 0;
     *oHeight = 0;  
     
     *oData = new agg::int8u[imageWidth * imageHeight * 4];
   }
  
   *oWidth = imageWidth;
   *oHeight = imageHeight;  
 
  agg::int8u** rowPointers = png_get_rows(png_ptr, info_ptr);
  agg::int8u* dstData = *oData;
  for(int h = 0; h < imageHeight; ++h) {
    agg::int8u* srcData = rowPointers[h];
    if(PNG_COLOR_TYPE_RGB_ALPHA == colorType) {
      for(int w = 0; w < imageWidth; ++w) {
        int8u r = *srcData++;
        int8u g = *srcData++;
        int8u b = *srcData++;
        int8u a = *srcData++;
        *dstData++ = b;
        *dstData++ = g;
        *dstData++ = r;
        *dstData++ = a;
      }
    }
    else {
      for(int w = 0; w < imageWidth; ++w) {
        int8u r = *srcData++;
        int8u g = *srcData++;
        int8u b = *srcData++;
        *dstData++ = b;
        *dstData++ = g;
        *dstData++ = r;
        *dstData++ = 0xff;
      }
    }
  }
  

   /* clean up after the read, and free any memory allocated - REQUIRED */
   png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

   /* close the file */
   fclose(fp);

   return true;
}


