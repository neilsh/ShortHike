//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "List.hpp"
#include "Graphics.hpp"
#include "GUIManager.hpp"

List::List(unsigned columnsP, ListListener *newListener, bool renderSelected)
  : mRenderSelected(renderSelected)
{
  const FontSize MAGIC_FONT_SIZE = 12;
  fontSize = MAGIC_FONT_SIZE;
  borders = 5;
  padding = 6;

  selected = 0;
  pointed = -1;
  listener = newListener;

  columns = columnsP;
  CHECK("Column number is positive", columns > 0);

  columnWidth.resize(columns);
  columnAlignments.resize(columns);

  for(unsigned columnIndex = 0; columnIndex < columns; ++columnIndex) {
    columnWidth[columnIndex] = 1.0f / columns;
    columnAlignments[columnIndex] = LEFT_ALIGN;
  }  

  updateItemDimensions();
}

List::~List()
{
}


int
List::size()
{
  return items.size();
}


void
List::addItem(wstring newString, bool enabled)
{
  vector<wstring> newItem(columns);

  newItem[0] = newString;
  for(unsigned fillIndex = 1; fillIndex < columns; ++fillIndex) {
    newItem[fillIndex] = L"";
  }
  
  addItem(newItem, enabled);
  markDirty();
}


void
List::addItem(vector<wstring> newItem, bool enabled)
{
  CHECK("New item column size matches list column size", newItem.size() == columns);

  items.push_back(newItem);
  itemEnabledState.push_back(enabled);
  
  updateItemDimensions();
  markDirty();
}


void
List::setItem(unsigned num, wstring newString, bool enabled)
{
  vector<wstring> newItem(columns);

  newItem[0] = newString;
  for(unsigned fillIndex = 1; fillIndex < columns; ++fillIndex) {
    newItem[fillIndex] = L"";
  }
  
  setItem(num, newItem, enabled);
  markDirty();
}


void
List::setItem(unsigned num, vector<wstring> newItem, bool enabled)
{
  CHECK("New item column size matches list column size", newItem.size() == columns);
  CHECK("List item index within bounds", num < items.size());
  items[num] = newItem;
  itemEnabledState[num] = enabled;
  updateItemDimensions();
  markDirty();
}


wstring
List::getItem(unsigned num, unsigned itemColumn)
{
  CHECK("Requested column is within column bounds", itemColumn < columns);
  CHECK("List item index within bounds", num < items.size());
  return items[num][itemColumn];
}


bool
List::isItemEnabled(unsigned num)
{
  CHECK("List item index within bounds", num < items.size());
  return itemEnabledState[num];
}



void
List::removeItem(unsigned num)
{
  CHECK("List item index within bounds", num < items.size());
  items.erase(items.begin() + num);
  itemEnabledState.erase(itemEnabledState.begin() + num);
  updateItemDimensions();
  markDirty();
}


void
List::removeAllItems()
{
  items.clear();
  itemEnabledState.clear();
  updateItemDimensions();
  selected = 0;
  pointed = -1;
  markDirty();
}


void
List::setSelected(unsigned num)
{
  selected = num;
  markDirty();
}

void
List::setColumnWidth(unsigned column, Real relativeWidth)
{
  CHECK("Requested column is within column bounds", column < columns);
  columnWidth[column] = relativeWidth;
  updateItemDimensions();
  markDirty();
}

void
List::setColumnAlignment(unsigned column, AlignType newAlignment)
{
  CHECK("Requested column is within column bounds", column < columns);
  columnAlignments[column] = newAlignment;
  markDirty();
}


void
List::updateItemDimensions()
{
  GUIManager* guiRoot = rGUIManager();
  if(guiRoot == NULL) return;
  
  itemDimensions.clear();
  itemDimensions.resize(items.size());
  itemHeights.clear();
  itemHeights.resize(items.size());
  
  int maxWidth = 0;
  int totalHeight = borders * 2;

  for(unsigned itemNum = 0; itemNum < items.size(); ++itemNum) {
    itemDimensions[itemNum].clear();
    itemDimensions[itemNum].resize(columns);
    itemHeights[itemNum] = 0;

    int rowWidth = borders * 2;
    for(unsigned columnNum = 0; columnNum < columns; ++columnNum) {
      wstring currentItem = items[itemNum][columnNum];
      Dimension currentDimension = guiRoot->getStringDimension(fontSize, currentItem);;
      itemDimensions[itemNum][columnNum] = currentDimension;
      if(itemHeights[itemNum] < currentDimension.h) {
        itemHeights[itemNum] = currentDimension.h;
      }
      rowWidth += currentDimension.w + padding;
    }
    rowWidth -= padding;

    totalHeight += itemHeights[itemNum] + padding;
    if(maxWidth < rowWidth) maxWidth = rowWidth;
  }  
  totalHeight -= padding;

  setPreferredSize(maxWidth, totalHeight);

  totalWidthWeight = 0;
  for(unsigned columnIndex = 0; columnIndex < columns; ++columnIndex) {
    totalWidthWeight += columnWidth[columnIndex];
  }
  markDirty();
}


int
List::findItemAtPosition(int y)
{
  int yposition = borders - padding/2;
  for(unsigned itemNum = 0; itemNum < items.size(); ++itemNum) {
    int itemHeight = itemHeights[itemNum];

    if(y >= yposition && y < (yposition + itemHeight + padding)) {
      return itemNum;
    }
    yposition += itemHeights[itemNum] + padding;
  }  
  return -1;
}



