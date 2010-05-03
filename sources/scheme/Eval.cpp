// --------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

#include "Common.hpp"
#include "Eval.hpp"
#include "Value.hpp"
#include "Read.hpp"
#include "Library.hpp"

ValuePtr evalStatement(EnvPtr env, ValuePtr statement);
ValuePtr evalSequence(EnvPtr env, ValuePtr sequence);
ValuePtr evalLambda(EnvPtr env, ValuePtr paramList, ValuePtr body);
ValuePtr evalDefine(EnvPtr env, ValuePtr statement);
ValuePtr evalSet(EnvPtr env, ValuePtr statement);

ValuePtr evalIf(EnvPtr env, ValuePtr statement);
ValuePtr evalCond(EnvPtr env, ValuePtr statement);
ValuePtr evalAnd(EnvPtr env, ValuePtr statement);
ValuePtr evalOr(EnvPtr env, ValuePtr statement);

ValuePtr evalLet(EnvPtr env, ValuePtr statement);

// --------------------------------------------------------------------------------

ValuePtr
eval(EnvPtr env, istream& dataStream)
{
  ValuePtr result = NULL;
  ValuePtr statement;
  while(read(dataStream, statement)) {
    result = eval(env, statement);
  }
  if(result == NULL) {
    CHECK_FAIL("Statenent invalid");
    return rsUndefined();
  }
  else {
    return result;
  }
}


ValuePtr
eval(EnvPtr env, string data)
{
  istringstream dataStream(data);
  return eval(env, dataStream);
}


// --------------------------------------------------------------------------------

ValuePtr
eval(EnvPtr env, ValuePtr data)
{
  // Self evaluating
  if(data->isBool() ||
     data->isNumber() ||
     data->isString()) {
    return data;
  }
  // Symbols
  else if(data->isSymbol()) {
    EnvPtr current = env;
    while(!(NULL == current)) {
      if(current->values.find(data->vString()) != current->values.end()) {
        return current->values[data->vString()];
      }
      current = current->parent;
    }
	CHECK_FAIL(string("Trying to access unknown symbol: ") + data->vString());
  }
  // Lists
  else if(data->isPair()) {
    if(!sListP(data)) {
      CHECK_FAIL("Unable to evaluate non-lists");
      return rsUndefined();
    }
    
    // ----------------------------------------
    // Check for special forms
    if(data->car()->isSymbol()) {

      // ----------------------------------------
      // Quote
      if(data->car()->vString() == string("quote")) {
        if(data->cdr()->isPair() &&
           data->cdr()->cdr()->isNull())
          return data->cdr()->car();
        else
          CHECK_FAIL("Quote error");
      }
      // ----------------------------------------
      // Lambda
      else if(data->car()->vString() == string("lambda")) {
        if(sListP(data->cdr()->car())) {
          return evalLambda(env, data->cdr()->car(), data->cdr()->cdr());
        }        
        else {
          CHECK_FAIL("Malformed lambda parameter sequence");
          return rsUndefined();
        }
      }
      // ----------------------------------------
      // Definitions: define, set!
      else if(data->car()->vString() == string("define")) {
        return evalDefine(env, data);
      }   
      else if(data->car()->vString() == string("set!")) {
        return evalSet(env, data);
      }      
      // ----------------------------------------
      // Conditionals and boolean: if, cond
      else if(data->car()->vString() == string("if")) {
        return evalIf(env, data);
      }
      else if(data->car()->vString() == string("cond")) {
        return evalCond(env, data);
      }
      else if(data->car()->vString() == string("and")) {
        return evalAnd(env, data);
      }
      else if(data->car()->vString() == string("or")) {
        return evalOr(env, data);
      }
      // ----------------------------------------
      // Binding constructs
      else if(data->car()->vString() == string("let")) {
        return evalLet(env, data);
      }      
      // ----------------------------------------
      // Sequencing
      else if(data->car()->vString() == string("begin")) {
        return evalSequence(env, data->cdr());
      }      
    }

    // Ok, standard statement
    return evalStatement(env, data);
  }  
  else {
    CHECK_FAIL("Trying to evaluate unknown type");
  }
  CHECK_FAIL("Eval error, this should never be reachable");
  return rsUndefined();
}


// --------------------------------------------------------------------------------

ValuePtr
evalStatement(EnvPtr env, ValuePtr data)
{
  ValuePtr current = data;
  ValuePtr call = new PairValue();
  ValuePtr callCurrent = call;
  while(current->isPair() && !current->isNull()) {
    callCurrent->car() = eval(env, current->car());
    ValuePtr newParam = new PairValue();
    callCurrent->cdr() = newParam;
    callCurrent = newParam;
    current = current->cdr();
  }
  if(current->isNull() == false) {
    CHECK_FAIL("Malformed statement");
    return NULL;
  }
  return apply(env, call);
}

// --------------------------------------------------------------------------------

