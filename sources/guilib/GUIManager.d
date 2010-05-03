//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import RenderSystem;
private import Dimension;
private import Point;
private import Rect;

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

// --------------------------------------------------------------------------------

version(Windows) 
{
  extern (Windows) {
    export SHORT GetKeyState(int nVirtKey);
  }
}

// --------------------------------------------------------------------------------
// Singleton implementation

GUIManager gGUIManager;

GUIManager
rGUIManager()
{
  if(gGUIManager is null)
    gGUIManager = new GUIManager();
  return gGUIManager;
}


// --------------------------------------------------------------------------------


struct WidgetID
{
  void* owner;
  int index;
  int subindex;
}

class IDValue : public Value
{
  this(WidgetID id) {mID = id;}
  Type type() {return Type.ID; }
  WidgetID mID;
}


WidgetID
cID(void* _owner, int _index)
{
  WidgetID id;
  id.owner = _owner;
  id.index = _index;
  id.subindex = 0;
  return id;
}

WidgetID
cID(WidgetID parent, int _subindex)
{
  WidgetID id = parent;
  id.subindex = _subindex;
  return id;
}


Value
scID(Environment env, Value[] args)
{
  assert(CHECK("Expected 2 arguments to cid", args.length == 2));
  assert(CHECK("Expected 2nd argument to cid to be in", args[1].isNumber));
  if(args[0].type == Type.PROCEDURE) {
    ProcedureValue proc = cast(ProcedureValue)args[0];
    int index = args[1].vInt;
    return new IDValue(cID(&proc.mBody, index));
  }
  else if(args[0].type == Type.ID) {
    WidgetID id = (cast(IDValue)args[0]).mID;
    int subIndex = args[1].vInt;
    return new IDValue(cID(id, subIndex));
  }
  else {
    assert(CHECK("Expected procedure or ID as first argument", args[0].type == Type.PROCEDURE || args[0].type == Type.ID));
    return rsUndefined();
  }  
}



enum KeyCode {
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
  LEFT, RIGHT, UP, DOWN, PAGE_UP, PAGE_DOWN, ESCAPE, DELETE,
  MOUSE1, MOUSE2, MOUSE3, MOUSE1_DCLICK, MOUSE2_DCLICK, MOUSE3_DCLICK,
  UNKNOWN
}

// --------------------------------------------------------------------------------

class GUIManager
{
  enum ButtonNum {
    LeftButton = 0,
    RightButton = 1,
    MiddleButton = 2
  };


  //--------------------------------------------------------------------------------

  // FIXME: should first check if anyone eats the button event before doing clicks.
  const float CLICK_LIMIT = 0.3f;
  const float MULTIPLE_CLICK_LIMIT = 0.5f;

  this()
  {
    // reset the counters to values that will not fire any events at startup
    for(uint iButton = 0; iButton < ARBITRARY_SUPPORTED_BUTTONS; ++iButton) {
      elapsedSinceRelease[iButton] = 2 * MULTIPLE_CLICK_LIMIT;
      elapsedSincePress[iButton] = 2* CLICK_LIMIT;    
    }
  
    mActive = cID(null, 0);
    mHot = cID(null, 0);

    for(KeyCode code = KeyCode.min; code <= KeyCode.max; ++code) {
      mKeyDown[code] = false;
      mKeyPressed[code] = false;
      mKeyReleased[code] = false;
    }
  }


  //--------------------------------------------------------------------------------

  int
  getScreenWidth()
  {
    return rRenderSystem().getWidth();
  }


  int
  getScreenHeight()
  {
    return rRenderSystem().getHeight();
  }

  Dimension
  getScreenDimension()
  {
    return new Dimension(getScreenWidth(), getScreenHeight());
  }


  //--------------------------------------------------------------------------------

  Point
  getCursorPosition()
  {
    return new Point(mMouseX, mMouseY);
  }


  int
  getCursorX()
  {
    return mMouseX;
  }

  int
  getCursorY()
  {
    return mMouseY;
  }

  Point
  getMousePosition()
  {
    return new Point(mMouseX, mMouseY);
  }
  

  Point
  getMouseDelta()
  {
    return new Point(mDeltaX, mDeltaY);
  }
  

  //--------------------------------------------------------------------------------
  // Utility
  

  Vector
  getRelativeCursorPosition(int x, int y)
  {
    float relativeX = cast(float)x / getScreenWidth();
    float relativeY = cast(float)y / getScreenHeight();
    return createVector(relativeX, relativeY, 0);
  }

  float
  getRelativeMouseX()
  {
    return cast(float)mMouseX / getScreenWidth();
  }

  float
  getRelativeMouseY()
  {
    return cast(float)mMouseY / getScreenHeight();
  }
  


  Vector
  getNormalizedCursorPosition(int x, int y)
  {
    float relativeX = cast(float)x / getScreenWidth();
    float relativeY = cast(float)y / getScreenWidth();
    return createVector(relativeX * 2.0f - 1.0f, relativeY * 2.0f - 1.0f, 0);
  }


  Point
  getScreenPosition(Vector position) 
  { 
    // FIXME:
    //   Vector hcsPosition = defaultCamera.getProjectionMatrix() * (defaultCamera.getViewMatrix() * position);
    //   float normalizedX = 1.0f - ((hcsPosition.x * 0.5f) + 0.5f);
    //   float normalizedY = ((hcsPosition.y * 0.5f) + 0.5f);
    //   return Point(normalizedX * getScreenWidth(), normalizedY * getScreenHeight());
    assert(CHECK_FAIL("Not implemented"));
    return new Point(0, 0);
  }


