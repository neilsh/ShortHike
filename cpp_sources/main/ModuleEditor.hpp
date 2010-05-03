//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_MODULE_EDITOR_HPP
#define SHORTHIKE_MODULE_EDITOR_HPP

#include "util/State.hpp"

class Camera;

class ModuleEditor : public State
{
  friend ModuleEditor* rModuleEditor();
public:
  ModuleEditor();

  virtual void enterState();
  virtual void exitState();
private:
  
  Camera* mCamera;
};

#endif
