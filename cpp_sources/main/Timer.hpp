//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef MAIN_TIMER
#define MAIN_TIMER

class Timer
{
public:
  Timer() {}  

  virtual void start() = 0;
  virtual int check(int fracSec) = 0;    //Enter the fractions of a second you'd like the result in, e.g. 1000 for ms.
};


#endif
