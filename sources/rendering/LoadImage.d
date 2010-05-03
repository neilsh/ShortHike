//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import std.stream;
private import std.c.string;
private import png;

// bool
// isPowerOfTwo(int size)
// {
//   const int LARGEST_POWER_OF_TWO = 1 << 28;
//   int base = 1;
//   while(base < LARGEST_POWER_OF_TWO) {
//     if(size == base) return true;
//     if(size < base) return false;
//     base = base * 2;
//   }
//   return false;
// }

// bool
// createMipMaps(int8u* originalMap, int width, int height, vector<int8u*>& oMipMaps)
// {  
//   if(!(isPowerOfTwo(width) && isPowerOfTwo(height))) {
//     ostringstream ossError;
//     ossError << "createMipMap: Only power of 2 textures supported: " << width << "x" << height;
//     logEngineError(ossError.str());
//     return false;
//   }

//   oMipMaps.push_back(originalMap);
  
//   int prevWidth = width;
//   int prevHeight = height;
//   int8u* prevMap = originalMap;
//   while(prevWidth > 1 && prevHeight > 1) {
//     int currWidth = prevWidth / 2;
//     int currHeight = prevHeight / 2;
//     int8u* currMap = new int8u[currWidth * currHeight * 4];
//     oMipMaps.push_back(currMap);
//     int8u* iMap = currMap;
//     int8u* ul = prevMap;
//     int8u* ur = prevMap + 4;
//     int8u* ll = prevMap + (prevWidth * 4);
//     int8u* lr = prevMap + (prevWidth * 4) + 4;

//     for(int y = 0; y < currHeight; y++) {
//       for(int x = 0; x < currWidth; x++) {
//         *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
//         *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
//         *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
//         *iMap++ = static_cast<int8u>((static_cast<int>(*ul++) + static_cast<int>(*ur++) + static_cast<int>(*ll++) + static_cast<int>(*lr++)) / 4);
//         ul += 4;
//         ur += 4;
//         ll += 4;
//         lr += 4;
//       }
//       ul += prevWidth * 4;
//       ur += prevWidth * 4;
//       ll += prevWidth * 4;
//       lr += prevWidth * 4;
//     }

//     prevWidth = currWidth;
//     prevHeight = currHeight;
//     prevMap = currMap;
//   }
//   return true;
// }

// --------------------------------------------------------------------------------
// PNG loading




extern (C) 
{  
  void pngErrorHandler(png_struct* png_ptr, char* data) {
    char[] message = data[0 .. strlen(data)];
    logError << "PNG error: " << message << endl;
  }

  void pngWarningHandler(png_struct* png_ptr, char* data) {
    char[] message = data[0 .. strlen(data)];
    logWarn << "PNG warning: " << message << endl;
  }
  
  
  void read_data (png_struct *png, byte *data, int leng) {
    Stream stream;    
    stream = cast(Stream) png_get_io_ptr (png);
    stream.readExact(data, leng);
  }

  int setjmp (void* __env);
}



bool
loadImagePNG(char[] fileName, out ubyte[] oData, out int oWidth, out int oHeight)
{
  if(!loadLibPNG()) {
    logError << "PNG loading disabled." << endl;
    return false;
  }
  
  
  png_struct* png_ptr;
  png_info* info_ptr;
  uint sig_read = 0;

  File stream = new File(fileName, FileMode.In);
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, null, cast(png_error*)&pngErrorHandler, cast(png_error*)&pngWarningHandler);


  // Longjmp error handling target
  if(setjmp(png_ptr.jmpbuf)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, null);    
    oData.length = 0;
    oWidth = 0;
    oHeight = 0;
    return false;
  }
  

  if (png_ptr is null) {
    return false;
  }
   
  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == null) {
    png_destroy_read_struct(&png_ptr, null, null);
    return false;
  }

  png_set_read_fn (png_ptr, cast(void *) stream, &read_data);

  png_set_sig_bytes(png_ptr, sig_read);
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY | PNG_TRANSFORM_PACKING, null);
   
  int imageWidth = cast(int)png_get_image_width(png_ptr, info_ptr);
  int imageHeight = cast(int)png_get_image_height(png_ptr, info_ptr);
  int colorType = png_get_color_type(png_ptr, info_ptr);
  if(!(colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_RGB_ALPHA)) {
    logError << "PNG loader: Only 24bit and 32bit images supported.";
    return false;
  }
   
  oData.length = imageWidth * imageHeight * 4;
  oWidth = imageWidth;
  oHeight = imageHeight;  
 
  ubyte** rowPointers = png_get_rows(png_ptr, info_ptr);
  ubyte* dstData = oData.ptr;
  for(int h = 0; h < imageHeight; ++h) {
    ubyte* srcData = rowPointers[h];
    if(PNG_COLOR_TYPE_RGB_ALPHA == colorType) {
      for(int w = 0; w < imageWidth; ++w) {
        ubyte r = *srcData++;
        ubyte g = *srcData++;
        ubyte b = *srcData++;
        ubyte a = *srcData++;
        *dstData++ = b;
        *dstData++ = g;
        *dstData++ = r;
        *dstData++ = a;
      }
    }
    else {
      for(int w = 0; w < imageWidth; ++w) {
        ubyte r = *srcData++;
        ubyte g = *srcData++;
        ubyte b = *srcData++;
        *dstData++ = b;
        *dstData++ = g;
        *dstData++ = r;
        *dstData++ = 0xff;
      }
    }
  }
  

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, null);

  return true;
}

