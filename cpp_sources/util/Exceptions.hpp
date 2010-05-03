//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef UTIL_EXCEPTIONS_HPP
#define UTIL_EXCEPTIONS_HPP

class SH_Exception
{
public:
  SH_Exception(string desc, string test, string file, int line)
    : mDescription(desc), mTest(test), mFileName(file), mLineNum(line)
  {
  }
  
  string mDescription;
  string mTest;
  string mFileName;
  int mLineNum;
};

#define CHECK(description, conditional) checkConditional(description, conditional, #conditional, __FILE__, __LINE__)
#define CHECK_FAIL(description) checkConditional(description, false, "Forced failure", __FILE__, __LINE__)

#define CHECK_EQUAL(description, className, expected, actual)         \
{                                                                     \
 ostringstream testStream;                                            \
 testStream << #actual << " == " << #expected ;                        \
 string expectations;                                                 \
 if(compareExpectations<className>(expected, actual, &expectations) == false) {  \
    ostringstream descriptionStream;                                  \
    descriptionStream << description << endl << " " << expectations;  \
    throw SH_Exception(testStream.str() + "\n" + descriptionStream.str(), "CHECK_EQUAL", __FILE__, __LINE__); \
 }                                                                    \
}


#define CHECK_GREATER(description, className, expected, actual)       \
{                                                                     \
 ostringstream testStream;                                            \
 testStream << #expected << " < " << #actual;                        \
 string expectations;                                                 \
 if(compareExpectationsGreater<className>(expected, actual, &expectations) == false) {  \
    ostringstream descriptionStream;                                  \
    descriptionStream << description << endl << " " << expectations;  \
    throw SH_Exception(testStream.str() + "\n" + descriptionStream.str(), "CHECK_GREATER", __FILE__, __LINE__); \
 }                                                                    \
}


#pragma warning(disable: 4100)
inline void
checkConditional(string description, bool conditional, string test, char* fileName, int line)
{
  if(conditional == false) {
    int* crash = NULL;
    *crash = 1;
    exit(0);
  }
  
//     throw SH_Exception(test + description, "checkConditional", fileName, line);
}


template<class C>
inline bool
compareExpectations(C expected, C actual, string* expectationsDescription)
{
  if(expected == actual) {
    return true;
  }
  else {
    ostringstream descriptionStream;
    descriptionStream << "Expected: " << expected  << "  Actual: " << actual;
    *expectationsDescription = descriptionStream.str();
    return false;
  }
}

template<>
inline bool
compareExpectations<float>(float expected, float actual, string* expectationsDescription)
{
  if(fabs(fabs(expected) - fabs(actual)) < 0.1f) {
    return true;
  }
  else {
    ostringstream descriptionStream;
    descriptionStream << "Expected: " << expected  << "  Actual: " << actual;
    *expectationsDescription = descriptionStream.str();
    return false;
  }
}

template<>
inline bool
compareExpectations<double>(double expected, double actual, string* expectationsDescription)
{
  if(abs(abs(expected) - abs(actual)) < 0.1f) {
    return true;
  }
  else {
    ostringstream descriptionStream;
    descriptionStream << "Expected: " << expected  << "  Actual: " << actual;
    *expectationsDescription = descriptionStream.str();
    return false;
  }
}



template<class C>
inline bool
compareExpectationsGreater(C expected, C actual, string* expectationsDescription)
{
  if(expected < actual) {
    return true;
  }
  else {
    ostringstream descriptionStream;
    descriptionStream << "Expected greater than: " << expected  << "  Actual: " << actual;
    *expectationsDescription = descriptionStream.str();
    return false;
  }
}


class TestExceptions : public TestFixture
{
public:
  TEST_FIXTURE_START(TestExceptions);
  TEST_CASE(assert_handling);
  TEST_FIXTURE_END();

  void assert_handling();
};


#endif
