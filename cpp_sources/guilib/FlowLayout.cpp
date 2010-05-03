// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com

#include "Common.hpp"

#include "FlowLayout.hpp"
#include "Widget.hpp"


FlowLayout::FlowLayout(Orientation orientationP, HorizontalAlignment horizontal_alignmentP,
                       VerticalAlignment vertical_alignmentP)
{
  layout_orientation = orientationP;
  horizontal_alignment = horizontal_alignmentP;
  vertical_alignment = vertical_alignmentP;

  borders = 1;
  padding = 1;
}

FlowLayout::~FlowLayout()
{
}


void
FlowLayout::doLayout(Widget *container)
{
  if(layout_orientation == VERTICAL) {
    do_vertical_layout(container);
  }
  else {
    do_horizontal_layout(container);
  }
}


void
FlowLayout::do_horizontal_layout(Widget* container)
{
  int container_height_limit = container->getHeight() - borders * 2 - padding * 2;
  int total_layout_width = preferredLayoutWidth(container, container_height_limit);

  int xpos;
  if(horizontal_alignment == LEFT) {
    xpos = borders + padding;
  }
  else if(horizontal_alignment == RIGHT) {
    xpos = container->getWidth() - total_layout_width + borders + padding;
  }
  else {
    xpos = (container->getWidth() - total_layout_width) / 2 + borders + padding;
  }
    
  for(Widget::const_iterator widgetI = container->begin(); widgetI != container->end(); ++widgetI) {
    Widget *currentWidget = *widgetI;

    int widget_height = currentWidget->getPreferredSize().h;
    if(vertical_alignment == FILL_VERTICALLY)
      widget_height = container_height_limit;
    
    if(widget_height > container_height_limit) widget_height = container_height_limit;
    int widget_width = currentWidget->getPreferredWidth(widget_height);
    
    int ypos;
    if(vertical_alignment == TOP) {
      ypos = borders + padding;
    }
    else if(vertical_alignment == BOTTOM) {
      ypos = container->getHeight() - widget_height - borders - padding;
    }
    else {
      ypos = (container->getHeight() - widget_height) / 2;
    }
    
    currentWidget->setPosition(xpos, ypos);
    currentWidget->setSize(widget_width, widget_height);
    xpos += widget_width + padding;
  }
}


void
FlowLayout::do_vertical_layout(Widget* container)
{
  int container_width_limit = container->getWidth() - borders * 2 - padding * 2;
  int total_layout_height = preferredLayoutHeight(container, container_width_limit);

  int ypos;
  if(vertical_alignment == TOP) {
    ypos = borders + padding;
  }
  else if(vertical_alignment == BOTTOM) {
    ypos = container->getHeight() - total_layout_height + borders + padding;
  }
  else {
    ypos = (container->getHeight() - total_layout_height) / 2 + borders + padding;
  }

  for(Widget::const_iterator widgetI = container->begin(); widgetI != container->end(); ++widgetI) {
    Widget *currentWidget = *widgetI;

    int widget_width = currentWidget->getPreferredSize().w;
    if(vertical_alignment == FILL_HORIZONTALLY)
      widget_width = container_width_limit;
    if(widget_width > container_width_limit) widget_width = container_width_limit;
    int widget_height = currentWidget->getPreferredHeight(widget_width);
    
    int xpos;
    if(horizontal_alignment == LEFT) {
      xpos = borders + padding;
    }
    else if(horizontal_alignment == RIGHT) {
      xpos = container->getWidth() - widget_width - borders - padding;
    }
    else {
      xpos = (container->getWidth() - widget_width) / 2;
    }
    
    currentWidget->setPosition(xpos, ypos);
    currentWidget->setSize(widget_width, widget_height);
    ypos += widget_height + padding;
  }
}


  
Dimension
FlowLayout::preferredLayoutSize(Widget *container)
{
  Dimension preferred_size(borders * 2 + padding, borders * 2 + padding);

  Widget::const_iterator widgetI = container->begin();
  while(widgetI != container->end()) {
    Widget *currentWidget = *widgetI;
    Dimension cpref = currentWidget->getPreferredSize();
    if(layout_orientation == VERTICAL) {
      int preferred_width = cpref.w + borders * 2 + padding * 2;
      if(preferred_width > preferred_size.w)
        preferred_size.w = preferred_width;
      preferred_size.h += cpref.h + padding;
    }
    else {
      int preferred_height = cpref.h + borders * 2 + padding * 2;
      if(preferred_height > preferred_size.h)
        preferred_size.h = preferred_height;
      preferred_size.w += cpref.w + padding;
    }
    ++widgetI;
  }
  return preferred_size;
}


