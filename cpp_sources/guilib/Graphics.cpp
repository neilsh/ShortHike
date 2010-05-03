// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com

#include "Common.hpp"

#include "Graphics.hpp"
#include "GUIManager.hpp"
#include "rendering/Surface.hpp"

//--------------------------------------------------------------------------------




Graphics::Graphics(Surface* surface, const GRect& _clip)
  : mSurface(surface), clip(_clip), fontSize(10),
    renderingBuffer(static_cast<agg::int8u*>(mSurface->getData()), mSurface->getWidth(), mSurface->getHeight(), mSurface->getWidth() * 4),
    pixelBuffer(renderingBuffer),
    baseRenderer(pixelBuffer),
    solidRenderer(baseRenderer),
    primitiveRenderer(baseRenderer),
    markerRenderer(baseRenderer)
{
  baseRenderer.clip_box(clip.x, clip.y, clip.x + clip.w - 1, clip.y + clip.h - 1);
  lineStartX = 0;
  caretX = 0;
  caretY = fontSize;
}

// Graphics::Graphics(const PixelBox& _pixelBox, const GRect& _clip)
//   : mSurface(NULL), pixelBox(_pixelBox), clip(_clip), fontSize(10),
//     renderingBuffer(static_cast<agg::int8u*>(pixelBox.data), pixelBox.getWidth(), pixelBox.getHeight(), pixelBox.rowPitch * 4),
//     pixelBuffer(renderingBuffer),
//     baseRenderer(pixelBuffer),
//     solidRenderer(baseRenderer),
//     primitiveRenderer(baseRenderer),
//     markerRenderer(baseRenderer)  
// {
//   baseRenderer.clip_box(clip.x, clip.y, clip.x + clip.w - 1, clip.y + clip.h - 1);
//   lineStartX = 0;
//   caretX = 0;
//   caretY = fontSize;
// }


Graphics*
Graphics::createSubContext(int x, int y, int w, int h)
{
  return createSubContext(GRect(x, y, w, h));
}


Graphics*
Graphics::createSubContext(GRect newClip)
{
  newClip.translate(clip.x, clip.y);
  GRect drawArea = intersection(newClip, clip);
  if(drawArea.w == 0 || drawArea.h == 0) return NULL;
  
  if(NULL == mSurface) return NULL;

  return new Graphics(mSurface, newClip);
}


//--------------------------------------------------------------------------------
// Color management

void
Graphics::setColor(float red, float green, float blue, float alpha)
{
  red = clamp(red, 0.0f, 1.0f);
  green = clamp(green, 0.0f, 1.0f);
  blue = clamp(blue, 0.0f, 1.0f);
  alpha = clamp(alpha, 0.0f, 1.0f);

  colorComposite = agg::rgba8((int)(red * 255.0f),  (int)(green * 255.0f), (int)(blue * 255.0f), (int)(alpha * 255.0f));
}


void
Graphics::setColor(agg::int8u r, agg::int8u g, agg::int8u b, agg::int8u a)
{
  colorComposite = agg::rgba8(r, g, b, a);
}

void
Graphics::setColor(ColorType color)
{
  colorComposite = color;
}

ColorType
Graphics::getColor() const
{
  return colorComposite;
}


//--------------------------------------------------------------------------------
// High level drawing primitives

void
Graphics::fillRect(GRect r)
{
  fillRect(r.x, r.y, r.w, r.h);
}

void
Graphics::drawRect(GRect r)
{
  drawRect(r.x, r.y, r.w, r.h);
}


void
Graphics::drawRect(int x, int y, int w, int h)
{
  primitiveRenderer.line_color(colorComposite);
  primitiveRenderer.rectangle(clip.x + x, clip.y + y, clip.x + x + w, clip.y + y + h);
}

void
Graphics::fillRect(int x, int y, int w, int h)
{
  primitiveRenderer.fill_color(colorComposite);
  primitiveRenderer.solid_rectangle(clip.x + x, clip.y + y, clip.x + x + w, clip.y + y+ h);
}

//--------------------------------------------------------------------------------
// High visibility rectangles

void
Graphics::drawHighVisibilityRect(int x, int y, int w, int h)
{
  agg::rgba8 oldColor = colorComposite;
  for(int edgeDistance = 0; edgeDistance < 3; ++edgeDistance) {
    if((edgeDistance % 2) == 0)
      setColor(0.0f, 0.0f, 0.0f);
    else
      setColor(1.0f, 1.0f, 1.0f);
    drawRect(x + edgeDistance, y + edgeDistance, w - (edgeDistance * 2), h - (edgeDistance * 2));
  }
  colorComposite = oldColor;
}

