//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import PlatformWin32;
private import Main;
private import std.c.string;
private import std.c.windows.windows;


HINSTANCE gCurrentInstance;

int WinMain2(HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR lpCmdLine,
             int nCmdShow)
{
  char[] cmdLine = lpCmdLine[0 .. strlen(lpCmdLine)].dup;
  char[][] args = split(cmdLine, " ");
  if(cmdLine.length > 0 && cmp(args[0], "--test") == 0) {
    runTestFixtures();
  }
  else {
    gCurrentInstance = hInstance;
    rMain().mainLoop();
  }
  return 0;
}

// --------------------------------------------------------------------------------
// Crash report

// string userExtraMessage;
// string userEmail;

// string
// getDialogString(HWND hwnd, int item)
// {
//   string dialogString = "";

//   int len = GetWindowTextLength(GetDlgItem(hwnd, item));
//   if(len > 0) {
//     char* buf;
    
//     buf = (char*)GlobalAlloc(GPTR, len + 1);
//     GetDlgItemText(hwnd, item, buf, len + 1);
          
//     dialogString = buf;
    
//     GlobalFree((HANDLE)buf);
//   }
//   return dialogString;
// }


// BOOL CALLBACK CrashDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
// {
//   switch(Message) {
//   case WM_INITDIALOG:    
//     SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), "SHORTHIKEICON"));
//     SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), "SHORTHIKEICON"));

//     // Center myself
//     int x, y, screenWidth, screenHeight;
//     RECT rcDlg;
//     GetWindowRect(hwnd, &rcDlg);
//     screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
//     screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

//     x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
//     y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);
    
//     MoveWindow(hwnd, x, y, (rcDlg.right - rcDlg.left),
//                (rcDlg.bottom - rcDlg.top), TRUE);

//     return TRUE;
//   case WM_COMMAND:
//     switch(LOWORD(wParam))
//       {
//       case IDOK:
//         userExtraMessage = getDialogString(hwnd, 1002);
//         userEmail = getDialogString(hwnd, 1006);
//         EndDialog(hwnd, IDOK);
//         break;
//       case IDCANCEL:
//         EndDialog(hwnd, IDCANCEL);
//         break;
//       }
//     break;
//   default:
//     return FALSE;
//   }
//   return TRUE;
// }

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

// void
// reportBug(string errorInfo, string extraInformation, EXCEPTION_POINTERS* exceptionPointers = NULL)
// {
//   string bugID = string("V") + BUILD_VERSION + " " + errorInfo;
//   int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(101), NULL, CrashDlgProc);
//   if(ret == IDOK){
//     // Write minidump
//     if(exceptionPointers != NULL) {
//       MINIDUMP_EXCEPTION_INFORMATION miniDumpExcept;
//       miniDumpExcept.ThreadId = GetCurrentThreadId();
//       miniDumpExcept.ExceptionPointers = exceptionPointers;
//       miniDumpExcept.ClientPointers = FALSE;
//       HANDLE dumpFile = CreateFile("ShortHikeCrash.dmp", FILE_WRITE_DATA, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//       if(FALSE == MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile,
//                                     MiniDumpNormal, &miniDumpExcept, NULL, NULL))
//         logEngineError("Error writing dump");
  
//       CloseHandle(dumpFile);
//       logEngineInfo("MiniDump written to ShortHikeCrash.dmp");
//     }

//     // Start building the POST request
//     ostringstream ossExtraFirst;
//     ossExtraFirst << "Build date: " << BUILD_DATE << "\n" << "Build revision: " << BUILD_REVISION << "\n\n";
//     ossExtraFirst << extraInformation << "\n" << "User message:\n" << userExtraMessage << "\n";

//     // Log file inclusion not supported as upstreams has extra size limits.
//     ossExtraFirst << "\nLog:\n";
//     ifstream logFile;
//     logFile.open("ShortHike.log", ios_base::in);
//     while(false == logFile.eof()) {
//       string logLine;
//       getline(logFile, logLine);
//       ossExtraFirst << logLine << "\n";
//     }
//     logFile.close();

//     // Replace all occurences of endl with "\r\n"
//     ostringstream ossExtra;
//     istringstream issExtraFirst(ossExtraFirst.str());
//     while(false == issExtraFirst.eof()) {
//       string line;
//       getline(issExtraFirst, line);
//       ossExtra << line << "\r\n";
//     }
    
//     HINTERNET internetBase = InternetOpen("ShortHike", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
//     if(NULL == internetBase) return;
    
//     HINTERNET serverConnection = InternetConnect(internetBase, "secure.shorthike.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
//                                                  INTERNET_SERVICE_HTTP, NULL, NULL);  
//     if(NULL == serverConnection) return;
    
