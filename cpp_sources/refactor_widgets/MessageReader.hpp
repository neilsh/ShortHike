//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_MESSAGE_READER_HPP
#define WIDGETS_MESSAGE_READER_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Dialog.hpp"
#include "guilib/List.hpp"

class List;
class Label;
class Button;

class MessageReader : public Dialog, public ActionListener, public ListListener, public Singleton<MessageReader>
{
public:
  MessageReader();
  
  void selectNewestMessage();

  virtual void update(float deltaT);
  virtual void actionPerformed(Widget* source);
  virtual void itemSelected(Widget* source, unsigned num);
private:
  List* mMessageList;
  Label* mMessagePane;

  Button* mCloseButton;
};



#endif
