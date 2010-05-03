//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private enum State
{
  READY,
  ADD,
  SELECTED
}

// --------------------------------------------------------------------------------

private import Common;
private import Main;
private import Mission;
private import Camera;
private import RenderSystem;
private import GUIManager;
private import Rect;
private import Point;
private import Texture;

private import SkyLayer;
private import Props;
private import Button;
private import Module;
private import Prototype;
private import Station;
private import Mesh;
private import Port;
private import Location;

// local
private import ModuleDock;
private import HUD;
private import Layers;

private import Value;
private import Eval;
private import Library;

// --------------------------------------------------------------------------------

private State mState = State.READY;
private Prototype mPrototype;
private Prototype mPrototypeStack0 = null;
private Prototype mPrototypeStack1 = null;
private Module mSelected;
private Layer mLayer;
private float mPrototypeAngle = 0;
private int mPrototypePort = 0;

// --------------------------------------------------------------------------------

Screen
doStationScreen(float timeStep, RenderSystem renderSystem, Mission mission, Camera camera)
{
  if(null !is mission)
    mission.update(timeStep);
  
  Rect screenRect = new Rect(0, 0, renderSystem.getWidth(), renderSystem.getHeight());
  Station station = mission.getStation;

  doRendering(cID(&doStationScreen, 100), screenRect, mission.getLocation, station, camera);
  doManipulation(cID(&doStationScreen, 200), mission, camera);
  if(mState == State.SELECTED)
    doHUD(cID(&doStationScreen, 300), mSelected, camera.getScreenPosition(mSelected.getPosition));

  // ModuleDock
  Prototype addModule = doModuleDock(cID(&doStationScreen, 1000), mission);
  if(addModule !is null) {
    setAddState(addModule);
  }

  // Run through and draw the Scheme widgets
  foreach(ProcedureValue iWidget; gSchemeWidgets) {
    try {
      apply(rEnv(), iWidget, rsEmptyList());  
    }
    catch(Exception e) {
      logWarn << "Scheme error: " << e.toString() << endl;
    }
  }  
  
  // Keyboard shortcuts
  doKeyboardShortcuts(mission);

  // Control
  return doControl(cID(&doStationScreen, 2000), mission);
}

// --------------------------------------------------------------------------------
// Scheme widgets

private ProcedureValue[] gSchemeWidgets;

Value
sAddWidget(Environment env, Value[] args)
{
  if(args.length == 1) {
    if(args[0].isProcedure()) {
      gSchemeWidgets ~= cast(ProcedureValue)(args[0]);
    }
  }
  return rsUndefined();
}


// --------------------------------------------------------------------------------
// State management

void
setReadyState()
{
  logInfo << "setReadyState" << endl;
  mState = State.READY;
  mPrototype = null;
}

void
setAddState(Prototype prototype)
{
  logInfo << "setAddState" << endl;
  mState = State.ADD;
  mPrototype = prototype;
  mPrototypeAngle = 0;
  mPrototypePort = 0;

  if(mPrototypeStack0 !is mPrototype) {
    mPrototypeStack1 = mPrototypeStack0;
    mPrototypeStack0 = mPrototype;
  }
}

void
setSelectedState(Module selected)
{
  logInfo << "setSelectedState" << endl;
  mState = State.SELECTED;
  mSelected = selected;  
}

void
setLayer(Layer layer)
{
  logInfo << "setLayer: " << LAYER_NAMES[layer] << endl;
  mLayer = layer;
}

// --------------------------------------------------------------------------------

Prototype
getLastPrototype()
{
  if(mState != State.ADD) {
    if(null !is mPrototypeStack0) {
      return mPrototypeStack0;
    }
  }
  else {
    if(null !is mPrototypeStack1) {
      return mPrototypeStack1;
    }
  }
  return null;
}

// --------------------------------------------------------------------------------
// Handles rendering

private void
doRendering(WidgetID id, Rect screenRect, Location location, Station station, Camera camera)
{
  // Render the sky
  Matrix viewMatrix = camera.getViewMatrix();
  viewMatrix.setTranslation(Vector.ZERO);
  rRenderSystem.setView(camera.getProjectionMatrix(), viewMatrix);
  doSkyLayer(rRenderSystem, location, screenRect);
  
  // Render the moon
  rRenderSystem.enableLighting(location.getLightEnv);
  rRenderSystem.enableZBuffer(false);
  rRenderSystem.enableSpecular(false);
  doProp("assets/map/moon.shd", createMatrix(createVector(-12, 3, 18)));
  rRenderSystem.enableZBuffer(true);
  rRenderSystem.enableSpecular(true);

  // Render the station itself
  rRenderSystem.setView(camera.getProjectionMatrix(), camera.getViewMatrix());
  doStation(rRenderSystem, station, screenRect, mLayer);
  if(mState == State.ADD) {
    doPorts(station);
  }
  else if(mState == State.SELECTED) {
    doPorts(station, mSelected);
    doMarker(mSelected, 0.025);
  }
}


