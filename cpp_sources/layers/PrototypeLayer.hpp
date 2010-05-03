//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_PART_LAYER_HPP
#define LAYERS_PART_LAYER_HPP

#include "guilib/Widget.hpp"
#include "guilib/ActionListener.hpp"

class Prototype;
class Frame;
class Label;
class Button;
class RasterImage;
class Mesh;

class PrototypeLayer : public Widget, public ActionListener
{
  friend PrototypeLayer* rPrototypeLayer();
public:
  virtual void update(float time);
  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);
  virtual void actionPerformed(Widget* source);
private:
  PrototypeLayer();
  void updateMeshInfo();
  void clear();

  Prototype* mPrototype;
  string mFileName;

  Mesh* mNormalMesh;
  int mCurrentPort;
  
  bool mRenderNormals;
  bool mRenderTexture;
  bool mRenderWireframe;

  // --------------------
  // Right Panel

  Frame* mRightPanel;

  Button* mNewPrototypeButton;
  Button* mLoadPrototypeButton;
  Button* mSavePrototypeButton;
  Button* mSaveAsPrototypeButton;

  Button* mImportOBJButton;
  Button* mImportSSMButton;

  Button* mImportTGAButton;
  Button* mImportPNGButton;
  Button* mImportIconButton;  

  Button* mToggleNormalsButton;
  Button* mToggleTextureButton;
  Button* mToggleWireframeButton;
  Button* mQuitButton;

  // --------------------
  // Mesh Stat panel
  Frame* mStatPanel;
  Label* mStatLabel;

  Frame* mIconPanel;
  RasterImage* mIconImage;

  // --------------------
  // Port Panel

  Frame* mPortPanel;
  Label* mPortNumLabel;
  Label* mPortPosLabel;

  Button* mPortNextButton;
  Button* mPortPrevButton;
  Button* mPortAddButton;
  Button* mPortRemoveButton;

  Button* mPortMovXAddButton;
  Button* mPortMovXSubButton;
  Button* mPortMovYAddButton;
  Button* mPortMovYSubButton;
  Button* mPortMovZAddButton;
  Button* mPortMovZSubButton;

  Button* mPortRotXAddButton;
  Button* mPortRotXSubButton;
  Button* mPortRotYAddButton;
  Button* mPortRotYSubButton;
  Button* mPortRotZAddButton;
  Button* mPortRotZSubButton;

  Mesh* mPortMesh;
};


#endif
