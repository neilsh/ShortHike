//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Read;
private import std.stream;
private import Rect;

typedef Value (*NativeProcedure)(Environment env, Value[] argumentList);

// --------------------------------------------------------------------------------


enum Type {
  UNDEFINED,
  BOOLEAN,
  SYMBOL,
  PAIR,
  NUMBER_INT,
  NUMBER_DOUBLE,
  STRING,
  PROCEDURE,
  NATIVE_PROCEDURE,
  ENVIRONMENT,
  RECT,
  ID
};


class Value
{
  this()
  {
    mLiveValues++;
  }

  ~this()
  {
    mLiveValues--;
  }

  bool isBool() {return false;}
  bool isString() {return false;}
  bool isSymbol() {return false;}
  bool isPair() {return false;}
  bool isNull() {return false;}
  bool isProcedure() {return false;}
  bool isNumber() {return false;}
  bool isExact() {return false;}
  Type type()  {return Type.UNDEFINED;}

  bool vBool() {assert(CHECK_FAIL("Accessing undefined")); return false;}
  char[] vString() {assert(CHECK_FAIL("Accessing undefined")); return "";}
  Value car() {assert(CHECK_FAIL("Accessing undefined")); return mNullPointer;}
  Value cdr() {assert(CHECK_FAIL("Accessing undefined")); return mNullPointer;}
  int vInt() {assert(CHECK_FAIL("Accessing undefined")); return mIntZero;}
  float vFloat() {assert(CHECK_FAIL("Accessing undefined")); return mFloatZero;}

  static int mLiveValues ;

  // --------------------------------------------------------------------------------
  // String conversion

  char[]
  toString()
  {
    MemoryStream memStream = new MemoryStream("");
    printValue(memStream, this);
    memStream.seekSet(0);
    return memStream.readString(memStream.available());
  }

private:
  static Value mNullPointer = null;
  static int mIntZero = 0;
  static float mFloatZero = 0.0;
};


// --------------------------------------------------------------------------------

class BoolValue : public Value
{
  this(bool value) {mBool = value;}

  bool isBool() {return true;}
  Type type()  {return Type.BOOLEAN;}
  bool vBool() {return mBool;}

  bool mBool;
};


class NumberValue : public Value
{
public:
  this(int number) {
    mExact = true;
    mInt = number;
  }

  this(float number) {
    mExact = false;
    mFloat = number;
  }

  bool isNumber() {return true;}
  bool isExact() {return mExact;}

  int vInt() {
    if(!mExact)
      assert(CHECK_FAIL("Trying to access inexact number as int"));
    return mInt;
  }
  int vInt(int i) {return mInt = i;}
  float vFloat() {
    if(mExact)
      assert(CHECK_FAIL("Trying to access exact number as float"));
    return mFloat;
  }
  float vFloat(float f) {return mFloat = f;}

  void toInExact()
  {
    if(mExact) {
      mFloat = mInt;
      mExact = false;
    }
  }
private:
  bool mExact;
  int mInt;
  float mFloat;
};


class StringValue : public Value
{
public:
  this(char[] text) { mString = text; }
  bool isString() {return true;}
  Type type()  {return Type.STRING;}
  char[] vString() {return mString;}
private:
  char[] mString;
};

class SymbolValue : public Value
{
public:
  this(char[] text) { mString = text; }

  bool isSymbol() {return true;}
  Type type()  {return Type.SYMBOL;}
  char[] vString() {return mString;}
private:
  char[] mString;
};

class PairValue : public Value
{
public:
  this(Value _car, Value _cdr) {mCar = _car; mCdr = _cdr;}
  this() {mCar = null; mCdr = null;}
  bool isPair() {return true;}
  bool isNull() {return cast(bool)(mCar is null && mCdr is null);}
  Type type() {return Type.PAIR;}

  Value car() {return mCar;}
  Value car(Value pCar) {return mCar = pCar; }  
  Value cdr() {return mCdr;}
  Value cdr(Value pCdr) {return mCdr = pCdr; }
private:
  Value mCar;
  Value mCdr;
};

class NativeProcedureValue : public Value
{
public:
  this(NativeProcedure proc) {mProc = proc;}
  bool isProcedure() {return true;}
  Type type() {return Type.NATIVE_PROCEDURE;}

  NativeProcedure mProc;  
};


class ProcedureValue : public Value
{
public:
  this() {}
  bool isProcedure() {return true;}
  Type type() {return Type.PROCEDURE;}

  Environment environment;
  char[][] paramList;
  Value mBody;
};



// --------------------------------------------------------------------------------

class Environment : public Value
{
public:
  Type type()  {return Type.ENVIRONMENT;}

  Value[char[]] values;
  Environment parent;
};

// --------------------------------------------------------------------------------

Value
rsEmptyList()
{
  return new PairValue();
}

Value
rsTrue()
{
  return new BoolValue(true);
}

Value
rsFalse()
{
  return new BoolValue(false);
}

Value
rsUndefined()
{
  return new Value();
}


// // --------------------------------------------------------------------------------
// // Manually defined for some reason..

// bool opEq(Value lhs, Value rhs)
// {
//   return lhs == rhs.mValue;
// }

// bool opEq(Value lhs, Value rhs)
// {
//   return lhs.mValue == rhs;
// }

// bool operator==(const Value& lhs, const Value& rhs)
// {
//   return lhs.mValue == rhs.mValue;
// }

// // --------------------------------------------------------------------------------
// // For Environment

// bool operator==(const Environment* lhs, const Environment& rhs)
// {
//   return lhs == rhs.mValue;
// }

// bool operator==(const Environment& lhs, const Environment* rhs)
// {
//   return lhs.mValue == rhs;
// }

// bool operator==(const Environment& lhs, const Environment& rhs)
// {
//   return lhs.mValue == rhs.mValue;
// }
