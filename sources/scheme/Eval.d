//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2006 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Value;
private import Read;
private import Library;
private import std.stream;

// --------------------------------------------------------------------------------


Value
eval(Environment env, char[] data)
{
  SchemeReader reader = new SchemeReader(data);
  Value returnValue = rsUndefined();
  Value parameter = rsUndefined();  
  while(reader.read(parameter) == ReadState.OK) {
    returnValue = eval(env, parameter);
  }  
  return returnValue;
}


// --------------------------------------------------------------------------------

Value
eval(Environment env, Value data)
{
  // Self evaluating
  if(data.isBool() ||
     data.isNumber() ||
     data.isString()) {
    return data;
  }
  // Symbols
  else if(data.isSymbol()) {
    Environment current = env;
    while(!(null is current)) {
      if(data.vString() in current.values) {
        return current.values[data.vString()];
      }
      current = current.parent;
    }
    assert(CHECK_FAIL("Trying to access unknown symbol: " ~ data.vString()));
  }
  // Lists
  else if(data.isPair()) {
    if(!sListP(env, data)) {
      assert(CHECK_FAIL("Unable to evaluate non-lists"));
      return rsUndefined();
    }
    if(data.isNull()) {
      return data;
    }
    
    // ----------------------------------------
    // assert(Check for special forms
    if(data.car().isSymbol()) {

      // ----------------------------------------
      // Quote
      if(data.car().vString() == "quote") {
        if(data.cdr().isPair() &&
           data.cdr().cdr().isNull())
          return data.cdr().car();
        else
          assert(CHECK_FAIL("Quote error"));
      }
      // ----------------------------------------
      // Lambda
      else if(data.car().vString() == "lambda") {
        if(sListP(env, data.cdr().car())) {
          return evalLambda(env, data.cdr().car(), data.cdr().cdr());
        }        
        else {
          assert(CHECK_FAIL("Malformed lambda parameter sequence"));
          return rsUndefined();
        }
      }

      // ----------------------------------------
      // Definitions: define, set!
      else if(data.car().vString() == "define") {
        return evalDefine(env, data);
      }   
      else if(data.car().vString() == "set!") {
        return evalSet(env, data);
      }      

      // ----------------------------------------
      // Conditionals and boolean: if, cond
      else if(data.car().vString() == "if") {
        return evalIf(env, data);
      }
      else if(data.car().vString() == "cond") {
        return evalCond(env, data);
      }
      else if(data.car().vString() == "and") {
        return evalAnd(env, data);
      }
      else if(data.car().vString() == "or") {
        return evalOr(env, data);
      }

      // ----------------------------------------
      // Binding constructs
      else if(data.car().vString() == "let") {
        return evalLet(env, data);
      }
      else if(data.car().vString() == "let*") {
        return evalLetStar(env, data);
      }

      // ----------------------------------------
      // Sequencing
      else if(data.car().vString() == "begin") {
        return evalSequence(env, data.cdr());
      }      
    }

    // Ok, standard statement
    return evalStatement(env, data);
  }  
  else {
    assert(CHECK_FAIL("Trying to evaluate unknown type"));
  }
  assert(CHECK_FAIL("Eval error, this should never be reachable"));
  return rsUndefined();
}


// --------------------------------------------------------------------------------

Value
evalStatement(Environment env, Value data)
{
  Value current = data;
  PairValue call = new PairValue();
  PairValue callCurrent = call;
  while(current.isPair() && !current.isNull()) {
    callCurrent.car = eval(env, current.car());
    PairValue newParam = new PairValue();
    callCurrent.cdr = newParam;
    callCurrent = newParam;
    current = current.cdr();
  }
  if(current.isNull() == false) {
    assert(CHECK_FAIL("Malformed statement"));
    return null;
  }
  return apply(env, call);
}

// --------------------------------------------------------------------------------

Value
evalLambda(Environment env, Value paramList, Value code)
{
  ProcedureValue proc = new ProcedureValue();
  proc.environment = env;
  proc.mBody = code;
  while(paramList.isNull() == false) {
    if(!paramList.car().isSymbol())
      assert(CHECK_FAIL("Non symbol in parameter list"));
    proc.paramList ~= paramList.car().vString();
    paramList = paramList.cdr();
  }

  return proc;
}


// --------------------------------------------------------------------------------

Value
evalSequence(Environment env, Value sequence)
{
  if(!sListP(env, sequence)) {
    assert(CHECK_FAIL("evalSequence argument not list"));
  }
  Value result = null;
  while(sequence.isNull() == false) {
    result = eval(env, sequence.car());
    sequence = sequence.cdr();
  }
  if(result is null) {
    assert(CHECK_FAIL("Trying to evaluate undefined sequence"));
  }
  return result;
}

// --------------------------------------------------------------------------------

