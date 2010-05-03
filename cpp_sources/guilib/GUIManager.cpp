//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "GUIManager.hpp"
#include "Label.hpp"
#include "FlowLayout.hpp"
#include "RasterImageButton.hpp"
#include "UserEventListener.hpp"
#include "Dialog.hpp"
#include "rendering/RenderSystem.hpp"


//--------------------------------------------------------------------------------
// Singleton implementation

GUIManager*
rGUIManager()
{
  static GUIManager* mGUIManager = new GUIManager();
  return mGUIManager;
}


//--------------------------------------------------------------------------------

// FIXME: should first check if anyone eats the button event before doing clicks.
const Time CLICK_LIMIT = 0.3f;
const Time MULTIPLE_CLICK_LIMIT = 0.5f;

GUIManager::GUIManager()
  : fontEngine(),
    fontManager(fontEngine)
{
  rootContainer = new Widget();
  rootContainer->setPosition(0, 0);
  rootContainer->setSize(getScreenWidth(), getScreenHeight());

  // reset the counters to values that will not fire any events at startup
  for(unsigned int iButton = 0; iButton < ARBITRARY_SUPPORTED_BUTTONS; ++iButton) {
    elapsedSinceRelease[iButton] = 2 * MULTIPLE_CLICK_LIMIT;
    elapsedSincePress[iButton] = 2* CLICK_LIMIT;    
  }

  
  CHECK("Font loading successfull",
        fontEngine.load_font("assets/user_interface/tahoma.ttf", 0, agg::glyph_ren_agg_gray8) == true);

  fontEngine.hinting(true);
  fontEngine.flip_y(true);
}




void
GUIManager::render(RenderSystem* renderSystem)
{
  for(list<Widget*>::const_iterator iLayer = mLayers.begin(); iLayer != mLayers.end(); ++iLayer) {
    Widget* layer = *iLayer;
    {
      Prof(render_renderAll);
      layer->renderAll(renderSystem, 0, 0);
    }
    {
      Prof(render_paintAll);
      layer->paintAll(NULL);
    }
  }
}

//--------------------------------------------------------------------------------

int
GUIManager::getScreenWidth()
{
  return rRenderSystem()->getWidth();
}


int
GUIManager::getScreenHeight()
{
  return rRenderSystem()->getHeight();
}

Dimension
GUIManager::getScreenDimension()
{
  return Dimension(getScreenWidth(), getScreenHeight());
}


//--------------------------------------------------------------------------------

Point
GUIManager::getCursorPosition()
{
  return Point(cursorX, cursorY);
}


int
GUIManager::getCursorX()
{
  return cursorX;
}

int
GUIManager::getCursorY()
{
  return cursorY;
}

//--------------------------------------------------------------------------------
// Utility
  

Vector
GUIManager::getRelativeCursorPosition(int x, int y)
{
  Real relativeX = static_cast<Real>(x) / getScreenWidth();
  Real relativeY = static_cast<Real>(y) / getScreenHeight();
  return Vector(relativeX, relativeY);
}


Vector
GUIManager::getNormalizedCursorPosition(int x, int y)
{
  Real relativeX = static_cast<Real>(x) / getScreenWidth();
  Real relativeY = static_cast<Real>(y) / getScreenWidth();
  return Vector(relativeX * 2.0f - 1.0f, relativeY * 2.0f - 1.0f);
}


Point
GUIManager::getScreenPosition(const Vector& position) 
{ 
  // FIXME:
//   Vector hcsPosition = defaultCamera->getProjectionMatrix() * (defaultCamera->getViewMatrix() * position);
//   float normalizedX = 1.0f - ((hcsPosition.x * 0.5f) + 0.5f);
//   float normalizedY = ((hcsPosition.y * 0.5f) + 0.5f);
//   return Point(normalizedX * getScreenWidth(), normalizedY * getScreenHeight());
  CHECK_FAIL("Not implemented");
  return Point(0, 0);
}


bool
GUIManager::isPositionOnScreen(const Vector& position)
{ 
  Point screenPos = getScreenPosition(position);
  GRect screenBounds;
  screenBounds.setSize(getScreenDimension());
  return screenBounds.contains(screenPos);
}

//--------------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
KeyModifier
GUIManager::getKeyModifier()
{
  KeyModifier modifier = 0;
  if(0 > GetKeyState(VK_SHIFT)) modifier = modifier | KEY_MODIFIER_SHIFT;
  if(0 > GetKeyState(VK_LSHIFT)) modifier = modifier | KEY_MODIFIER_LSHIFT;
  if(0 > GetKeyState(VK_RSHIFT)) modifier = modifier | KEY_MODIFIER_RSHIFT;
  if(0 > GetKeyState(VK_MENU)) modifier = modifier | KEY_MODIFIER_ALT;
  if(0 > GetKeyState(VK_LMENU)) modifier = modifier | KEY_MODIFIER_LALT;
  if(0 > GetKeyState(VK_RMENU)) modifier = modifier | KEY_MODIFIER_RALT;
  if(0 > GetKeyState(VK_CONTROL)) modifier = modifier | KEY_MODIFIER_CONTROL;
  if(0 > GetKeyState(VK_LCONTROL)) modifier = modifier | KEY_MODIFIER_LCONTROL;
  if(0 > GetKeyState(VK_RCONTROL)) modifier = modifier | KEY_MODIFIER_RCONTROL;
  return modifier;
}
#endif

//--------------------------------------------------------------------------------
// Font access

FontEngineType&
GUIManager::getFontEngine()
{
  return fontEngine;
}

