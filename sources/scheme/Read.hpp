//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef SCHEME_READ_HPP
#define SCHEME_READ_HPP

#include "Value.hpp"

extern bool read(string code, ValuePtr& value);
extern bool read(istream& stream, ValuePtr& value);
extern void print(ostream& stream, ValuePtr value);

// --------------------------------------------------------------------------------

class TestRead : public TestFixture
{
public:
  TEST_FIXTURE_START(TestRead);
  TEST_CASE(read_simple);
  TEST_CASE(read_list);
  TEST_CASE(read_quote);
  TEST_FIXTURE_END();

  void read_simple();
  void read_list();
  void read_quote();
};


#endif
