// $Id: hud.hpp,v 1.68 2003/07/04 13:49:57 Kai-Peter Exp $
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_LIST_HPP
#define GUILIB_LIST_HPP

#include "Widget.hpp"

class Graphics;


class ListListener
{
public:
  virtual void itemPointed(Widget*, int ) {};  
  virtual void itemSelected(Widget* source, unsigned num) = 0;
  virtual void itemActivated(Widget* source, unsigned num) {itemSelected(source, num);}
};



class List : public Widget
{
public:
  List(unsigned columns = 1, ListListener* listener = NULL, bool renderSelected = false);
  virtual ~List();

  int size();

  void addItem(wstring newItem, bool enabled = true);
  void addItem(vector<wstring> newItem, bool enabled = true);
  void setItem(unsigned num, wstring newItem, bool enabled = true);
  void setItem(unsigned num, vector<wstring> newItem, bool enabled = true);
  wstring getItem(unsigned num, unsigned column = 0);
  bool isItemEnabled(unsigned num);

  void removeItem(unsigned num);
  void removeAllItems();  

  void setSelected(unsigned num);
  unsigned getSelected() {return selected;}

  void resetPointed() {pointed = -1;}

  enum AlignType {LEFT_ALIGN, MIDDLE_ALIGN, RIGHT_ALIGN};
  void setColumnWidth(unsigned column, Real relative);
  void setColumnAlignment(unsigned column, AlignType columnAlignment);

  virtual bool mouseClickEvent(int button, unsigned clickCount, int x, int y);
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);
  virtual void mouseLeaveEvent();
  virtual bool keyboardEvent(char keyCode, bool pressed);

  virtual void paint(Graphics* graphicsContext);
private:
  ListListener* listener;

  int findItemAtPosition(int y);
  void updateItemDimensions();

  int borders;
  int padding;
  FontSize fontSize;

  Real totalWidthWeight;
  vector<Real> columnWidth;
  vector<AlignType> columnAlignments;

  bool mRenderSelected;
  unsigned selected;
  int pointed;
  int lastPointed;
  unsigned columns;

  vector<vector<wstring> > items;
  vector<bool> itemEnabledState;
  
  vector<vector<Dimension> > itemDimensions;
  vector<int> itemHeights;
};


class TestList : public TestFixture
{
public:
  TEST_FIXTURE_START(TestList);
  TEST_CASE(list_enabled);
  TEST_FIXTURE_END();

  void list_enabled();
};

#endif
