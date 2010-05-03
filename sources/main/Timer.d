//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

class Timer
{
  abstract void start();
  abstract long check(long fracSec);    //Enter the fractions of a second you'd like the result in, e.g. 1000 for ms.
};
