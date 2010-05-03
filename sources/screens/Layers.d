//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Station;
private import Rect;

// game
private import Module;
private import Prototype;
private import Port;

// guilib
private import GUIManager;

// rendering
private import Camera;
private import Mesh;
private import RenderSystem;
private import Texture;
private import StationScreen;

private import Filters;

// --------------------------------------------------------------------------------

const char[][] LAYER_NAMES = [
  "Visual",
  "Energy",
  "Thermal"
];


private enum Layer
{
  VISUAL,
  ENERGY,
  THERMAL
}

// --------------------------------------------------------------------------------

enum FocusType {
  BACKGROUND,
  MODULE,
  PORT
}


private struct StationFocus
{
  FocusType type;
  Module unit;
  int port;

  bool opEqual(StationFocus rhs)
  {
    return cast(bool)(type == rhs.type && unit is rhs.unit && port == rhs.port);
  }  
}


// --------------------------------------------------------------------------------

void doStation(RenderSystem renderSystem, Station station, Rect clip, Layer layer)
{
  renderSystem.enableAlphaBlending(false);

  RenderTypeFilter rTypeFilter = getRenderTypeFilter(layer);
  RenderGradientFilter rGradientFilter = getRenderGradientFilter(layer);

  Module[] modules = station.getModules();
  bool[Prototype] visiblePrototypes;
  Module[] visibleModules;
  
  // Prototype and module visibility check
  for(int iModule = 0; iModule < modules.length; ++iModule) {
    Module currentModule = modules[iModule];
    // Check for module visibility here ..
    visiblePrototypes[currentModule.getPrototype] = true;
    visibleModules ~= currentModule;
  }
  
  // Batched module rendering
  for(int iPrototype = 0; iPrototype < visiblePrototypes.keys.length; ++iPrototype) {
    Prototype prototype = visiblePrototypes.keys[iPrototype];
    switch(rTypeFilter(prototype)) {
    case RenderType.NORMAL:
      renderSystem.setMaterialTexture(prototype.getColorTexture());
      renderSystem.setMesh(prototype.getMesh());
      for(int iModule = 0; iModule < visibleModules.length; ++iModule) {
        Module currentModule = visibleModules[iModule];
        if(currentModule.getPrototype() == prototype)
          renderSystem.renderMesh(currentModule.getTransform());
      }
      break;
    case RenderType.WIREFRAME:
      renderSystem.setMaterialColor(cColor(0.5, 0.5, 0.5, 1), cColor(0, 0, 0, 0), cColor(0.5, 0.5, 0.5));
      renderSystem.setMesh(prototype.getMesh(), true);
      for(int iModule = 0; iModule < visibleModules.length; ++iModule) {
        Module currentModule = visibleModules[iModule];
        if(currentModule.getPrototype() == prototype)
          renderSystem.renderMesh(currentModule.getTransform());
      }
      break;
    case RenderType.GRADIENT:
      renderSystem.setMesh(prototype.getMesh());
      for(int iModule = 0; iModule < visibleModules.length; ++iModule) {
        Module currentModule = visibleModules[iModule];
        renderSystem.setMaterialColor(rGradientFilter(currentModule), cColor(0, 0, 0, 0), cColor(0.5, 0.5, 0.5));
        if(currentModule.getPrototype() == prototype)
          renderSystem.renderMesh(currentModule.getTransform());
      }
      break;
    default:
      assert(CHECK_FAIL("Unknown rendering type"));
      break;
    }
  }  
}

// --------------------------------------------------------------------------------

RenderTypeFilter
getRenderTypeFilter(Layer layer)
{
  switch(layer) {
  case Layer.VISUAL:
    return &visualRenderType;
  case Layer.THERMAL:
    return &thermalRenderType;
  case Layer.ENERGY:
    return &energyRenderType;
//   case Layer.DES_TIME:
//     return &destimeRenderType;
  default:
  }
  assert(CHECK_FAIL("Unknown layer type"));
  return null;
}

RenderGradientFilter
getRenderGradientFilter(Layer layer)
{
  switch(layer) {
  case Layer.VISUAL:
    return &visualRenderGradient;
  case Layer.THERMAL:
    return &thermalRenderGradient;
  case Layer.ENERGY:
    return &energyRenderGradient;
//   case Layer.DES_TIME:
//     return &destimeRenderGradient;
  default:    
  }
  assert(CHECK_FAIL("Unknown layer type"));
  return null;  
}


// --------------------------------------------------------------------------------

private Mesh mPortMesh = null;
private Texture mPortTexture = null;

