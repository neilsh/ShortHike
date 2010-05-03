//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_FLOW_LAYOUT_HPP
#define GUILIB_FLOW_LAYOUT_HPP

#include "LayoutManager.hpp"
#include "Dimension.hpp"

class Widget;

class FlowLayout : public LayoutManager
{
public:
  enum Orientation {VERTICAL, HORIZONTAL};
  enum HorizontalAlignment {LEFT, RIGHT, CENTER_HORIZONTALLY, FILL_HORIZONTALLY};
  enum VerticalAlignment {TOP, BOTTOM, CENTER_VERTICALLY, FILL_VERTICALLY};

  FlowLayout(Orientation o = HORIZONTAL, HorizontalAlignment = LEFT, VerticalAlignment = CENTER_VERTICALLY);
  virtual ~FlowLayout();

  virtual void doLayout(Widget *c);
  virtual Dimension preferredLayoutSize(Widget *c);

  virtual int preferredLayoutWidth(Widget* c, int targetHeight);
  virtual int preferredLayoutHeight(Widget* c, int targetWidth);

  void set_orientation(Orientation new_orientation) { layout_orientation = new_orientation; }
  void set_horizontal_alignment(HorizontalAlignment new_horizontal_alignment) {horizontal_alignment = new_horizontal_alignment;}
  void set_vertical_alignment(VerticalAlignment new_vertical_alignment) {vertical_alignment = new_vertical_alignment;}
  
  void set_borders(unsigned new_border_size) {borders = new_border_size;}
  void set_padding(unsigned new_padding) {padding = new_padding;}

private:
  void do_horizontal_layout(Widget *container);
  void do_vertical_layout(Widget *container);

  Orientation layout_orientation;
  HorizontalAlignment horizontal_alignment;
  VerticalAlignment vertical_alignment;

  unsigned borders;
  unsigned padding;
};



class Test_FlowLayout : public TestFixture
{
public:
  TEST_FIXTURE_START(Test_FlowLayout);
  TEST_CASE(horizontal_layout);
  TEST_CASE(vertical_layout);
  TEST_CASE(horizontal_centering);
  TEST_CASE(vertical_centering);
  TEST_CASE(horizontal_right);
  TEST_CASE(vertical_bottom);
  TEST_CASE(nested_widgets);
  TEST_FIXTURE_END();

  virtual void setUp();
  virtual void tearDown();

  void horizontal_layout();
  void vertical_layout();

  void horizontal_centering();
  void vertical_centering();
  
  void horizontal_right();
  void vertical_bottom();

  void nested_widgets();
private:
  Widget* container;
  Widget* child1;
  Widget* child2;
  Widget* child3;
  Widget* child4;
  Widget* child5;

  FlowLayout* layout_manager;
};



#endif
