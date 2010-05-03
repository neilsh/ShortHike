//--------------------------------------------------------------------------------
//
//  ShortHike
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef GUILIB_USER_EVENT_LISTENER_HPP
#define GUILIB_USER_EVENT_LISTENER_HPP

#pragma warning(disable: 4100)

class UserEventListener
{
public:
  virtual bool keyboardEvent(char keyCode, bool pressed) {return false;}
  virtual bool characterEvent(wchar_t character) {return false;}
  virtual bool mouseDownEvent(int button, int x, int y) {return false;}
  virtual bool mouseUpEvent(int button, int x, int y) {return false;}
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y) {return false;}
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel) {}
  virtual void mouseEnterEvent() {}
  virtual void mouseLeaveEvent() {}
};

#endif
