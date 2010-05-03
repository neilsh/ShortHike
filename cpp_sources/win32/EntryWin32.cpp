//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include <Wininet.h>
#include <DbgHelp.h>

#include "main/Entry.hpp"
#include "main/Main.hpp"
#include "PlatformWin32.hpp"
#include "StackWalker.hpp"

#include "main/Root.hpp"
#include "main/Timer.hpp"
#include "resource.h"
#include "rendering/RenderSystem.hpp"
#include "guilib/GUIManager.hpp"
#include "version.hpp"
#include "generated/build_info.hpp"

#pragma warning(disable: 4100)

//--------------------------------------------------------------------------------

string userExtraMessage;
string userEmail;

string
getDialogString(HWND hwnd, int item)
{
  string dialogString = "";

  int len = GetWindowTextLength(GetDlgItem(hwnd, item));
  if(len > 0) {
    char* buf;
    
    buf = (char*)GlobalAlloc(GPTR, len + 1);
    GetDlgItemText(hwnd, item, buf, len + 1);
          
    dialogString = buf;
    
    GlobalFree((HANDLE)buf);
  }
  return dialogString;
}


BOOL CALLBACK CrashDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message) {
  case WM_INITDIALOG:    
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), "SHORTHIKEICON"));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), "SHORTHIKEICON"));

    // Center myself
    int x, y, screenWidth, screenHeight;
    RECT rcDlg;
    GetWindowRect(hwnd, &rcDlg);
    screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

    x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
    y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);
    
    MoveWindow(hwnd, x, y, (rcDlg.right - rcDlg.left),
               (rcDlg.bottom - rcDlg.top), TRUE);

    return TRUE;
  case WM_COMMAND:
    switch(LOWORD(wParam))
      {
      case IDOK:
        userExtraMessage = getDialogString(hwnd, 1002);
        userEmail = getDialogString(hwnd, 1006);
        EndDialog(hwnd, IDOK);
        break;
      case IDCANCEL:
        EndDialog(hwnd, IDCANCEL);
        break;
      }
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------------------------

// Args:
//              ScoutDefaultMessage - Message to return to app if none is found in the database
//              ScoutUserName - User to create new bugs under
//              ScoutProject - Project to put bug into
//              ScoutArea - Area to put bug into
//              Description - Error number and text description which is unique to error (e.g. "AppName:Version:Function:LineNumber") (no longer than 255 chars)
//              Email - email address for contact
//              Extra - any extra info included that added by the user
//              FriendlyResponse - set to "1" to return HTML instead of XML

