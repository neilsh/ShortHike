//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_LOADING_DIALOG
#define USER_INTERFACE_LOADING_DIALOG

#include "guilib/Dialog.hpp"
#include "guilib/ProgressListener.hpp"

class Label;
class ProgressBar;
class RasterImage;

class LoadingDialog : public Dialog, public ProgressListener
{
public:
  LoadingDialog(string namePrefix);

  void startTask(wstring taskName, float maxTick = 1.0);
  void setTickItem(wstring itemName, float tickPoint);
  void reportProgress(float progress);
  void endTask();
  void setByteProgress(bool byteProgres);
  
  virtual bool isFullScreen() { return true; }
private:
  void updateNumerical();

  float mMaxTick;
  bool mByteProgress;

  Label* taskName;
  Label* itemName;
  Label* mNumericalProgress;
  ProgressBar* progressBar;

  RasterImage* frameDetail;
};


#endif
