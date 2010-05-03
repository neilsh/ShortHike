// --------------------------------------------------------------------------------
//
// ShortHike, www.shorthike.com
//
// Copyright 2002-2006 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

private import Common;
private import Version;
private import BuildInfo;
private import Value;
private import Eval;
private import Read;
private import Library;
private import Main;

private import std.socket;
private import std.socketstream;
private import std.thread;

private ushort SCHEME_REPL_PORT = 52310;
private char[] SCM_PROMPT = "shorthike> ";

private struct ThreadData
{
  SocketREPL repl;
  SocketStream stream;
}


class SocketREPL
{
  this(Environment env)
  {
    mEnv = env;
    mServer = new Socket(AddressFamily.INET, SocketType.STREAM, ProtocolType.TCP);
    mServer.setOption(SocketOptionLevel.IP, SocketOption.REUSEADDR, 1);
    
    InternetAddress serverAddress = new InternetAddress(InternetAddress.ADDR_ANY, SCHEME_REPL_PORT);
    mServer.bind(serverAddress);
    mServer.listen(1);
                
    mReadSet = new SocketSet();
  }
  
  // FIXME: How to reclaim closed connections..
  void
  processFrame()
  {
    assert(CHECK("Server is alive", mServer.isAlive()));

    mReadSet.reset();
    mReadSet.add(mServer);
    Socket.select(mReadSet, null, null, 1);

    // Open new client connection
    if(mReadSet.isSet(mServer)) {
      Socket newSocket = mServer.accept();
      SocketStream newClient = new SocketStream(newSocket);
      logInfo << "New SocketREPL connection from: " << newClient.socket().remoteAddress().toString() << endl;
      mClients ~= newClient;
      mReaders[newClient] = new SchemeReader();
      newClient.writeString("ShortHike " ~ BUILD_VERSION ~ " [" ~ BUILD_REVISION ~ "]\n\r" ~ SCM_PROMPT);
      ThreadData* data = new ThreadData;
      data.repl = this;
      data.stream = newClient;
      Thread clientThread = new Thread(&REPLReader, data);
      clientThread.start();
    }

    processLines();
  }

  // --------------------------------------------------------------------------------
  // The scheme input processing itself

  void postLine(SocketStream stream, char[] line)
  {
    synchronized(this) {
      SchemeReader reader = mReaders[stream];
      reader.pushData(line);
    }
  }  

  void processLines()
  {
    synchronized(this) {
      char[] logOutput = rLogFile().getPendingLines();
      foreach(SocketStream stream; mClients) {
        SchemeReader reader = mReaders[stream];
              
        // We should really split the receiver thread into a class
        // to synch the writes and drop the data once the connection
        // is lost.
//         if(logOutput != "") {
//           try {
//             stream.writeString("\r" ~ logOutput ~ SCM_PROMPT);
//           }
//           catch(Exception e) {
//           }          
//         }
        
        if(reader.hasNewData()) {
          Value parameter;
          try {
            ReadState state = reader.read(parameter);
            while(state != ReadState.PARTIAL) {
              if(state == ReadState.ERROR) {
                stream.writeString("Error: " ~ reader.getError() ~ "\n\r" ~ SCM_PROMPT);
              }
              else {
                try {
                  rMain().renderNextFrame();
                  char[] result = eval(mEnv, parameter).toString();
                  logInfo << "Eval: " << result << endl;
                  stream.writeString(result ~ "\n\r" ~ SCM_PROMPT);
                }
                catch(Error e) {
                  stream.writeString(e.toString() ~ "\n\r" ~ SCM_PROMPT);
                }
              }
              state = reader.read(parameter);
            }
          }
          catch(Exception e) {
            reader.reset();
            stream.writeString("Fatal error: " ~ e.toString() ~ "\n\r" ~ SCM_PROMPT);
          }
        }
      }
    }    
  }
  


private:
  Environment mEnv;

  SocketStream[] mClients;
  SchemeReader[SocketStream] mReaders;

  Socket mServer;

  SocketSet mReadSet;
}


// --------------------------------------------------------------------------------


ubyte TELNET_NUL = 0;
ubyte TELNET_CR = 13;
ubyte TELNET_LF = 10;
ubyte TELNET_WILL = 251;
ubyte TELNET_WONT = 252;
ubyte TELNET_DO = 253;
ubyte TELNET_DONT = 254;
ubyte TELNET_IAC = 255;
                  
ubyte TELNET_OPTION_ECHO = 1;
ubyte TELNET_OPTION_SUPRESS_GO_AHEAD = 3;
ubyte TELNET_OPTION_NAWS = 31;
ubyte TELNET_OPTION_TERMINAL_SPEED = 32;

ubyte USASCII_GRAPHIC_MIN = 32;
ubyte USASCII_GRAPHIC_MAX = 126;

int REPLReader(void* arg)
{
  logInfo << "REPLReader started" << endl;
  ThreadData* data = cast(ThreadData*)arg;
  SocketREPL repl = data.repl;
  SocketStream stream = data.stream;

  ubyte next;
  bool running = true;
  try {
    char[] line = "";
    while(running) {
      stream.read(next);
      // Identify telnet command
      if(TELNET_IAC == next) {
        stream.read(next);
        // The other party proposes an option
        if(TELNET_WILL == next) {
          stream.read(next);
          logInfo << "Telnet: WILL ";
          // We ask it not to
          stream.write(TELNET_IAC);
          stream.write(TELNET_DONT);
          stream.write(next);
          logInfo << next << " refused" << endl;
        }
        // The other party requests an option
        else if(TELNET_DO == next) {
          stream.read(next);
          logInfo << "Telnet: DO " << next;
          // We refuse to cooperate
          stream.write(TELNET_IAC);
          stream.write(TELNET_WONT);
          stream.write(next);
          logInfo << " refused" << endl;
        }        
      }
      // We have received a carriage return, check what the next char is.
      // We only accept TELNET_LF. TELNET_NUL and everything else is discarded.
      else if (TELNET_CR == next){
        stream.read(next);
        if(TELNET_LF == next) {
          line ~= "\n";
          repl.postLine(stream, line);
          line = "";
        }
        else if(TELNET_NUL == next) {
          // Valid sequence but ignored for now
        }
      }
      // We also accept naked carriage returns, even if this isn't strictly correct
      // This compensates for certain types of embedding like emacs
      else if(TELNET_LF == next) {
        line ~= "\n";
        repl.postLine(stream, line);
        line = "";
      }
      // Standard character, append to line
      else if((USASCII_GRAPHIC_MIN <= next && next <= USASCII_GRAPHIC_MAX)
              || next == '\t') {
        line ~= cast(char)next;
      }
      // Unknown character we can't handle
      else {
        logInfo << "Telnet: unknown char: " << next << " '" << cast(char)next << "'" << endl;
      }
    }  
  }
  catch(Exception e) {
    logInfo << "SocketREPL halted with: " << e.toString() << endl;
  }  
  return 0;
}
