//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Texture;
private import RenderSystem;
private import GUIManager;
private import Rect;

private import freetype;

void
renderText(Texture target, char[] text, float fontSize, bool shadow)
{
  if(!loadLibFreeType()) return;

  if(FT_Set_Char_Size(face, 0, cast(int)(fontSize * 64), 72, 72)) {
    logError << "Error in setting character size" << endl;
  }

  FT_GlyphSlot  slot = face.glyph;  /* a small shortcut */
  uint glyph_index;

  int pen_x = 0;
  int pen_y = cast(int)(fontSize);

   for (int n = 0; n < text.length; n++ ) {
     /* load glyph image into the slot (erase previous one) */
     if(FT_Load_Char( face, text[n], FT_LOAD_RENDER )) {
       logError << "Error loading glyph" << endl;
       continue;
     }
     
     int tx = pen_x + slot.bitmap_left;
     int ty = pen_y - slot.bitmap_top;

     // Shadow
     if(shadow) {
       int shadowX = cast(int)(3 * fontSize / 16);
       int shadowY = cast(int)(2 * fontSize / 16);

       for(int y = 0; y < slot.bitmap.rows; y++) {
         for(int x = 0; x < slot.bitmap.width; x++) {
           ubyte grey = *(slot.bitmap.buffer + x + y * slot.bitmap.pitch);
           if(grey > 0)
             target.setPixel(tx + x + shadowX, ty + y + shadowY, 0, 0, 0, cast(ubyte)(grey >> 1));
         }
       }
     }
     
     // Font
     for(int y = 0; y < slot.bitmap.rows; y++) {
       for(int x = 0; x < slot.bitmap.width; x++) {
         ubyte grey = *(slot.bitmap.buffer + x + y * slot.bitmap.pitch);
         if(grey > 0)
           target.setPixel(tx + x, ty + y, 255, 255, 255, grey);
       }
     }
     pen_x += slot.advance.x >> 6;
   }   
   target.flush();
   ubyte* data = target.mData;
}

struct FontTexture
{
  Texture texture;
  char[] string;
}


FontTexture[WidgetID] mFontTextures;

void
doText(WidgetID id, char[] text, float fontSize, Color color, int xpos, int ypos, bool shadow = true)
{
  FontTexture tex;
  // Create new texture if it doesn't exist
  if(id in mFontTextures is null) {
    tex.texture = rRenderSystem.createTexture();
    mFontTextures[id] = tex;
  }
  else {
    tex = mFontTextures[id];
  }
  // Update text if it's missing
  if(tex.string != text) {
    tex.string = text;
    // FIXME: Hardcoded, use some heuristics instead
    tex.texture.setSize(256, 32);
    renderText(tex.texture, tex.string, fontSize, shadow);
    mFontTextures[id] = tex;
  }
  
  Rect screenRect = new Rect(xpos, ypos, tex.texture.getWidth(), tex.texture.getHeight());
  Rect textureRect = new Rect(0, 0, screenRect.w, screenRect.h);
  rRenderSystem.renderTexture(tex.texture, screenRect, textureRect);
}

void
doText(WidgetID id, char[] text, int xpos, int ypos)
{
  doText(id, text, 12, Color.WHITE, xpos, ypos, false);
}
