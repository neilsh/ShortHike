//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import std.socketstream;
private import std.stream;
private import std.string;
private import std.stdio;
private import std.c.windows.windows;
private import Value;
private import Read;

//--------------------------------------------------------------------------------
// FIXME: All log messages should be stored even if we don't print them with
// our current level. This way we can create a reliable crash report. Same with
// file and line numbers


version(Windows) 
{
  extern(Windows) {
    export void OutputDebugStringA(LPCTSTR lpOutputString);
  }  
}



class LogFile
{
  this(char[] fileName)
  {
    mLogFile = new File(fileName, FileMode.OutNew);
  }

  void writeString(char[] string)
  {
    synchronized(this){
      mLogFile.writeString(string);
      version(Windows) {
        OutputDebugStringA(toStringz(string));
      }
      writef("%s", string);
      mLine ~= string;
      if(string == "\n") {
        mPendingLines ~= mLine;
        mLine = "";
      }
    }
  }  

  char[] getPendingLines()
  {
    char[] lines = mPendingLines;
    mPendingLines = "";
    return lines;
  }  

  char[] mLine;
  char[] mPendingLines;
  File mLogFile;
}

private LogFile mLogFile;

LogFile
rLogFile()
{
  if(mLogFile is null)
    mLogFile = new LogFile("ShortHike.log");
  return mLogFile;
}

// --------------------------------------------------------------------------------

const char endLog = '\0';
const char endl = '\n';
const char[] horizontal_rule = "--------------------------------------------------------------------------------\n";

struct LogInstance
{
  LogInstance
  opShl(char output) 
  {
    switch(output) {
    case endLog:
      rLogFile().writeString("\n");
      break;
    case endl:
      rLogFile().writeString("\n");
      break;
    default:
      rLogFile().writeString(toString(output));
    }    
    return *this;
  }

  LogInstance
  opShl(ubyte output) 
  {
    rLogFile().writeString(toString(output));
    return *this;
  }  
  

  LogInstance
  opShl(char[] output)
  {
    rLogFile().writeString(output);
    return *this;
  }

  LogInstance
  opShl(int output)
  {
    rLogFile().writeString(toString(output));
    return *this;
  }

  LogInstance
  opShl(uint output)
  {
    rLogFile().writeString(toString(output));
    return *this;
  }

  LogInstance
  opShl(long output)
  {
    rLogFile().writeString(toString(output));
    return *this;
  }

  LogInstance
  opShl(float output)
  {
    rLogFile().writeString(toString(output));
    return *this;
  }  

  LogInstance
  opShl(Value output)
  {
    rLogFile().writeString(output.toString());
    return *this;
  }
}


LogInstance
logInfo()
{
  LogInstance log;
  return log;
}

LogInstance
logWarn()
{
  LogInstance log;
  return log;
}

LogInstance
logError()
{
  LogInstance log;
  return log;
}

LogInstance
logFail()
{
  LogInstance log;
  return log;
}

//--------------------------------------------------------------------------------

private int gFrameCount = 0;

bool
rFrame()
{
  if(gFrameCount == 0) return true;
  else return false;
}

void
rIncreaseFrame()
{
  gFrameCount++;
  if(gFrameCount > 100) gFrameCount = 0;
}


// LogInstance
// rLogInfo()
// {
//   LogInstance instance;
//   return instance;
// }


// ostream&
// rLogInfo()
// {
//   static ofstream* logFile = new ofstream("ShortHike.log", ios_base::out | ios_base::trunc);  
//   return *logFile;
// }



// void
// logMessage(LogClass , LogLevel , string message)
// {
//   rLog() << message << endl;
//   rLog().flush();
//   cout << message << endl;
//   OutputDebugString(message.c_str());
//   OutputDebugString("\n");
// }


// enum LogLevel {
//   LOG_LEVEL_INFO,
//   LOG_LEVEL_WARN,
//   LOG_LEVEL_ERROR,
//   LOG_LEVEL_FAIL
// };

// void logMessage(LogClass logClass, LogLevel logLevel, string message);

// // FIXME: Make these macros and have them add file and line info for crash logs
// inline void logEngineInfo(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_INFO, message);}
// inline void logEngineWarn(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_WARN, message);}
// inline void logEngineError(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_ERROR, message);}
// inline void logEngineFail(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_FAIL, message);}

// inline void logGameInfo(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_INFO, message);}
// inline void logGameWarn(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_WARN, message);}
// inline void logGameError(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_ERROR, message);}
// inline void logGameFail(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_FAIL, message);}


// #endif