bool
List::mouseClickEvent(int button, unsigned clickCount, int , int y)
{
  if(button != 0) return false;

  int yposition = borders - padding/2;
  for(unsigned itemNum = 0; itemNum < items.size(); ++itemNum) {
    int itemHeight = itemHeights[itemNum];

    if(y >= yposition && y < (yposition + itemHeight + padding)
       && itemEnabledState[itemNum] == true) {
      if(listener != NULL) {
        if((clickCount % 2) == 0 && selected == itemNum)
          listener->itemActivated(this, itemNum);
        else
          listener->itemSelected(this, itemNum);
      }
      // FIXME: Sound
//       FSOUND_PlaySound(FSOUND_FREE, GUILib::getSingleton()->selectSound);
      selected = itemNum;
      return true;
    }
    yposition += itemHeights[itemNum] + padding;
  }  
  return false;
}


void
List::mouseMotionEvent(int , int y, float , float , float )
{
  // FIXME: don't fire messages for all mouse movement.
  pointed = findItemAtPosition(y);
  if(pointed != lastPointed) {
    if(listener != NULL)
      listener->itemPointed(this, pointed);
    lastPointed = pointed;
    markDirty();
  }
}

void
List::mouseLeaveEvent()
{
  if(listener != NULL) {
    pointed = -1;
    listener->itemPointed(this, -1);
    lastPointed = -1;
  }
  markDirty();
}


bool
List::keyboardEvent(char keyCode, bool pressed)
{
  if(pressed == false) {
    int minimum = 0;
    int maximum = size() - 1;
    int invalid = -1;

    if(keyCode == VK_DOWN) {
      if(pointed == invalid) {
        pointed = minimum;
      }
      else {
        pointed++;
        if(pointed > maximum) {
          pointed = minimum;
        }       
      }
      markDirty();
      return true;
    }
    else if(keyCode == VK_UP) {
      if(pointed == invalid) {
        pointed = maximum;
      }
      else {
        pointed--;
        if(pointed < minimum) {
          pointed = maximum;
        }       
      }
      markDirty();
      return true;
    }
    else if(keyCode == VK_RETURN) {
      if(pointed != invalid)
        listener->itemActivated(this, pointed);
      return true;
    }    
  }
  return false;
}




void
List::paint(Graphics* graphicsContext)
{
  int yposition = borders;
  graphicsContext->setFontSize(fontSize);
  
  for(unsigned itemNum = 0; itemNum < items.size(); ++itemNum) {
    int itemHeight = itemHeights[itemNum];
    
    if(mRenderSelected && selected == itemNum) {
      if(pointed != -1 && static_cast<unsigned>(pointed) == itemNum)
        graphicsContext->setColor(0.6f, 0.6f, 1.0f);
      else
        graphicsContext->setColor(0.6f, 0.6f, 0.71f);
      graphicsContext->fillRect(2, yposition - padding/2, getWidth() - 5, itemHeight + padding);
    }
    else {
      if(pointed != -1 && static_cast<unsigned>(pointed) == itemNum) {
        graphicsContext->setColor(rGUIManager()->getSkinColor(GUIManager::SKIN_BUTTON_MOUSEOVER_COLOR));
        graphicsContext->fillRect(2, yposition - padding/2, getWidth() - 5, itemHeight + padding);
      }      
    }

    if(itemEnabledState[itemNum] == true) {
      graphicsContext->setColor(1.0f, 1.0f, 1.0f);
    }
    else {
      graphicsContext->setColor(0.6f, 0.6f, 0.6f);
    }
    
    int xposition = borders;
    for(unsigned columnNum = 0; columnNum < columns; ++columnNum) {
      wstring currentItem = items[itemNum][columnNum];
      Dimension currentDimension = itemDimensions[itemNum][columnNum];
      int currentWidth = static_cast<int>(getWidth() * columnWidth[columnNum] / totalWidthWeight);

      int xdelta;
      if(columnAlignments[columnNum] == RIGHT_ALIGN) {
        xdelta = currentWidth - currentDimension.w;
      }
      else if(columnAlignments[columnNum] == MIDDLE_ALIGN) {
        xdelta = (currentWidth - currentDimension.w) / 2;
      }
      else {
        xdelta = 0;
      }    
      
      graphicsContext->drawString(currentItem, xposition + xdelta, yposition);

      xposition += currentWidth;
    }
    yposition += itemHeight + padding;
  }
}

//================================================================================
// TestList unit test

TEST_FIXTURE_DEFINITION("unit/guilib/List", TestList);

void
TestList::list_enabled()
{
  List* target = new List();
  target->addItem(L"TestItem1", false);
  target->addItem(L"TestItem2", true);
  CHECK("Inital validity of Item1 enabled", target->isItemEnabled(0) == false);
  CHECK("Inital validity of Item2 enabled", target->isItemEnabled(1) == true);

  target->removeAllItems();
  target->addItem(L"TestItem1", true);
  target->addItem(L"TestItem2", false);
  CHECK("Enable validity of Item1 after removeAllItems", target->isItemEnabled(0) == true);
  CHECK("Enable validity of Item2 after removeAllItems", target->isItemEnabled(1) == false);

  target->removeItem(1);
  target->removeItem(0);
  target->addItem(L"TestItem1", false);
  target->addItem(L"TestItem2", true);
  CHECK("Enable validity of Item1 after removeItem", target->isItemEnabled(0) == false);
  CHECK("Enable validity of Item2 after removeItem", target->isItemEnabled(1) == true);
}


