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
// Enums and constants

extern (C)
{
  alias int FT_Error;
  alias int FT_Long;
  alias uint FT_ULong;
  alias int FT_Int32;
  alias char FT_String;
  alias uint FT_UInt;
  alias int FT_Int;
  alias ushort FT_UShort;
  alias short FT_Short;
  alias int FT_Pos;
  alias int FT_Fixed;

  alias void* FT_SubGlyph;

  const int FT_LOAD_RENDER = 0x4;
}

// --------------------------------------------------------------------------------
// Structs

extern (C)
{
  alias void* FT_Library;

  struct FT_CharMapRec {
  }
  alias FT_CharMapRec* FT_CharMap;

  struct FT_Bitmap_Size {
  }  

  alias void* FT_Generic_Finalizer;

  struct FT_Generic {
    void* data;
    FT_Generic_Finalizer  finalizer;
  }  

  struct FT_BBox {
    FT_Pos  xMin, yMin;
    FT_Pos  xMax, yMax;
  }

  struct FT_Vector
  {
    FT_Pos  x;
    FT_Pos  y;
  }


  struct FT_Glyph_Metrics
  {
    FT_Pos  width;
    FT_Pos  height;

    FT_Pos  horiBearingX;
    FT_Pos  horiBearingY;
    FT_Pos  horiAdvance;

    FT_Pos  vertBearingX;
    FT_Pos  vertBearingY;
    FT_Pos  vertAdvance;
  }


  struct FT_SizeRec {
  }
  alias FT_SizeRec* FT_Size;

  enum FT_Glyph_Format {
    INVALID
  }
  
  struct FT_Outline {
    short n_contours;      /* number of contours in glyph        */
    short n_points;        /* number of points in the glyph      */

    FT_Vector*  points;          /* the outline's points               */
    char* tags;            /* the points flags                   */
    short* contours;        /* the contour end points             */

    int flags;           /* outline masks                      */
  }
  

  struct FT_GlyphSlotRec {
    FT_Library        library;
    FT_Face           face;
    FT_GlyphSlot      next;
    FT_UInt           reserved;       /* retained for binary compatibility */
    FT_Generic        generic;

    FT_Glyph_Metrics  metrics;
    FT_Fixed          linearHoriAdvance;
    FT_Fixed          linearVertAdvance;
    FT_Vector         advance;

    FT_Glyph_Format   format;

    FT_Bitmap         bitmap;
    FT_Int            bitmap_left;
    FT_Int            bitmap_top;

    FT_Outline        outline;

    FT_UInt           num_subglyphs;
    FT_SubGlyph       subglyphs;

    void*             control_data;
    FT_Long           control_len;

    FT_Pos            lsb_delta;
    FT_Pos            rsb_delta;

    void*             other;
  }
  alias FT_GlyphSlotRec* FT_GlyphSlot;

  struct FT_Bitmap
  {
    int             rows;
    int             width;
    int             pitch;
    char*  buffer;
    short           num_grays;
    char            pixel_mode;
    char            palette_mode;
    void*           palette;
  };


  // Only exporting public part
  struct FT_FaceRec {
    FT_Long           num_faces;
    FT_Long           face_index;

    FT_Long           face_flags;
    FT_Long           style_flags;

    FT_Long           num_glyphs;

    FT_String*        family_name;
    FT_String*        style_name;

    FT_Int            num_fixed_sizes;
    FT_Bitmap_Size*   available_sizes;

    FT_Int            num_charmaps;
    FT_CharMap*       charmaps;

    FT_Generic        generic;

    /*# the following are only relevant to scalable outlines */
    FT_BBox           bbox;

    FT_UShort         units_per_EM;
    FT_Short          ascender;
    FT_Short          descender;
    FT_Short          height;

    FT_Short          max_advance_width;
    FT_Short          max_advance_height;

    FT_Short          underline_position;
    FT_Short          underline_thickness;

    FT_GlyphSlot      glyph;
    FT_Size           size;
    FT_CharMap        charmap;
  }
  alias FT_FaceRec* FT_Face;

}

// --------------------------------------------------------------------------------
// Functions

extern (C)
{
  FT_Error (*FT_Init_FreeType)(FT_Library* library);
  FT_Error (*FT_New_Face)(FT_Library library, char* fileName, FT_Long face_index, FT_Face* aface);
  FT_Error (*FT_Set_Char_Size) (FT_Face face, int char_width, int char_height, uint horz_resolution, uint vert_resolution);
  FT_Error (*FT_Load_Char) (FT_Face face, FT_ULong  char_code, FT_Int32 load_flags);
}


// --------------------------------------------------------------------------------

private void* mLibFreeTypeHandle;

// ----------------------------------------
// Initialize library
FT_Library library;
FT_Face face;
 
bool
loadLibFreeType()
{
  if(mLibFreeTypeHandle !is null) return true;
  if(mLibFreeTypeHandle is null && !rPlatform.loadLibrary("freetype.dll", mLibFreeTypeHandle)) {
    logError << "Unable to load freetype.dll. Font support disabled." << endl;
    return false;
  }    
  
  // ----------------------------------------
  // Link functions
  load(cast(void**) &FT_Init_FreeType, "FT_Init_FreeType");
  load(cast(void**) &FT_New_Face, "FT_New_Face");
  load(cast(void**) &FT_Set_Char_Size, "FT_Set_Char_Size");
  load(cast(void**) &FT_Load_Char, "FT_Load_Char");

  
  int error = FT_Init_FreeType(&library);
  if (error) {
    logError << "Unable to initialize FreeType" << endl;
  }

  error = FT_New_Face(library, toStringz("assets/gui/tahoma.ttf"), 0, &face );
  if ( error ) {
    logError << "Unable to load file" << endl;
//     ... another error code means that the font file could not
//     ... be opened or read, or simply that it is broken...
  }

  return true;
}

void load (void** pointer, char[] name)
{
  if(!rPlatform().linkFunction(mLibFreeTypeHandle, name, *pointer)) {
    logError << "Unable to link to function: " << name << endl;
  }    
}