int
FlowLayout::preferredLayoutHeight(Widget *container, int target_width)
{
  int total_layout_height = borders * 2 + padding;
  for(Widget::const_iterator widgetI = container->begin(); widgetI != container->end(); ++widgetI) {
    Widget *currentWidget = *widgetI;
    total_layout_height += currentWidget->getPreferredHeight(target_width) + padding;
  }
  return total_layout_height;
}


int
FlowLayout::preferredLayoutWidth(Widget *container, int target_height)
{
  int total_layout_width = borders * 2 + padding;
  for(Widget::const_iterator widgetI = container->begin(); widgetI != container->end(); ++widgetI) {
    Widget *currentWidget = *widgetI;
    total_layout_width += currentWidget->getPreferredWidth(target_height) + padding;
  }
  return total_layout_width;  
}




//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/guilib/FlowLayout", Test_FlowLayout);


namespace
{
  
  class AdaptiveWidget : public Widget
  {
  public:
    virtual int getPreferredWidth(int givenHeight)
    {
      if(givenHeight < getPreferredSize().w)
        return givenHeight;
      else
        return getPreferredSize().w;
    }
    
    virtual int getPreferredHeight(int givenWidth)
    {
      if(givenWidth  < getPreferredSize().h)
        return givenWidth;
      else
        return getPreferredSize().h;
    }    
  };
}


  
void
Test_FlowLayout::setUp()
{
  container = new Widget();
  layout_manager = new FlowLayout(FlowLayout::HORIZONTAL, FlowLayout::LEFT, FlowLayout::TOP);
  layout_manager->set_borders(5);
  layout_manager->set_padding(5);
  container->setLayoutManager(layout_manager);

  child1 = new Widget();
  child1->setPreferredSize(10, 50);
  container->add(child1);

  child2 = new AdaptiveWidget();
  child2->setPreferredSize(150, 150);
  container->add(child2);

  child3 = new Widget();
  child3->setPreferredSize(0, 0);
  container->add(child3);

  child4 = new Widget();
  child4->setPreferredSize(50, 10);
  container->add(child4);

  child5 = new Widget();
  child5->setPreferredSize(200, 200);
  container->add(child5);
}


