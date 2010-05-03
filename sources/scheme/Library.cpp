// --------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

#include "Common.hpp"

#include "Library.hpp"
#include "Value.hpp"
#include "Eval.hpp"
#include "Read.hpp"

// REFACTOR exported functions
#include "REFACTOR_Common.hpp"
#include "simulation/craft.hpp"
#include "eva.hpp"
#include "gamestate.hpp"

// --------------------------------------------------------------------------------
// 6.1 Equivalence predicates

bool
sEqP(ValuePtr lhs, ValuePtr rhs)
{
  if(lhs->type() != rhs->type()) return false;
  if(lhs->isBool() && lhs->vBool() == rhs->vBool()) return true;
  else if(lhs->isNumber() && lhs->isExact() && rhs->isExact() && lhs->vInt() == rhs->vInt()) return true;
  else if(lhs->isNumber() && !lhs->isExact() && !rhs->isExact() && lhs->vFloat() == rhs->vFloat()) return true;
  else if(lhs->isString() && string(lhs->vString()) == string(rhs->vString())) return true;
  else if(lhs->isSymbol() && string(lhs->vString()) == string(rhs->vString())) return true;
  else if(lhs->isPair() && (lhs == rhs || (lhs->isNull() && rhs->isNull()))) return true;
  else if(lhs->isProcedure() && (lhs.mValue == rhs.mValue)) return true;
  else if(lhs->type() == Value::UNDEFINED) return false;
  return false;
}

// ================================================================================
// 6.2.5 Numerical operations

bool
sPureInteger(ValuePtr args)
{
  while(args->isNull() == false) {
    if(!(args->car()->isNumber() && args->car()->isExact())) return false;
    args = args->cdr();
  }
  return true;
}

float
sAsFloat(ValuePtr number)
{
  CHECK("Error convering non number to float", number->isNumber());
  if(!number->isExact()) {
    return number->vFloat();
  }
  else {
    return static_cast<float>(number->vInt());
  }
}

#define NUM_TEST(ACTUAL_TEST) \
  if(!sPureInteger(args)) { \
    float prev = sAsFloat(args->car()); \
    args = args->cdr(); \
    while(args->isNull() == false) { \
      float curr = sAsFloat(args->car()); \
      if(!(prev ACTUAL_TEST curr)) return rsFalse(); \
      prev = curr; \
      args = args->cdr(); \
    } \
  } \
  else { \
    int prev = args->car()->vInt(); \
    args = args->cdr(); \
    while(args->isNull() == false) { \
      int curr = args->car()->vInt(); \
      if(!(prev ACTUAL_TEST curr)) return rsFalse(); \
      prev = curr; \
      args = args->cdr(); \
    } \
  }   \
  return rsTrue();


ValuePtr sNumEqual(EnvPtr env, ValuePtr args) { NUM_TEST(==) }
ValuePtr sNumLess(EnvPtr env, ValuePtr args) { NUM_TEST(<) }
ValuePtr sNumGreater(EnvPtr env, ValuePtr args) { NUM_TEST(>) }
ValuePtr sNumLessEqual(EnvPtr env, ValuePtr args) { NUM_TEST(<=) }
ValuePtr sNumGreaterEqual(EnvPtr env, ValuePtr args) { NUM_TEST(>=) }

// --------------------------------------------------------------------------------

ValuePtr
sAdd(EnvPtr env, ValuePtr args)
{
  ValuePtr result = new NumberValue(0);
  while(args->isNull() == false) {
    if(args->car()->isNumber() && args->car()->isExact())
      result->vInt() += args->car()->vInt();
    args = args->cdr();
  }
  return result;
}

// ================================================================================
// 6.2.5 Numerical input and output

ValuePtr
sNumberToString(EnvPtr env, ValuePtr args)
{
  if(sLength(args) == 1 && args->car()->isNumber()) {
    ostringstream ossStream;
    if(args->car()->isExact())
      ossStream << args->car()->vInt();
    else
      ossStream << args->car()->vFloat();
    return new StringValue(ossStream.str());
  }  
  return rsUndefined();
}


// --------------------------------------------------------------------------------
// 6.3.1 Booleans

ValuePtr
sNot(EnvPtr, ValuePtr args)
{
  CHECK("Single argument", sLength(args) == 1);
  if(sEqP(args->car(), rsFalse()))
    return rsTrue();
  else
    return rsFalse();
}



// --------------------------------------------------------------------------------
// 6.3.2 Pairs and lists

ValuePtr
sCons(EnvPtr env, ValuePtr args)
{
  CHECK("Number of arguments is 2", sLength(args) == 2);
  return new PairValue(args->car(), args->cdr()->car());
}

