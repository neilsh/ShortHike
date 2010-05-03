//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Label.hpp"
#include "Graphics.hpp"
#include "GUIManager.hpp"

//--------------------------------------------------------------------------------

const int MAGIC_HORIZONTAL_PADDING = 5;
const int MAGIC_VERTICAL_PADDING = 2;
const int MAGIC_MINIMUM_FONT_SIZE = 6;

//--------------------------------------------------------------------------------


Label::Label() :
  Widget()
{
  init();
}


Label::Label(std::wstring label, bool b) :
  Widget()
{
  init();
  hasBorder = b;
  setLabel(label);
}


Label::Label(int label, bool b) :
  Widget()
{
  init();
  hasBorder = b;
  setLabel(label);
}


Label::Label(wstring label, float _fontSize, bool _hasBorder)
{
  init();
  setLabel(label);
  setFontSize(_fontSize);
  setBorder(_hasBorder);
}

//--------------------------------------------------------------------------------

void
Label::init()
{
  fontSize = 14;
  textColor = ColorType(255, 255, 255);
  verticalAlignment = CENTER_VERTICAL;
  horizontalAlignment = LEFT;
  setLabel(L"");
  hasBorder = false;
}



Label::~Label()
{
}



wstring
Label::getLabel()
{
  return myLabel;
}

//--------------------------------------------------------------------------------

void
Label::setLabel(int num)
{
  if(num == 0) {
    setLabel(L"-");
  }
  else {
    wostringstream o;
    o << num;
    setLabel(o.str());
  }  
}

void
Label::setLabel(wstring newLabel)
{
  if(myLabel == newLabel) return;
  myLabel = newLabel;
  updatePreferredSize();
}



//--------------------------------------------------------------------------------

void
Label::setBorder(bool b)
{
  hasBorder = b;
  markDirty();
}

void
Label::setVerticalAlignment(VerticalAlignment newAlignment)
{
  verticalAlignment = newAlignment;
  markDirty();
}

void
Label::setHorizontalAlignment(HorizontalAlignment newAlignment)
{
  horizontalAlignment = newAlignment;
  markDirty();
}

//--------------------------------------------------------------------------------

void
Label::setFontSize(float _fontSize)
{
  fontSize = _fontSize;
  updatePreferredSize();
}


void
Label::setColor(ColorType newColor)
{
  textColor = newColor;
  markDirty();
}


void
Label::setColor(float r, float g, float b)
{
  setColor(ColorType(r * 255, g * 255, b * 255));
  markDirty();
}


//--------------------------------------------------------------------------------

void
Label::updatePreferredSize()
{
  Dimension stringDimension = rGUIManager()->getStringDimension(fontSize, myLabel);
  stringDimension.w += MAGIC_HORIZONTAL_PADDING * 2;
  stringDimension.h += MAGIC_VERTICAL_PADDING * 2;
  setPreferredSize(stringDimension);
  markDirty();
}

//--------------------------------------------------------------------------------

void
Label::paint(Graphics* graphicsContext)
{  
  if(hasBorder) {
    GRect r;
    r.setSize(getSize());
    graphicsContext->setColor(0.0f, 0.8f, 0.0f);
    graphicsContext->drawHighVisibilityRect(r);
  }  


  Dimension mySize = getSize();
  // Scale font to height
  int textLines = count(myLabel.begin(), myLabel.end(), '\n') + 1;
  int paintHeight = mySize.h;
  FontSize paintFontSize = fontSize;
  if((textLines * fontSize) > paintHeight)
    paintFontSize = (paintHeight / textLines);
  if(paintFontSize < MAGIC_MINIMUM_FONT_SIZE) paintFontSize = MAGIC_MINIMUM_FONT_SIZE;

  int verticalPadding = MAGIC_VERTICAL_PADDING;
  int horizontalPadding = MAGIC_HORIZONTAL_PADDING;

  graphicsContext->setColor(textColor);
  graphicsContext->setFontSize(paintFontSize);
  
  GRect clip = graphicsContext->getClipRectangle();
  int xPosition;
  int yPosition;

  if(verticalAlignment == TOP) {
    yPosition = verticalPadding;
  }
  else if(verticalAlignment == BOTTOM) {
    yPosition = (clip.h - mySize.h) - verticalPadding;
  }
  else {
    yPosition = (clip.h - mySize.h) / 2;
  }  

  wstring line;
  unsigned leftoverIndex = 0;
  unsigned linebreakIndex;
  while(leftoverIndex < myLabel.size()
        && (linebreakIndex = myLabel.find('\n', leftoverIndex)) != wstring::npos) {
    line = myLabel.substr(leftoverIndex, (linebreakIndex - leftoverIndex));
    Dimension lineDimension = graphicsContext->getStringDimension(line);
    
    if(horizontalAlignment == LEFT) {
      xPosition = horizontalPadding;
    }
    else if(horizontalAlignment == RIGHT) {
      xPosition = (clip.w - lineDimension.w) - horizontalPadding;
    }
    else {
      xPosition = (clip.w - lineDimension.w) / 2;
    }
    
    graphicsContext->drawString(line, xPosition, yPosition);
    yPosition += paintFontSize;
    leftoverIndex = linebreakIndex + 1;
  }

  if(leftoverIndex < myLabel.size()) {
    line = myLabel.substr(leftoverIndex, (myLabel.size() - leftoverIndex));
    Dimension lineDimension = graphicsContext->getStringDimension(line);
    
    if(horizontalAlignment == LEFT) {
      xPosition = horizontalPadding;
    }
    else if(horizontalAlignment == RIGHT) {
      xPosition = (clip.w - lineDimension.w) - horizontalPadding;
    }
    else {
      xPosition = (clip.w - lineDimension.w) / 2;
    }
    
    graphicsContext->drawString(line, xPosition, yPosition);
  }
}



