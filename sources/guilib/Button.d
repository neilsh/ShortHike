//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import RenderSystem;
private import GUIManager;
private import Rect;
private import Font;
private import Texture;

private import Point;
private import Value;
private import Library;

// --------------------------------------------------------------------------------
// Texture pool

private static Texture[char[]] gTexturePool;

private Texture textureFromPool(char[] textureName)
{
  if((textureName in gTexturePool) is null) {
    Texture texture = rRenderSystem.createTexture;
    texture.loadFromPNG(textureName);
    gTexturePool[textureName] = texture;
    return texture;
  }
  else {
    return gTexturePool[textureName];
  }
}


//--------------------------------------------------------------------------------

struct FrameTexture
{
  char[] name;
  int frameW;
  int frameH;
}


FrameTexture cFrameTexture(char[] n, int w, int h)
{
  FrameTexture ft;
  ft.name = n;
  ft.frameW = w;
  ft.frameH = h;
  return ft;
}


void doFrameTexture(Rect screenRect, FrameTexture frameTexture, int frame)
{
  Texture texture = textureFromPool(frameTexture.name);

  Rect textureRect = new Rect();
  textureRect.w = texture.getWidth() / frameTexture.frameW;
  textureRect.h = texture.getHeight() / frameTexture.frameH;
  textureRect.x = (frame % frameTexture.frameW) * textureRect.w;
  textureRect.y = (frame / frameTexture.frameW) * textureRect.h;
  rRenderSystem.renderTexture(texture, screenRect, textureRect);
}


// --------------------------------------------------------------------------------

Value eDoButton(Environment env, Value[] args)
{
  assert(CHECK("6 arguments expeted", args.length == 6));
  assert(CHECK("Agument 1 expected to be ID", args[0].type == Type.ID));
  assert(CHECK("Agument 2 expected to be string", args[1].isString()));
  assert(CHECK("Agument 3 expected to be int", args[2].isNumber()));
  assert(CHECK("Agument 4 expected to be int", args[3].isNumber()));
  assert(CHECK("Agument 5 expected to be rect", args[4].type == Type.RECT));
  assert(CHECK("Agument 6 expected to be int", args[5].isNumber()));
  WidgetID id = (cast(IDValue)args[0]).mID;
  Rect dstRect = (cast(RectValue)args[4]).mRect;
  return new BoolValue(doButton(id, cFrameTexture(args[1].vString, args[2].vInt, args[3].vInt),
                                dstRect, args[5].vInt));
}


bool doButton(WidgetID id, FrameTexture texture, Rect clip, int startFrame = 0)
{
  const int DISABLED_FRAME = 0;
  const int NORMAL_FRAME = 1;
  const int HOT_FRAME = 2;
  const int ACTIVE_FRAME = 3;
  if(rGUIManager.isActive(id)) {
    if(rGUIManager.isHot(id)) {
      doFrameTexture(clip, texture, startFrame + ACTIVE_FRAME);
      if(rGUIManager.wasKeyReleased(KeyCode.MOUSE1)) {
        rGUIManager.releaseActive(id);
        return true;
      }
    }
    else {
      doFrameTexture(clip, texture, startFrame + HOT_FRAME);
      if(rGUIManager.wasKeyReleased(KeyCode.MOUSE1)) {
        rGUIManager.releaseActive(id);
      }
    }
  }
  else if(rGUIManager.isHot(id)) {
    doFrameTexture(clip, texture, startFrame + HOT_FRAME);
    if(rGUIManager.wasKeyPressed(KeyCode.MOUSE1) && rGUIManager.isKeyDown(KeyCode.MOUSE1)) {
      rGUIManager.makeActive(id);
    }
  }
  else {
    doFrameTexture(clip, texture, startFrame + NORMAL_FRAME);
  }
  if(clip.contains(rGUIManager.getMousePosition())) {
    rGUIManager.makeHot(id);
  }
  
  return false;
}


// --------------------------------------------------------------------------------


private float[WidgetID] mSlideProgress;
  
