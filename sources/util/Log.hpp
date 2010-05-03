//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef UTIL_LOG_HPP
#define UTIL_LOG_HPP

enum LogClass {
  LOG_CLASS_ENGINE,
  LOG_CLASS_GAME
};

enum LogLevel {
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FAIL
};

void logMessage(LogClass logClass, LogLevel logLevel, string message);

// FIXME: Make these macros and have them add file and line info for crash logs
inline void logEngineInfo(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_INFO, message);}
inline void logEngineWarn(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_WARN, message);}
inline void logEngineError(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_ERROR, message);}
inline void logEngineFail(string message) {logMessage(LOG_CLASS_ENGINE, LOG_LEVEL_FAIL, message);}

inline void logGameInfo(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_INFO, message);}
inline void logGameWarn(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_WARN, message);}
inline void logGameError(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_ERROR, message);}
inline void logGameFail(string message) {logMessage(LOG_CLASS_GAME, LOG_LEVEL_FAIL, message);}


#endif