void
Graphics::drawHighVisibilityRect(GRect r)
{
  drawHighVisibilityRect(r.x, r.y, r.w, r.h);
}

//--------------------------------------------------------------------------------
// Rounded rectangles

void
Graphics::drawRoundedRect(float x, float y, float w, float h, float radius)
{
  // Creating a rounded rectangle
  agg::rounded_rect r(clip.x + x + 0.5f, clip.y + y + 0.5f, clip.x + x + w - 0.5f, clip.y + y + h - 0.5f, radius);
  r.normalize_radius();
  
  // Drawing as an outline
  agg::conv_stroke<agg::rounded_rect> rectanglePath(r);
  rectanglePath.width(1.0);
  scanlineRasterizer.reset();
  scanlineRasterizer.add_path(rectanglePath);
  solidRenderer.color(colorComposite);
  agg::render_scanlines(scanlineRasterizer, scanlineBuffer, solidRenderer);
}

void
Graphics::fillRoundedRect(float x, float y, float w, float h, float radius)
{
  // Creating a rounded rectangle
  agg::rounded_rect r(clip.x + x + 0.5f, clip.y + y + 0.5f, clip.x + x + w - 0.5f, clip.y + y + h - 0.5f, radius);
  r.normalize_radius();
  
  scanlineRasterizer.reset();
  scanlineRasterizer.add_path(r);
  solidRenderer.color(colorComposite);
  agg::render_scanlines(scanlineRasterizer, scanlineBuffer, solidRenderer);
}


void
Graphics::drawBevelRect(float x, float y, float width, float height, float radius)
{
  setColor(rGUIManager()->getSkinColor(GUIManager::SKIN_BEVEL_LIGHT_COLOR));
  drawRoundedRect(x, y, width, height);
  drawRoundedRect(x, y, width - 1, height - 1);
  setColor(GUIManager::getSkinColor(GUIManager::SKIN_BEVEL_DARK_COLOR));
  drawRoundedRect(x + 1, y + 1, width - 1, height - 1);
}

//--------------------------------------------------------------------------------
// Ellipses

void
Graphics::fillEllipse(float x, float y, float rx, float ry)
{
  primitiveRenderer.fill_color(colorComposite);
  primitiveRenderer.solid_ellipse(clip.x + x, clip.y + y, rx, ry);
}


//--------------------------------------------------------------------------------
// Markers

void
Graphics::markerPixel(float x, float y)
{
  markerRenderer.fill_color(colorComposite);
  markerRenderer.line_color(colorComposite);
  markerRenderer.pixel(clip.x + x, clip.y + y, 1);
}


//--------------------------------------------------------------------------------
// Text and font management

void
Graphics::setFontSize(float _fontSize)
{
  fontSize = _fontSize;
}


float
Graphics::getFontSize()
{
  return fontSize;
}


void
Graphics::drawString(wstring text, int x, int y)
{
  lineStartX = x;
  caretX = lineStartX;
  caretY = y + fontSize;
  drawString(text);
}



void
Graphics::drawString(wstring text)
{  
  FontEngineType& fontEngine = rGUIManager()->getFontEngine();
  fontEngine.height(fontSize);
  fontEngine.width(fontSize);

  FontManagerType& fontManager = rGUIManager()->getFontManager();

  double x  = caretX + clip.x;
  double y0 = caretY + clip.y;
  double y  = y0;

  for(wstring::const_iterator charI = text.begin(); charI != text.end(); ++charI) {
    const agg::glyph_cache* glyph = fontManager.glyph(*charI);
    if(glyph) {
      fontManager.add_kerning(&x, &y);
      fontManager.init_embedded_adaptors(glyph, x, y);
      
      solidRenderer.color(colorComposite);
      agg::render_scanlines(fontManager.gray8_adaptor(),
                            fontManager.gray8_scanline(),
                            solidRenderer);
      
      // increment pen position
      x += glyph->advance_x;
      y += glyph->advance_y;
    }
  }
}


Dimension
Graphics::getStringDimension(wstring text)
{
  return rGUIManager()->getStringDimension(fontSize, text);
}


//--------------------------------------------------------------------------------
// Image drawing

void
Graphics::drawSurface(Surface* surface, float xpos, float ypos)
{
  xpos += clip.x;
  ypos += clip.y;

  for(int yspan = 0; yspan < surface->getHeight(); ++yspan) {
    baseRenderer.blend_color_hspan(xpos, ypos + yspan, surface->getWidth(),
                                   static_cast<agg::rgba8*>(surface->getData()) + yspan * surface->getWidth(), NULL);
  }
}



//--------------------------------------------------------------------------------
// Clearing

void
Graphics::clear()
{
  baseRenderer.clear(agg::rgba(0,0,0,0));
}

