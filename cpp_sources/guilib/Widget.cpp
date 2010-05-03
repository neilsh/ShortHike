//--------------------------------------------------------------------------------
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.shorthike.com
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Widget.hpp"
#include "LayoutManager.hpp"
#include "Graphics.hpp"

//--------------------------------------------------------------------------------


Widget::Widget(LayoutManager *m)
  : isDirty(true), mouseInside(false), background(NULL), isVisible(true),
    hasToolTip(false), toolTip("")
{
  bounds.x = 0; 
  bounds.y = 0; 
  bounds.w = 0; 
  bounds.h = 0;
  preferredSize.w = 0;
  preferredSize.h = 0;

  setLayoutManager(m);
}




Widget::~Widget()
{
}

//--------------------------------------------------------------------------------
// Painting

void
Widget::paintAll(Graphics* graphicsContext)
{
  if(!isVisible) return;
  
  if(graphicsContext != NULL) {
    if(graphicsContext->getClipRectangle().w <= 0 || graphicsContext->getClipRectangle().h <= 0) return;
    
    if(background != NULL) {
      background->paint(graphicsContext);
    }

    paint(graphicsContext);
  }
  
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget *childWidget = *childI;
    if(graphicsContext != NULL) {
      Graphics* childContext = graphicsContext->createSubContext(childWidget->getBounds());
      if(childContext != NULL) {
        childWidget->paintAll(childContext);
        delete childContext;
      }
    }
    else {
      childWidget->paintAll(NULL);
    }
  }
  isDirty = false;
}


void
Widget::paint(Graphics*)
{
}

//--------------------------------------------------------------------------------
// REFACTOR: Remove OGRE stuff

// void
// Widget::renderAll(RenderSystem* renderSystem, int deltaX, int deltaY)
// {
//   if(!isVisible) return;
//   int myDeltaX = getX() + deltaX;
//   int myDeltaY = getY() + deltaY;
  
//   render(renderSystem, myDeltaX, myDeltaY);
//   for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
//     Widget *childWidget = *childI;
//     childWidget->renderAll(renderSystem, myDeltaX, myDeltaY);
//   }  
// }


// void
// Widget::render(RenderSystem* renderSystem, int, int)
// {
// }

//--------------------------------------------------------------------------------

void
Widget::renderAll(RenderSystem* renderSystem, int deltaX, int deltaY)
{
  if(!isVisible) return;
  int myDeltaX = getX() + deltaX;
  int myDeltaY = getY() + deltaY;
  
  render(renderSystem, myDeltaX, myDeltaY);
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget *childWidget = *childI;
    childWidget->renderAll(renderSystem, myDeltaX, myDeltaY);
  }  
}


void
Widget::render(RenderSystem* renderSystem, int, int)
{
}

//--------------------------------------------------------------------------------


bool
Widget::isWidgetDirty()
{
  if(isDirty == true) return true;
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget *childWidget = *childI;
    if(childWidget->isWidgetDirty() == true) return true;
  }
  return false;
}


void
Widget::markDirty()
{
  isDirty = true;
}



//--------------------------------------------------------------------------------


void
Widget::updateAll(Time deltaT)
{
  if(getVisible() == false) return;
  update(deltaT);
  
  vector<Widget *>::const_iterator c = children.begin();
  while(c != children.end()) {
    (*c)->updateAll(deltaT);
    c++;
  }
}


void
Widget::update(Time )
{
}


//--------------------------------------------------------------------------------

bool
Widget::handleKeyboardEvent(char keyCode, bool pressed)
{
  if(!isVisible) return false;
  
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    if(currentChild->handleKeyboardEvent(keyCode, pressed)) return true;
  }
  
  return keyboardEvent(keyCode, pressed);
}

bool
Widget::handleCharacterEvent(wchar_t character)
{
  if(!isVisible) return false;
  
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    if(currentChild->handleCharacterEvent(character)) return true;
  }
  
  return characterEvent(character);
}


//--------------------------------------------------------------------------------

void
Widget::handleMouseMotionEvent(int localX, int localY, float deltaX, float deltaY, float deltaWheel)
{
  if(!isVisible) return;

  Point localPos(localX, localY);

  if(mouseInside == false) {
    mouseInside = true;
    mouseEnterEvent();
    // REFACTOR
//     if(hasToolTip)
//       GUILib::toolTip(toolTip, this);
  }
  else {
    mouseMotionEvent(localX, localY, deltaX, deltaY, deltaWheel);
  }
  
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    GRect childBounds = currentChild->getBounds();
    if(childBounds.contains(localPos)) {
      Point childOrigin = currentChild->getPosition();
      Point childPos(localPos.x - childOrigin.x, localPos.y - childOrigin.y);
      currentChild->handleMouseMotionEvent(childPos.x, childPos.y, deltaX, deltaY, deltaWheel);
    }
    else if(currentChild->mouseInside == true) {
      currentChild->handleMouseLeaveEvent();
    }
  }
}


void
Widget::handleMouseLeaveEvent()
{
  if(!isVisible) return;

  mouseInside = false;
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    currentChild->handleMouseLeaveEvent();
  }
  // REFACTOR: Tooltips
//   if(hasToolTip)
//     GUILib::clearToolTip(this);
  
  mouseLeaveEvent();
}