  bool
  isPositionOnScreen(Vector position)
  { 
    Point screenPos = getScreenPosition(position);
    Rect screenBounds;
    screenBounds.setSize(getScreenDimension());
    return screenBounds.contains(screenPos);
  }

  //--------------------------------------------------------------------------------

  version(Windows) 
  {
    
    KeyModifier
      getKeyModifier()
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
  }

  //--------------------------------------------------------------------------------

  void
  injectKeyboardEvent(KeyCode keyCode, bool pressed)
  {
    if(pressed) {
      mKeyPressed[keyCode] = true;
      mKeyDown[keyCode] = true;
    }    
    else {
      mKeyReleased[keyCode] = true;
      mKeyDown[keyCode] = true;
    }
  }


  void
  injectCharacterEvent(wchar character)
  {
  }


  void
  injectMouseButtonEvent(int buttonID, bool pressed)
  {
    int xpos = cast(int)(mMouseX);
    int ypos = cast(int)(mMouseY);

    assert(CHECK("Button count within number of supported button", buttonID >= 0 && buttonID < ARBITRARY_SUPPORTED_BUTTONS));
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

        // FIXME:
        // Handle mouse click

        elapsedSinceRelease[buttonID] = 0;
      } 
    }
    
    // FIXME:
    // Handle mouse button

    KeyCode code;
    if(buttonID == 0)
      code = KeyCode.MOUSE1;
    else if(buttonID == 1)
      code = KeyCode.MOUSE2;
    else
      code = KeyCode.MOUSE3;
    
    if(pressed) {
      mKeyPressed[code] = true;
      mKeyDown[code] = true;
    }    
    else {
      mKeyReleased[code] = true;
      mKeyDown[code] = false;
    }
  }
  

  void
  injectMouseMotionEvent(float newX, float newY, float deltaWheel)
  {
    float deltaX = newX - mMouseX;
    float deltaY = newY - mMouseY;
    mMouseX = cast(int)newX;
    mMouseY = cast(int)newY;
    mDeltaX += cast(int)deltaX;
    mDeltaY += cast(int)deltaY;
  }

  // --------------------------------------------------------------------------------
  // IMGUI stuff

  void
  updateFrame(float step)
  {
    timeStep = step;
    // IM update stuff
    mHot = mNextHot;
    mNextHot = NULL_WIDGET_ID;
    for(KeyCode code = KeyCode.min; code <= KeyCode.max; ++code) {
      mKeyPressed[code] = false;
      mKeyReleased[code] = false;
    }
    mDeltaX = 0;
    mDeltaY = 0;

    for(uint buttonI = 0; buttonI < ARBITRARY_SUPPORTED_BUTTONS; ++buttonI) {
      elapsedSincePress[buttonI] += timeStep;
      elapsedSinceRelease[buttonI] += timeStep;
    }
  }
  

  bool isKeyDown(KeyCode code)
  {
    return mKeyDown[code];
  }

  bool wasKeyPressed(KeyCode code)
  {
    return mKeyPressed[code];
  }
  
  bool wasKeyReleased(KeyCode code)
  {
    return mKeyReleased[code];
  }

  void makeHot(WidgetID id)
  {
    if(mActive == id || mActive == NULL_WIDGET_ID)
      mNextHot = id;
  }

  bool isHot(WidgetID id)
  {
    return cast(bool)(mHot == id);
  }

  bool makeActive(WidgetID id)
  {
    if(mActive == NULL_WIDGET_ID) {
      mActive = id;
      return true;
    }
    else {
      return false;
    }
  }


  void releaseActive(WidgetID id)
  {
    if(mActive == id)
      mActive = NULL_WIDGET_ID;
  }
  
  bool isActive(WidgetID id)
  {
    return cast(bool)(mActive == id);
  }

//   bool isActive(void* owner)
//   {
//     return cast(bool)(mActive.owner is owner);
//   }

  bool isParentActive(WidgetID id)
  {
    // Ignore subindex
    return cast(bool)(mActive.owner == id.owner && mActive.index == id.index);    
  }
  
  
  

  // --------------------------------------------------------------------------------

  float timeStep = 0;

private:
  int mMouseX;
  int mMouseY;
  int mDeltaX;
  int mDeltaY;

  const uint ARBITRARY_SUPPORTED_BUTTONS = 8;
  float elapsedSincePress[ARBITRARY_SUPPORTED_BUTTONS];
  float elapsedSinceRelease[ARBITRARY_SUPPORTED_BUTTONS];
  uint clickCount[ARBITRARY_SUPPORTED_BUTTONS];

  static const WidgetID NULL_WIDGET_ID = {owner: null, index: 0, subindex: 0};
  WidgetID mActive;
  WidgetID mHot;
  WidgetID mNextHot;

  bool[KeyCode] mKeyDown;
  bool[KeyCode] mKeyPressed;
  bool[KeyCode] mKeyReleased;
}


// --------------------------------------------------------------------------------
// Scheme exports

Value
eGUIMakeActive(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments, 1 expected", args.length == 1));
  assert(CHECK("Rect expected as first argument", args[0].type() == Type.ID));
  return new BoolValue(rGUIManager.makeActive((cast(IDValue)args[0]).mID));
}