ValuePtr
evalLambda(EnvPtr env, ValuePtr paramList, ValuePtr body)
{
  ProcedureValue* proc = new ProcedureValue();
  proc->environment = env;
  proc->body = body;
  while(paramList->isNull() == false) {
    if(!paramList->car()->isSymbol())
      CHECK_FAIL("Non symbol in parameter list");
    proc->paramList.push_back(paramList->car()->vString());
    paramList = paramList->cdr();
  }

  return proc;
}


// --------------------------------------------------------------------------------

ValuePtr
evalSequence(EnvPtr env, ValuePtr sequence)
{
  if(!sListP(sequence)) {
    CHECK_FAIL("evalSequence argument not list");
  }
  ValuePtr result = NULL;
  while(sequence->isNull() == false) {
    result = eval(env, sequence->car());
    sequence = sequence->cdr();
  }
  if(result == NULL) {
    CHECK_FAIL("Trying to evaluate undefined sequence");
  }
  return result;
}

// --------------------------------------------------------------------------------

ValuePtr
evalDefine(EnvPtr env, ValuePtr statement)
{
  if(!(env->parent == NULL)) {
    CHECK_FAIL("You can only use defines at the top level");
  }
  if(statement->cdr()->car()->isSymbol()) {
    string name = statement->cdr()->car()->vString();
    env->values[name] = eval(env, statement->cdr()->cdr()->car());
    // FIXME: Should return undefined instead ..
    return env->values[name];
  }
  else if(statement->cdr()->car()->isPair()) {
    if(!statement->cdr()->car()->car()->isSymbol())
      CHECK_FAIL("First token should be symbol for function define");
    string name = statement->cdr()->car()->car()->vString();
    env->values[name] = evalLambda(env, statement->cdr()->car()->cdr(), statement->cdr()->cdr());
    return env->values[name];
  }
  else {
    CHECK_FAIL("unknown first parameter to define");
    return rsUndefined();
  }
}


// --------------------------------------------------------------------------------
ValuePtr
evalSet(EnvPtr env, ValuePtr statement)
{
  if(statement->cdr()->car()->isSymbol()) {
    string name = statement->cdr()->car()->vString();
    EnvPtr scope = env;
    while(!(scope == NULL)) {
      if(scope->values.find(name) != scope->values.end()) {
        scope->values[name] = eval(env, statement->cdr()->cdr()->car());
        return scope->values[name];
      }
      scope = scope->parent;
    }
    CHECK_FAIL("Variable not set in any environment");
    return rsUndefined();
  }
  else {
    CHECK_FAIL("set! not used correctly");
    return NULL;
  }
}

// --------------------------------------------------------------------------------

ValuePtr
evalIf(EnvPtr env, ValuePtr statement)
{
  int length = sLength(statement);
  if(length == 3) {
    if(sEqP(eval(env, statement->cdr()->car()), rsFalse()) == false) {
      return eval(env, statement->cdr()->cdr()->car());
    }
    else {
      return rsUndefined();
    }    
  }
  else if(length == 4) {
    if(sEqP(eval(env, statement->cdr()->car()), rsFalse()) == false) {
      return eval(env, statement->cdr()->cdr()->car());
    }
    else {
      return eval(env, statement->cdr()->cdr()->cdr()->car());
    }    
  }
  else {
    CHECK_FAIL("Wrong number of arguments to if");
    return rsUndefined();
  }
}

// --------------------------------------------------------------------------------
// Eh ..

ValuePtr
evalCond(EnvPtr env, ValuePtr statement) 
{
  CHECK_FAIL("Not implemented");
  return rsUndefined();
}

// --------------------------------------------------------------------------------

ValuePtr
evalAnd(EnvPtr env, ValuePtr statement) 
{
  ValuePtr args = statement->cdr();
  while(args->isNull() == false) {
    ValuePtr test = eval(env, args->car());
    if(sEqP(test, rsFalse()))
      return rsFalse();
    args = args->cdr();
  }
  return rsTrue();
}

// --------------------------------------------------------------------------------

ValuePtr
evalOr(EnvPtr env, ValuePtr statement) 
{
  ValuePtr args = statement->cdr();
  while(args->isNull() == false) {
    ValuePtr test = eval(env, args->car());
    if(!sEqP(test, rsFalse()))
      return rsTrue();
    args = args->cdr();
  }
  return rsFalse();
}

// --------------------------------------------------------------------------------

ValuePtr
evalLet(EnvPtr env, ValuePtr statement)
{
  EnvPtr letEnv = new Environment();
  letEnv->parent = env;
  ValuePtr bindings = statement->cdr()->car();
  CHECK("One or more expressions", sLength(statement) >= 3);
  CHECK("Let bindings are a list", sListP(bindings));
  while(bindings->isNull() == false) {
    CHECK("Let assignment is a list", sListP(bindings->car()));
    CHECK("Let assignment is a 2 item list", sLength(bindings->car()) == 2);
    ValuePtr variable = bindings->car()->car();
    CHECK("Assigning to a symbol", variable->isSymbol());
    ValuePtr value = eval(env, bindings->car()->cdr()->car());
    letEnv->values[variable->vString()] = value;
    bindings = bindings->cdr();
  }
  return evalSequence(letEnv, statement->cdr()->cdr());
}


