//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_PROFILE_HPP
#define WIDGETS_PROFILE_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Dialog.hpp"

class Label;
class Button;

class PlayerProfile : public Dialog, public ActionListener, public Singleton<PlayerProfile>
{
public:
  PlayerProfile();

  virtual void show();
  
  virtual void actionPerformed(Widget* source);
private:
  Label* mHeading;
  Label* mProfile;
  Button* mCloseButton;
};



#endif