Value
evalDefine(Environment env, Value statement)
{
  if(!(env.parent is null)) {
    assert(CHECK_FAIL("You can only use defines at the top level"));
  }
  if(statement.cdr().car().isSymbol()) {
    char[] name = statement.cdr().car().vString();
    env.values[name] = eval(env, statement.cdr().cdr().car());
    return rsUndefined();
  }
  else if(statement.cdr().car().isPair()) {
    if(!statement.cdr().car().car().isSymbol())
      assert(CHECK_FAIL("First token should be symbol for function define"));
    char[] name = statement.cdr().car().car().vString();
    env.values[name] = evalLambda(env, statement.cdr().car().cdr(), statement.cdr().cdr());
    return rsUndefined();
  }
  else {
    assert(CHECK_FAIL("unknown first parameter to define"));
    return rsUndefined();
  }
}


// --------------------------------------------------------------------------------
Value
evalSet(Environment env, Value statement)
{
  if(statement.cdr().car().isSymbol()) {
    char[] name = statement.cdr().car().vString();
    Environment frame = env;
    while(!(frame is null)) {
      if(name in frame.values) {
        frame.values[name] = eval(env, statement.cdr().cdr().car());
        return frame.values[name];
      }
      frame = frame.parent;
    }
    assert(CHECK_FAIL("Variable not set in any environment"));
    return rsUndefined();
  }
  else {
    assert(CHECK_FAIL("set! not used correctly"));
    return null;
  }
}

// --------------------------------------------------------------------------------

Value
evalIf(Environment env, Value statement)
{
  int length = sLength(env, statement);
  if(length == 3) {
    if(sEqP(eval(env, statement.cdr().car()), rsFalse()) == false) {
      return eval(env, statement.cdr().cdr().car());
    }
    else {
      return rsUndefined();
    }    
  }
  else if(length == 4) {
    if(sEqP(eval(env, statement.cdr().car()), rsFalse()) == false) {
      return eval(env, statement.cdr().cdr().car());
    }
    else {
      return eval(env, statement.cdr().cdr().cdr().car());
    }    
  }
  else {
    assert(CHECK_FAIL("Wrong number of arguments to if"));
    return rsUndefined();
  }
}

// --------------------------------------------------------------------------------
// Eh ..

Value
evalCond(Environment env, Value statement) 
{
  assert(CHECK_FAIL("Not implemented"));
  return rsUndefined();
}

// --------------------------------------------------------------------------------

Value
evalAnd(Environment env, Value statement) 
{
  Value args = statement.cdr();
  while(args.isNull() == false) {
    Value test = eval(env, args.car());
    if(sEqP(test, rsFalse()))
      return rsFalse();
    args = args.cdr();
  }
  return rsTrue();
}

// --------------------------------------------------------------------------------

Value
evalOr(Environment env, Value statement) 
{
  Value args = statement.cdr();
  while(args.isNull() == false) {
    Value test = eval(env, args.car());
    if(!sEqP(test, rsFalse()))
      return rsTrue();
    args = args.cdr();
  }
  return rsFalse();
}

// --------------------------------------------------------------------------------

Value
evalLet(Environment env, Value statement)
{
  Environment letEnv = new Environment();
  letEnv.parent = env;
  Value bindings = statement.cdr().car();
  assert(CHECK("One or more expressions", sLength(env, statement) >= 3));
  assert(CHECK("Let bindings are a list", sListP(env, bindings)));
  while(bindings.isNull() == false) {
    assert(CHECK("Let assignment is a list", sListP(env, bindings.car())));
    assert(CHECK("Let assignment is a 2 item list", sLength(env, bindings.car()) == 2));
    Value variable = bindings.car().car();
    assert(CHECK("Assigning to a symbol", variable.isSymbol()));
    Value value = eval(env, bindings.car().cdr().car());
    letEnv.values[variable.vString()] = value;
    bindings = bindings.cdr();
  }
  return evalSequence(letEnv, statement.cdr().cdr());
}

Value
evalLetStar(Environment env, Value statement)
{
  Environment letEnv = new Environment();
  letEnv.parent = env;
  Value bindings = statement.cdr().car();
  assert(CHECK("One or more expressions", sLength(env, statement) >= 3));
  assert(CHECK("Let bindings are a list", sListP(env, bindings)));
  while(bindings.isNull() == false) {
    assert(CHECK("Let assignment is a list", sListP(env, bindings.car())));
    assert(CHECK("Let assignment is a 2 item list", sLength(env, bindings.car()) == 2));
    Value variable = bindings.car().car();
    assert(CHECK("Assigning to a symbol", variable.isSymbol()));
    Value value = eval(letEnv, bindings.car().cdr().car());
    letEnv.values[variable.vString()] = value;
    bindings = bindings.cdr();
  }
  return evalSequence(letEnv, statement.cdr().cdr());
}


// ================================================================================

Value
apply(Environment env, Value statement)
{
  if(!sListP(env, statement)) {
    assert(CHECK_FAIL("Error in apply: not list"));
  }
  Value procedure = statement.car();
  Value args = statement.cdr();
  return apply(env, procedure, args);
}

Value[] arrayFromList(Value args)
{
  Value[] list;
  while(!args.isNull()) {
    list ~= args.car();
    args = args.cdr();
  }
  return list;
}

