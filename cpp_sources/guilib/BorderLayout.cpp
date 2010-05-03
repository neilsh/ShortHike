// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com

#include "Common.hpp"

#include "BorderLayout.hpp"
#include "Widget.hpp"



BorderLayout::BorderLayout(unsigned initial_borders, unsigned initial_padding)
{
  borders = initial_borders;
  padding = initial_padding;
}



BorderLayout::~BorderLayout()
{
}



void
BorderLayout::doLayout(Widget *container)
{
  int container_width = container->getWidth();
  int container_height = container->getHeight();
  int top = borders + padding, left = borders + padding;
  int remaining_height = container_height - borders * 2 - padding * 2;
  int remaining_width = container_width - borders * 2 - padding * 2;

  Widget* current_widget;
  if((current_widget = container->getKey("north")) != NULL) {
    current_widget->setSize(remaining_width, current_widget->getPreferredSize().h);
    current_widget->setPosition(borders + padding, borders + padding);
    remaining_height -= current_widget->getHeight() + padding;
    top = current_widget->getHeight() + borders + 2*padding;
  }
  if((current_widget = container->getKey("south")) != NULL) {
    current_widget->setSize(remaining_width, current_widget->getPreferredSize().h);
    current_widget->setPosition(borders + padding, container_height - current_widget->getHeight() - borders - padding);
    remaining_height -= current_widget->getHeight() + padding;
  }
  if((current_widget = container->getKey("west")) != NULL) {
    current_widget->setSize(current_widget->getPreferredSize().w, remaining_height);
    current_widget->setPosition(borders + padding, top);
    remaining_width -= current_widget->getWidth() + padding;
    left = current_widget->getWidth() + borders + 2*padding;
  }
  if((current_widget = container->getKey("east")) != NULL) {
    current_widget->setSize(current_widget->getPreferredSize().w, remaining_height);
    current_widget->setPosition(container_width - current_widget->getWidth() - borders - padding, top);
    remaining_width -= current_widget->getWidth() + padding;
  }
  if((current_widget = container->getKey("center")) != NULL) {
    current_widget->setSize(remaining_width, remaining_height);
    current_widget->setPosition(left, top);
  }  
}



Dimension
BorderLayout::preferredLayoutSize(Widget *container)
{
  int middlew = 0, middleh = 0;
  Dimension result(0, 0);
  
  Widget *c;
  if((c = container->getKey("north")) != NULL) {
    Dimension pref = c->getPreferredSize();
    pref.w += borders * 2 + padding * 2;
    pref.h += borders * 2 + padding * 2;
    result = pref;
  }
  if((c = container->getKey("south")) != NULL) {
    Dimension pref = c->getPreferredSize();
    pref.w += borders * 2 + padding * 2;
    pref.h += padding;
    if(result.w < pref.w)
      result.w = pref.w;
    result.h += pref.h;
  }
  if((c = container->getKey("west")) != NULL) {
    Dimension pref = c->getPreferredSize();
    pref.h += padding;
    middleh = pref.h;
    middlew = pref.w;
  }
  if((c = container->getKey("east")) != NULL) {
    Dimension pref = c->getPreferredSize();
    pref.h += padding;
    if(middleh < pref.h)
      middleh = pref.h;
    middlew += pref.w + padding;
  }
  if((c = container->getKey("center")) != NULL) {
    Dimension pref = c->getPreferredSize();
    pref.h += padding;
    if(middleh < pref.h)
      middleh = pref.h;
    middlew += pref.w + padding;
  }  
  result.h += middleh;
  if(result.w < middlew)
    result.w = middlew;
  
  return result;
}
 



//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/guilib/BorderLayout", Test_BorderLayout);

  
void
Test_BorderLayout::setUp()
{
  container = new Widget();
  layout_manager = new BorderLayout();
  container->setLayoutManager(layout_manager);
  container->setSize(150, 180);
  layout_manager->set_borders(10);
  layout_manager->set_padding(10);

  child1 = new Widget();
  child1->setPreferredSize(0, 500);
  container->add(child1, "center");

  child2 = new Widget();
  child2->setPreferredSize(10, 500);
  container->add(child2, "west");

  child3 = new Widget();
  child3->setPreferredSize(20, 0);
  container->add(child3, "east");

  child4 = new Widget();
  child4->setPreferredSize(0, 10);
  container->add(child4, "north");

  child5 = new Widget();
  child5->setPreferredSize(500, 20);
  container->add(child5, "south");

  container->layout();  
}


void
Test_BorderLayout::tearDown()
{
  delete child5;
  delete child4;
  delete child3;
  delete child2;
  delete child1;
  delete layout_manager;
  delete container;  
}


void
Test_BorderLayout::layout_size()
{
  CHECK_EQUAL("Correct layout size", Dimension, Dimension(540, 590), layout_manager->preferredLayoutSize(container));
}


void
Test_BorderLayout::simple_layout()
{
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(20, 20, 110, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(20, 140, 110, 20), child5->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(20, 40, 10, 90), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(110, 40, 20, 90), child3->getBounds());
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(40, 40, 60, 90), child1->getBounds());
}

