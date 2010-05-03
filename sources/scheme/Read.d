//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


private import Common;
private import Value;
private import Library;
private import std.stream;
private import std.conv;

// --------------------------------------------------------------------------------

enum ReadState {
  PARTIAL, OK, ERROR
}  


class SchemeReader
{
  this()
  {
    mNewData = false;
  }

  this(char[] buffer)
  {
    mBuffer = buffer[];
    mNewData = true;
  }  


  char[] getError()
  {
    return mErrorMessage;
  }  


  void pushData(char[] data)
  {
    mBuffer ~= data;
    mNewData = true;
  }

  bool hasNewData()
  {
    bool status = mNewData;
    mNewData = false;
    return status;
  }

  void reset()
  {
    mBuffer = "";
    mNewData = false;
    mPos = 0;
    mErrorMessage = "";
  }
  


  // ------------------------------------------------------------
  // Actual item reader

  ReadState read(out Value value)
  {
    try {
      mErrorMessage = "";
      value = readItem();
    }
    catch(ReadAbort abort) {
      assert(CHECK("OK State should not end up here", abort.mState != ReadState.OK));
      if(ReadState.PARTIAL == abort.mState) {
        mPos = 0;
        value = rsUndefined();
        return ReadState.PARTIAL;
      }
      else if(ReadState.ERROR == abort.mState) {
        // Skip everything and parse to next endline or end of buffer
        mBuffer = mBuffer[mPos .. mBuffer.length];
        int newline = find(mBuffer, "\n");
        if(newline == -1)
          mBuffer.length = 0;
        else
          mBuffer = mBuffer[newline + 1 .. mBuffer.length];
        mPos = 0;

        value = rsUndefined();
        return ReadState.ERROR;
      }
    }
    mBuffer = mBuffer[mPos .. mBuffer.length];
    mPos = 0;
    return ReadState.OK;
  }    
  
  
  // ------------------------------------------------------------
  // Actual item reader

  private Value readItem()
  {
    skipToToken();
    char next = getc();
    
    // ----------------------------------------
    // Parse list
    if('(' == next) {
      PairValue first = new PairValue;
      PairValue current = first;
      skipToToken();
      while(!isEndOfList(peek())) {
        current.car = readItem();
        current.cdr = new PairValue();
        current = cast(PairValue)current.cdr;
        skipToToken();
      }
      // Skip the end of list token
      getc();
      return first;
    }
    else if(')' == next) {
      error("Unbalanced parenthesis");
    }
    
    
    // ----------------------------------------
    // Quote expansion
    
    else if('\'' == next) {
      return new PairValue(new SymbolValue("quote"),
                           new PairValue(readItem(), new PairValue()));
    }
    
    // ----------------------------------------
    // Parse string
    else if('"' == next) {
      errorOnEOF("String without closing double quote");
      char[] string;
      next = getc();
      while('"' != next)  {
        errorOnEOF("String without closing double quote");
        if('\\' == next) {
          next = getc();
          errorOnEOF("String without closing double quote");
          if('n' == next)
            string ~= '\n';
          else
            string ~= next;
        }
        else {
          string ~= next;
        }   
        next = getc();
      }
      return new StringValue(string);
    }

    // ----------------------------------------
    // Parse number

    else if(std.string.isdigit(next) || '-' == next || '.' == next) {
      // We need to shortcircuit for the symbol "-"
      if('-' == next && (eof() || isSeparator(peek()))) {
        return new SymbolValue("-");
      }
      
      bool isFloat = false;
      char[] numString = "" ~ next;
      while(!eof() && !isSeparator(peek())) {
        next = getc();
        if(!(std.string.isdigit(next) || next == '-'))
          isFloat = true;
        numString ~= next;
      }
      // NB. D sometimes returns true on isNumeric even if it doesn't parse it.
      // Most notably .456 type numbers.
      try {
        if(!std.string.isNumeric(numString)) error("Symbol starting with numeric character: " ~ numString);
        if(isFloat)
          return new NumberValue(std.conv.toFloat(numString));
        else
          return new NumberValue(std.conv.toInt(numString));
      }
      catch(Exception e) {
        error("Error parsing numerical argument: " ~ e.toString());
      }
    }

    // ----------------------------------------
    // Parse special symbols
    else if('#' == next) {
      errorOnEOF("Hash sequence abruptly terminated");
      next = getc();
      if('f' == next)
        return new BoolValue(false);
      else if('t' == next)
        return new BoolValue(true);
      else
        error("Non recognized special symbol #" ~ next);
    }
    // ----------------------------------------
    // Parse symbol
    else {
      char[] readString = "";
      readString ~= next;
      while(!eof() && !isSeparator(peek)) {
        readString ~= getc();
      }
      return new SymbolValue(readString);
    }
    assert(CHECK_FAIL("PARSE FAILURE"));
    return rsUndefined();
  }


