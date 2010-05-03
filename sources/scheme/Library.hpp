// --------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

#pragma once
#ifndef SCHEME_LIBRARY_HPP
#define SCHEME_LIBRARY_HPP

#include "Value.hpp"

// --------------------------------------------------------------------------------

extern EnvPtr libraryEnvironment();
extern EnvPtr standardEnvironment();

// ----------------------------------------
// 6.1 Equivalence predicates

extern bool sEqP(ValuePtr lhs, ValuePtr rhs);

// ---------------------------------------- 
// 6.2.5 Numerics

extern ValuePtr sAdd(EnvPtr, ValuePtr args);

// ----------------------------------------
// 6.3.1 Booleans
extern ValuePtr sNot(EnvPtr, ValuePtr args);

// ---------------------------------------- 
// 6.3.2 Pairs and lists

extern ValuePtr sCons(ValuePtr args);
extern ValuePtr sCar(ValuePtr args);
extern ValuePtr sCdr(ValuePtr args);

extern bool sListP(ValuePtr args);
extern ValuePtr sList(ValuePtr args);

extern int sLength(ValuePtr args);
extern ValuePtr sReverse(EnvPtr env, ValuePtr args);
extern ValuePtr sAssoq(ValuePtr args);

// ----------------------------------------
// 6.3.5 Strings

extern ValuePtr sStringAppend(EnvPtr env, ValuePtr args);

// ----------------------------------------
// 6.4 Control Features 

extern ValuePtr sForEach(EnvPtr env, ValuePtr args);

// ----------------------------------------
// 6.6.3

extern ValuePtr sWrite(EnvPtr env, ValuePtr args);

// ---------------------------------------- 
// Predicates
extern bool sBooleanP(ValuePtr args);
extern bool sSymbolP(ValuePtr args);
extern bool sProcedureP(ValuePtr args);
extern bool sPairP(ValuePtr args);
extern bool sNumberP(ValuePtr args);
extern bool sStringP(ValuePtr args);

// --------------------------------------------------------------------------------
// Exported functions

extern ValuePtr sCountModules(EnvPtr env, ValuePtr args);
extern ValuePtr sShowMessage(EnvPtr env, ValuePtr args);
extern ValuePtr sEnableModule(EnvPtr env, ValuePtr args);

// --------------------------------------------------------------------------------

class TestLibrary : public TestFixture
{
public:
  TEST_FIXTURE_START(TestLibrary);
  TEST_CASE(numerical_operations);
  TEST_CASE(booleans);
  TEST_CASE(pairs_lists);
  TEST_CASE(strings);
  TEST_CASE(control_operations);
  TEST_FIXTURE_END();

  void numerical_operations();
  void booleans();
  void pairs_lists();
  void strings();
  void control_operations();
};

#endif
