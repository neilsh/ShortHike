//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "ProgressBar.hpp"
#include "Graphics.hpp"
#include "GUIManager.hpp"

//--------------------------------------------------------------------------------

ProgressBar::ProgressBar()
  : Widget(), maxValue(0), currentValue(0)
{
}

//--------------------------------------------------------------------------------
  
void
ProgressBar::setMax(float _maxValue)
{
  maxValue = _maxValue;
  if(maxValue < 0) maxValue = 0;
}

void
ProgressBar::setCurrent(float _currentValue)
{
  currentValue = clamp(_currentValue, 0.0f, maxValue);
  markDirty();
}

  
void
ProgressBar::paint(Graphics* gfxContext)
{
  int width = getWidth();
  int height = getHeight();
  
  int barWidth = (maxValue == 0) ? 0 : (currentValue / maxValue) * width;

  gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_BUTTON_DISABLED_COLOR));
  gfxContext->fillRoundedRect(0, 0, width, height);

  if(barWidth > 0) {
    gfxContext->setColor(rGUIManager()->getSkinColor(GUIManager::SKIN_PROGRESSBAR_COLOR));
    gfxContext->fillRoundedRect(0, 0, barWidth, height);
  }
  
  gfxContext->drawBevelRect(0, 0, width, height);
}

