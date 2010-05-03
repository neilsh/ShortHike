//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Log.hpp"

//--------------------------------------------------------------------------------
// FIXME: All log messages should be stored even if we don't print them with
// our current level. This way we can create a reliable crash report. Same with
// file and line numbers
// FIXME: add operator << support so that the log can be used as a stream.

ostream&
rLog()
{
  static ofstream* logFile = new ofstream("ShortHike.log", ios_base::out | ios_base::trunc);  
  return *logFile;
}



void
logMessage(LogClass , LogLevel , string message)
{
  rLog() << message << endl;
  rLog().flush();
  cout << message << endl;
  OutputDebugString(message.c_str());
  OutputDebugString("\n");
}


