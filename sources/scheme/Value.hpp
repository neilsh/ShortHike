//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef SCHEME_VALUE_HPP
#define SCHEME_VALUE_HPP

class Value;
class Environment;

#pragma warning(disable: 4201)

template<class C>
class RefPtr
{
public:
  RefPtr();
  RefPtr(C*);
  RefPtr(const RefPtr& rhs);
  ~RefPtr();
  
  RefPtr& operator=(C* value);
  RefPtr& operator=(const RefPtr& rhs);
  C* operator->();
  
  C* mValue;
};


typedef RefPtr<Value> ValuePtr;
extern bool operator==(const Value* lhs, const ValuePtr& rhs);
extern bool operator==(const ValuePtr& lhs, const Value* rhs);
extern bool operator==(const ValuePtr& lhs, const ValuePtr& rhs);

typedef RefPtr<Environment> EnvPtr;
extern bool operator==(const Environment* lhs, const EnvPtr& rhs);
extern bool operator==(const EnvPtr& lhs, const Environment* rhs);
extern bool operator==(const EnvPtr& lhs, const EnvPtr& rhs);

typedef ValuePtr (*NativeProcedure)(EnvPtr env, ValuePtr argumentList);

// --------------------------------------------------------------------------------

class Value
{
public:
  Value()
  {
    refCount = 0;
    liveValues++;
  }

  virtual ~Value()
  {
    liveValues--;
  }  


  void release()
  {
    refCount--;
    if(refCount <= 0)
      delete this;
  }

  void addRef()
  {
    refCount++;
  }  
  


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
    ENVIRONMENT
  };


  virtual bool isBool() {return false;}
  virtual bool isString() {return false;}
  virtual bool isSymbol() {return false;}
  virtual bool isPair() {return false;}
  virtual bool isNull() {return false;}
  virtual bool isProcedure() {return false;}
  virtual bool isNumber() {return false;}
  virtual bool isExact() {return false;}
  virtual Type type()  {return UNDEFINED;}

  virtual bool vBool() {CHECK_FAIL("Accessing undefined"); return false;}
  virtual string vString() {CHECK_FAIL("Accessing undefined"); return "";}
  virtual ValuePtr& car() {CHECK_FAIL("Accessing undefined"); return mNullPointer;}
  virtual ValuePtr& cdr() {CHECK_FAIL("Accessing undefined"); return mNullPointer;}
  virtual int& vInt() {CHECK_FAIL("Accessing undefined"); return mIntZero;}
  virtual float& vFloat() {CHECK_FAIL("Accessing undefined"); return mFloatZero;}

  static int liveValues ;

private:
  int refCount;
  static ValuePtr mNullPointer;
  static int mIntZero;
  static float mFloatZero;
};


// --------------------------------------------------------------------------------

class BoolValue : public Value
{
public:
  BoolValue(bool value) {mBool = value; }
  virtual bool isBool() {return true;}
  virtual Type type()  {return BOOLEAN;}
  virtual bool vBool() {return mBool;}
private:
  bool mBool;
};


class NumberValue : public Value
{
public:
  NumberValue(int number) {
    mExact = true;
    mInt = number;
  }

  NumberValue(float number) {
    mExact = false;
    mFloat = number;
  }

  virtual bool isNumber() {return true;}
  virtual bool isExact() {return mExact;}

  virtual int& vInt() {
    if(!mExact)
      CHECK_FAIL("Trying to access inexact number as int");
    return mInt;
  }
  virtual float& vFloat() {
    if(mExact)
      CHECK_FAIL("Trying to access exact number as float");
    return mFloat;
  }
private:
  bool mExact;
  int mInt;
  float mFloat;
};


class StringValue : public Value
{
public:
  StringValue(string text) { mString = text; }
  virtual bool isString() {return true;}
  virtual Type type()  {return STRING;}
  virtual string vString() {return mString;}
private:
  string mString;
};

class SymbolValue : public Value
{
public:
  SymbolValue(string text) { mString = text; }

  virtual bool isSymbol() {return true;}
  virtual Type type()  {return SYMBOL;}
  virtual string vString() {return mString;}
private:
  string mString;
};

class PairValue : public Value
{
public:
  PairValue(ValuePtr _car, ValuePtr _cdr) {mCar = _car; mCdr = _cdr;}
  PairValue() {mCar = NULL; mCdr = NULL;}
  virtual bool isPair() {return true;}
  virtual bool isNull() {return mCar == NULL && mCdr == NULL;}
  virtual Type type() {return PAIR;}
  virtual ValuePtr& car() {return mCar;}
  virtual ValuePtr& cdr() {return mCdr;}
private:
  ValuePtr mCar;
  ValuePtr mCdr;
};

class NativeProcedureValue : public Value
{
public:
  NativeProcedureValue(NativeProcedure proc) {mProc = proc;}
  virtual bool isProcedure() {return true;}
  virtual Type type() {return NATIVE_PROCEDURE;}

  NativeProcedure mProc;  
};

class ProcedureValue : public Value
{
public:
  ProcedureValue() {}
  virtual bool isProcedure() {return true;}
  virtual Type type() {return PROCEDURE;}

  EnvPtr environment;
  vector<string> paramList;
  ValuePtr body;
};



// --------------------------------------------------------------------------------

class Environment : public Value
{
public:
  virtual Type type()  {return ENVIRONMENT;}

  map<string, ValuePtr> values;
  EnvPtr parent;
};

extern ValuePtr rsTrue();
extern ValuePtr rsFalse();
extern ValuePtr rsUndefined();


// --------------------------------------------------------------------------------
// Template definitions need to be up here

template<class C>
RefPtr<C>::RefPtr()
{
  mValue = NULL;
}

template<class C>
RefPtr<C>::RefPtr(C* value)
{
  mValue = value;
  if(mValue != NULL)
    mValue->addRef();
}

template<class C>
RefPtr<C>::RefPtr(const RefPtr& rhs)
{
  if(rhs.mValue != NULL)
    rhs.mValue->addRef();
  mValue = rhs.mValue;
}



template<class C>
RefPtr<C>::~RefPtr()
{
  if(NULL != mValue) {
    mValue->release();
  }
}
  
template<class C>
RefPtr<C>& RefPtr<C>::operator=(C* value)
{
  if(mValue == value) return *this;
  if(value != NULL)
    value->addRef();
  if(mValue != NULL)
    mValue->release();
  mValue = value;
  return *this;
}

template<class C>
RefPtr<C>& RefPtr<C>::operator=(const RefPtr& rhs)
{
  if(mValue == rhs.mValue) return *this;
  if(rhs.mValue != NULL)
    rhs.mValue->addRef();
  if(mValue != NULL)
    mValue->release();
  mValue = rhs.mValue;
  return *this;
}


template<class C>
C* RefPtr<C>::operator->()
{
  return mValue;
}





#endif