ValuePtr
sCar(EnvPtr env, ValuePtr args)
{
  CHECK("Number of arguments", sLength(args) == 1);
  CHECK("Argument is pair", args->car()->isPair());
  return args->car()->car();
}

ValuePtr
sCdr(EnvPtr env, ValuePtr args)
{
  CHECK("Number of arguments", sLength(args) == 1);
  CHECK("Argument is pair", args->car()->isPair());
  return args->car()->cdr();
}

ValuePtr
sSetCar(EnvPtr env, ValuePtr args)
{
  args->car()->car() = args->cdr()->car();
  return rsUndefined();
}

ValuePtr
sSetCdr(EnvPtr env, ValuePtr args)
{
  args->car()->cdr() = args->cdr()->car();
  return rsUndefined();
}



bool
sListP(ValuePtr data)
{
  while(data->isPair() && !data->isNull())
    data = data->cdr();
  return data->isNull();
}

ValuePtr
sList(EnvPtr env, ValuePtr args)
{
  ValuePtr result = new PairValue();
  ValuePtr current = result;
  while(args->isNull() == false) {
    current->car() = args->car();
    ValuePtr next = new PairValue();
    current->cdr() = next;
    current = next;
    args = args->cdr();
  }
  return result;
}


int
sLength(ValuePtr data)
{
  int length = 0;
  ValuePtr ptr = data;
  while(ptr->isNull() == false) {
    if(ptr->isPair() == false) CHECK_FAIL("Malformed list to length");
    length++;
    ptr = ptr->cdr();
  }
  return length;
}

ValuePtr
sReverse(EnvPtr, ValuePtr args)
{
  CHECK("Single argument to reverse", sLength(args) == 1 && sListP(args->car()));
  ValuePtr current = new PairValue();
  ValuePtr rList = args->car();
  while(rList->isNull() == false) {
    ValuePtr next = new PairValue(rList->car(), current);
    current = next;
    rList = rList->cdr();
  }
  return current;
}


ValuePtr
sAssq(EnvPtr, ValuePtr args)
{
  if(sLength(args) != 2)
    CHECK_FAIL("Wrong number of arguments to assq");
  ValuePtr obj = args->car();
  ValuePtr alist = args->cdr()->car();
  while(alist->isNull() == false) {
    if(alist->isPair() == false && alist->car()->isPair() == false)
      CHECK_FAIL("Malformed alist");
    if(sEqP(obj, alist->car()->car())) {
      return alist->car();
    }
    alist = alist->cdr();
 }
  return rsFalse();
}

// ================================================================================
// 6.3.5 Strings

ValuePtr
sStringAppend(EnvPtr env, ValuePtr args)
{
  ostringstream ossStream;
  while(args->isNull() == false) {
    if(args->car()->isString())
      ossStream << args->car()->vString();
    args = args->cdr();
  }
  return new StringValue(ossStream.str());
}




// ================================================================================
// 6.4 Control features

ValuePtr
sApply(EnvPtr env, ValuePtr args)
{
  CHECK("Two parameters required", sLength(args) == 2);
  return apply(env, args->car(), args->cdr()->car());
}


ValuePtr
sMap(EnvPtr env, ValuePtr args) 
{
  CHECK("Only single parameter map supported", sLength(args) == 2);
  ValuePtr proc = args->car();
  ValuePtr list1 = args->cdr()->car();
  CHECK("Valid arguments to map", (proc->isProcedure()) && sListP(list1));

  ValuePtr result = new PairValue();
  ValuePtr current = result;
  
  ValuePtr argList = new PairValue(NULL, new PairValue());
  
  while(list1->isNull() == false) {    
    argList->car() = list1->car();
    current->car() = apply(env, proc, argList);
    ValuePtr next = new PairValue();
    current->cdr() = next;
    current = next;

    list1 = list1->cdr();
  }  
  return result;
}

ValuePtr
sForEach(EnvPtr env, ValuePtr args) 
{
  CHECK("Only single parameter for-each supported", sLength(args) == 2);
  ValuePtr proc = args->car();
  ValuePtr list1 = args->cdr()->car();
  CHECK("Valid arguments to foreach", (proc->isProcedure()) && sListP(list1));

  ValuePtr argList = new PairValue(NULL, new PairValue());
  
  while(list1->isNull() == false) {
    argList->car() = list1->car();
    apply(env, proc, argList);
    list1 = list1->cdr();
  }  
  
  return rsUndefined();
}


// ================================================================================
// 6.6.3

