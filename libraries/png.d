//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Platform;

// --------------------------------------------------------------------------------

extern (C)
{
  alias int __jmp_buf[6];

  struct png_struct
  {
    __jmp_buf jmpbuf;            /* used in png_error */
  }

  struct png_info
  {
  }

  typedef void (*png_error) (png_struct *, char *);
  typedef void (*png_rw) (png_struct *, byte *, int);
        
  const char* PNG_LIBPNG_VER_STRING = "1.2.8";

  //   /* These describe the color_type field in png_info. */
  //   /* color type masks */
  //   const int PNG_COLOR_MASK_PALETTE = 1;
  //   const int PNG_COLOR_MASK_COLOR = 2;
  //   const int PNG_COLOR_MASK_ALPHA = 4;

  //   /* color types.  Note that not all combinations are legal */
  //   const int PNG_COLOR_TYPE_GRAY = 0;
  //   const int PNG_COLOR_TYPE_PALETTE = (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE);
  //   const int PNG_COLOR_TYPE_RGB = (PNG_COLOR_MASK_COLOR);
  //   const int PNG_COLOR_TYPE_RGB_ALPHA = (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA);
  //   const int PNG_COLOR_TYPE_GRAY_ALPHA = (PNG_COLOR_MASK_ALPHA);

  //   /* This is for compression type. PNG 1.0-1.2 only define the std.math.single type. */
  //   const int PNG_COMPRESSION_TYPE_BASE = 0; /* Deflate method 8, 32K window */
  //   const int PNG_COMPRESSION_TYPE_DEFAULT = PNG_COMPRESSION_TYPE_BASE;

  //   /* This is for filter type. PNG 1.0-1.2 only define the std.math.single type. */
  //   const int PNG_FILTER_TYPE_BASE = 0; /* Single row per-byte filtering */
  //   const int PNG_FILTER_TYPE_DEFAULT = PNG_FILTER_TYPE_BASE;

  //   /* Supported compression types for text in PNG files (tEXt, and zTXt).
  //    * The values of the PNG_TEXT_COMPRESSION_ defines should NOT be changed. */
  //   const int PNG_TEXT_COMPRESSION_NONE_WR = -3;
  //   const int PNG_TEXT_COMPRESSION_zTXt_WR = -2;
  //   const int PNG_TEXT_COMPRESSION_NONE = -1;
  //   const int PNG_TEXT_COMPRESSION_zTXt = 0;
  //   const int PNG_ITXT_COMPRESSION_NONE = 1;
  //   const int PNG_ITXT_COMPRESSION_zTXt = 2;
  //   const int PNG_TEXT_COMPRESSION_LAST = 3;  /* Not a valid value */


  //   const int PNG_TRANSFORM_IDENTITY = 0x0000;    /* read and write */
  //   const int PNG_TRANSFORM_STRIP_16 = 0x0001;    /* read only */
  //   const int PNG_TRANSFORM_STRIP_ALPHA = 0x0002;    /* read only */
  //   const int PNG_TRANSFORM_PACKING = 0x0004;    /* read and write */
  //   const int PNG_TRANSFORM_PACKSWAP = 0x0008;    /* read and write */
  //   const int PNG_TRANSFORM_EXPAND = 0x0010;    /* read only */
  //   const int PNG_TRANSFORM_INVERT_MONO = 0x0020;    /* read and write */
  //   const int PNG_TRANSFORM_SHIFT = 0x0040;    /* read and write */
  //   const int PNG_TRANSFORM_BGR = 0x0080;    /* read and write */
  //   const int PNG_TRANSFORM_SWAP_ALPHA = 0x0100;    /* read and write */
  //   const int PNG_TRANSFORM_SWAP_ENDIAN = 0x0200;    /* read and write */
  //   const int PNG_TRANSFORM_INVERT_ALPHA = 0x0400;    /* read and write */
  //   const int PNG_TRANSFORM_STRIP_FILLER = 0x0800;    /* WRITE only */

  const int PNG_TRANSFORM_IDENTITY = 0x0000;    /* read and write */
  const int PNG_TRANSFORM_PACKING = 0x0004;    /* read and write */
  
  const int PNG_COLOR_MASK_COLOR = 2;  
  const int PNG_COLOR_MASK_ALPHA = 4;
  const int PNG_COLOR_TYPE_RGB = PNG_COLOR_MASK_COLOR;
  const int PNG_COLOR_TYPE_RGB_ALPHA = PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA;

  png_info *(*png_create_info_struct) (png_struct *png);
  png_struct *(*png_create_read_struct) (char *user_png_ver, void *error_ptr, png_error *error_fn, png_error *warn_fn);
  void (*png_destroy_read_struct) (png_struct **png, png_info **info, png_info **end);
  void *(*png_get_io_ptr) (png_struct *png);
  void (*png_set_read_fn) (png_struct *png, void *io_ptr, png_rw read_data_fn);
  void (*png_set_sig_bytes) (png_struct *png, int num_bytes);
  void (*png_read_png) (png_struct* png_ptr, png_info* info_ptr, int transforms, void* params);
  void (*png_read_info) (png_struct* png_ptr, png_info* info_ptr);
  int (*png_get_image_width) (png_struct* png_ptr, png_info* info_ptr);
  int (*png_get_image_height) (png_struct* png_ptr, png_info* info_ptr);
  ubyte (*png_get_color_type) (png_struct* png_ptr, png_info* info_ptr);
  ubyte** (*png_get_rows) (png_struct* png_ptr, png_info* info_ptr);

}


// --------------------------------------------------------------------------------
// libpng export

private void* mLibPNGHandle;
private void* mZLibHandle;

bool
loadLibPNG()
{
  if(mZLibHandle !is null && mLibPNGHandle !is null) return true;
  if(mZLibHandle is null && !rPlatform.loadLibrary("zlib1.dll", mZLibHandle)) {
    logError << "Unable to load zlib1.dll. PNG support disabled." << endl;
    return false;
  }    
  if(mLibPNGHandle is null && !rPlatform.loadLibrary("libpng13.dll", mLibPNGHandle)) {
    logError << "Unable to load libpng13.dll. PNG support disabled." << endl;
    return false;
  }
  
  load(cast(void**) &png_create_info_struct, "png_create_info_struct");
  load(cast(void**) &png_create_read_struct, "png_create_read_struct");
  load(cast(void **) &png_destroy_read_struct, "png_destroy_read_struct");
  //   load (cast(void **) &png_init_io, "png_init_io");
  load(cast(void**) &png_set_read_fn, "png_set_read_fn");
  load(cast(void**) &png_get_io_ptr, "png_get_io_ptr");
  load(cast(void**) &png_set_sig_bytes, "png_set_sig_bytes");
  load(cast(void**) &png_read_png, "png_read_png");
  load(cast(void**) &png_read_info, "png_read_info");
  load(cast(void**) &png_get_image_height, "png_get_image_height");
  load(cast(void**) &png_get_image_width, "png_get_image_width");
  load(cast(void**) &png_get_color_type, "png_get_color_type");
  load(cast(void**) &png_get_rows, "png_get_rows");
  return true;
}

void load (void** pointer, char[] name)
{
  if(!rPlatform().linkFunction(mLibPNGHandle, name, *pointer)) {
    logError << "Unable to link to function: " << name << endl;
  }    
}

