//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_GRID_LAYOUT_HPP
#define GUILIB_GRID_LAYOUT_HPP

#include "LayoutManager.hpp"
#include "Dimension.hpp"

class Widget;

class GridLayout : public LayoutManager
{
  friend class Test_GridLayout;
public:
  GridLayout(Widget *c, int w, int h);
  virtual ~GridLayout();
  virtual void doLayout(Widget *container);
  virtual Dimension preferredLayoutSize(Widget *container);
  
  void set_borders(unsigned new_border_size) {borders = new_border_size;}
  void set_padding(unsigned new_padding) {padding = new_padding;}

  void add(Widget *new_child, int layout_x, int layout_y, int layout_width = 1, int layout_height = 1);

  int getLayoutWidth() {return layout_total_width;}
  int getLayoutHeight() {return layout_total_height;}
private:
  int get_x_position(int container_width, int layout_xpos);
  int get_y_position(int container_height, int layout_ypos);
  int get_width(int container_width, int layout_width);
  int get_height(int continer_heigh, int layout_height);

  class Item 
  {
  public:
    Item(Widget *comp, int xpos, int ypos, int width, int height)
    {
      child_widget = comp;
      layout_x = xpos;
      layout_y = ypos;
      layout_width = width;
      layout_height = height;
    }

    Widget *child_widget;
    int layout_x, layout_y, layout_width, layout_height;
  };  
  
  Widget *parent_container;
  vector<Item> children;

  int layout_total_width;
  int layout_total_height;

  unsigned borders;
  unsigned padding;
};



class Test_GridLayout : public TestFixture
{
public:
  TEST_FIXTURE_START(Test_GridLayout);
  TEST_CASE(bounds_calculation);
  TEST_CASE(padded_layout);
  TEST_FIXTURE_END();

  virtual void setUp();
  virtual void tearDown();

  void bounds_calculation();
  void padded_layout();
private:
  Widget* container;
  Widget* child1;
  Widget* child2;
  Widget* child3;
  Widget* child4;
  Widget* child5;

  GridLayout* layout_manager;
};



#endif
