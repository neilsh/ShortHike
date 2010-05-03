//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_BORDER_LAYOUT_HPP
#define GUILIB_BORDER_LAYOUT_HPP

#include "LayoutManager.hpp"
#include "Dimension.hpp"

class Widget;

class BorderLayout : public LayoutManager
{
public:
  BorderLayout(unsigned initial_borders = 1, unsigned initial_padding = 1);
  virtual ~BorderLayout();
  virtual void doLayout(Widget *c);
  virtual Dimension preferredLayoutSize(Widget *c);

  void set_borders(unsigned new_borders) {borders = new_borders;}
  void set_padding(unsigned new_padding) {padding = new_padding;}
private:
  unsigned borders;
  unsigned padding;
};


class Test_BorderLayout : public TestFixture
{
public:
  TEST_FIXTURE_START(Test_BorderLayout);
  TEST_CASE(layout_size);
  TEST_CASE(simple_layout);
  TEST_FIXTURE_END();

  virtual void setUp();
  virtual void tearDown();

  void layout_size();
  void simple_layout();
private:
  Widget* container;
  Widget* child1;
  Widget* child2;
  Widget* child3;
  Widget* child4;
  Widget* child5;

  BorderLayout* layout_manager;
};



#endif
