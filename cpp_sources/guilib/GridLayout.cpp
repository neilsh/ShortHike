//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
// www.shorthik.com
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "GridLayout.hpp"
#include "Widget.hpp"


//--------------------------------------------------------------------------------


GridLayout::GridLayout(Widget *container, int layout_width, int layout_height)
{
  layout_total_width = layout_width;
  layout_total_height= layout_height;
  parent_container = container;

  borders = 0;
  padding = 0;

  parent_container->setLayoutManager(this);  
}


GridLayout::~GridLayout()
{
}


//--------------------------------------------------------------------------------


void
GridLayout::add(Widget *new_child, int layout_x, int layout_y, int layout_width, int layout_height)
{
  if(new_child == NULL) return;

  CHECK("New GridLayout child withing bound", 
        layout_x >=0 && layout_width > 0 && (layout_x + layout_width) <= layout_total_width &&
        layout_y >= 0 && layout_height > 0 && (layout_y + layout_height) <= layout_total_height);
  children.push_back(Item(new_child, layout_x, layout_y, layout_width, layout_height));    
  parent_container->add(new_child);
}


void
GridLayout::doLayout(Widget *container)
{
  CHECK("Are we requested to layout our parent container", container == parent_container);

  int container_width = container->getWidth();
  int container_height = container->getHeight();
  for(vector<Item>::const_iterator childI = children.begin(); childI != children.end(); ++childI) {
    Item current_item = *childI;
    Widget *current_child = current_item.child_widget;
    
    current_child->setPosition(get_x_position(container_width, current_item.layout_x),
                               get_y_position(container_height, current_item.layout_y));

    current_child->setSize(get_width(container_width, current_item.layout_width),
                           get_height(container_height, current_item.layout_height));
  }  
}


//--------------------------------------------------------------------------------

int
GridLayout::get_x_position(int container_width, int layout_xpos)
{
  int xposition = layout_xpos * (container_width - borders * 2 - padding) / layout_total_width;
  return borders + padding + xposition;
}


int
GridLayout::get_y_position(int container_height, int layout_ypos)
{
  int yposition = layout_ypos * (container_height - borders * 2 - padding) / layout_total_height;
  return borders + padding + yposition;
}


int
GridLayout::get_width(int container_width, int layout_width)
{
  int cell_and_padding_width = (container_width - borders * 2 - padding);
  return layout_width * cell_and_padding_width / layout_total_width - padding;
}


int
GridLayout::get_height(int container_height, int layout_height)
{
  int cell_and_padding_height = (container_height - borders * 2 - padding);
  return layout_height * cell_and_padding_height / layout_total_height - padding;
}

//--------------------------------------------------------------------------------


Dimension
GridLayout::preferredLayoutSize(Widget*)
{
  return Widget::UNLIMITED_PREFERRED_SIZE;
}
 


//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/guilib/GridLayout", Test_GridLayout);

  
void
Test_GridLayout::setUp()
{
  container = new Widget();
  layout_manager = new GridLayout(container, 4, 4);
  layout_manager->set_borders(5);
  layout_manager->set_padding(5);

  child1 = new Widget();
  child1->setPreferredSize(10, 50);
  layout_manager->add(child1, 1, 0, 3, 1);

  child2 = new Widget();
  child2->setPreferredSize(150, 150);
  layout_manager->add(child2, 1, 1);

  child3 = new Widget();
  child3->setPreferredSize(0, 0);
  layout_manager->add(child3, 1, 2, 1, 2);

  child4 = new Widget();
  child4->setPreferredSize(50, 10);
  layout_manager->add(child4, 0, 2);

  child5 = new Widget();
  child5->setPreferredSize(200, 200);
  layout_manager->add(child5, 2, 2, 2, 2);
}


void
Test_GridLayout::tearDown()
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
Test_GridLayout::bounds_calculation()
{
  int container_width = 75;
  int container_height = 75;
  container->setSize(container_width, container_height);

  CHECK_EQUAL("X position 1", int, 10, layout_manager->get_x_position(container_width, 0));
  CHECK_EQUAL("X position 2", int, 25, layout_manager->get_x_position(container_width, 1));
  CHECK_EQUAL("X position 3", int, 40, layout_manager->get_x_position(container_width, 2));
  CHECK_EQUAL("X position 4", int, 55, layout_manager->get_x_position(container_width, 3));

  CHECK_EQUAL("Y position 1", int, 10, layout_manager->get_y_position(container_height, 0));
  CHECK_EQUAL("Y position 2", int, 25, layout_manager->get_y_position(container_height, 1));
  CHECK_EQUAL("Y position 3", int, 40, layout_manager->get_y_position(container_height, 2));
  CHECK_EQUAL("Y position 4", int, 55, layout_manager->get_y_position(container_height, 3));

  CHECK_EQUAL("Width 1", int, 10, layout_manager->get_width(container_width, 1));
  CHECK_EQUAL("Width 2", int, 25, layout_manager->get_width(container_width, 2));
  CHECK_EQUAL("Width 3", int, 40, layout_manager->get_width(container_width, 3));
  CHECK_EQUAL("Width 4", int, 55, layout_manager->get_width(container_width, 4));

  CHECK_EQUAL("Height 1", int, 10, layout_manager->get_height(container_height, 1));
  CHECK_EQUAL("Height 2", int, 25, layout_manager->get_height(container_height, 2));
  CHECK_EQUAL("Height 3", int, 40, layout_manager->get_height(container_height, 3));
  CHECK_EQUAL("Height 4", int, 55, layout_manager->get_height(container_height, 4));

  Widget* test_widget = new Widget();
  GridLayout* dense_layout = new GridLayout(test_widget, 100, 50);
  dense_layout->set_borders(1);
  dense_layout->set_padding(1);

  int dense_container_width = 203;
  int dense_container_height = 103;
  test_widget->setSize(dense_container_width, dense_container_height);
  CHECK_EQUAL("Corner x", int, 200, dense_layout->get_x_position(dense_container_width, 99));
  CHECK_EQUAL("Corner y", int, 100, dense_layout->get_y_position(dense_container_height, 49));
  CHECK_EQUAL("Corner width", int, 199,  dense_layout->get_width(dense_container_width, 100));
  CHECK_EQUAL("Corner height", int, 99,  dense_layout->get_height(dense_container_height, 50));

  delete dense_layout;
  delete test_widget;
}


void
Test_GridLayout::padded_layout()
{
  container->setSize(75, 75);
  container->layout();
  
  CHECK_EQUAL("Child 1 bounds", GRect, GRect(25, 10, 40, 10), child1->getBounds());
  CHECK_EQUAL("Child 2 bounds", GRect, GRect(25, 25, 10, 10), child2->getBounds());
  CHECK_EQUAL("Child 3 bounds", GRect, GRect(25, 40, 10, 25), child3->getBounds());
  CHECK_EQUAL("Child 4 bounds", GRect, GRect(10, 40, 10, 10), child4->getBounds());
  CHECK_EQUAL("Child 5 bounds", GRect, GRect(40, 40, 25, 25), child5->getBounds());
}
 




