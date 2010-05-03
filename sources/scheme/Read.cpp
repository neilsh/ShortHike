//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Read.hpp"
#include "Library.hpp"

// --------------------------------------------------------------------------------

bool
read(string code, ValuePtr& value)
{
  istringstream codeStream(code);
  return read(codeStream, value);
}

// --------------------------------------------------------------------------------
// Stream utility functions

void
skipWhite(istream& input)
{
  while(!input.eof() && input.peek() != -1 && (isspace(input.peek()) || input.peek() == '\n' || input.peek() == '\r'))
     input.ignore();
}

  
void
skipWhiteAndComments(istream& input)
{
  skipWhite(input);
  while(!input.eof() && input.peek() != -1 && input.peek() == ';') {
    while(!input.eof() && input.peek() != -1 && input.peek() != '\n' && input.peek() != '\r')
      input.ignore();
    skipWhite(input);
  }
}

bool
isNextEndOfList(istream &input)
{
  char next = input.peek();
  return input.eof() || next == -1 || next == ')';
}


bool
isNextSeparator(istream& input)
{
  char next = input.peek();
  return (isNextEndOfList(input) || isspace(next));
}


// --------------------------------------------------------------------------------

bool
read(istream& input, ValuePtr& value)
{
  value = new Value();
  skipWhiteAndComments(input);
  if(input.eof()) return false;

  char nextChar = input.peek();

  // ----------------------------------------
  // Parse list
  if('(' == nextChar) {
    input.ignore();
    value = new PairValue();
    ValuePtr current = value;
    skipWhiteAndComments(input);
    while(!isNextEndOfList(input)) {
      ValuePtr item;
      if(!read(input, item))
        return false;
      current->car() = item;
      ValuePtr newPair = new PairValue();
      current->cdr() = newPair;
      current = newPair;
      skipWhiteAndComments(input);
    }
    if(')' == input.peek()) {
      input.ignore();
      return true;
    }
    else {
      value = rsUndefined();
      return false;
    }
  }
  // ----------------------------------------
  // Quote expansion
  else if('\'' == nextChar) {
    input.ignore();
    ValuePtr item;
    if(!read(input, item))
      return false;

    ValuePtr list1 = new PairValue(item, new PairValue());
    ValuePtr quoteSymbol = new SymbolValue("quote");
    value = new PairValue(quoteSymbol, list1);

    return true;
  }
  
  // ----------------------------------------
  // Parse string
  else if('"' == nextChar) {
    string readString;
    char c;
    input.get(c);
    if(input.eof()) return false;
    input.get(c);
    if(input.eof()) return false;
    while('"' != c)  {
      if('\\' == c) {
        input.get(c);
        if(input.eof()) return false;
        if('"' == c) {
          readString.push_back('"');
        }
        else if('n' == c) {
          readString.push_back('\n');
        }
        else {
          // Error. Unknown character escape
        }
      }
      else {
        readString.push_back(c);
      }      
      input.get(c);
      if(input.eof()) return false;      
    }
    value = new StringValue(readString);
    return true;
  }

  // ----------------------------------------
  // Parse number
  else if(isdigit(nextChar) || '-' == nextChar || '.' == nextChar) {
    int intValue = 0;
    bool negate = false;
    if('-' == input.peek()) {
      negate = true;
      input.ignore();
      if(input.eof()) return false;
    }
    while(!isNextSeparator(input) && '.' != input.peek()) {
      if(!isdigit(input.peek())) return false;
      input.get(nextChar);
      intValue *= 10;
      intValue += nextChar - '0';
    }
    if(isNextSeparator(input)){
      if(negate)
        value = new NumberValue(-intValue);
      else
        value = new NumberValue(intValue);
      return true;
    }
    else if('.' == input.peek()) {
      float floatValue = intValue;      
      float decimal = 1;
      input.ignore();
      while(!isNextSeparator(input)) {
        if(!isdigit(input.peek())) return false;
        input.get(nextChar);
        decimal *= 0.1;
        floatValue += decimal * (nextChar - '0');
      }
      if(negate)
        value = new NumberValue(-floatValue);
      else
        value = new NumberValue(floatValue);
      return true;
    }
    // non-numeric characters in number
    else {
      return false;
    }
  }

  // ----------------------------------------
  // Parse special symbols
  else if('#' == nextChar) {
    input.ignore();
    char c;
    input.get(c);
    if('f' == c) {
      value = new BoolValue(false);
    }
    else if('t' == c) {
      value = new BoolValue(true);
    }
    else {
      return false;
    }
    return true;
  }
  // ----------------------------------------
  // Parse symbol
  else {
    string readString;
    while(!isNextSeparator(input)) {
      char c;
      input.get(c);
      readString.push_back(c);
    }
    value = new SymbolValue(readString);
    return true;
  }  
}


