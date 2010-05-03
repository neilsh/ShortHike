//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef DRM_LIMIT_DIALOG
#define DRM_LIMIT_DIALOG

#include "guilib/Dialog.hpp"
#include "guilib/ActionListener.hpp"

class RasterImage;
class TextField;
class Button;
class LoadingDialog;

class LimitDialog : public Dialog, public ActionListener, public Singleton<LimitDialog>
{
public:
  LimitDialog();

  virtual void update(float timeStep);
  void actionPerformed(Widget* source);

  void setQuitMode(bool quitMode);
private:
  bool mQuitMode;

  RasterImage* mBuyMessage;
  Button* mBuyButton;
  Button* mActivateButton;
  Button* mQuitButton;
};

extern bool showLimitDialog(bool quitMode = true);

#endif
