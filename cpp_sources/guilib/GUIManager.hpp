//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef GUILIB_GUIMANAGER
#define GUILIB_GUIMANAGER

#include "Point.hpp"
#include "Dimension.hpp"
#include "Widget.hpp"

class Widget;
class Frame;
class Dialog;
class Screen;
class Label;
class RenderSystem;

typedef agg::font_engine_freetype_int32 FontEngineType;
typedef agg::font_cache_manager<FontEngineType> FontManagerType;

typedef int KeyModifier;
const KeyModifier KEY_MODIFIER_SHIFT = 1 << 0;
const KeyModifier KEY_MODIFIER_LSHIFT = 1 << 1;
const KeyModifier KEY_MODIFIER_RSHIFT = 1 << 2;
const KeyModifier KEY_MODIFIER_ALT = 1 << 3;
const KeyModifier KEY_MODIFIER_LALT = 1 << 4;
const KeyModifier KEY_MODIFIER_RALT = 1 << 5;
const KeyModifier KEY_MODIFIER_CONTROL = 1 << 6;
const KeyModifier KEY_MODIFIER_LCONTROL = 1 << 7;
const KeyModifier KEY_MODIFIER_RCONTROL = 1 << 8;


class GUIManager
{
  friend class DialogProxy;
public:
  GUIManager();

  void injectKeyboardEvent(char keyCode, bool pressed);
  void injectCharacterEvent(wchar_t character);
  void injectMouseButtonEvent(int buttonID, bool pressed);
  void injectMouseMotionEvent(float newX, float newY, float deltaWheel);
  void update(Time deltaT);

  void render(RenderSystem* renderSystem);

  Widget* getRootContainer();

  void clearLayers();
  void addLayer(Widget* layerWidget);

  Dimension getScreenDimension();
  int getScreenWidth();
  int getScreenHeight();
  Point getScreenPosition(const Vector& position);
  bool isPositionOnScreen(const Vector& position);  

  Point getCursorPosition();
  int getCursorX();
  int getCursorY();
  Vector getRelativeCursorPosition(int cursorX, int cursorY); // [0,1]
  Vector getNormalizedCursorPosition(int cursorX, int cursorY); //[-1,1]
  KeyModifier getKeyModifier();

  FontEngineType& getFontEngine();
  FontManagerType& getFontManager();
  Dimension getStringDimension(FontSize size, wstring text);
  int getFontHeight(FontSize size);

  enum SkinColor {
    SKIN_BUTTON_DISABLED_COLOR,
    SKIN_BUTTON_ENABLED_COLOR,
    SKIN_BUTTON_MOUSEOVER_COLOR,
    SKIN_BUTTON_OUTLINE_COLOR,
    SKIN_FRAME_COLOR,
    SKIN_FRAME_COLOR_DENSE,
    SKIN_FRAME_OUTLINE_COLOR,
    SKIN_PROGRESSBAR_COLOR,
    SKIN_BEVEL_LIGHT_COLOR,
    SKIN_BEVEL_DARK_COLOR,
  };

  static ColorType getSkinColor(SkinColor colorType);  

  enum ButtonNum {
    LeftButton = 0,
    RightButton = 1,
    MiddleButton = 2
  };
private:
  FontEngineType fontEngine;
  FontManagerType fontManager;

  list<Widget*> mLayers;
  Widget* rootContainer;

  int cursorX;
  int cursorY;

  static const unsigned ARBITRARY_SUPPORTED_BUTTONS = 8;
  Time elapsedSincePress[ARBITRARY_SUPPORTED_BUTTONS];
  Time elapsedSinceRelease[ARBITRARY_SUPPORTED_BUTTONS];
  unsigned clickCount[ARBITRARY_SUPPORTED_BUTTONS];
};


#endif