//     HINTERNET bugRequest = HttpOpenRequest(serverConnection, "POST", "/fogbugz/scoutSubmit.php?ScoutUserName=Kai%20Backman&ScoutProject=Player%20Support&ScoutArea=Crash%20Reports", NULL, NULL, NULL,
//                                            INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, NULL);
//     if(NULL == bugRequest) return;

//     string boundary = "---------------------------1d7fc0e8b9a7be975043c4543975a113";
//     string contentTypeHeader = "Content-Type: multipart/form-data; boundary=" + boundary;
//     HttpAddRequestHeaders(bugRequest, contentTypeHeader.c_str(), (DWORD)-1L, HTTP_ADDREQ_FLAG_ADD);    
    
//     string requestBody;
//     requestBody.append("--" + boundary + "\r\n");
//     requestBody.append("Content-Disposition: form-data; name=\"Description\"\r\n"); 
//     requestBody.append("Content-Type: application/octet-stream\r\n");
//     requestBody.append("\r\n");
//     requestBody.append(bugID + "\r\n");

//     requestBody.append("--" + boundary + "\r\n");
//     requestBody.append("Content-Disposition: form-data; name=\"Email\"\r\n"); 
//     requestBody.append("Content-Type: application/octet-stream\r\n");
//     requestBody.append("\r\n");
//     requestBody.append(userEmail + "\r\n");

//     requestBody.append("--" + boundary + "\r\n");
//     requestBody.append("Content-Disposition: form-data; name=\"Extra\"\r\n"); 
//     requestBody.append("Content-Type: application/octet-stream\r\n");
//     requestBody.append("\r\n");
//     requestBody.append(ossExtra.str() + "\r\n");

//     LPVOID optional = (LPVOID)(requestBody.data());
//     DWORD optionalLength = (DWORD)requestBody.size();

//     if(FALSE == HttpSendRequest(bugRequest, NULL, 0, optional, optionalLength)) return;
    
//     InternetCloseHandle(bugRequest);
//     InternetCloseHandle(serverConnection);
//     InternetCloseHandle(internetBase);    

//     MessageBox(NULL, "Thank you for your crash report.\n\n -Kai" , "ShortHike crash report delivered",
//                MB_OK | MB_ICONINFORMATION);
//   }
// }

// --------------------------------------------------------------------------------
// extern (Windows) 
// {
//   const DWORD EXCEPTION_EXECUTE_HANDLER = 0x00001;

//   struct EXCEPTION_RECORD {
//   }

//   struct CONTEXT {
//   }

//   struct EXCEPTION_POINTERS {
//     EXCEPTION_RECORD* ExceptionRecord;
//     CONTEXT* ContextRecord;
//   }

//   alias LONG (TOP_LEVEL_EXCEPTION_FILTER) (EXCEPTION_POINTERS *ExceptionInfo);

//   TOP_LEVEL_EXCEPTION_FILTER*
//     SetUnhandledExceptionFilter(TOP_LEVEL_EXCEPTION_FILTER* lpTopLevelExceptionFilter);
// }


// // extern (Windows) LONG handleStructuredException(EXCEPTION_POINTERS* exceptionPointers)
// extern (Windows) LONG handleStructuredException(EXCEPTION_POINTERS* exceptionPointers)
// {
// //       logEngineError("--------------------------------------------------------------------------------");
// //       logStructuredException(exceptionPointers);
// //       // Print stack trace
// //       MyStackWalker stackWalk;
// //       stackWalk.ShowCallstack(GetCurrentThread(), exceptionPointers->ContextRecord);
// //       string identifier = "UNKNOWN FAILURE";
// //       if(stackWalk.mIdentifier != "")
// //         identifier = stackWalk.mIdentifier;
  
// //       reportBug(identifier, stackWalk.mTraceStream, exceptionPointers);
// //       logEngineError(stackWalk.mTraceStream);
// //       logEngineError("Error reporting completed. HALTING.");
//   logError << "General protection fault caught" << endl;
//   return EXCEPTION_EXECUTE_HANDLER;
// }

// --------------------------------------------------------------------------------
// Boiler plate D runtime initialization

extern (C) void gc_init();
extern (C) void gc_term();
extern (C) void _minit();
extern (C) void _moduleCtor();
extern (C) void _moduleDtor();
extern (C) void _moduleUnitTests();

extern (Windows)
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nCmdShow)
{
  int result;
  
  gc_init();			// initialize garbage collector
  _minit();			// initialize module constructor table
  
  try {
    _moduleCtor();		// call module constructors
    _moduleUnitTests();	// run unit tests (optional)
    
    result = WinMain2(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    
    _moduleDtor();		// call module destructors
  }
  
  catch (Object o)		// catch any uncaught exceptions
    {
      MessageBoxA(null, cast(char *)o.toString(), "Error",
                  MB_OK | MB_ICONEXCLAMATION);
      result = 0;		// failed
    }

  gc_term();			// run finalizers; terminate garbage collector
  return result;
}
