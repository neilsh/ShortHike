//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "OrbitMap.hpp"
#include "Orbit.hpp"
#include "guilib/Graphics.hpp"
#include "guilib/FrameBackground.hpp"

//--------------------------------------------------------------------------------

const float TIME_MULTIPLIER = 0.20f;
const float PLOT_INTERVAL = 30.0f;
const int PLOT_HISTORY = 180;

//--------------------------------------------------------------------------------

OrbitMap::OrbitMap(Orbit* _orbitScreen, string imageName)
  : orbitScreen(_orbitScreen), RasterImage(imageName), phase(0.0), drawTimer(0)
{  
}


//--------------------------------------------------------------------------------

void
OrbitMap::paint(Graphics* gfxContext)
{
  RasterImage::paint(gfxContext);

  ColorType colorMarkerStart(43, 108, 35);
  ColorType colorMarkerEnd(31, 78, 25, 180);

  float invPlotSize = 1.0 / plot.size();
  float markerPos = 0;
  for(list<Vector2>::const_iterator markerI = plot.begin(); markerI != plot.end(); ++markerI) {
    const Vector2& marker = *markerI;
    gfxContext->setColor(colorMarkerStart.gradient(colorMarkerEnd, markerPos * invPlotSize));
    gfxContext->markerPixel(marker[0], marker[1]);
    markerPos += 1.0f;
  }
  
  float phaseGradient;
  if(phase > (Math::HALF_PI - 0.2))
    phaseGradient = Math::Sin(phase);
  else
    phaseGradient = 0;
  
  ColorType colorPositionLow(48, 133, 31);
  ColorType colorPositionHigh(148, 249, 116);

  if(phaseGradient < 0) phaseGradient = 0;

  Vector2 position = plotCurrentPosition();
  gfxContext->setColor(colorPositionLow.gradient(colorPositionHigh, phaseGradient));
  gfxContext->fillEllipse(position[0], position[1], 2, 2);  
}

//--------------------------------------------------------------------------------
// Pulse

void
OrbitMap::update(Time deltaT)
{
  phase += deltaT * TIME_MULTIPLIER;
  if(phase > Math::TWO_PI) phase -= Math::TWO_PI;
  if(phase > (Math::HALF_PI - 0.2) && phase < (Math::PI))
    drawTimer += deltaT;
  
  const float MAGIC_DRAW_INTERVAL = 0.5;
  if(drawTimer > MAGIC_DRAW_INTERVAL) {
    markDirty();
    drawTimer = 0;
  }

  plotTimer += deltaT;
  if(plotTimer > PLOT_INTERVAL) {
    while(plot.size() >= PLOT_HISTORY) {
      plot.pop_back();
    }
    plot.push_front(plotCurrentPosition());
    plotTimer = 0;
  }
}

//--------------------------------------------------------------------------------

void
OrbitMap::rePlot()
{
  plot.clear();
  float secondsAgo = PLOT_HISTORY * PLOT_INTERVAL;
  for(int step = 0; step < PLOT_HISTORY; ++step) {
    plot.push_front(plotPosition(orbitScreen->getLongitude(secondsAgo), orbitScreen->getLatitude(secondsAgo)));
    secondsAgo -= PLOT_INTERVAL;
  }  
  plot.push_front(plotCurrentPosition());
}


Vector2
OrbitMap::plotCurrentPosition()
{
  return plotPosition(orbitScreen->getLongitude(), orbitScreen->getLatitude());
}


Vector2
OrbitMap::plotPosition(Degree longitude, Degree latitude)
{
  float xPos = (longitude.valueDegrees() / 180.0f + 1) * ((getWidth() - getPadding()) * 0.5) + getPadding() * 0.5;
  float yPos = (-latitude.valueDegrees() / 90.0f + 1) * ((getHeight() - getPadding()) * 0.5) + getPadding() * 0.5;
  return Vector2(xPos, yPos);
}
