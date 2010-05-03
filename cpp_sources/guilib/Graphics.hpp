//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//

#pragma once
#ifndef GUILIB_GRAPHICS_HPP
#define GUILIB_GRAPHICS_HPP

#include "GRect.hpp"

class Frame;
class Surface;

class Graphics
{
  friend class TestGUILib;
public:
  Graphics(Surface* surface, const GRect& clip);
//   Graphics(const PixelBox& pixelBox, const GRect& clip);
  Graphics* createSubContext(int x, int y, int w, int h);
  Graphics* createSubContext(GRect r);
  
  void setClipRectangle(GRect newClip) {clip = newClip;}
  GRect getClipRectangle() {return clip;}
  
  void setColor(float r, float g, float b, float a = 1.0);
  void setColor(agg::int8u r, agg::int8u g, agg::int8u b, agg::int8u a = 255);
  void setColor(ColorType color);
  ColorType getColor() const;

  void fillRect(GRect r);
  void drawRect(GRect r);
  void drawHighVisibilityRect(int x, int y, int w, int h);
  void drawHighVisibilityRect(GRect r);
  void fillRect(int x, int y, int w, int h);
  void drawRect(int x, int y, int w, int h);

  void drawRoundedRect(float x, float y, float w, float h, float radius = 3);
  void fillRoundedRect(float x, float y, float w, float h, float radius = 3);
  void drawBevelRect(float x, float y, float w, float h, float radius = 3);

  void fillEllipse(float x, float y, float rx, float ry);
  void markerPixel(float x, float y);

  void clear();

  void setFontSize(float fontSize);
  float getFontSize();
  void drawString(wstring, int x, int y);
  void drawString(wstring);

  void drawSurface(Surface* surface, float x, float y);  
  Dimension getStringDimension(wstring text);


  friend std::ostream& operator<<(std::ostream&, const Graphics&);

  //--------------------------------------------------------------------------------
  // Anti-Grain types

  typedef agg::pixfmt_bgra32 PixelFormatType;
  typedef agg::renderer_base<PixelFormatType> BaseRendererType;
  typedef agg::renderer_scanline_aa_solid<BaseRendererType> SolidRendererType;
  typedef agg::renderer_primitives<BaseRendererType> PrimitiveRendererType;
  typedef agg::renderer_markers<BaseRendererType> MarkerRendererType;

private:
  Surface* mSurface;

  agg::rendering_buffer renderingBuffer;
  PixelFormatType pixelBuffer;
  BaseRendererType baseRenderer;
  SolidRendererType solidRenderer;  
  PrimitiveRendererType primitiveRenderer;
  MarkerRendererType markerRenderer;

  agg::rasterizer_scanline_aa<> scanlineRasterizer;
  agg::scanline_u8 scanlineBuffer;

  int caretX;
  int caretY;
  int lineStartX;
  float fontSize;

  GRect clip;
  agg::rgba8 colorComposite;
};

#endif
