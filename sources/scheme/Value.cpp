// --------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

#include "Common.hpp"
#include "Value.hpp"


// --------------------------------------------------------------------------------
int Value::liveValues = 0;
ValuePtr Value::mNullPointer = NULL;
int Value::mIntZero = 0;
float Value::mFloatZero = 0;


ValuePtr
rsTrue()
{
  return new BoolValue(true);
}

ValuePtr
rsFalse()
{
  return new BoolValue(false);
}

ValuePtr
rsUndefined()
{
  return new Value();
}


// --------------------------------------------------------------------------------
// Manually defined for some reason..

bool operator==(const Value* lhs, const ValuePtr& rhs)
{
  return lhs == rhs.mValue;
}

bool operator==(const ValuePtr& lhs, const Value* rhs)
{
  return lhs.mValue == rhs;
}

bool operator==(const ValuePtr& lhs, const ValuePtr& rhs)
{
  return lhs.mValue == rhs.mValue;
}

// --------------------------------------------------------------------------------
// For EnvPtr

bool operator==(const Environment* lhs, const EnvPtr& rhs)
{
  return lhs == rhs.mValue;
}

bool operator==(const EnvPtr& lhs, const Environment* rhs)
{
  return lhs.mValue == rhs;
}

bool operator==(const EnvPtr& lhs, const EnvPtr& rhs)
{
  return lhs.mValue == rhs.mValue;
}
