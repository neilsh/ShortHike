//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef SCHEME_EVAL_HPP
#define SCHEME_EVAL_HPP

#include "Value.hpp"

extern ValuePtr eval(EnvPtr env, string data);
extern ValuePtr eval(EnvPtr env, istream& dataStream);
extern ValuePtr eval(EnvPtr env, ValuePtr data);

extern ValuePtr apply(EnvPtr env, ValuePtr statement);
extern ValuePtr apply(EnvPtr env, ValuePtr proc, ValuePtr args);

// --------------------------------------------------------------------------------

class TestEval : public TestFixture
{
public:
  TEST_FIXTURE_START(TestEval);
  TEST_CASE(eval_simple);
  TEST_CASE(eval_apply);
  TEST_CASE(eval_conditionals);
  TEST_CASE(eval_let);
  TEST_CASE(eval_define);
  TEST_CASE(eval_lambda);
  TEST_FIXTURE_END();

  void eval_simple();
  void eval_apply();
  void eval_conditionals();
  void eval_let();
  void eval_define();
  void eval_lambda();
};


#endif