FontManagerType&
GUIManager::getFontManager()
{
  return fontManager;
}


Dimension
GUIManager::getStringDimension(FontSize size, wstring text)
{
  fontEngine.height(size);
  fontEngine.width(size);
  
  double x  = 0;
  double y  = 0;
  GRect textBounds;
  
  for(wstring::const_iterator charI = text.begin(); charI != text.end(); ++charI) {
    const agg::glyph_cache* glyph = fontManager.glyph(*charI);
    
    if(glyph) {
      if(glyph->bounds.is_valid()) {
        textBounds.addPoint(x + glyph->bounds.x1, y + glyph->bounds.y1);
        textBounds.addPoint(x + glyph->bounds.x2, y + glyph->bounds.y2);
      }
      x += glyph->advance_x;
      y += glyph->advance_y;
    }
  }

  // Use hardcoded height
  return Dimension(textBounds.w, size);
}

int
GUIManager::getFontHeight(FontSize size)
{
  return size;
}



//--------------------------------------------------------------------------------


Widget*
GUIManager::getRootContainer()
{
  return rootContainer;
}


//--------------------------------------------------------------------------------

void
GUIManager::clearLayers()
{
  mLayers.clear();
  rootContainer->removeAll();
}


void
GUIManager::addLayer(Widget* layer)
{
  mLayers.push_back(layer);
  rootContainer->add(layer);
  layer->setPosition(0, 0);
  layer->setSize(rootContainer->getSize());
}






//--------------------------------------------------------------------------------
// Central repository for important color information

ColorType
GUIManager::getSkinColor(SkinColor colorType)
{
  if(colorType == SKIN_BUTTON_DISABLED_COLOR)
    return ColorType(76, 88, 109, 185);
  if(colorType == SKIN_BUTTON_ENABLED_COLOR)
    return ColorType(76, 88, 109, 185);
  if(colorType == SKIN_BUTTON_MOUSEOVER_COLOR)
    return ColorType(0, 57, 157, 185);
  if(colorType == SKIN_BUTTON_OUTLINE_COLOR)
    return ColorType(180, 180, 180, 185);


  //------------------------------------------------------------

  if(colorType == SKIN_FRAME_COLOR)
    return ColorType(67, 83, 86, 200);
  if(colorType == SKIN_FRAME_COLOR_DENSE)
    return ColorType(67, 83, 86, 245);

  if(colorType == SKIN_FRAME_OUTLINE_COLOR)
    return ColorType(180, 180, 180, 200);

  if(colorType == SKIN_BEVEL_LIGHT_COLOR)
    return ColorType(180, 180, 180, 230);
  if(colorType == SKIN_BEVEL_DARK_COLOR)
    return ColorType(100, 104, 108, 230);

  if(colorType == SKIN_PROGRESSBAR_COLOR) {
    return ColorType(0, 57, 157, 200);
  }
  
  return ColorType(1, 1, 1);
}


//--------------------------------------------------------------------------------

void
GUIManager::injectKeyboardEvent(char keyCode, bool pressed)
{
  for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
    Widget* layer = *irLayer;
    if(layer->handleKeyboardEvent(keyCode, pressed)) break;
  }  
}

void
GUIManager::injectCharacterEvent(wchar_t character)
{
  for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
    Widget* layer = *irLayer;
    if(layer->handleCharacterEvent(character)) break;
  }  
}



void
GUIManager::injectMouseButtonEvent(int buttonID, bool pressed)
{
  int xpos = static_cast<int>(cursorX);
  int ypos = static_cast<int>(cursorY);

  CHECK("Button count within number of supported button", buttonID >= 0 && buttonID < ARBITRARY_SUPPORTED_BUTTONS);
  if(pressed) {
    elapsedSincePress[buttonID] = 0;
  }
  else {
    if(elapsedSincePress[buttonID] < CLICK_LIMIT) {
      if(elapsedSinceRelease[buttonID] < MULTIPLE_CLICK_LIMIT) {
        clickCount[buttonID]++;
      }
      else {
        clickCount[buttonID] = 1;
      }

      for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
        Widget* layer = *irLayer;
        if(layer->handleMouseClickEvent(buttonID, clickCount[buttonID], xpos, ypos)) break;
      }

      elapsedSinceRelease[buttonID] = 0;
    } 
  }
  
  // This is the updated layer functionality
  for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
    Widget* layer = *irLayer;
    if(layer->handleMouseButtonEvent(buttonID, pressed, xpos, ypos)) break;
  }
}


void
GUIManager::injectMouseMotionEvent(float newX, float newY, float deltaWheel)
{
  float deltaX = newX - cursorX;
  float deltaY = newY - cursorY;
  cursorX = newX;
  cursorY = newY;
  
  for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
    Widget* layer = *irLayer;
    layer->handleMouseMotionEvent(static_cast<int>(cursorX), static_cast<int>(cursorY), deltaX, deltaY, deltaWheel);
  }
}



void
GUIManager::update(Time deltaT)
{
  if(deltaT > 2.0) return;

  for(unsigned buttonI = 0; buttonI < ARBITRARY_SUPPORTED_BUTTONS; ++buttonI) {
    elapsedSincePress[buttonI] += deltaT;
    elapsedSinceRelease[buttonI] += deltaT;
  }
  
  for(list<Widget*>::reverse_iterator irLayer = mLayers.rbegin(); irLayer != mLayers.rend(); ++irLayer) {
    Widget* layer = *irLayer;
    layer->updateAll(deltaT);
  }
}