  // ------------------------------------------------------------
  // Exception class used in error conditions

  private class ReadAbort
  {
    this(ReadState state)
    {
      mState = state;
    }
    ReadState mState;    
  }
  
  
  // ------------------------------------------------------------
  // Stream primitives

  private char getc()
  {
    return mBuffer[mPos++];
  }  

  private char peek()
  {
    return mBuffer[mPos];
  }  

  private bool eof()
  {
    return mPos >= mBuffer.length;
  }

  private void lookAhead(char[] message)
  {
    const int LOOKAHEAD_LENGTH = 40;
    if((mBuffer.length - mPos) < LOOKAHEAD_LENGTH)
      logInfo() << "DEBUG: lookAhead : \"" << mBuffer[mPos .. mBuffer.length] << "\" " << message << endl;
    else
      logInfo() << "DEBUG: lookAhead : \"" << mBuffer[mPos .. mPos + LOOKAHEAD_LENGTH] << "\" " << message << endl;
  }
  

  // ------------------------------------------------------------
  // Lexer methods

  private void skipWhite()
  {
    while(!eof && std.string.iswhite(peek))
      getc();
    if(eof) throw new ReadAbort(ReadState.PARTIAL);
  }

  private void skipToToken()
  {
    skipWhite();
    while(!eof && peek == ';') {
      char next = getc();
      while(!eof && next != '\n')
        next = getc();
      skipWhite();
    }
    if(eof()) throw new ReadAbort(ReadState.PARTIAL);
  }

  private bool
  isEndOfList(char next)
  {
    return cast(bool)(next == ')');
  }

  private bool
  isSeparator(char next)
  {
    return cast(bool)(isEndOfList(next) || std.string.iswhite(next));
  }

  private char[]
  snippet()
  {
    if(0 > (mPos - 50))
      return mBuffer[0 .. mPos];
    else
      return mBuffer[mPos - 50 .. mPos];       
  }  


  // ------------------------------------------------------------
  // Error handling

  private void errorOnEOF(char[] message)
  {
    if(eof()) error(message);
  }

  private void error(char[] message)
  {
    mErrorMessage = message ~ ": " ~ snippet();
    throw new ReadAbort(ReadState.ERROR);
  }


private:
  bool mNewData = false;
  int mPos = 0;
  char[] mBuffer;
  char[] mErrorMessage;
}


// --------------------------------------------------------------------------------

bool
read(char[] buffer, out Value value)
{
  SchemeReader stream = new SchemeReader(buffer);
  return stream.read(value) == ReadState.OK;
}


// --------------------------------------------------------------------------------


// ReadState
// readStateful(InputStream input, out Value value)
// {
//   char[] data = input.readString(input.available());
//   return readStateful(data, value);
// }

// ReadState
// readStateful(inout char[] buffer, out Value value)
// {
//   value = rsUndefined();
//   if(!striplComments(buffer)) return ReadState.FRAGMENT;

//   // ----------------------------------------
//   // Parse list
//   if('(' == buffer[0]) {
//     buffer = buffer[1 .. buffer.length];
//     if(!striplComments(buffer)) {value = rsUndefined(); return ReadState.FRAGMENT; }
//     PairValue current = new PairValue();
//     value = current;
//     while(!buffer[0] == ')') {
//       Value item;
//       ReadState state = readStateful(buffer, item);
//       if(state != ReadState.OK) {value = rsUndefined(); return state; }
//       current.car = item;
//       current.cdr = new PairValue();
//       current = cast(PairValue)current.cdr;
//       if(!striplComments(buffer)) {value = rsUndefined(); return ReadState.FRAGMENT; }
//     }
//     buffer = buffer[1 .. buffer.length];
//     return ReadState.OK;
//   }
//   // Error, we should never start with a closing parenthesis
//   else if(')' == buffer[0]) {
//     value = rsUndefined();
//     return ReadState.ERROR;
//   }  
//   // ----------------------------------------
//   // Quote expansion
//   else if('\'' == buffer[0]) {
//     buffer = buffer[1 .. buffer.length];
//     Value item;
//     ReadState state = readStateful(buffer, item);
//     if(state != ReadState.OK) { value = rsUndefined(); return state; }