// Add Minidump as encoded?
void
reportBug(string errorInfo, string extraInformation, EXCEPTION_POINTERS* exceptionPointers = NULL)
{
  string bugID = string("V") + BUILD_VERSION + " " + errorInfo;
  int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(101), NULL, CrashDlgProc);
  if(ret == IDOK){
    // Write minidump
    if(exceptionPointers != NULL) {
      MINIDUMP_EXCEPTION_INFORMATION miniDumpExcept;
      miniDumpExcept.ThreadId = GetCurrentThreadId();
      miniDumpExcept.ExceptionPointers = exceptionPointers;
      miniDumpExcept.ClientPointers = FALSE;
      HANDLE dumpFile = CreateFile("ShortHikeCrash.dmp", FILE_WRITE_DATA, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if(FALSE == MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile,
                                    MiniDumpNormal, &miniDumpExcept, NULL, NULL))
        logEngineError("Error writing dump");
  
      CloseHandle(dumpFile);
      logEngineInfo("MiniDump written to ShortHikeCrash.dmp");
    }

    // Start building the POST request
    ostringstream ossExtraFirst;
    ossExtraFirst << "Build date: " << BUILD_DATE << "\n" << "Build revision: " << BUILD_REVISION << "\n\n";
    ossExtraFirst << extraInformation << "\n" << "User message:\n" << userExtraMessage << "\n";

    // Log file inclusion not supported as upstreams has extra size limits.
    ossExtraFirst << "\nLog:\n";
    ifstream logFile;
    logFile.open("ShortHike.log", ios_base::in);
    while(false == logFile.eof()) {
      string logLine;
      getline(logFile, logLine);
      ossExtraFirst << logLine << "\n";
    }
    logFile.close();

    // Replace all occurences of endl with "\r\n"
    ostringstream ossExtra;
    istringstream issExtraFirst(ossExtraFirst.str());
    while(false == issExtraFirst.eof()) {
      string line;
      getline(issExtraFirst, line);
      ossExtra << line << "\r\n";
    }
    
    HINTERNET internetBase = InternetOpen("ShortHike", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    if(NULL == internetBase) return;
    
    HINTERNET serverConnection = InternetConnect(internetBase, "secure.shorthike.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                                                 INTERNET_SERVICE_HTTP, NULL, NULL);  
    if(NULL == serverConnection) return;
    
    HINTERNET bugRequest = HttpOpenRequest(serverConnection, "POST", "/fogbugz/scoutSubmit.php?ScoutUserName=Kai%20Backman&ScoutProject=Player%20Support&ScoutArea=Crash%20Reports", NULL, NULL, NULL,
                                           INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, NULL);
    if(NULL == bugRequest) return;

    string boundary = "---------------------------1d7fc0e8b9a7be975043c4543975a113";
    string contentTypeHeader = "Content-Type: multipart/form-data; boundary=" + boundary;
    HttpAddRequestHeaders(bugRequest, contentTypeHeader.c_str(), (DWORD)-1L, HTTP_ADDREQ_FLAG_ADD);    
    
    string requestBody;
    requestBody.append("--" + boundary + "\r\n");
    requestBody.append("Content-Disposition: form-data; name=\"Description\"\r\n"); 
    requestBody.append("Content-Type: application/octet-stream\r\n");
    requestBody.append("\r\n");
    requestBody.append(bugID + "\r\n");

    requestBody.append("--" + boundary + "\r\n");
    requestBody.append("Content-Disposition: form-data; name=\"Email\"\r\n"); 
    requestBody.append("Content-Type: application/octet-stream\r\n");
    requestBody.append("\r\n");
    requestBody.append(userEmail + "\r\n");

    requestBody.append("--" + boundary + "\r\n");
    requestBody.append("Content-Disposition: form-data; name=\"Extra\"\r\n"); 
    requestBody.append("Content-Type: application/octet-stream\r\n");
    requestBody.append("\r\n");
    requestBody.append(ossExtra.str() + "\r\n");

    LPVOID optional = (LPVOID)(requestBody.data());
    DWORD optionalLength = (DWORD)requestBody.size();

    if(FALSE == HttpSendRequest(bugRequest, NULL, 0, optional, optionalLength)) return;
    
    InternetCloseHandle(bugRequest);
    InternetCloseHandle(serverConnection);
    InternetCloseHandle(internetBase);    

    MessageBox(NULL, "Thank you for your crash report.\n\n -Kai" , "ShortHike crash report delivered",
               MB_OK | MB_ICONINFORMATION);
  }
}



//--------------------------------------------------------------------------------
// Needs to be a separate function as structured and C++ can't mix in the same
// function

#pragma warning(disable: 4127)

void
main2()
{
  // C++ exception handling. We only throw exceptions when it really hurts
//   try {
    logEngineInfo("ShortHike started");
    printVersionInformation();    
    rMain()->mainLoop();    
//   }
  // This usually provides enough information about the crash, no need to dump
//   catch( SH_Exception&) {
//     ostringstream errorStream;
//     errorStream << e.getFullDescription() << endl;
//     logEngineError(errorStream.str());
//     ostringstream ossIdentifier;
//     ossIdentifier << e.getFile() << " line " << e.getLine();
//     reportBug(ossIdentifier.str(), e.getFullDescription());
//   }
}

//--------------------------------------------------------------------------------
// We run this in the __except filter to avoid copying stuff. This creates a 
// improved log message and a MiniDump for much better error handling and fixing.

void logStructuredException(EXCEPTION_POINTERS* exceptionPointers);

//   // Entry for each Callstack-Entry
//   typedef struct CallstackEntry
//   {
//     DWORD64 offset;  // if 0, we have no valid entry
//     CHAR name[STACKWALK_MAX_NAMELEN];
//     CHAR undName[STACKWALK_MAX_NAMELEN];
//     CHAR undFullName[STACKWALK_MAX_NAMELEN];
//     DWORD64 offsetFromSmybol;
//     DWORD offsetFromLine;
//     DWORD lineNumber;
//     CHAR lineFileName[STACKWALK_MAX_NAMELEN];
//     DWORD symType;
//     LPCSTR symTypeString;
//     CHAR moduleName[STACKWALK_MAX_NAMELEN];
//     DWORD64 baseOfImage;
//     CHAR loadedImageName[STACKWALK_MAX_NAMELEN];
//   } CallstackEntry;

//   typedef enum CallstackEntryType {firstEntry, nextEntry, lastEntry};

//c:\work\shorthike\sources\main\main.cpp (603): Main::mainLoop

class MyStackWalker : public StackWalker
{
public:
  MyStackWalker()
    : StackWalker(), mIdentifier("")
  {
  }
  
  virtual void
  OnOutput(LPCSTR szText)
  {
    string line(szText);
    replace(line.begin(), line.end(), '\n',' ');
    logEngineError(line);
  }

  virtual void
  OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
  {
    if ((eType != lastEntry) && (entry.offset != 0)) {
      ostringstream ossEntry;
      ostringstream ossIdentifier;
      string entryName;
      
      // Find entry name
      if (entry.name[0] == 0)
        entryName = "(function-name not available)";
      else
        entryName = entry.name[0];
      if (entry.undName[0] != 0)
        entryName = entry.undName;
      if (entry.undFullName[0] != 0)
        entryName = entry.undFullName;
      if (entry.lineFileName[0] == 0) {
        string entryModuleName;
        if (entry.moduleName[0] == 0)
          entryModuleName = "(module-name not available)";
        else
          entryModuleName = entry.moduleName;
        
        if(mIdentifier == "") {
          ossIdentifier << entryModuleName << " function " << entryName;
          mIdentifier = ossIdentifier.str();
        }
        
        ossEntry << reinterpret_cast<void*>(entry.offset) << " (" << entryModuleName << "): (filename not available) " << entryName;
      }
      else {
        if(mIdentifier == "") {
          ossIdentifier << entry.lineFileName << " line " << entry.lineNumber;
          mIdentifier = ossIdentifier.str();
        }
        
        ossEntry << entry.lineFileName << " (" << entry.lineNumber << "): " << entryName;
      }
      mTraceStream += ossEntry.str() + "\n";
    }  
  }

  string mIdentifier;
  string mTraceStream;
};

// --------------------------------------------------------------------------------

LONG
handleStructuredException(EXCEPTION_POINTERS* exceptionPointers)
{
  logEngineError("--------------------------------------------------------------------------------");
  logStructuredException(exceptionPointers);
  // Print stack trace
  MyStackWalker stackWalk;
  stackWalk.ShowCallstack(GetCurrentThread(), exceptionPointers->ContextRecord);
  string identifier = "UNKNOWN FAILURE";
  if(stackWalk.mIdentifier != "")
    identifier = stackWalk.mIdentifier;
  
  reportBug(identifier, stackWalk.mTraceStream, exceptionPointers);
  logEngineError(stackWalk.mTraceStream);
  logEngineError("Error reporting completed. HALTING.");

  return EXCEPTION_EXECUTE_HANDLER;
}

// --------------------------------------------------------------------------------
// Print structured exception information

void
logStructuredException(EXCEPTION_POINTERS* exceptionPointers)
{
  EXCEPTION_RECORD* eRecord = exceptionPointers->ExceptionRecord;
  ostringstream ossIdentifier;
  ostringstream ossExtra;
  
  // Print a bit more about access violations
  if(EXCEPTION_ACCESS_VIOLATION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_ACCESS_VIOLATION";
    ossExtra << "The thread tried to read from or write to a virtual address for which it does not have the appropriate access. \n";
    if(eRecord->ExceptionInformation[0] == 0)
      ossExtra << "Mode: READ\n";
    else
      ossExtra << "Mode: WRITE\n";
    ossExtra << "Address: " << (void*)(eRecord->ExceptionInformation[1]) << "\n";
  }
  else if(EXCEPTION_ARRAY_BOUNDS_EXCEEDED == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    ossExtra << "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.\n";
  }
  else if(EXCEPTION_BREAKPOINT == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_BREAKPOINT";
    ossExtra << "A breakpoint was encountered.\n";
  }
  else if(EXCEPTION_DATATYPE_MISALIGNMENT == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_DATATYPE_MISALIGNMENT";
    ossExtra << "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.\n";
  }
  else if(EXCEPTION_FLT_DENORMAL_OPERAND == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_DENORMAL_OPERAND";
    ossExtra << "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.\n";
  }
  else if(EXCEPTION_FLT_DIVIDE_BY_ZERO == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_DIVIDE_BY_ZERO";
    ossExtra << "The thread tried to divide a floating-point value by a floating-point divisor of zero.\n";
  }
  else if(EXCEPTION_FLT_INEXACT_RESULT == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_INEXACT_RESULT";
    ossExtra << "The result of a floating-point operation cannot be represented exactly as a decimal fraction.\n";
  }
  else if(EXCEPTION_FLT_INVALID_OPERATION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_INVALID_OPERATION";
    ossExtra << "This exception represents any floating-point exception not included in this list.\n";
  }
  else if(EXCEPTION_FLT_OVERFLOW == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_OVERFLOW";
    ossExtra << "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.\n";
  }
  else if(EXCEPTION_FLT_STACK_CHECK == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_STACK_CHECK";
    ossExtra << "The stack overflowed or underflowed as the result of a floating-point operation.\n";
  }
  else if(EXCEPTION_FLT_UNDERFLOW == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_FLT_UNDERFLOW";
    ossExtra << "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.\n";
  }
  else if(EXCEPTION_ILLEGAL_INSTRUCTION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_ILLEGAL_INSTRUCTION";
    ossExtra << "The thread tried to execute an invalid instruction.\n";
  }
  else if(EXCEPTION_IN_PAGE_ERROR == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_IN_PAGE_ERROR";
    ossExtra << "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.\n";
  }
  else if(EXCEPTION_INT_DIVIDE_BY_ZERO == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_INT_DIVIDE_BY_ZERO";
    ossExtra << "The thread tried to divide an integer value by an integer divisor of zero.\n";
  }
  else if(EXCEPTION_INT_OVERFLOW == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_INT_OVERFLOW";
    ossExtra << "The result of an integer operation caused a carry out of the most significant bit of the result.\n";
  }
  else if(EXCEPTION_INVALID_DISPOSITION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_INVALID_DISPOSITION";
    ossExtra << "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.\n";
  }
  else if(EXCEPTION_NONCONTINUABLE_EXCEPTION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    ossExtra << "The thread tried to continue execution after a noncontinuable exception occurred.\n";
  }
  else if(EXCEPTION_PRIV_INSTRUCTION == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_PRIV_INSTRUCTION";
    ossExtra << "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.\n";
  }
  else if(EXCEPTION_SINGLE_STEP == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_SINGLE_STEP";
    ossExtra << "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.\n";
  }
  else if(EXCEPTION_STACK_OVERFLOW == eRecord->ExceptionCode) {
    ossIdentifier << "EXCEPTION_STACK_OVERFLOW";
    ossExtra << "The thread used up its stack.\n";
  }
  else {
    ossIdentifier << "UNKNOWN STRUCTURED EXCEPTION";
    ossExtra << "Unknown Win32 structured exception.\n";
  }
  ossIdentifier << " address " << exceptionPointers->ExceptionRecord->ExceptionAddress;

  logEngineWarn(ossIdentifier.str());
  logEngineWarn(ossExtra.str());
}


//--------------------------------------------------------------------------------
// Global entry point

INT WINAPI WinMain(HINSTANCE pCurrentInstance, HINSTANCE, LPSTR , INT )
{
  __try {
//     currentInstance = pCurrentInstance;
    main2();
  }
  // Now we encountered something less funny, a structured exception. Time to dump
  // a nice stack trace.
  __except(handleStructuredException(GetExceptionInformation())) {
  }
  
  return 0;
}

