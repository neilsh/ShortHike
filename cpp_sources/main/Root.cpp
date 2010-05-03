//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Root.hpp"
#include "version.hpp"
#include "generated/build_info.hpp"

//--------------------------------------------------------------------------------
// Prints version information about this build of ShortHike

void
printVersionInformation()
{
  logEngineInfo("--------------------------------------------------------------------------------");
  
  // Name and version
  {
    ostringstream nameStream;
    nameStream << " ShortHike (";
#ifdef _DEBUG
    nameStream << "DEBUG ";
#endif

#ifdef BUILD_UNSTABLE
    nameStream << "Unstable)";
#else
    nameStream << "Stable)";
#endif
    nameStream << " " << BUILD_VERSION << " " << BUILD_REVISION;
    logEngineInfo(nameStream.str());
  }  

  logEngineInfo(string("  Build date: ") + BUILD_DATE);
  
//   // Platform
//   {
// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//     logEngineInfo("  Platform: Windows (OGRE_PLATFORM_WIN32)");
// #elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
//     logEngineInfo("  Platform: Mac OS X (OGRE_PLATFORM_APPLE)");
// #elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
//     logEngineInfo("  Platform: Linux (OGRE_PLATFORM_LINUX)");
// #else
//     logEngineWarn("  Platform: UNKNOWN PLATFORM");
// #endif
//   }

//   // Compiler
//   {
// #if OGRE_COMPILER == OGRE_COMPILER_MSVC
//     logEngineInfo("  Compiler: Microsoft Visual C++ (OGRE_COMPILER_MSVC)");
// #elif OGRE_COMPILER == OGRE_COMPILER_GNUC
//     logEngineInfo("  Compiler: GCC (OGRE_COMPILER_GNUC)");
// #elif OGRE_COMPILER == OGRE_COMPILER_BORL
//     logEngineInfo("  Compiler: Borland (OGRE_COMPILER_BORL)");
// #else
//     logEngineWarn("  Compiler: UNKNOWN COMPILER");
// #endif
//   }
  
//   // System Endianess
//   {
// #if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
//     logEngineInfo("  Endian: Little Endian (OGRE_ENDIAN_LITTLE)");
// #else
//     logEngineInfo("  Endian: Big Endian (OGRE_ENDIAN_BIG)");
// #endif
//   }
  
  logEngineInfo("--------------------------------------------------------------------------------");

//   ostringstream ossLicenseInfo;
//   ossLicenseInfo << ConfigManager::getCurrent()->getLicense();
//   logEngineInfo(ossLicenseInfo.str());
}