void doPorts(Station station, Module ignoreModule = null) 
{
  initLayers();
  rRenderSystem.setMesh(mPortMesh);
  rRenderSystem.setMaterialColor(cColor(0, 0, 0.8, 1), cColor(0, 0, 0, 0), cColor(0, 0, 0.5, 1));

  Module[] modules = station.getModules();
  for(int iModule = 0; iModule < modules.length; ++iModule) {
    Module currentModule = modules[iModule];
    if(ignoreModule is currentModule) continue;
    Matrix moduleTransform = currentModule.getTransform();
    Prototype prototype = currentModule.getPrototype();
    
    Port[] ports = prototype.getPorts();
    for(uint iPort = 0; iPort < ports.length; ++iPort) {
      if(!currentModule.isPortConnected(iPort)) {
        Matrix portTransform = createMatrix(ports[iPort].getPosition(), ports[iPort].getOrientation());
        Matrix combinedTransform = portTransform * moduleTransform;
        {
          rRenderSystem.renderMesh(combinedTransform);
        }          
      }
    }
  }
}

// --------------------------------------------------------------------------------

void doPrototype(Prototype prototype, Matrix fullTransform)
{
  rRenderSystem.setMaterialTexture(prototype.getColorTexture());
  rRenderSystem.setMesh(prototype.getMesh(), true);
  rRenderSystem.renderMesh(fullTransform);
}


private Mesh mConnectionMesh = null;
private Texture mConnectionTexture = null;  

void doConnections(Prototype prototype, Matrix fullTransform, Station station, Module ignoreModule = null)
{
  initLayers();
  PortConnection[] connected = station.findConnectedPorts(prototype, fullTransform, ignoreModule);
  rRenderSystem.setMaterialTexture(mConnectionTexture);
  rRenderSystem.setMesh(mConnectionMesh, false, false);
  for(uint iPort = 0; iPort < connected.length; ++iPort) {
    if(null !is connected[iPort].mod) {
      rRenderSystem.renderMesh(prototype.getPort(iPort).getTransform() * fullTransform);
    }
  }
}


// --------------------------------------------------------------------------------
// Picking


StationFocus
pickStationFocus(Station station, Camera camera, State state)
{
  initLayers();
  
  // Pick the nearest module
  StationFocus focus;
  Ray pickRay = camera.getCameraToViewportRay(rGUIManager.getRelativeMouseX(), rGUIManager.getRelativeMouseY());
  float moduleDistance;
  float portDistance;
  Module focusModule = pickModule(station, pickRay, moduleDistance);
  Module portFocusModule;
  int iPort;
  if(state == State.ADD) {
    portFocusModule = pickPort(station, pickRay, iPort, portDistance);
  }
  if(focusModule !is null && portFocusModule !is null) {
    if(moduleDistance < portDistance) {
      focus.type = FocusType.MODULE;
      focus.unit = focusModule;
    }
    else {
      focus.type = FocusType.PORT;
      focus.unit = portFocusModule;
      focus.port = iPort;
    }
  }
  else if(focusModule !is null) {
    focus.type = FocusType.MODULE;
    focus.unit = focusModule;
  }
  else if(portFocusModule !is null) {
    focus.type = FocusType.PORT;
    focus.unit = portFocusModule;
    focus.port = iPort;
  }
  else {
    focus.type = FocusType.BACKGROUND;
  }
  return focus;
}

  

Module
pickModule(Station station, Ray pickRay, out float oDistance)
{
  Module result = null;
  oDistance = float.max;
  foreach(Module mod; station.getModules) {
    Mesh mesh = mod.getPrototype.getMesh;
    float distance;
    Matrix transform = mod.getTransform();
    if(mesh.intersectRayBox(pickRay, transform, distance) &&
       mesh.intersectRayTri(pickRay, transform, distance) &&
       distance < oDistance) {
      result = mod;
      oDistance = distance;
    }
  }
  return result;
}

Module
pickPort(Station station, Ray pickRay, out int oPort, out float oDistance)
{
  Module result = null;
  oPort = -1;
  oDistance = float.max;
  foreach(Module iModule; station.getModules) {
    float distance;
    Matrix transform = iModule.getTransform();
    Port[] ports = iModule.getPrototype.getPorts;
    for(int iPort = 0; iPort < ports.length; iPort++) {
      if(!iModule.isPortConnected(iPort)) {
        Matrix portTransform = createMatrix(ports[iPort].getPosition, ports[iPort].getOrientation());
        Matrix combinedTransform = portTransform * transform;
        if(Layers.mPortMesh.intersectRayBox(pickRay, combinedTransform, distance) &&
           distance < oDistance) {
          result = iModule;
          oPort = iPort;
          oDistance = distance;
        }
      }
    }
  }
  return result;
}


// --------------------------------------------------------------------------------

void initLayers()
{
  if(mPortMesh !is null) return;
  
  const float PORT_SIZE = 2;
  RenderSystem rSystem = rRenderSystem();

  mPortMesh = rSystem.createMesh(true);
  mPortMesh.loadCube(PORT_SIZE);
  mPortMesh.translate(createVector(0, 0, PORT_SIZE * 0.5));
  mPortTexture = rSystem.createTexture();
  mPortTexture.loadSolid(0, 0, 0xcf, 0xff);

  mConnectionMesh = rSystem.createMesh();
  mConnectionMesh.load("assets/gui/ConnectionMarker.shd");
  mConnectionTexture = rSystem.createTexture();
  mConnectionTexture.loadSolid(0, 0, 0xcf, 0xff);
}