// --------------------------------------------------------------------------------
// Handles state and manipulation

private StationFocus mActiveFocus;
private StationFocus mHotFocus;
private float pressDelay;
private const float MOUSE_CLICK_LIMIT = 0.3;

private void
doManipulation(WidgetID id, Mission mission, Camera camera)
{
  Station station = mission.getStation;
  StationFocus focus = pickStationFocus(station, camera, mState);

  // --------------------------------------------------------------------------------
  // Active
  if(rGUIManager.isActive(id)) {
    pressDelay += rGUIManager.timeStep;
    Point pos = rGUIManager().getMousePosition();
    camera.mouseMove(createVector(pos.x, pos.y, 0));
 
    if(rGUIManager.wasKeyReleased(KeyCode.MOUSE1)) {
      // Something was pressed
      if(rGUIManager.isHot(id) && mHotFocus == mActiveFocus && pressDelay < MOUSE_CLICK_LIMIT) {
        if(mActiveFocus.type == FocusType.MODULE) {
          setSelectedState(mActiveFocus.unit);
        }
        else if(mState == State.ADD && mActiveFocus.type == FocusType.PORT) {
          Module target = mPrototype.createModule();
          // FIXME: Add port and angle
          Matrix portAlignTransform = mPrototype.getTransformAgainstPort(mPrototypePort, mHotFocus.unit.getPrototype, mHotFocus.port, mPrototypeAngle);
          target.setTransform(portAlignTransform * mHotFocus.unit.getTransform);
          station.addModule(target);
          if(mission.isAspect(Aspect.CREDITS))
            mission.creditTransaction(-mPrototype.getSimFloat(SimFloat.COST_HARDWARE));
          mission.useActionPoint();        
          // --------------------------------------------------------------------------------
          // Profile: Update number of modules added
          //         ConfigManager::getPlayer()->mModulesAdded++;
        }
        else {
          setReadyState();
        }
      }
      camera.stop();
      rGUIManager.releaseActive(id);
    }
    if(rGUIManager.wasKeyReleased(KeyCode.MOUSE2)) {
      camera.stop();
      rGUIManager.releaseActive(id);
    }
  }

  // --------------------------------------------------------------------------------
  // Hot
  if(rGUIManager.isHot(id)) {
    if(mHotFocus.type == FocusType.PORT && mState == State.ADD) {
      // FIXME: Add port and angle
      Matrix portAlignTransform = mPrototype.getTransformAgainstPort(mPrototypePort, mHotFocus.unit.getPrototype, mHotFocus.port, mPrototypeAngle);
      Matrix fullTransform = portAlignTransform * mHotFocus.unit.getTransform;
      doPrototype(mPrototype, fullTransform);
      doConnections(mPrototype, fullTransform, station);
    }    
    Point pos = rGUIManager().getMousePosition();
    if(rGUIManager.wasKeyPressed(KeyCode.MOUSE1) || rGUIManager.wasKeyPressed(KeyCode.MOUSE2)) {
      rGUIManager.makeActive(id);
      pressDelay = 0;
      mActiveFocus = mHotFocus;
      if(rGUIManager.wasKeyPressed(KeyCode.MOUSE1))
        camera.startOrbit(createVector(pos.x, pos.y, 0));
      else
        camera.startZoom(createVector(pos.x, pos.y, 0));
    }
  }
  rGUIManager.makeHot(id);
  mHotFocus = focus;
}


// --------------------------------------------------------------------------------
// Render a marker around the given module

private Mesh mMarkerMesh;