//     Value list1 = new PairValue(item, new PairValue());
//     Value quoteSymbol = new SymbolValue("quote");
//     value = new PairValue(quoteSymbol, list1);
//     return ReadState.OK;
//   }
  
//   // ----------------------------------------
//   // Parse string
//   else if('"' == buffer[0]) {
//     buffer = buffer[1 .. buffer.length];
//     char[] readString;
//     if(buffer.length == 0) {value = rsUndefined(); return ReadState.ERROR;}    
//     while('"' != buffer[0])  {
//       if('\\' == buffer[0]) {
//         if(buffer.length == 0) {value = rsUndefined(); return ReadState.ERROR;}
//         if('n' == buffer[0]) {
//           readString ~= '\n';
//         }
//         // Just quote whatever is after the slash
//         else {
//           readString ~= buffer[0];
//         }
//       }
//       else {
//         readString ~= buffer[0];
//       }      
//       buffer = buffer[1 .. buffer.length];
//       if(buffer.length == 0) {value = rsUndefined(); return ReadState.ERROR;}
//     }
//     value = new StringValue(readString);
//     buffer = buffer[1 .. buffer.length];
//     return ReadState.OK;
//   }

//   // ----------------------------------------
//   // Parse number
//   else if(std.string.isdigit(buffer[0]) || '-' == buffer[0] || '.' == buffer[0]) {
//     int intValue = 0;
//     bool negate = false;
//     if('-' == buffer[0]) {
//       negate = true;
//       buffer = buffer[1 .. buffer.length];
//       if(buffer.length == 0) {value = rsUndefined(); return ReadState.ERROR;}
//     }
//     while(!isSeparator(buffer[0]) && '.' != buffer[0] && buffer.length > 0) {
//       if(!std.string.isdigit(buffer[0])) return ReadState.ERROR;
       
//       char nextChar = buffer[0];
//       buffer = buffer[1 .. buffer.length];
//       //        logInfo << "NextDigit: " << nextChar << " a: " << input.available() << endl;
//       intValue *= 10;
//       intValue += nextChar - '0';
//       //        if(isNextEndOfList(input))
//       //          logInfo << "Next is end of list" << endl;
//     }

//     if(buffer.length == 0 || isSeparator(buffer[0])){
//       if(negate)
//         value = new NumberValue(-intValue);
//       else
//         value = new NumberValue(intValue);
//       return ReadState.OK;
//     }
//     else if('.' == buffer[0]) {
//       float floatValue = intValue;      
//       float decimal = 1;
//       buffer = buffer[1 .. buffer.length];
//       while(buffer.length > 0 && !isSeparator(buffer[0])) {
//         if(!std.string.isdigit(buffer[0])) {value = rsUndefined(); return ReadState.ERROR;}
//         char nextChar = buffer[0];
//         buffer = buffer[1 .. buffer.length];
//         decimal *= 0.1;
//         floatValue += decimal * (nextChar - '0');
//       }
//       if(negate)
//         value = new NumberValue(-floatValue);
//       else
//         value = new NumberValue(floatValue);
//       return ReadState.OK;
//     }
//     // non-numeric characters in number
//     else {
//       value = rsUndefined();
//       return ReadState.ERROR;
//     }
//   }

//   // ----------------------------------------
//   // Parse special symbols
//   else if('#' == buffer[0]) {
//     buffer = buffer[1 .. buffer.length];
//     if(buffer.length == 0) {value = rsUndefined(); return ReadState.ERROR;}
//     if('f' == buffer[0]) {
//       value = new BoolValue(false);
//     }
//     else if('t' == buffer[0]) {
//       value = new BoolValue(true);
//     }
//     else {
//       value = rsUndefined();
//       return ReadState.ERROR;
//     }
//     return ReadState.OK;
//   }
//   // ----------------------------------------
//   // Parse symbol
//   else {
//     char[] readString;
//     while(buffer.length > 0 && !isSeparator(buffer[0])) {
//       readString ~= buffer[0];
//       buffer = buffer[1 .. buffer.length];
//     }
//     value = new SymbolValue(readString);
//     return ReadState.OK;
//   }  
// }