ValuePtr
sWrite(EnvPtr, ValuePtr args)
{
  ostringstream ossValue;
  print(ossValue, args->car());
  logEngineInfo(ossValue.str());
  return rsUndefined();
}

// ================================================================================
// Local exports

ValuePtr
sCountModules(EnvPtr env, ValuePtr args)
{
  using SSM::Craft;
  Craft* station = Craft::getSingleton();
  return new NumberValue(station->countUnits());
}


ValuePtr
sShowMessage(EnvPtr env, ValuePtr args)
{
  using SSM::EVA;
  using SSM::Controller;
  
  if(sLength(args) == 1 && args->car()->isString()) {
    EVA* evaScreen = Controller::getSingleton()->evaScreen;
    evaScreen->missionMessage(string(args->car()->vString()));
  }
  return rsUndefined();
}


ValuePtr
sEnableModule(EnvPtr env, ValuePtr args)
{
  using SSM::EVA;
  using SSM::Controller;
  
  if(sLength(args) == 1 && args->car()->isString()) {
    EVA* evaScreen = Controller::getSingleton()->evaScreen;
    evaScreen->enableModule(string(args->car()->vString()));
  }
  return rsUndefined();
}

ValuePtr
sGetCreditResult(EnvPtr env, ValuePtr args)
{
  using SSM::Craft;
  
  return new NumberValue(Craft::getSingleton()->getCreditResult());
}

ValuePtr
sGetFlow(EnvPtr env, ValuePtr args)
{
  using namespace SSM;

  if(sLength(args) == 1 && args->car()->isSymbol()) {
    Craft* station = Craft::getSingleton();
    if(args->car()->vString() == "energy") {
      return new NumberValue(station->getFlow(ENERGY));
    }
    else if(args->car()->vString() == "thermal-control") {
      return new NumberValue(station->getFlow(COOLING));
    }
    else {
      return new NumberValue(station->getFlow(LIFE_SUPPORT));
    }
  }
  return rsUndefined();
}

ValuePtr
sGetPositiveCapacity(EnvPtr env, ValuePtr args)
{
  using namespace SSM;

  if(sLength(args) == 1 && args->car()->isSymbol()) {
    if(args->car()->vString() == "crew") {
      return new NumberValue(Craft::getSingleton()->getLocalResourcePositiveCapacity(CREW_SPACE));
    }
  }  
  return rsUndefined();
}


// ================================================================================
// Binding

void
bindFunction(EnvPtr env, string name, NativeProcedure proc)
{
  env->values[name] = new NativeProcedureValue(proc);
}

// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// Wrapped native functions

ValuePtr eEqP(EnvPtr, ValuePtr param) {return new BoolValue(sEqP(param->car(), param->cdr()->car()));}

ValuePtr eListP(EnvPtr, ValuePtr param) { return new BoolValue(sListP(param)); }
ValuePtr eLength(EnvPtr, ValuePtr param) {return new NumberValue(sLength(param->car()));}


EnvPtr
libraryEnvironment()
{
  EnvPtr env = new Environment();
  env->parent = NULL;
  // 6.1 Equivalence predicates
  bindFunction(env, "eq?", eEqP);

  // 6.2.5 Numerical operations
  bindFunction(env, "=", sNumEqual);
  bindFunction(env, "<", sNumLess);
  bindFunction(env, ">", sNumGreater);
  bindFunction(env, "<=", sNumLessEqual);
  bindFunction(env, ">=", sNumGreaterEqual);
  bindFunction(env, "+", sAdd);
  // 6.2.6 Numerical input and output
  bindFunction(env, "number->string", sNumberToString);

  // 6.3.1 Booleans
  bindFunction(env, "not", sNot);
  // 6.3.2 Pairs and lists
  bindFunction(env, "cons", sCons);
  bindFunction(env, "car", sCar);
  bindFunction(env, "cdr", sCdr);
  bindFunction(env, "set-car!", sSetCar);
  bindFunction(env, "set-cdr!", sSetCdr);
  bindFunction(env, "list?", eListP);
  bindFunction(env, "length", eLength);

  bindFunction(env, "list", sList);
  bindFunction(env, "reverse", sReverse);
  bindFunction(env, "assq", sAssq);
  // 6.3.5 Strings
  bindFunction(env, "string-append", sStringAppend);
  
  // 6.4 Control features
  bindFunction(env, "apply", sApply);
  bindFunction(env, "map", sMap);
  bindFunction(env, "for-each", sForEach);


  // 6.6.3 Output
  bindFunction(env, "write", sWrite);
  return env;
}

// --------------------------------------------------------------------------------

