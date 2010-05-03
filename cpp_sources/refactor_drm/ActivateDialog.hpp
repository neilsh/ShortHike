//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef DRM_ACTIVATE_DIALOG
#define DRM_ACTIVATE_DIALOG

#include "guilib/Dialog.hpp"
#include "guilib/ActionListener.hpp"

class Label;
class TextField;
class Button;
class LoadingDialog;

class ActivateDialog : public Dialog, public ActionListener, public Singleton<ActivateDialog>
{
public:
  ActivateDialog();

  bool hasActiveLicense(string scriptCommand = "");

  void actionPerformed(Widget* source);
private:
  Label* mInstructions;
  TextField* mEmailField;
  Button* mActivateButton;
  Button* mCancelButton;
  
  LoadingDialog* mLoadingDialog;

  string mScriptCommand;
};


#endif