// --------------------------------------------------------------------------------
// Utility functions

// char
// peek(InputStream input)
// {
//   int peekBefore = input.available();
//   char next = input.getc();
//   input.ungetc(next);
//   int peekAfter = input.available();
//   logInfo << "peek: " << peekBefore << " " << peekAfter << endl;
//   return next;
// }


// --------------------------------------------------------------------------------
// Stream utility functions

// void
// skipWhite(InputStream input)
// {
//   while(!input.eof  && (peek(input) == '\n' || peek(input) == '\r' || std.string.iswhite(peek(input))))
//     input.getc();
// }

  
// void
// skipWhiteAndComments(InputStream input)
// {
//   skipWhite(input);
//   while(!input.eof() && peek(input) == ';') {
//     input.readLine();
//     skipWhite(input);
//   }
// }

// bool
// isNextEndOfList(InputStream input)
// {
//   char next = peek(input);
//   return cast(bool)(input.eof() || next == ')');
// }


// bool
// isNextSeparator(InputStream input)
// {
//   char next = peek(input);
//   return cast(bool)(isNextEndOfList(input) || std.string.iswhite(next));
// }


// // --------------------------------------------------------------------------------

// bool
// read(InputStream pInputStream, out Value value)
// {
//   value = new Value();
//   skipWhiteAndComments(input);
//   if(input.eof()) return false;

//   char nextChar = peek(input);

//   // ----------------------------------------
//   // Parse list
//   if('(' == nextChar) {
//     input.getc();
//     PairValue current = new PairValue();
//     value = current;
//     skipWhiteAndComments(input);
//     while(!isNextEndOfList(input)) {
//       Value item;
//       if(!read(input, item))
//         return false;
//       current.car = item;
//       PairValue newPair = new PairValue();
//       current.cdr = newPair;
//       current = newPair;
//       skipWhiteAndComments(input);
//     }
//     if(')' == peek(input)) {
//       input.getc();
//       return true;
//     }
//     else {
//       value = rsUndefined();
//       return false;
//     }
//   }
//   // ----------------------------------------
//   // Quote expansion
//   else if('\'' == nextChar) {
//     input.getc();
//     Value item;
//     if(!read(input, item))
//       return false;

//     Value list1 = new PairValue(item, new PairValue());
//     Value quoteSymbol = new SymbolValue("quote");
//     value = new PairValue(quoteSymbol, list1);

//     return true;
//   }
  
//   // ----------------------------------------
//   // Parse string
//   else if('"' == nextChar) {
//     char[] readString;
//     char c;
//     c = input.getc();
//     if(input.eof()) return false;
//     c = input.getc();
//     if(input.eof()) return false;
//     while('"' != c)  {
//       if('\\' == c) {
//         c = input.getc();
//         if(input.eof()) return false;
//         if('"' == c) {
//           readString ~= '"';
//         }
//         else if('n' == c) {
//           readString ~= '\n';
//         }
//         else {
//           // Error. Unknown character escape
//         }
//       }
//       else {
//         readString ~= c;
//       }      
//       if(input.eof()) return false;      
//       c = input.getc();
//     }
//     value = new StringValue(readString);
//     return true;
//   }

//   // ----------------------------------------
//   // Parse number
//   else if(std.string.isdigit(nextChar) || '-' == nextChar || '.' == nextChar) {
//     int intValue = 0;
//     bool negate = false;
//     if('-' == peek(input)) {
//       negate = true;
//       input.getc();
//       if(input.eof()) return false;
//     }
//     while(!isNextSeparator(input) && '.' != peek(input)) {
//       if(!std.string.isdigit(peek(input))) return false;
//       nextChar = input.getc();
//       logInfo << "NextDigit: " << nextChar << " a: " << input.available() << endl;
//       intValue *= 10;
//       intValue += nextChar - '0';
//       if(isNextEndOfList(input))
//         logInfo << "Next is end of list" << endl;
//     }
//     if(isNextSeparator(input)){
//       if(negate)
//         value = new NumberValue(-intValue);
//       else
//         value = new NumberValue(intValue);
//       return true;
//     }
//     else if('.' == peek(input)) {
//       float floatValue = intValue;      
//       float decimal = 1;
//       input.getc();
//       while(!isNextSeparator(input)) {
//         if(!std.string.isdigit(peek(input))) return false;
//         nextChar = input.getc();
//         decimal *= 0.1;
//         floatValue += decimal * (nextChar - '0');
//       }
//       if(negate)
//         value = new NumberValue(-floatValue);
//       else
//         value = new NumberValue(floatValue);
//       return true;
//     }
//     // non-numeric characters in number
//     else {
//       return false;
//     }
//   }