EnvPtr
standardEnvironment()
{
  EnvPtr env = libraryEnvironment();
  
  bindFunction(env, "count-modules", sCountModules);
  bindFunction(env, "show-message", sShowMessage);
  bindFunction(env, "enable-module", sEnableModule);
  
  bindFunction(env, "get-credit-result", sGetCreditResult);
  bindFunction(env, "get-flow", sGetFlow);
  bindFunction(env, "get-positive-capacity", sGetPositiveCapacity);
  
  return env;
}



//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/scheme/Library", TestLibrary);

  
void
TestLibrary::numerical_operations()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result;
  // +
  result = eval(env, "(+ 1 2 3 4 5)");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 15, result->vInt());

  // =
  result = eval(env, "(= 123 123 123)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(= 12.3 12.3 12.3)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(= 123 456 123)");
  CHECK("Result is false", result->isBool() && !result->vBool());  

  // <
  result = eval(env, "(< 1 2 3 4 5)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(< 1.0 2.0 3.0 4.0 5.0)");
  CHECK("Result is true", result->isBool() && result->vBool());  
  result = eval(env, "(< 6 5 4 3 2)");
  CHECK("Result is false", result->isBool() && !result->vBool());  
  result = eval(env, "(< 10 10 10)");
  CHECK("Result is false", result->isBool() && !result->vBool());  

  // >
  result = eval(env, "(> 6 5 4 3 2)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(> 5.0 4.0 3.0 2.0 1.0)");
  CHECK("Result is true", result->isBool() && result->vBool());  
  result = eval(env, "(> 1 2 3 4 5)");
  CHECK("Result is false", result->isBool() && !result->vBool());  
  result = eval(env, "(> 10 10 10)");
  CHECK("Result is false", result->isBool() && !result->vBool());  

  // <=
  result = eval(env, "(<= 1 2 4 4 5)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(<= 1.0 2.0 4.0 4.0 5.0)");
  CHECK("Result is true", result->isBool() && result->vBool());  
  result = eval(env, "(<= 6 5 4 3 2)");
  CHECK("Result is false", result->isBool() && !result->vBool());  

  // >=
  result = eval(env, "(>= 6 5 4 4 2)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(>= 5.0 4.0 4.0 2.0 1.0)");
  CHECK("Result is true", result->isBool() && result->vBool());  
  result = eval(env, "(>= 1 2 3 4 5)");
  CHECK("Result is false", result->isBool() && !result->vBool());  

  // number->string
  result = eval(env, "(number->string 10)");
  CHECK("Result is string", result->isString());
  CHECK_EQUAL("Correct value", string, "10", result->vString());
}

// --------------------------------------------------------------------------------

void
TestLibrary::booleans()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(not #t)");
  CHECK("Result is false", result->isBool() && !result->vBool());  
  result = eval(env, "(not (list 'a 'b 'c))");
  CHECK("Result is false", result->isBool() && !result->vBool());  
  result = eval(env, "(not #f)");
  CHECK("Result is true", result->isBool() && result->vBool());
}


// --------------------------------------------------------------------------------


void
TestLibrary::pairs_lists()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(cdr (car (cons (cons 1 2) 3)))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 2, result->vInt());

  result = eval(env, "(car (cdr (cdr (list 1 2 3 4 5))))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 3, result->vInt());  

  result = eval(env, "(define test (cons 1 2)) (set-car! test 5) (car test)");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 5, result->vInt());  
  
  result = eval(env, "(define test2 (cons 1 2)) (set-cdr! test2 10) (cdr test2)");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 10, result->vInt());  

  result = eval(env, "(cdr (assq 'c (list (cons 'a 1) (cons 'b 2) (cons 'c 3) (cons 'd 4))))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 3, result->vInt());  

  result = eval(env, "(car (reverse '(1 2 3 4)))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 4, result->vInt());  
}

// --------------------------------------------------------------------------------

void
TestLibrary::strings()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result;

  // number->string
  result = eval(env, "(string-append \"abc\" \"def\")");
  CHECK("Result is string", result->isString());
  CHECK_EQUAL("Correct value", string, "abcdef", result->vString());
}



// --------------------------------------------------------------------------------

void
TestLibrary::control_operations()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result;
  
  result = eval(env, "(apply + '(1 2 3))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 6, result->vInt());

  result = eval(env, "(apply + (map car '((1 2 3) (1 2 3) (1 2 3))))");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 3, result->vInt());

  result = eval(env, "(define a 10) (for-each (lambda (b) (set! a (+ a b))) '(10 20 30 40)) a");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 110, result->vInt());
}
