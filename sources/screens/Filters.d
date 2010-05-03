//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;

// game
private import Module;
private import Prototype;
private import Port;

// renderSystem
private import RenderSystem;

// --------------------------------------------------------------------------------

enum RenderType {
  NORMAL,
  GRADIENT,
  WIREFRAME
}


typedef RenderType function(Prototype prototype) RenderTypeFilter;
typedef Color function(Module prototype) RenderGradientFilter;

// --------------------------------------------------------------------------------

private static Color COLOR_PRODUCER_ONLINE;
private static Color COLOR_PRODUCER_OFFLINE;
private static Color COLOR_CONSUMER_ONLINE;
private static Color COLOR_CONSUMER_OFFLINE;

static this()
{
  COLOR_PRODUCER_ONLINE = cColor(0, 0, 1);
  COLOR_PRODUCER_OFFLINE = cColor(0.5, 0, 0.8);
  COLOR_CONSUMER_ONLINE = cColor(0.5, 1, 0.5);
  COLOR_CONSUMER_OFFLINE = cColor(1, 0.2, 0.2);
}

// --------------------------------------------------------------------------------
// Visual layer

RenderType
visualRenderType(Prototype prototype)
{
  return RenderType.NORMAL;
}

Color
visualRenderGradient(Module target)
{
  return Color.WHITE;
}


// --------------------------------------------------------------------------------

private Color
onlineColor(float production, bool online, bool resourceOnline)
{
  if(production < 0) {
     if(resourceOnline)
       return COLOR_CONSUMER_ONLINE;
     else
       return COLOR_CONSUMER_OFFLINE;
  }
  else {
    if(online)
      return COLOR_PRODUCER_ONLINE;
    else
      return COLOR_PRODUCER_OFFLINE;
  }
}

// --------------------------------------------------------------------------------
// Energy layer

RenderType
energyRenderType(Prototype prototype)
{
  float energy = prototype.getSimFloat(SimFloat.ENERGY);
  if(energy != 0) {
    return RenderType.GRADIENT;
  }
  else {
    return RenderType.WIREFRAME;
  }  
}

Color
energyRenderGradient(Module target)
{
  return onlineColor(target.getPrototype.getSimFloat(SimFloat.ENERGY), target.isOnline, target.isEnergyOnline);
}

// --------------------------------------------------------------------------------
// Thermal layer

RenderType
thermalRenderType(Prototype prototype)
{
  float thermal = prototype.getSimFloat(SimFloat.THERMAL);
  if(thermal != 0) {
    return RenderType.GRADIENT;
  }
  else {
    return RenderType.WIREFRAME;
  }  
}

Color
thermalRenderGradient(Module target)
{
  return onlineColor(target.getPrototype.getSimFloat(SimFloat.THERMAL), target.isOnline, target.isThermalOnline);
}

// --------------------------------------------------------------------------------
// DES Iterations, gives number of DES iterations

// RenderType
// desIterationsRenderType(Prototype prototype)
// {
//   return RenderType.GRADIENT;  
// }

// Color
// desIterationsGradient(Module target)
// {
  
//   return
// }