void
Test_FlowLayout::tearDown()
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
Test_FlowLayout::horizontal_layout()
{
  container->setSize(100, 50);
  layout_manager->set_orientation(FlowLayout::HORIZONTAL);
  layout_manager->set_horizontal_alignment(FlowLayout::LEFT);
  layout_manager->set_vertical_alignment(FlowLayout::TOP);
  container->layout();
  
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(10, 10, 10, 30), child1->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(25, 10, 30, 30), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(60, 10, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(65, 10, 50, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(120, 10, 200, 30), child5->getBounds());
}



void
Test_FlowLayout::vertical_layout()
{
  container->setSize(50, 400);
  layout_manager->set_orientation(FlowLayout::VERTICAL);
  layout_manager->set_horizontal_alignment(FlowLayout::RIGHT);
  layout_manager->set_vertical_alignment(FlowLayout::TOP);
  container->layout();
  
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(30, 10, 10, 50), child1->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(10, 65, 30, 30), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(40, 100, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(10, 105, 30, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(10, 120, 30, 200), child5->getBounds());

}


void
Test_FlowLayout::horizontal_centering()
{
  container->setSize(800, 400);
  layout_manager->set_orientation(FlowLayout::HORIZONTAL);
  layout_manager->set_horizontal_alignment(FlowLayout::CENTER_HORIZONTALLY);
  layout_manager->set_vertical_alignment(FlowLayout::CENTER_VERTICALLY);
  container->layout();

  CHECK_EQUAL("Child 1 bounds", GRect, GRect(185, 175, 10, 50), child1->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(200, 125, 150, 150), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(355, 200, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(360, 195, 50, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(415, 100, 200, 200), child5->getBounds());
}


void
Test_FlowLayout::vertical_centering()
{
  container->setSize(200, 800);
  layout_manager->set_orientation(FlowLayout::VERTICAL);
  layout_manager->set_horizontal_alignment(FlowLayout::CENTER_HORIZONTALLY);
  layout_manager->set_vertical_alignment(FlowLayout::CENTER_VERTICALLY);
  container->layout();

  CHECK_EQUAL("Child 1 bounds", GRect, GRect(95, 185, 10, 50), child1->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(25, 240, 150, 150), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(100, 395, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(75, 400, 50, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(10, 415, 180, 200), child5->getBounds());
}


void
Test_FlowLayout::horizontal_right()
{
  container->setSize(400, 50);
  layout_manager->set_orientation(FlowLayout::HORIZONTAL);
  layout_manager->set_horizontal_alignment(FlowLayout::RIGHT);
  layout_manager->set_vertical_alignment(FlowLayout::TOP);
  container->layout();
  
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(190, 10, 200, 30), child5->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(135, 10, 50, 10), child4->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(130, 10, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(95, 10, 30, 30), child2->getBounds());
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(80, 10, 10, 30), child1->getBounds());
}


void
Test_FlowLayout::vertical_bottom()
{
  container->setSize(100, 400);
  layout_manager->set_orientation(FlowLayout::VERTICAL);
  layout_manager->set_horizontal_alignment(FlowLayout::CENTER_HORIZONTALLY);
  layout_manager->set_vertical_alignment(FlowLayout::BOTTOM);
  container->layout();
  
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(10, 190, 80, 200), child5->getBounds());  
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(25, 175, 50, 10), child4->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(50, 170, 0, 0), child3->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(10, 85, 80, 80), child2->getBounds());
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(45, 30, 10, 50), child1->getBounds());
}


void
Test_FlowLayout::nested_widgets()
{
  Widget* container_outer = new Widget();
  FlowLayout* layout_manager_outer = new FlowLayout(FlowLayout::HORIZONTAL, FlowLayout::CENTER_HORIZONTALLY, FlowLayout::CENTER_VERTICALLY);
  layout_manager_outer->set_borders(5);
  layout_manager_outer->set_padding(5);
  container_outer->setLayoutManager(layout_manager_outer);
  container_outer->setPreferredSize(Widget::UNLIMITED_PREFERRED_SIZE);
  container_outer->setSize(100,100);

  Widget* container_middle = new Widget();
  FlowLayout* layout_manager_middle = new FlowLayout(FlowLayout::VERTICAL, FlowLayout::CENTER_HORIZONTALLY, FlowLayout::CENTER_VERTICALLY);
  layout_manager_middle->set_borders(5);
  layout_manager_middle->set_padding(5);
  container_middle->setLayoutManager(layout_manager_middle);
  container_middle->setPreferredSize(Widget::UNLIMITED_PREFERRED_SIZE);
  container_outer->add(container_middle);

  Widget* container_inner = new Widget();
  container_inner->setPreferredSize(Widget::UNLIMITED_PREFERRED_SIZE);
  container_middle->add(container_inner);

  container_outer->layout();

  CHECK_EQUAL("Outer widget", GRect, GRect(0, 0, 100, 100), container_outer->getBounds());
  CHECK_EQUAL("Huge middle widget", GRect, GRect(-1073741783, 10, -2147483629, 80), container_middle->getBounds());
  CHECK_EQUAL("Gigantic inner widget", GRect, GRect(10, -1073741783, 2147483647, 2147483647), container_inner->getBounds());

  delete container_outer;
  delete layout_manager_outer;
  delete container_middle;
  delete layout_manager_middle;
  delete container_inner;
}
 