//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/scheme/Read", TestRead);

  
void
TestRead::read_simple()
{
  // Base types
  ValuePtr value = new Value();
  // string
  CHECK("Read succeeded", read("\"foobarbaz\\n test\\\" 3.1415 test2\"", value));
  CHECK("String type", value->isString());
  CHECK_EQUAL("String content", string, "foobarbaz\n test\" 3.1415 test2", value->vString());
  // integer
  CHECK("Read succeeded", read("12345678", value));
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 12345678, value->vInt());
  // negative integer
  CHECK("Read succeeded", read("-12345678", value));
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, -12345678, value->vInt());
  // float
  CHECK("Read succeeded", read("123.456", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, 123.456, value->vFloat());
  // negative float
  CHECK("Read succeeded", read("-123.456", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, -123.456, value->vFloat());
  // float decimal
  CHECK("Read succeeded", read(".456", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, .456, value->vFloat());
  // negative float decimal
  CHECK("Read succeeded", read("-.456", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, -.456, value->vFloat());
  // float integer
  CHECK("Read succeeded", read("123.", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, 123., value->vFloat());
  // negative float integer
  CHECK("Read succeeded", read("-123.", value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, -123., value->vFloat());

  // symbol
  CHECK("Read succeeded", read("foo_12345", value));
  CHECK("Symbol type", value->isSymbol());
  CHECK_EQUAL("Symbol content", string, "foo_12345", value->vString());

  // boolean false
  CHECK("Read succeeded", read("#f", value));
  CHECK("Boolean false type", value->isBool() && !value->vBool());

  // boolean true
  CHECK("Read succeeded", read("#t", value));
  CHECK("Boolean true type", value->isBool() && value->vBool());

  // ----------------------------------------
  // Simple error situations. These should never hang
  CHECK("No content", !read("", value));
  CHECK("Malformed string", !read("\"this is a string that never closes, but has a trailing quote\\\"", value));
  CHECK("Malformed number", !read("123ab", value));
  CHECK("Malformed number", !read("12.3ab", value));
  CHECK("Malformed number", !read("123a.b", value));

  // ----------------------------------------
  // Compound statement
  istringstream data("1234 #f -1234 \"foo\\n\" 1.234 foobar_1234");

  // first int
  CHECK("Read succeeded", read(data, value));
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 1234, value->vInt());

  // boolean false
  CHECK("Read succeeded", read(data, value));
  CHECK("Boolean false type", value->isBool() && !value->vBool());

  // second int
  CHECK("Read succeeded", read(data, value));
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, -1234, value->vInt());

  // string
  CHECK("Read succeeded", read(data, value));
  CHECK("String type", value->isString());
  CHECK_EQUAL("String content", string, "foo\n", value->vString());

  // float
  CHECK("Read succeeded", read(data, value));
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, 1.234, value->vFloat());

  // symbol
  CHECK("Read succeeded", read(data, value));
  CHECK("Symbol type", value->isSymbol());
  CHECK_EQUAL("Symbol content", string, "foobar_1234", value->vString());
}


void
TestRead::read_list()
{
  ValuePtr value = new Value();

  // List
  CHECK("Read succeeded", read("(1234 a 1.2 \"foo\" (1))", value));
  ValuePtr current = value;

  CHECK("Pair type", current->isPair());
  CHECK("Integer type", current->car()->isNumber() && current->car()->isExact());
  CHECK_EQUAL("Integer content", int, 1234, current->car()->vInt());
  current = current->cdr();
  
  CHECK("Pair type", current->isPair());
  CHECK("Symbol type", current->car()->isSymbol());
  CHECK_EQUAL("Symbol content", string, "a", current->car()->vString());
  current = current->cdr();

  CHECK("Pair type", current->isPair());
  CHECK("Float type", current->car()->isNumber() && !current->car()->isExact());
  CHECK_EQUAL("Float content", float, 1.2, current->car()->vFloat());
  current = current->cdr();

  CHECK("Pair type", current->isPair());
  CHECK("String type", current->car()->isString());
  CHECK_EQUAL("String content", string, "foo", current->car()->vString());
  current = current->cdr();

  // Nested list
  CHECK("Pair type", current->isPair());
  CHECK("Pair type", current->car()->isPair());
  CHECK("Empty list type", current->car()->cdr()->isNull());
  CHECK("Integer type", current->car()->car()->isNumber() && current->car()->car()->isExact());
  CHECK_EQUAL("Integer content", int, 1, current->car()->car()->vInt());
  current = current->cdr();

  CHECK("Empty List type", current->isNull());

  // ----------------------------------------
  // Malformed content
  CHECK("Malformed list", !read("(", value));
  CHECK("Malformed list", !read("( 12a )", value));
  CHECK("Malformed list", !read("( (()())", value));
  CHECK("Malformed list", !read("( (( ; and ends with a comment", value));

}

void
TestRead::read_quote()
{
  ValuePtr value = new Value();

  CHECK("Read succeeded", read("'(foo  ; this is a funky! comment with a list()\n ) ;and this is our end parenthesis", value));
  ValuePtr current = value;

  CHECK("Pair type", current->isPair());
  CHECK("Symbol type", current->car()->isSymbol());
  CHECK_EQUAL("Symbol content", string, "quote", current->car()->vString());
  current = current->cdr();

  CHECK("Pair type", current->isPair());
  CHECK("Pair type", current->car()->isPair());
  CHECK("Symbol type", current->car()->car()->isSymbol());
  CHECK_EQUAL("Symbol content", string, "foo", current->car()->car()->vString());
  CHECK("Empty list type", current->car()->cdr()->isNull());
  current = current->cdr();

  CHECK("Empty list type", current->isNull());  
}


// --------------------------------------------------------------------------------

void
print(ostream& stream, ValuePtr value)
{
  if(value->isPair() && value->isNull()) {
    stream << "()";
  }  
  else if(value->isPair()) {
    if(sListP(value)) {
      stream << "(";
      print(stream, value->car());
      value = value->cdr();
      while(value->isNull() == false) {
        stream << " ";
        print(stream, value->car());
        value = value->cdr();
      }
      stream << ")";
    }
    else {
      stream << "(";
      print(stream, value->car());
      stream << " . ";
      print(stream, value->cdr());
      stream << ")";
    }
  }
  else if(value->isBool() && value->vBool()) {
    stream << "#t";
  }
  else if(value->isBool() && !value->vBool()) {
    stream << "#f";
  }
  else if(value->isString()) {
    stream << "\"" << value->vString() << "\"";
  }
  else if(value->isSymbol()) {
    stream << value->vString();
  }
  else if(value->isProcedure()) {
    stream << "#procedure";
  }
  else if(value->isNumber() && value->isExact()) {
    stream << value->vInt();
  }
  else if(value->isNumber() && !value->isExact()) {
    stream << value->vFloat();
  }
  else {
    stream << "#unknown:" << value->type();
  }  
}