bool doMenuButton(WidgetID id, char[] label, int num) {
  static const int BUTTON_WIDTH = 256;
  static const int BUTTON_HEIGHT = 32;
  static const float BUTTON_SPEED = 3.2;
  static const float BUTTON_SEPARATION = 0.05;
  static const float BUTTON_SLIDE_START = -1;

  if(id in mSlideProgress is null) {
    mSlideProgress[id] = BUTTON_SLIDE_START;
  }
  mSlideProgress[id] += rGUIManager.timeStep * BUTTON_SPEED;

  float localProgress = mSlideProgress[id] - num * BUTTON_SEPARATION;
  if(localProgress < 0) localProgress = 0;
  if(localProgress > Math.PI_2) localProgress = Math.PI_2;
  int xPos = cast(int)(rRenderSystem.getWidth() - sin(localProgress) * BUTTON_WIDTH);
    
  Rect buttonRect = new Rect(xPos + 50, 100 + num * BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);
  bool fired = doButton(id, cFrameTexture("assets/gui/MenuButton.png", 1, 4), buttonRect);
  doText(id, label, 12, Color.WHITE, buttonRect.x + 18, buttonRect.y + 8);
  return fired;
}


// --------------------------------------------------------------------------------

void
sDoTexture(char[] textureName, Rect dstRect)
{  
  Texture texture = textureFromPool(textureName);
  Rect srcRect = new Rect(0, 0, texture.getWidth(), texture.getHeight());
  rRenderSystem.renderTexture(texture, dstRect, srcRect);
}

void
sDoTexture(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments", args.length == 2));
  assert(CHECK("First argument expected to be string", args[0].isString()));
  assert(CHECK("Second argument expected to be Rect", args[1].type == Type.RECT));
  char[] textureName = args[0].vString();
  Rect dstRect = (cast(RectValue)args[1]).mRect;
  sDoTexture(textureName, dstRect);
}

Value
eDoTexture(Environment env, Value[] args)
{
  sDoTexture(env, args);
  return rsUndefined();
}


// --------------------------------------------------------------------------------
// A textured box that stretches

Value
eDoTextureBox(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments", args.length == 7));
  assert(CHECK("Agument 1 expected to be ID", args[0].type == Type.ID));
  assert(CHECK("Agument 2 expected to be string", args[1].isString()));
  assert(CHECK("Agument 3 expected to be rect", args[2].type == Type.RECT));
  assert(CHECK("Agument 4 expected to be int", args[3].isNumber()));
  assert(CHECK("Agument 5 expected to be int", args[4].isNumber()));
  assert(CHECK("Agument 6 expected to be int", args[5].isNumber()));
  assert(CHECK("Agument 7 expected to be int", args[6].isNumber()));
  WidgetID id = (cast(IDValue)args[0]).mID;
  Rect dstRect = (cast(RectValue)args[2]).mRect;
  doTextureBox(id, args[1].vString, dstRect, args[3].vInt, args[4].vInt, args[5].vInt, args[6].vInt);
  return rsUndefined();
}


