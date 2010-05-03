//--------------------------------------------------------------------------------
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

// Test harness definition  

private import std.math;
private import std.string;
private import Log;

// --------------------------------------------------------------------------------

struct TestSummary
{
  uint total = 0;
  uint passed = 0;
  uint failed = 0;
  uint failed_setUp = 0;
  uint failed_tearDown = 0;
}

// --------------------------------------------------------------------------------
// Global test manager

private static TestFixture[] gTestFixtures;

void
addTestFixture(TestFixture fixture)
{
    gTestFixtures ~= fixture;
}


void
runTestFixtures(char[] prefix = "")
{
  TestSummary summary;
  for(int iTest = 0; iTest < gTestFixtures.length; ++iTest) {
    TestFixture fixture = gTestFixtures[iTest];
    fixture.runTests(summary);
  }

  logInfo << endl << "-- Test summary results --" << endl;

  if(summary.failed_setUp > 0) {
    logInfo << "ERROR: " << summary.failed_setUp << " fixtures failed to setUp. Number of tests might be incorrect!" << endl;
  }

  if(summary.failed_tearDown > 0) {
    logInfo << "WARNING: " << summary.failed_tearDown << " fixtures failed tearDown." << endl;
  }
  
  logInfo << "PASSED: " << summary.passed << endl;
  logInfo << "FAILED: " << summary.failed << endl;
  logInfo << "TOTAL:  " << summary.total << endl;
}


// --------------------------------------------------------------------------------

const int TEST_WIDTH = 50;

class TestFixture
{
  void setUp() {};
  void tearDown() {};

  void runTests(inout TestSummary summary)
  {
    try {
      setUp();
    }
    catch(Exception e) {
      summary.failed_setUp++;
      return;
    }
    for(int iTest = 0; iTest < mTestNames.length; ++iTest) {
      summary.total++;
      char[] testID = ljustify(mFixtureName ~ ":" ~ mTestNames[iTest], TEST_WIDTH);
      try {
        void delegate() test = mTestMethods[iTest];
        test();
//         logInfo << testID << " : PASSED " << endl;
        summary.passed++;
      }
      catch(Exception e) {
        logInfo << testID << " : FAILED " << endl
          << e.toString() << endl;
        summary.failed++;
      }
    }
    try {
      tearDown();
    }
    catch(Exception e) {
      summary.failed_tearDown++;
    }
  }


  char[] getName()
  {
    return mFixtureName;
  }

  void
  addTest(char[] name, void delegate() test)
  {
    mTestNames ~= name;
    mTestMethods ~= test;
  }
  

  char[][] mTestNames;
  void delegate()[] mTestMethods;
  char[] mFixtureName;
}

// --------------------------------------------------------------------------------

class TestBase : public TestFixture
{
  this()
  {
    mFixtureName = "TestBase";
    addTest("simple_pass", &simple_pass);
  }

  static this()
  {
    addTestFixture(new TestBase());
  }
  
  void simple_pass()
  {
  }
}