void doMarker(Module mod, float padding)
{
  if(mod is null) return;
  if(mMarkerMesh is null) {
    mMarkerMesh = rRenderSystem.createMesh();
    mMarkerMesh.loadCube(1);
  }
  AxisAlignedBox box = mod.getPrototype.getMesh.getBoundingBox();
  rRenderSystem.setMesh(mMarkerMesh);
  rRenderSystem.setMaterialColor(cColor(0.5, 0.6, 0.8, 0.5), cColor(0, 0, 0, 0), cColor(0.6, 0.6, 1, 1));
  Matrix transform = box.transform(padding) * mod.getTransform;
  rRenderSystem.enableAlphaBlending(true);
  rRenderSystem.renderMesh(transform);
  rRenderSystem.enableAlphaBlending(false);
}

// --------------------------------------------------------------------------------

private Screen
doControl(WidgetID id, Mission mission)
{
  // DEBUG
  return Screen.STATION;

//   const int CONTROL_WIDTH = 38;
//   const int CONTROL_HEIGHT = 2 * 35 + 3;

//   Rect dstRect = new Rect(0, 0, CONTROL_WIDTH, CONTROL_HEIGHT);

//   // Render background box
//   doTextureBox(id, "assets/gui/DockBackground.png", dstRect, 8, 8, 8, 8);

//   const int CONTROL_BUTTON_SIZE = 32;
//   WidgetID layerMenuID = cID(&doControl, 1010);
//   WidgetID systemMenuID = cID(&doControl, 1020);
//   Rect buttonRect = new Rect(dstRect.x + 3, dstRect.y + 3, CONTROL_BUTTON_SIZE, CONTROL_BUTTON_SIZE);

//   // System button
//   if(doButton(cID(&doControl, 200), cFrameTexture("assets/gui/ControlButtons.png", 4, 4), buttonRect, 4)) {
//     rGUIManager.makeActive(systemMenuID);
//   }
//   buttonRect.y += CONTROL_BUTTON_SIZE + 3;

//   // Layer button
//   if(doButton(cID(&doControl, 10), cFrameTexture("assets/gui/ControlButtons.png", 4, 4), buttonRect)) {
//     rGUIManager.makeActive(layerMenuID);
//   }
//   buttonRect.y += CONTROL_BUTTON_SIZE + 3;

//   // Popup menus


//   const char[][] SYSTEM_MENU_ITEMS = [
//     "Quit"
//   ];  
  
//   int menuItem = doPopup(systemMenuID, SYSTEM_MENU_ITEMS);
//   if(menuItem == 0) rMain().quit();

//   Layer iLayer;
//   if((iLayer = cast(Layer)doPopup(layerMenuID, LAYER_NAMES)) != -1)
//     setLayer(iLayer);

//   return Screen.STATION;
}



// --------------------------------------------------------------------------------

void
doKeyboardShortcuts(Mission mission)
{
  // Shortcut keys for selected state
  if(mState == State.SELECTED) {
    if(mSelected.canRotate()) {
      float angle = 45;
      if(rGUIManager.getKeyModifier() & KEY_MODIFIER_SHIFT)
        angle = angle / 4;
      if(rGUIManager.wasKeyPressed(KeyCode.X)) {
        mSelected.rotate(angle);
      }
      else if(rGUIManager.wasKeyPressed(KeyCode.C)) {    
        mSelected.rotate(-angle);
      }
      else if(rGUIManager.wasKeyPressed(KeyCode.V)) {
        mSelected.flipPort();
      }
    }
    if(rGUIManager.wasKeyPressed(KeyCode.DELETE) && mission.getStation().getModules().length > 1) {
      Module next = mSelected.getFirstConnection();
      mission.getStation.removeModule(mSelected);
      if(next !is null) {
        setSelectedState(next);
      }
      else {
        setReadyState();
      }
    }    
  }
  // Shortcut keys for add state
  else if(mState == State.ADD) {
    float angle = 45;
    if(rGUIManager.getKeyModifier() & KEY_MODIFIER_SHIFT)
      angle = angle / 4;
    if(rGUIManager.wasKeyPressed(KeyCode.X)) {
      mPrototypeAngle += angle;
    }
    else if(rGUIManager.wasKeyPressed(KeyCode.C)) {    
      mPrototypeAngle -= angle;
    }
    else if(rGUIManager.wasKeyPressed(KeyCode.V)) {
      mPrototypePort = (mPrototypePort + 1) % mPrototype.getPorts.length;
    }
  }

  if(rGUIManager.wasKeyPressed(KeyCode.Z)) {
    toggleBuildMode();
  }  
}

void
toggleBuildMode()
{
   Prototype lastPrototype = getLastPrototype();
   if(null !is lastPrototype) {
     setAddState(lastPrototype);
   }
}



