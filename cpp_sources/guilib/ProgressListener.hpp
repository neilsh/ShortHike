//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef GUILIB_PROGRESS_LISTENER_HPP
#define GUILIB_PROGRESS_LISTENER_HPP

class ProgressListener
{
public:
  virtual void reportProgress(float progress) = 0;
};

#endif