Value
apply(Environment env, Value procedure, Value args)
{
  if(procedure.type() == Type.NATIVE_PROCEDURE) {
    NativeProcedureValue proc = cast(NativeProcedureValue)procedure;
    return (*proc.mProc)(env, arrayFromList(args));
  }
  else if(procedure.type() == Type.PROCEDURE) {
    Environment callEnvironment = new Environment;
    ProcedureValue proc = cast(ProcedureValue)procedure;
    callEnvironment.parent = proc.environment;
    int iParam = 0;
    while(args.isNull() == false) {
      if(iParam == cast(int)(proc.paramList.length)) {
        assert(CHECK_FAIL("Too many arguments to procedure"));
      }
      callEnvironment.values[proc.paramList[iParam]] = args.car();
      iParam++;
      args = args.cdr();
    }
    if(iParam != cast(int)(proc.paramList.length)) {
      assert(CHECK_FAIL("Too few arguments to procedure"));
    }
    return evalSequence(callEnvironment, proc.mBody);
  }
  else {
    // DEBUG
//     sWrite(env, new PairValue(procedure, new PairValue()));
    assert(CHECK_FAIL("Wrong type of argument to apply: not procedure"));
    return null;
  }  
  
}




//================================================================================
// Unit test

class TestEval : public TestFixture
{
  this()
  {
    mFixtureName = "TestEval";
    addTest("eval_simple", &eval_simple);
    addTest("eval_apply", &eval_apply);
    addTest("eval_conditionals", &eval_conditionals);
    addTest("eval_let", &eval_let);
    addTest("eval_define", &eval_define);
    addTest("eval_lambda", &eval_lambda);
  }

  void
  eval_simple()
  {
    Environment env = new Environment();
    env.parent = null;
    Value value;

    // bool false
    value = eval(env, "#f");
    assert(CHECK("Boolean false", value.isBool() && !value.vBool()));

    // integer
    value = eval(env, "#t");
    assert(CHECK("Boolean true", value.isBool() && value.vBool()));

    // integer
    value = eval(env, "123");
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 123, value.vInt()));

    // double
    value = eval(env, "123.456");
    assert(CHECK("Float type", value.isNumber() && !value.isExact()));
    assert(CHECK_EQUAL("Float content", 123.456, value.vFloat()));

    // string
    value = eval(env, "\"foobar\"");
    assert(CHECK("String type", value.isString()));
    assert(CHECK_EQUAL("string content", "foobar", value.vString()));

    // Symbol
    Value intValue = new NumberValue(10);
    env.values["foo_symbol!"] = intValue;
    value = eval(env, "foo_symbol!");
    assert(CHECK("Symbol lookup", intValue == value));

    // Quote
    value = eval(env, "'(foo)");
    assert(CHECK("Pair type", value.isPair()));
    assert(CHECK("Empty list type", value.cdr().isNull()));
    assert(CHECK("Symbol type", value.car().isSymbol()));
    assert(CHECK_EQUAL("Symbol value", "foo", value.car().vString()));  

    // Empty list
    value = eval(env, "()");
    assert(CHECK("Pair type", value.isPair()));
    assert(CHECK("Empty list type", value.isNull()));
  }


  void
  eval_apply()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(list? '(foo bar baz))");
    assert(CHECK("Result is boolean", result.isBool() && result.vBool()));
  }


  void
  eval_conditionals()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(if #t 'yes 'no)");
    assert(CHECK("Symbol type", result.isSymbol()));
    assert(CHECK_EQUAL("Symbol result", "yes", result.vString()));  
  
    result = eval(env, "(and '(foo) (= 10 10) 1)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(and '(foo) (= 10 20) 1)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));

    result = eval(env, "(or (= 10 20) 10 1)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(or #f (= 10 20) (< 10 5))");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));
  }

  void
  eval_let()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(define a 10) (let ((b 5) (a 20)) a)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 20, result.vInt()));  

    result = eval(env, "(define a 10) (let ((b 5) (c 20)) a)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 10, result.vInt()));  

    result = eval(env, "(define a 10) (let* ((b 20) (a b)) a)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 20, result.vInt()));  
  }

  void
  eval_define()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(define a 10) (define (foo x) (set! a x)) (let ((b 5)) (foo 20)) a");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 20, result.vInt()));  
  }



  void
  eval_lambda()
  {
    Environment env = libraryEnvironment();
    Value value = eval(env, "((lambda (a b c) (+ a b c)) 1 2 3)");
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 6, value.vInt()));

    value = eval(env, "(define foo (lambda (a b c) (+ a b c)))\n(foo 1 2 3)\n");
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 6, value.vInt()));  

    value = eval(env, "
(define (accum n) (lambda (a) (set! n (+ a n)) n))
(define foo (accum 5))
(define bar (accum 100))
(foo 10) ;Comment
(bar 10)
(foo 10)");
    assert(CHECK("Integer type", value.isNumber() && value.isExact()));
    assert(CHECK_EQUAL("Integer content", 25, value.vInt()));  
  }
}