// ================================================================================

ValuePtr
apply(EnvPtr env, ValuePtr statement)
{
  if(!sListP(statement)) {
    CHECK_FAIL("Error in apply: not list");
  }
  ValuePtr procedure = statement->car();
  ValuePtr args = statement->cdr();
  return apply(env, procedure, args);
}

ValuePtr
apply(EnvPtr env, ValuePtr procedure, ValuePtr args)
{
  if(procedure->type() == Value::NATIVE_PROCEDURE) {
    NativeProcedureValue* proc = static_cast<NativeProcedureValue*>(procedure.mValue);
    return (*proc->mProc)(env, args);
  }
  else if(procedure->type() == Value::PROCEDURE) {
    EnvPtr callEnvironment = new Environment;
    ProcedureValue* proc = static_cast<ProcedureValue*>(procedure.mValue);
    callEnvironment->parent = proc->environment;
    int iParam = 0;
    while(args->isNull() == false) {
      if(iParam == static_cast<int>(proc->paramList.size())) {
        CHECK_FAIL("Too many arguments to procedure");
      }
      callEnvironment->values[proc->paramList[iParam]] = args->car();
      iParam++;
      args = args->cdr();
    }
    if(iParam != static_cast<int>(proc->paramList.size())) {
      CHECK_FAIL("Too few arguments to procedure");
    }
    return evalSequence(callEnvironment, proc->body);
  }
  else {
    sWrite(env, new PairValue(procedure, new PairValue()));
    CHECK_FAIL("Wrong type of argument to apply: not procedure");
    return NULL;
  }  
  
}




//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/scheme/Eval", TestEval);

  
void
TestEval::eval_simple()
{
  EnvPtr env = new Environment();
  env->parent = NULL;
  ValuePtr value;

  // bool false
  value = eval(env, "#f");
  CHECK("Boolean false", value->isBool() && !value->vBool());

  // integer
  value = eval(env, "#t");
  CHECK("Boolean true", value->isBool() && value->vBool());

  // integer
  value = eval(env, "123");
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 123, value->vInt());

  // double
  value = eval(env, "123.456");
  CHECK("Float type", value->isNumber() && !value->isExact());
  CHECK_EQUAL("Float content", float, 123.456, value->vFloat());

  // string
  value = eval(env, "\"foobar\"");
  CHECK("String type", value->isString());
  CHECK_EQUAL("string content", string, "foobar", value->vString());

  // Symbol
  ValuePtr intValue = new NumberValue(10);
  env->values["foo_symbol!"] = intValue;
  value = eval(env, "foo_symbol!");
  CHECK("Symbol lookup", intValue == value);

  // Quote
  value = eval(env, "'(foo)");
  CHECK("Pair type", value->isPair());
  CHECK("Empty list type", value->cdr()->isNull());
  CHECK("Symbol type", value->car()->isSymbol());
  CHECK_EQUAL("Symbol value", string, "foo", value->car()->vString());  
}


void
TestEval::eval_apply()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(list? '(foo bar baz))");
  CHECK("Result is boolean", result->isBool() && result->vBool());
}


void
TestEval::eval_conditionals()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(if #t 'yes 'no)");
  CHECK("Symbol type", result->isSymbol());
  CHECK_EQUAL("Symbol result", string, "yes", result->vString());  
  
  result = eval(env, "(and '(foo) (= 10 10) 1)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(and '(foo) (= 10 20) 1)");
  CHECK("Result is false", result->isBool() && !result->vBool());

  result = eval(env, "(or (= 10 20) 10 1)");
  CHECK("Result is true", result->isBool() && result->vBool());
  result = eval(env, "(or #f (= 10 20) (< 10 5))");
  CHECK("Result is false", result->isBool() && !result->vBool());
}

void
TestEval::eval_let()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(define a 10) (let ((b 5) (a 20)) a)");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 20, result->vInt());  

  result = eval(env, "(define a 10) (let ((b 5) (c 20)) a)");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 10, result->vInt());  
}

void
TestEval::eval_define()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr result = eval(env, "(define a 10) (define (foo x) (set! a x)) (let ((b 5)) (foo 20)) a");
  CHECK("Result is integer", result->isNumber() && result->isExact());
  CHECK_EQUAL("Value is correct", int, 20, result->vInt());  
}



void
TestEval::eval_lambda()
{
  EnvPtr env = libraryEnvironment();
  ValuePtr value = eval(env, "((lambda (a b c) (+ a b c)) 1 2 3)");
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 6, value->vInt());

  value = eval(env, "(define foo (lambda (a b c) (+ a b c)))\n(foo 1 2 3)\n");
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 6, value->vInt());  

  value = eval(env, "\n\
(define (accum n) (lambda (a) (set! n (+ a n)) n))\n \
(define foo (accum 5))\n\
(define bar (accum 100))\n\
(foo 10) ;Comment\n\
(bar 10)\n\
(foo 10)");
  CHECK("Integer type", value->isNumber() && value->isExact());
  CHECK_EQUAL("Integer content", int, 25, value->vInt());  
}



