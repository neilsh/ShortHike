//--------------------------------------------------------------------------------
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

// Test harness definition

#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP


class TestResult
{
public:
  TestResult()
  {
    total = 0;
    passed = 0;
    failed = 0;
    fixtures_failed_setUp = 0;
    fixtures_failed_tearDown = 0;
  }
  
  unsigned total;
  unsigned passed;
  unsigned failed;
  unsigned fixtures_failed_setUp;
  unsigned fixtures_failed_tearDown;
  
  string to_string();
};


class TestFixture
{
public:
  virtual void setUp() {};
  virtual void tearDown() {};

  void runTests(TestResult& results);

  string getName() {return fixtureName;}

  static void register_test_fixture(string path, TestFixture* fixture);
  static TestResult run_test_fixtures(string prefix = "");
protected:
  class TestCase {
  public:
    string description;
    virtual void getName() {};
  };

  virtual void runSingleTest(TestCase* testCase) = 0;

  void addTestCase(TestCase* newTestCase);
  void setName(string name) {fixtureName = name;}
private:  
  string fixtureName;
  vector<TestCase*> testCases;

  typedef map<string, TestFixture*> FIXTURE_DIRECTORY;
  static FIXTURE_DIRECTORY* get_global_fixture_directory();
};


// Begin: TEST_FIXTURE_START
#define TEST_FIXTURE_START(className)                              \
                                                                   \
typedef className FixtureClass;                                    \
                                                                   \
className::className()                                             \
{                                                                  \
 registerTests();                                                  \
}                                                                  \
                                                                   \
class AutoTestRegister                                             \
{                                                                  \
public:                                                            \
  AutoTestRegister(string path)                                    \
  {                                                                \
    register_test_fixture(path, new className());                  \
  }                                                                \
};                                                                 \
                                                                   \
static AutoTestRegister automatic_registering;                     \
                                                                   \
class LocalTestCase : public TestCase                              \
{                                                                  \
 public: void(className::* method)();                              \
};                                                                 \
virtual void runSingleTest(TestCase* testCase)                     \
{                                                                  \
LocalTestCase* myCase = dynamic_cast<LocalTestCase*>(testCase);    \
(this->*(myCase->method))();                                       \
}                                                                  \
void registerTests()                                               \
{                                                                  \
 setName(#className);
// End: TEST_FIXTURE_START



// Begin: TEST_FIXTURE_END
#define TEST_FIXTURE_END() }                                       \
// End: TEST_FIXTURE_END



// Begin: TEST_CASE
#define TEST_CASE(methodName)                                      \
{                                                                  \
 LocalTestCase* newCase = new LocalTestCase;                       \
 newCase->description = #methodName;                               \
 newCase->method = &FixtureClass::methodName;                      \
 addTestCase(newCase);                                             \
}
// End: TEST_CASE


// Begin: TEST_FIXTURE_DEFINITION
#define TEST_FIXTURE_DEFINITION(path, className)                   \
className::AutoTestRegister className::automatic_registering(path);
// End: TEST_FIXTURE_DEFINITION


#endif



