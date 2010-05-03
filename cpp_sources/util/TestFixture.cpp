//--------------------------------------------------------------------------------
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

// Test harness definition


#include "Common.hpp"
#include "Exceptions.hpp"

string
TestResult::to_string()
{
  ostringstream output;
  output << endl << "-- Test summary results --" << endl;

  if(fixtures_failed_setUp > 0) {
    output << "ERROR: " << fixtures_failed_setUp << " fixtures failed to setUp. Number of tests might be incorrect!" << endl;
  }

  if(fixtures_failed_tearDown > 0) {
    output << "WARNING: " << fixtures_failed_tearDown << " fixtures failed tearDown." << endl;
  }
  
  output << "PASSED: " << passed << endl;
  output << "FAILED: " << failed << endl;
  output << "TOTAL:  " << total << endl;

  return output.str();
}



void
TestFixture::register_test_fixture(string path, TestFixture* fixture)
{
  FIXTURE_DIRECTORY* directory = get_global_fixture_directory();
  (*directory)[path] = fixture;
}

TestResult
TestFixture::run_test_fixtures(string prefix)
{
  TestResult results;
  cout << "Running test fixtures: [" << prefix << "]" << endl;
  FIXTURE_DIRECTORY* directory = get_global_fixture_directory();
  for(FIXTURE_DIRECTORY::const_iterator fixtureI = directory->begin(); fixtureI != directory->end(); ++fixtureI) {
    string path = (*fixtureI).first;
    if(prefix.compare(0, prefix.size(), path, 0, prefix.size()) == 0) {
      TestFixture* currentFixture = (*fixtureI).second;
      // REFACTOR -- Move down to Fixture
      try {
        currentFixture->setUp();
        currentFixture->runTests(results);
      }
      catch(SH_Exception& except) {
        cout << currentFixture->getName() << " setUp FAILED" << endl << except.mDescription << endl;
        results.fixtures_failed_setUp++;
      }    
      catch(...) {
        cout << currentFixture->getName() << " setUp FAILED" << endl << " Uknown runtime exception." << endl;
        results.fixtures_failed_setUp++;
      }

      try {
        currentFixture->tearDown();
      }
      catch(SH_Exception& except) {
        cout << currentFixture->getName() << " setUp FAILED" << endl << except.mDescription << endl;
        results.fixtures_failed_setUp++;
      }    
      catch(...) {
        cout << currentFixture->getName() << " tearDown FAILED" << endl << " Uknown runtime exception." << endl;
        results.fixtures_failed_tearDown++;
      }
    }
  }
  cout << results.to_string() << endl;
  return results;
}

TestFixture::FIXTURE_DIRECTORY*
TestFixture::get_global_fixture_directory()
{
  static FIXTURE_DIRECTORY* directory = new FIXTURE_DIRECTORY();
  return directory;
}



void
TestFixture::addTestCase(TestCase* newTest)
{
  testCases.push_back(newTest);
}

void
TestFixture::runTests(TestResult& results)
{
  if(testCases.size() == 0) {
    cout << fixtureName + ":: - No tests to run -" << endl;
    return;
  }

  for(vector<TestCase*>::const_iterator caseI = testCases.begin(); caseI != testCases.end(); ++caseI) {
    TestCase* currentTest = *caseI;

    ostringstream testResult;
    string testDescription = fixtureName + "::" + currentTest->description;

    const unsigned ARBITRARY_DESCRIPTION_WIDTH = 40;
    if(testDescription.size() < ARBITRARY_DESCRIPTION_WIDTH)
      testDescription.append(ARBITRARY_DESCRIPTION_WIDTH - testDescription.size(), ' ');
    testDescription.append(" : ");
    
    testResult << testDescription << flush;

    results.total++;
    try {
      runSingleTest(currentTest);
      testResult << "OK";
      results.passed++;
    }
    catch(SH_Exception& except) {
      testResult << "FAIL" << endl << except.mFileName << "(" << except.mLineNum << ")" << endl << except.mDescription << endl;
      results.failed++;
    }    
    catch(...) {
      testResult << "FAIL" << endl << " Unknown runtime exception." << endl;
      results.failed++;
    }    
    
    logEngineInfo(testResult.str());
  }  
}