//   // ----------------------------------------
//   // Parse special symbols
//   else if('#' == nextChar) {
//     input.getc();
//     char c;
//     c = input.getc();
//     if('f' == c) {
//       value = new BoolValue(false);
//     }
//     else if('t' == c) {
//       value = new BoolValue(true);
//     }
//     else {
//       return false;
//     }
//     return true;
//   }
//   // ----------------------------------------
//   // Parse symbol
//   else {
//     char[] readString;
//     while(!isNextSeparator(input)) {
//       char c;
//       c = input.getc();
//       readString ~= c;
//     }
//     value = new SymbolValue(readString);
//     return true;
//   }  
// }

// --------------------------------------------------------------------------------

// FIXME: Refactor this into Value.toString

void
printValue(OutputStream stream, Value value)
{
  if(value.isPair() && value.isNull()) {
    stream.writeString(cast(char[])"()");
  }  
  else if(value.isPair()) {
    if(sListP(value)) {
      stream.writeString(cast(char[])"(");
      printValue(stream, value.car());
      value = value.cdr();
      while(value.isNull() == false) {
        stream.writeString(cast(char[])" ");
        printValue(stream, value.car());
        value = value.cdr();
      }
      stream.writeString(cast(char[])")");
    }
    else {
      stream.writeString(cast(char[])"(");
      printValue(stream, value.car());
      stream.writeString(cast(char[])" . ");
      printValue(stream, value.cdr());
      stream.writeString(cast(char[])")");
    }
  }
  else if(value.isBool() && value.vBool()) {
    stream.writeString(cast(char[])"#t");
  }
  else if(value.isBool() && !value.vBool()) {
    stream.writeString(cast(char[])"#f");
  }
  else if(value.isString()) {
    stream.writeString(cast(char[])"\"" ~ value.vString() ~ cast(char[])"\"");
  }
  else if(value.isSymbol()) {
    stream.writeString(value.vString());
  }
  else if(value.isProcedure()) {
    stream.writeString(cast(char[])"#procedure");
  }
  else if(value.isNumber() && value.isExact()) {
    stream.writeString(toString(value.vInt()));
  }
  else if(value.isNumber() && !value.isExact()) {
    stream.writeString(toString(value.vFloat()));
  }
  else {
    // FIXME: Add value.type() to output
    stream.writeString(cast(char[])"#unknown:");
  }  
}


//================================================================================
// Unit test

// --------------------------------------------------------------------------------

class TestRead : public TestFixture
{
  this()
  {
    mFixtureName = "TestRead";
    addTest("read_simple", &read_simple);
    addTest("read_list", &read_list);
    addTest("read_quote", &read_quote);
  }
  