bool
Widget::handleMouseButtonEvent(int button, bool pressed, int localX, int localY)
{
  if(!isVisible) return false;

  Point localPos(localX, localY);
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    GRect childBounds = currentChild->getBounds();
    if(childBounds.contains(localPos)) {
      Point childOrigin = currentChild->getPosition();
      Point childPos(localPos.x - childOrigin.x, localPos.y - childOrigin.y);
      if (currentChild->handleMouseButtonEvent(button, pressed, childPos.x, childPos.y)) return true;
    }
  }
  if(pressed)
    return mouseDownEvent(button, localX, localY);
  else
    return mouseUpEvent(button, localX, localY);
}


bool
Widget::handleMouseClickEvent(int button, unsigned clickCount, int localX, int localY)
{
  if(!isVisible) return false;

  Point localPos(localX, localY);
  for(vector<Widget *>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Widget* currentChild = *childI;
    GRect childBounds = currentChild->getBounds();
    if(childBounds.contains(localPos)) {
      Point childOrigin = currentChild->getPosition();
      Point childPos(localPos.x - childOrigin.x, localPos.y - childOrigin.y);
      if (currentChild->handleMouseClickEvent(button, clickCount, childPos.x, childPos.y)) return true;
    }
  }
  return mouseClickEvent(button, clickCount, localX, localY);
}

//--------------------------------------------------------------------------------
// By default the widget is marked dirty when moved or resized. Frames override this
// for their own purpose.

void
Widget::boundsUpdatedEvent()
{
  layout();
  markDirty();
}

//--------------------------------------------------------------------------------


void
Widget::setBackground(Drawable* newBackground)
{
  background = newBackground;
  markDirty();
}



Drawable*
Widget::getBackground()
{
  return background;
}


string
Widget::getToolTip()
{
  if(hasToolTip)
    return toolTip;
  else
    return "";
}



void
Widget::setToolTip(string newToolTip)
{
  hasToolTip = true;
  toolTip = newToolTip;
}



void
Widget::add(Widget *c)
{
  children.push_back(c);
  markDirty();
}



void
Widget::add(Widget *c, string k)
{
  children.push_back(c);
  keys[k] = c;
  markDirty();
}


// FIXME: NO-OP removeChild
void
Widget::remove(Widget*)
{
  markDirty();
  CHECK_FAIL("Widget removal not supported");
}


void
Widget::removeAll()
{
  children.clear();
  keys.clear();
  markDirty();
}



Widget *
Widget::getKey(string key)
{
  return keys[key];
}



Widget::const_iterator
Widget::begin()
{
  return children.begin();
}


Widget::const_iterator
Widget::end()
{
  return children.end();
}



void
Widget::setLayoutManager(LayoutManager *mgr)
{
  myLayout = mgr;
}



void
Widget::layout()
{
  if(0 == children.size()) {
    return;
  }
  else if(myLayout != NULL) {
    myLayout->doLayout(this);
  
    const_iterator i = children.begin();
    while(i != children.end()) {
      (*i)->layout();
      i++;
    }
  }
  else if(1 == children.size()) {
    Widget* child = *(children.begin());
    child->setSize(getSize());
  }
  else {
    CHECK_FAIL("Unable to layout without manager and more than one child");
  }
  
  markDirty();
}


const Dimension
Widget::UNLIMITED_PREFERRED_SIZE = Dimension(INT_MAX, INT_MAX);

Dimension
Widget::getPreferredSize()
{
  if(myLayout != NULL) {
    Dimension d = myLayout->preferredLayoutSize(this);
    if(d.w >= 0 && d.h >= 0)
      return d;
  }
  return preferredSize;
}

int
Widget::getPreferredWidth(int targetHeight)
{
  if(myLayout != NULL) {
    int width = myLayout->preferredLayoutWidth(this, targetHeight);
    if(width != 0) return width;
  }

  return getPreferredSize().w;
}

int
Widget::getPreferredHeight(int targetWidth)
{
  if(myLayout != NULL) {
    int height = myLayout->preferredLayoutHeight(this, targetWidth);
    if(height != 0) return height;
  }

  return getPreferredSize().h;
}



void
Widget::setPreferredSize(int w, int h)
{
  preferredSize.w = w;
  preferredSize.h = h;
}



std::ostream&
operator<<(std::ostream& os, const Widget& c)
{
  os << "Widget (" << c.getX() << ", " << c.getY() << ") [" << c.getWidth() << ", " << c.getHeight() << "]";
  return os;
}


//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/guilib/Widget", Test_Widget);

  
void
Test_Widget::setUp()
{
test_widget = new Widget();
}


void
Test_Widget::tearDown()
{
  delete test_widget;
}


namespace
{
  class OwnPreferred : public Widget
  {
  public:
    virtual Dimension getPreferredSize()
    {
      return Dimension(100, 50);
    }
  };
}

void
Test_Widget::preferred_sizes()
{
  OwnPreferred* own_size = new OwnPreferred();
  own_size->setPreferredSize(500, 500);
  CHECK_EQUAL("Correct width preferred", int, 100, own_size->getPreferredWidth(500));
  CHECK_EQUAL("Correct height preferred", int, 50, own_size->getPreferredHeight(500));

  delete own_size;
}