void
doTextureBox(WidgetID id, char[] textureName, Rect screenRect, int left, int right, int top, int bottom)
{
  Texture texture = textureFromPool(textureName);
  
  Rect srcRect = new Rect();
  Rect dstRect = new Rect();
  int tw = texture.getWidth;
  int th = texture.getHeight;

  // Top left
  srcRect.setBounds(0, 0, left, top);
  dstRect.setBounds(screenRect.x, screenRect.y, left, top);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Top
  srcRect.setBounds(left, 0, tw - left - right, top);
  dstRect.setBounds(screenRect.x + left, screenRect.y, screenRect.w - left - right, top);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Top right
  srcRect.setBounds(tw - right, 0, right, top);
  dstRect.setBounds(screenRect.x + screenRect.w - right, screenRect.y, right, top);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Left
  srcRect.setBounds(0, top, left, th - top - bottom);
  dstRect.setBounds(screenRect.x, screenRect.y + top, left, screenRect.h - top - bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Middle
  srcRect.setBounds(right, top, tw - left - right, th - top - bottom);
  dstRect.setBounds(screenRect.x + right, screenRect.y + top, screenRect.w - left - right, screenRect.h - top - bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Right
  srcRect.setBounds(tw - right, top, right, th - top - bottom);
  dstRect.setBounds(screenRect.x + screenRect.w - right, screenRect.y + top, right, screenRect.h - top - bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Bottom left
  srcRect.setBounds(0, th - bottom, left, bottom);
  dstRect.setBounds(screenRect.x, screenRect.y + screenRect.h - bottom, left, bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Bottom
  srcRect.setBounds(right, th - bottom, tw - left - right, top);
  dstRect.setBounds(screenRect.x + right, screenRect.y + screenRect.h - bottom, screenRect.w - left - right, bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);

  // Bottom right
  srcRect.setBounds(tw - right, th - bottom, right, bottom);
  dstRect.setBounds(screenRect.x + screenRect.w - right, screenRect.y + screenRect.h - bottom, right, bottom);
  rRenderSystem.renderTexture(texture, dstRect, srcRect);
}

// --------------------------------------------------------------------------------

Value
eDoPopup(Environment env, Value args[])
{
  assert(CHECK("Wrong number of arguments", args.length == 2));
  assert(CHECK("Agument 1 expected to be ID", args[0].type == Type.ID));
  assert(CHECK("Agument 2 expected to be List", sListP(args[1])));
  WidgetID id = (cast(IDValue)args[0]).mID;
  PairValue iItem = cast(PairValue)args[1];
  char[][] items;
  while(!iItem.isNull()) {
    assert(CHECK("String expected", iItem.car.type == Type.STRING));
    items ~= iItem.car.vString;
    iItem = cast(PairValue)iItem.cdr;
  }
  return new NumberValue(doPopup(id, items)); 
}


int
doPopup(WidgetID id, char[][] items)
{
  const int POPUP_WIDTH = 128;
  const int POPUP_ITEM_HEIGHT = 16;

  static bool isVisible = false;
  static Point menuPoint;
     
  if(rGUIManager.isParentActive(id)) {
    if(!isVisible) {
      isVisible = true;
      menuPoint = rGUIManager.getMousePosition();
    } 
    Rect dstRect = new Rect();
    dstRect.setPosition(menuPoint);
    dstRect.setSize(POPUP_WIDTH, POPUP_ITEM_HEIGHT * items.length);
    if(dstRect.x + dstRect.w > rRenderSystem.getWidth) {
      dstRect.x = rRenderSystem.getWidth - dstRect.w;
    }
    if(dstRect.y + dstRect.h > rRenderSystem.getHeight) {
      dstRect.y = rRenderSystem.getHeight - dstRect.h;
    }

    Rect buttonRect = new Rect();
    buttonRect.x = dstRect.x;
    buttonRect.y = dstRect.y;
    buttonRect.w = dstRect.w;
    buttonRect.h = POPUP_ITEM_HEIGHT;

    dstRect.grow(3);
    doTextureBox(id, "assets/gui/PopupBackground.png", dstRect, 3, 3, 3, 3);

    int itemResult = -1;
    rGUIManager.releaseActive(id);
    for(int iItem = 0; iItem < items.length; iItem++) {
      if(doButton(cID(id, iItem + 5000), cFrameTexture("assets/gui/PopupButtons.png", 1, 4), buttonRect))
        itemResult = iItem;
      doText(cID(id, iItem + 10000), items[iItem], 12, Color.WHITE, buttonRect.x + 4, buttonRect.y, true);
      buttonRect.y += POPUP_ITEM_HEIGHT;
    }
    rGUIManager.makeActive(id);
        
    if(itemResult != -1 || (rGUIManager.isActive(id) && rGUIManager.wasKeyPressed(KeyCode.MOUSE1))) {
      rGUIManager.releaseActive(id);
      isVisible = false;
    }
    return itemResult;
  }
  return -1;
}