  void
  read_simple()
  {
    // Base types
    Value value = new Value();
    // string
    assert(CHECK("Read succeeded", read("\"foobarbaz\\n test\\\" 3.1415 test2\"", value)));
    assert(CHECK("String type", value.isString()));
    assert(CHECK_EQUAL("String content", "foobarbaz\n test\" 3.1415 test2", value.vString()));
    assert(CHECK_EQUAL("Print form", "\"foobarbaz\n test\" 3.1415 test2\"", value.toString()));
    // integer
    assert(CHECK("Read succeeded", read("12345678", value)));
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 12345678, value.vInt()));
    // negative integer
    assert(CHECK("Read succeeded", read("-12345678", value)));
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", -12345678, value.vInt()));
    // float
    assert(CHECK("Read succeeded", read("123.456", value)));
    assert(CHECK("Float type", value.isNumber() && !value.isExact()));
    assert(CHECK_EQUAL("Float content", 123.456, value.vFloat()));
    // negative float
    assert(CHECK("Read succeeded", read("-123.456", value)));
    assert(CHECK("Float type", value.isNumber() && !value.isExact()));
    assert(CHECK_EQUAL("Float content", -123.456, value.vFloat()));

    // symbol
    assert(CHECK("Read succeeded", read("foo_12345", value)));
    assert(CHECK("Symbol type", value.isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "foo_12345", value.vString()));

    // Special case symbol "-"
    assert(CHECK("Read succeeded", read("-", value)));
    assert(CHECK("Symbol type", value.isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "-", value.vString()));

    // boolean false
    assert(CHECK("Read succeeded", read("#f", value)));
    assert(CHECK("Boolean false type", value.isBool() && !value.vBool()));

    // boolean true
    assert(CHECK("Read succeeded", read("#t", value)));
    assert(CHECK("Boolean true type", value.isBool() && value.vBool()));

    // ----------------------------------------
    // Simple error situations. These should never hang
    assert(CHECK("No content", !read("", value)));
    assert(CHECK("Malformed string", !read("\"this is a string that never closes, but has a trailing quote\\\"", value)));
    assert(CHECK("Malformed number", !read("123ab", value)));
    assert(CHECK("Malformed number", !read("12.3ab", value)));
    assert(CHECK("Malformed number", !read("123a.b", value)));

    // ----------------------------------------
    // Compound statement
    SchemeReader reader = new SchemeReader("1234 #f -1234 \"foo\\n\" 1.234 foobar_1234");

    // first int
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 1234, value.vInt()));

    // boolean false
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("Boolean false type", value.isBool() && !value.vBool()));

    // second int
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", -1234, value.vInt()));

    // string
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("String type", value.isString()));
    assert(CHECK_EQUAL("String content", "foo\n", value.vString()));

    // float
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("Float type", value.isNumber() && !value.isExact()));
    assert(CHECK_EQUAL("Float content", 1.234, value.vFloat()));

    // symbol
    assert(CHECK("Read succeeded", reader.read(value) == ReadState.OK));
    assert(CHECK("Symbol type", value.isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "foobar_1234", value.vString()));

    // And now there should be no more data to read
    assert(CHECK("Reader in partial state", reader.read(value) == ReadState.PARTIAL));
  }
  

  void
  read_list()
  {
    Value value = new Value();

    // List
    assert(CHECK("Read succeeded", read("(1234 a 1.2 \"foo\" (1))", value)));
    Value current = value;
    assert(CHECK("Pair and not null type: " ~ value.toString(), current.isPair() && !current.isNull()));
    assert(CHECK("Integer type", current.car().isNumber() && current.car().isExact()));
    assert(CHECK_EQUAL("Integer content", 1234, current.car().vInt()));
    current = current.cdr();
  
    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("Symbol type", current.car().isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "a", current.car().vString()));
    current = current.cdr();

    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("Float type", current.car().isNumber() && !current.car().isExact()));
    assert(CHECK_EQUAL("Float content", 1.2, current.car().vFloat()));
    current = current.cdr();

    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("String type", current.car().isString()));
    assert(CHECK_EQUAL("String content", "foo", current.car().vString()));
    current = current.cdr();

    // Nested list
    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("Pair type", current.car().isPair()));
    assert(CHECK("Empty list type", current.car().cdr().isNull()));
    assert(CHECK("Integer type", current.car().car().isNumber() && current.car().car().isExact()));
    assert(CHECK_EQUAL("Integer content", 1, current.car().car().vInt()));
    current = current.cdr();

    assert(CHECK("Empty List type", current.isNull()));

    // ----------------------------------------
    // Malformed content
    assert(CHECK("Malformed list", !read("(", value)));
    assert(CHECK("Malformed list", !read("( 12a )", value)));
    assert(CHECK("Malformed list", !read("( (()())", value)));
    assert(CHECK("Malformed list", !read("( (( ; and ends with a comment", value)));

  }

  void
  read_quote()
  {
    Value value = new Value();

    assert(CHECK("Read succeeded", read("'(foo  ; this is a funky! comment with a list ()\n ) ;and this is our end parenthesis", value)));
    Value current = value;

    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("Symbol type", current.car().isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "quote", current.car().vString()));
    current = current.cdr();

    assert(CHECK("Pair type", current.isPair()));
    assert(CHECK("Pair type", current.car().isPair()));
    assert(CHECK("Symbol type", current.car().car().isSymbol()));
    assert(CHECK_EQUAL("Symbol content", "foo", current.car().car().vString()));
    assert(CHECK("Empty list type", current.car().cdr().isNull()));
    current = current.cdr();

    assert(CHECK("Empty list type", current.isNull()));  
  }
}

