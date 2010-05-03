//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//


#pragma once

#ifndef GUILIB_WIDGET_HPP
#define GUILIB_WIDGET_HPP

#include "Drawable.hpp"
#include "Point.hpp"
#include "Dimension.hpp"
#include "GRect.hpp"
#include "UserEventListener.hpp"

class Graphics;
class LayoutManager;
class RenderSystem;

class Widget : public Drawable, public UserEventListener
{
public:
  Widget(LayoutManager *m = NULL);

  virtual ~Widget();

  virtual void paintAll(Graphics* graphicsContext);
  virtual void paint(Graphics* graphicsContext);
  virtual void renderAll(RenderSystem* renderSystem, int deltaX, int deltaY);
  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);
  virtual bool isWidgetDirty();
  virtual void markDirty();

  virtual void updateAll(Time deltaT);
  virtual void update(Time deltaT);
  virtual void boundsUpdatedEvent();

  bool handleKeyboardEvent(char keyCode, bool pressed);
  bool handleCharacterEvent(wchar_t character);
  bool handleMouseButtonEvent(int button, bool pressed, int x, int y);
  bool handleMouseClickEvent(int buttonID, unsigned clickCount, int x, int y);
  void handleMouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);

  // Position management, no clipping
  int getX() const {return bounds.x;}
  int getY() const {return bounds.y;}
  Point getPosition() {return Point(bounds.x, bounds.y);}
  virtual void setPosition(const int xpos, const int ypos) {bounds.x = xpos; bounds.y = ypos; boundsUpdatedEvent(); }
  virtual void setPosition(Point p) {setPosition(p.x, p.y);}

  int getWidth() const {return bounds.w;}
  int getHeight() const {return bounds.h;}
  Dimension getSize() {return Dimension(bounds.w, bounds.h);}
  virtual void setSize(const int w, const int h) {bounds.w = w; bounds.h = h; boundsUpdatedEvent();}
  virtual void setSize(Dimension d) {setSize(d.w, d.h);}

  GRect getBounds() const {return bounds;}

  const static Dimension UNLIMITED_PREFERRED_SIZE;
  virtual Dimension getPreferredSize();
  virtual int getPreferredWidth(int givenHeight);
  virtual int getPreferredHeight(int givenWidth);
  void setPreferredSize(int w, int h);
  void setPreferredSize(Dimension d) {setPreferredSize(d.w, d.h);}

  virtual void setVisible(bool visibility) {isVisible = visibility;}
  virtual bool getVisible() {return isVisible;}

  // Children management
  typedef std::vector<Widget *>::const_iterator const_iterator;

  void add(Widget *c);
  void add(Widget *c, std::string key);
  Widget* getKey(std::string key);
  void remove(Widget *c);
  void removeAll();
  const_iterator begin();
  const_iterator end();
  void setLayoutManager(LayoutManager *m);
  void layout();

  void setBackground(Drawable* background);
  Drawable* getBackground();

  string getToolTip();
  void setToolTip(string toolTip);

  friend std::ostream& operator<<(std::ostream&, const Widget&);
private:
  void handleMouseLeaveEvent();

  GRect bounds;
  bool mouseInside;

  bool isVisible;
  bool isDirty;

  bool hasToolTip;
  string toolTip;
  
  Drawable* background;
  std::vector<Widget *> children;
  std::map<std::string, Widget *> keys;

  Dimension preferredSize;
  LayoutManager *myLayout;
};


class Test_Widget : public TestFixture
{
public:
  TEST_FIXTURE_START(Test_Widget);
  TEST_CASE(preferred_sizes);
  TEST_FIXTURE_END();

  virtual void setUp();
  virtual void tearDown();

  void preferred_sizes();
private:
  Widget* test_widget;
};

#endif
