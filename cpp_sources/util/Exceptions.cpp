//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Exceptions.hpp"

//--------------------------------------------------------------------------------



// #include "OgreErrorDialog.h"
 
// //--------------------------------------------------------------------------------
// // Set termination handlers for different platforms

// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

// void win32StructuredTranslator(unsigned code, EXCEPTION_POINTERS *)
// {
// //   Exception structuredException();
  
// //   EXCEPTION_RECORD const &exception = *(info->ExceptionRecord);
// //   address_ = exception.ExceptionAddress;
// //   code_ = exception.ExceptionCode;
//   ostringstream output;
//   output << "Win32 Structured Exception Code: " << code;

//   throw Exception(Exception::ERR_INTERNAL_ERROR, output.str(), "unknown");
// }

// #endif


// void handleTerminate(void)
// {
//   logEngineFail("Termination handler: uncaught exception!");
  
//   Root::getSingleton().shutdown();
  
//   ErrorDialog* dlg = PlatformManager::getSingleton().createErrorDialog();
  
// //   Exception* e = Exception::getLastException();
  
//   //   if (e)
//   //     dlg->display(e->getFullDescription());
//   //   else
//   //     dlg->display("Unknown");
//   dlg->display("ShortHike term handler");
  
//   // Abort
//   exit(-1);  
// }



// void
// installTermHandler()
// {
// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//   _set_se_translator(win32StructuredTranslator);
// #endif
//   set_terminate(handleTerminate);
// }




//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/TestExceptions", TestExceptions);


void
TestExceptions::assert_handling()
{
  try {
    CHECK("Testing assert failure handling", false);
  }
  catch(SH_Exception& ) {
    return;
  }

  // No idea testing anything if the test facility is broken
  logEngineFail("Error handling mechanism broken. Forced exit.");
  cout << endl << __FILE__ << "(" << __LINE__ << ") : Error handling mechanism broken. Forced exit." << endl;
  exit(-1);
}



