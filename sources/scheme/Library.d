// --------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

private import Common;
private import Value;
private import Eval;
private import Read;
private import std.stream;

// --------------------------------------------------------------------------------
// 6.1 Equivalence predicates

bool
sEqP(Value lhs, Value rhs)
{
  if(lhs.type() != rhs.type()) return false;
  if(lhs.isBool() && lhs.vBool() == rhs.vBool()) return true;
  else if(lhs.isNumber() && lhs.isExact() && rhs.isExact() && lhs.vInt() == rhs.vInt()) return true;
  else if(lhs.isNumber() && !lhs.isExact() && !rhs.isExact() && lhs.vFloat() == rhs.vFloat()) return true;
  else if(lhs.isString() && cmp(lhs.vString(), rhs.vString()) == 0) return true;
  else if(lhs.isSymbol() && cmp(lhs.vString(), rhs.vString()) == 0) return true;
  else if(lhs.isPair() && (lhs == rhs || (lhs.isNull() && rhs.isNull()))) return true;
  else if(lhs.isProcedure() && lhs == rhs) return true;
  else if(lhs.type() == Type.UNDEFINED) return false;
  return false;
}

// ================================================================================
// 6.2.5 Numerical operations

bool
sPureInteger(Value[] args)
{
  foreach(Value arg; args) {
    if(!(arg.isNumber() && arg.isExact())) return false;
  }
  return true;
}

float
sAsFloat(Value number)
{
  assert(CHECK("Error convering non number to float", number.isNumber()));
  if(!number.isExact()) {
    return number.vFloat();
  }
  else {
    return cast(float)(number.vInt());
  }
}

private Value sNumericalTest(Environment env, Value[] args, bool function(float, float) floatTest,
                        bool function(int, int) intTest)
{
  if(!sPureInteger(args)) {
    float prev = sAsFloat(args[0]);
    foreach(Value arg; args[1 .. args.length]) {
      float curr = sAsFloat(arg);
      if(!floatTest(prev, curr)) return rsFalse();
      prev = curr;
    }
  }
  else {
    float prev = args[0].vInt;
    foreach(Value arg; args[1 .. args.length]) {
      float curr = arg.vInt;
      if(!floatTest(prev, curr)) return rsFalse();
      prev = curr;
    }
  }   
  return rsTrue();
}


Value sNumEqual(Environment env, Value[] args)
{
  return sNumericalTest(env, args,
                        function bool(float lhs, float rhs) {return lhs == rhs;},
                        function bool(int lhs, int rhs) {return lhs == rhs;});
}

Value sNumLess(Environment env, Value[] args)
{
  return sNumericalTest(env, args,
                        function bool(float lhs, float rhs) {return lhs < rhs;},
                        function bool(int lhs, int rhs) {return lhs < rhs;});
}

Value sNumGreater(Environment env, Value[] args)
{
  return sNumericalTest(env, args,
                        function bool(float lhs, float rhs) {return lhs > rhs;},
                        function bool(int lhs, int rhs) {return lhs > rhs;});
}

Value sNumLessEqual(Environment env, Value[] args)
{
  return sNumericalTest(env, args,
                        function bool(float lhs, float rhs) {return lhs <= rhs;},
                        function bool(int lhs, int rhs) {return lhs <= rhs;});
}

Value sNumGreaterEqual(Environment env, Value[] args)
{
  return sNumericalTest(env, args,
                        function bool(float lhs, float rhs) {return lhs >= rhs;},
                        function bool(int lhs, int rhs) {return lhs >= rhs;});
}

// --------------------------------------------------------------------------------

Value
eNumOperator(Value[] args, int (*intOp)(int, int), float (*floatOp)(float, float))
{
  assert(CHECK("Numeric arguments expected", args[0].isNumber));
  NumberValue result = cast(NumberValue)args[0];
  
  foreach(Value arg; args[1 .. args.length]) {
    assert(CHECK("Numeric arguments expected", arg.isNumber()));
    if(result.isExact()) {
      if(arg.isExact()) {
        result.vInt = intOp(result.vInt, arg.vInt);
      }
      else {
        result.toInExact();
        result.vFloat = floatOp(result.vFloat, arg.vFloat);
      }
    }
    else {
      if(arg.isExact()) {
        result.vFloat = floatOp(result.vFloat, arg.vInt);
      }
      else {
        result.vFloat = floatOp(result.vFloat, arg.vFloat);
      }      
    }    
  }
  return result;
}


Value
eAdd(Environment env, Value[] args)
{
  return eNumOperator(args,
                      function int(int lhs, int rhs) {return lhs + rhs;},
                      function float(float lhs, float rhs) {return lhs + rhs;});
}

Value
eSub(Environment env, Value[] args)
{
  return eNumOperator(args,
                      function int(int lhs, int rhs) {return lhs - rhs;},
                      function float(float lhs, float rhs) {return lhs - rhs;});
}

Value
eMul(Environment env, Value[] args)
{
  return eNumOperator(args,
                      function int(int lhs, int rhs) {return lhs * rhs;},
                      function float(float lhs, float rhs) {return lhs * rhs;});
}

Value
eDiv(Environment env, Value[] args)
{
  return eNumOperator(args,
                      function int(int lhs, int rhs) {return lhs / rhs;},
                      function float(float lhs, float rhs) {return lhs / rhs;});
}

// ================================================================================
// 6.2.5 Numerical input and output

Value
sNumberToString(Environment env, Value[] args)
{
  if(args.length == 1 && args[0].isNumber()) {
    if(args[0].isExact())
      return new StringValue(toString(args[0].vInt));
    else
      return new StringValue(toString(args[0].vFloat));
  }  
  return rsUndefined();
}


// --------------------------------------------------------------------------------
// 6.3.1 Booleans

Value
sNot(Environment env, Value[] args)
{
  assert(CHECK("Single argument", args.length == 1));
  if(sEqP(args[0], rsFalse()))
    return rsTrue();
  else
    return rsFalse();
}



// --------------------------------------------------------------------------------
// 6.3.2 Pairs and lists

Value
sCons(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments is 2", args.length == 2));
  return new PairValue(args[0], args[1]);
}

Value
sCar(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 1));
  assert(CHECK("Argument is pair", args[0].isPair()));
  return args[0].car();
}

Value
sCdr(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 1));
  assert(CHECK("Argument is pair", args[0].isPair()));
  return args[0].cdr();
}

Value
sSetCar(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 2));
  assert(CHECK("Argument is pair", args[0].isPair()));
  PairValue pair = cast(PairValue)(args[0]);
  pair.car = args[1];
  return rsUndefined();
}

Value
sSetCdr(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 2));
  assert(CHECK("Argument is pair", args[0].isPair()));
  PairValue pair = cast(PairValue)(args[0]);
  pair.cdr = args[1];
  return rsUndefined();
}


bool
sListP(Value arg)
{
  return sListP(null, arg);
}

bool
sListP(Environment env, Value arg)
{
  Value[] args;
  args ~= arg;
  return sListP(env, args);
}


bool
sListP(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 1));
  Value data = args[0];
  while(data.isPair() && !data.isNull())
    data = data.cdr();
  return data.isNull();
}


Value
sList(Environment env, Value[] args)
{
  PairValue current = new PairValue();
  PairValue result = current;
  foreach(Value arg; args) {
    current.car = arg;
    PairValue next = new PairValue();
    current.cdr = next;
    current = next;
  }
  return result;
}


int
sLength(Environment env, Value arg)
{
  Value[] args;
  args ~= arg;
  return sLength(env, args);
}


int
sLength(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments", args.length == 1));
  int length = 0;
  Value ptr = args[0];
  while(ptr.isNull() == false) {
    if(ptr.isPair() == false) assert(CHECK_FAIL("Malformed list to length"));
    length++;
    ptr = ptr.cdr();
  }
  return length;
}


Value
sReverse(Environment env, Value[] args)
{
  assert(CHECK("Single argument to reverse", args.length == 1));
  Value current = new PairValue();
  Value rList = args[0];
  while(rList.isNull() == false) {
    Value next = new PairValue(rList.car(), current);
    current = next;
    rList = rList.cdr();
  }
  return current;
}


Value
sAssq(Environment env, Value[] args)
{
  assert(CHECK("Wrong number of arguments to sAssq", args.length == 2));
  Value obj = args[0];
  Value alist = args[1];
  while(alist.isNull() == false) {
    if(alist.isPair() == false && alist.car().isPair() == false)
      assert(CHECK_FAIL("Malformed alist"));
    if(sEqP(obj, alist.car().car())) {
      return alist.car();
    }
    alist = alist.cdr();
 }
  return rsFalse();
}

// ================================================================================
// 6.3.5 Strings

Value
sStringAppend(Environment env, Value[] args)
{
  char[] result = "";
  foreach(Value arg; args) {
    assert(CHECK("Non string passed to string-append", arg.isString()));
    result ~= arg.vString();
  }
  return new StringValue(result);
}




// ================================================================================
// 6.4 Control features

Value
sApply(Environment env, Value[] args)
{
  assert(CHECK("Two parameters required", args.length == 2));
  return apply(env, args[0], args[1]);
}


Value
sMap(Environment env, Value[] args) 
{
  assert(CHECK("Only a single parameter supported", args.length == 2));
  Value proc = args[0];
  Value list1 = args[1];
  assert(CHECK("Valid arguments to map", (proc.isProcedure()) && sListP(env, list1)));

  PairValue current = new PairValue();
  PairValue result = current;
  
  PairValue argList = new PairValue(null, new PairValue());
  
  while(list1.isNull() == false) {    
    argList.car = list1.car();
    current.car = apply(env, proc, argList);
    PairValue next = new PairValue();
    current.cdr = next;
    current = next;

    list1 = list1.cdr();
  }  
  return result;
}

Value
sForEach(Environment env, Value[] args) 
{
  assert(CHECK("Only single parameter for-each supported", args.length == 2));
  Value proc = args[0];
  Value list1 = args[1];
  assert(CHECK("Valid arguments to foreach", (proc.isProcedure()) && sListP(env, list1)));

  PairValue argList = new PairValue(null, new PairValue());
  
  while(list1.isNull() == false) {
    argList.car = list1.car();
    apply(env, proc, argList);
    list1 = list1.cdr();
  }  
  
  return rsUndefined();
}


// ================================================================================
// 6.6.3

Value
sWrite(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments is 1", args.length == 1));
  logInfo << args[0].toString() << endl;
  return rsUndefined();
}

// ================================================================================
// 6.6.3

Value
sLoad(Environment env, char[] fileName)
{
  std.stream.File fileStream = new std.stream.File(fileName, std.stream.FileMode.In);
  char[] data = "";
  while(fileStream.available() > 0) {
    data ~= fileStream.readString(fileStream.available());
  }
  fileStream.close();
  SchemeReader reader = new SchemeReader(data);
  Value statement;
  Value returnValue = rsUndefined();
  ReadState state = reader.read(statement);
  while(ReadState.OK == state) {
    returnValue = eval(env, statement);
    state = reader.read(statement);
  }
  if(ReadState.ERROR == state) {
    assert(CHECK_FAIL("Error: " ~ reader.getError()));
  }  
  return returnValue;
}

Value
sLoad(Environment env, Value[] args)
{
  assert(CHECK("Number of arguments is 1", args.length == 1));
  assert(CHECK("Argument is tring", args[0].isString()));
  char[] fileName = args[0].vString();
  return sLoad(env, fileName);
}


// // ================================================================================
// // Local exports

// Value
// sCountModules(Environment env, Value args)
// {
//   using SSM::Craft;
//   Craft* station = Craft::getSingleton();
//   return new NumberValue(station.countUnits());
// }


// Value
// sShowMessage(Environment env, Value args)
// {
//   using SSM::EVA;
//   using SSM::Controller;
  
//   if(sLength(args) == 1 && args.car().isString()) {
//     EVA* evaScreen = Controller::getSingleton().evaScreen;
//     evaScreen.missionMessage(string(args.car().vString()));
//   }
//   return rsUndefined();
// }


// Value
// sEnableModule(Environment env, Value args)
// {
//   using SSM::EVA;
//   using SSM::Controller;
  
//   if(sLength(args) == 1 && args.car().isString()) {
//     EVA* evaScreen = Controller::getSingleton().evaScreen;
//     evaScreen.enableModule(string(args.car().vString()));
//   }
//   return rsUndefined();
// }

// Value
// sGetCreditResult(Environment env, Value args)
// {
//   using SSM::Craft;
  
//   return new NumberValue(Craft::getSingleton().getCreditResult());
// }

// Value
// sGetFlow(Environment env, Value args)
// {
//   using namespace SSM;

//   if(sLength(args) == 1 && args.car().isSymbol()) {
//     Craft* station = Craft::getSingleton();
//     if(args.car().vString() == "energy") {
//       return new NumberValue(station.getFlow(ENERGY));
//     }
//     else if(args.car().vString() == "thermal-control") {
//       return new NumberValue(station.getFlow(COOLING));
//     }
//     else {
//       return new NumberValue(station.getFlow(LIFE_SUPPORT));
//     }
//   }
//   return rsUndefined();
// }

// Value
// sGetPositiveCapacity(Environment env, Value args)
// {
//   using namespace SSM;

//   if(sLength(args) == 1 && args.car().isSymbol()) {
//     if(args.car().vString() == "crew") {
//       return new NumberValue(Craft::getSingleton().getLocalResourcePositiveCapacity(CREW_SPACE));
//     }
//   }  
//   return rsUndefined();
// }


// ================================================================================
// Binding

void
bindFunction(Environment env, char[] name, NativeProcedure proc)
{
  env.values[name] = new NativeProcedureValue(proc);
}

// --------------------------------------------------------------------------------

// void unpack(Value input, out int output)
// {
//   assert(CHECK("Wrong type of argument, integer expected", input.type == Type.NUMBER_INT));
//   output = input.vInt;
// }

// void pack(int input, out Value output)
// {
//   output = new NumberValue(input);
// }



// // --------------------------------------------------------------------------------

// template TWrap0(ReturnType)
// {
//   typedef ReturnType (*WrapProc)(Environment env);

//   bool bind(Environment env, Value[] args, void* procPtr) {
//     assert(CHECK("Wrong number of arguments", args.length == 0));
//     assert(CHECK("[Internal Error] Procedure parameter is null", proc !is null));
//     WrapProc proc = cast(WrapProc)procPtr;
//     ReturnType result;
//     pack(proc(env), result);
//     return result;
//   }  
// }

// template TWrap1(ReturnType, ArgType0)
// {
//   typedef ReturnType (*WrapProc)(Environment env, ArgType0);

//   bool bind(Environment env, Value[] args, void* procPtr) {
//     assert(CHECK("Wrong number of arguments", args.length == 0));
//     assert(CHECK("[Internal Error] Procedure parameter is null", proc !is null));
//     WrapProc proc = cast(WrapProc)procPtr;
//     ReturnType result;
//     pack(proc(env), result);
//     return result;
//   }  
// }

//     assert(CHECK("Argument 1 of wrong type of argument to " ~ name,
//                  unpack(args[0], arg1)));


// --------------------------------------------------------------------------------
// Wrapped native functions

Value eEqP(Environment env, Value[] param) {return new BoolValue(sEqP(param[0], param[1]));}

Value eListP(Environment env, Value[] param) { return new BoolValue(sListP(env, param)); }
Value eLength(Environment env, Value[] param) {return new NumberValue(sLength(env, param[0]));}


Environment
libraryEnvironment()
{
  Environment env = new Environment();
  env.parent = null;
  // 6.1 Equivalence predicates
  bindFunction(env, "eq?", &eEqP);

  // 6.2.5 Numerical operations
  bindFunction(env, "=", &sNumEqual);
  bindFunction(env, "<", &sNumLess);
  bindFunction(env, ">", &sNumGreater);
  bindFunction(env, "<=", &sNumLessEqual);
  bindFunction(env, ">=", &sNumGreaterEqual);
  bindFunction(env, "+", &eAdd);
  bindFunction(env, "-", &eSub);
  bindFunction(env, "*", &eMul);
  bindFunction(env, "/", &eDiv);
  

  // 6.2.6 Numerical input and output
  bindFunction(env, "number->string", &sNumberToString);

  // 6.3.1 Booleans
  bindFunction(env, "not", &sNot);

  // 6.3.2 Pairs and lists
  bindFunction(env, "cons", &sCons);
  bindFunction(env, "car", &sCar);
  bindFunction(env, "cdr", &sCdr);
  bindFunction(env, "set-car!", &sSetCar);
  bindFunction(env, "set-cdr!", &sSetCdr);
  bindFunction(env, "list?", &eListP);
  bindFunction(env, "length", &eLength);
  //  TBind1!(int, Value).bind(env, "length", &sLength);

  bindFunction(env, "list", &sList);
  bindFunction(env, "reverse", &sReverse);
  bindFunction(env, "assq", &sAssq);
  // 6.3.5 Strings
  bindFunction(env, "string-append", &sStringAppend);
  
  // 6.4 Control features
  bindFunction(env, "apply", &sApply);
  bindFunction(env, "map", &sMap);
  bindFunction(env, "for-each", &sForEach);

  // 6.6.3 Output
  bindFunction(env, "write", &sWrite);

  // 6.6.4 System Interface
  bindFunction(env, "load", &sLoad);

  return env;
}



//================================================================================
// Unit test

// --------------------------------------------------------------------------------

class TestLibrary : public TestFixture
{
  this()
  {
    mFixtureName = "TestLibrary";
    addTest("numerical_operations", &numerical_operations);
    addTest("booleans", &booleans);
    addTest("pairs_lists", &pairs_lists);
    addTest("strings", &strings);
    addTest("control_operations", &control_operations);
  }

  static this()
  {
     addTestFixture(new TestRead());
     addTestFixture(new TestEval());
     addTestFixture(new TestLibrary());
  }
  
  void
  numerical_operations()
  {
    Environment env = libraryEnvironment();
    Value result;
    // +
    result = eval(env, "(+ 1 2 3 4 5)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 15, result.vInt()));

    // -
    result = eval(env, "(- 20 1 2 3 4 5)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 5, result.vInt()));

    // *
    result = eval(env, "(* 2 4 10)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 80, result.vInt()));

    // /
    result = eval(env, "(/ 100 2 5)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 10, result.vInt()));

    // =
    result = eval(env, "(= 123 123 123)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(= 12.3 12.3 12.3)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(= 123 456 123)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  

    // <
    result = eval(env, "(< 1 2 3 4 5)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(< 1.0 2.0 3.0 4.0 5.0)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));  
    result = eval(env, "(< 6 5 4 3 2)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  
    result = eval(env, "(< 10 10 10)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  

    // >
    result = eval(env, "(> 6 5 4 3 2)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(> 5.0 4.0 3.0 2.0 1.0)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));  
    result = eval(env, "(> 1 2 3 4 5)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  
    result = eval(env, "(> 10 10 10)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  

    // <=
    result = eval(env, "(<= 1 2 4 4 5)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(<= 1.0 2.0 4.0 4.0 5.0)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));  
    result = eval(env, "(<= 6 5 4 3 2)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  

    // >=
    result = eval(env, "(>= 6 5 4 4 2)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
    result = eval(env, "(>= 5.0 4.0 4.0 2.0 1.0)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));  
    result = eval(env, "(>= 1 2 3 4 5)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  

    // number->string
    result = eval(env, "(number->string 10)");
    assert(CHECK("Result is string", result.isString()));
    assert(CHECK_EQUAL("Correct value", "10", result.vString()));
  }

  // --------------------------------------------------------------------------------

  void
  booleans()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(not #t)");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  
    result = eval(env, "(not (list 'a 'b 'c))");
    assert(CHECK("Result is false", result.isBool() && !result.vBool()));  
    result = eval(env, "(not #f)");
    assert(CHECK("Result is true", result.isBool() && result.vBool()));
  }


  // --------------------------------------------------------------------------------


  void
  pairs_lists()
  {
    Environment env = libraryEnvironment();
    Value result = eval(env, "(cdr (car (cons (cons 1 2) 3)))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 2, result.vInt()));

    result = eval(env, "(car (cdr (cdr (list 1 2 3 4 5))))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 3, result.vInt()));  

    result = eval(env, "(define test (cons 1 2)) (set-car! test 5) (car test)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 5, result.vInt()));  
  
    result = eval(env, "(define test2 (cons 1 2)) (set-cdr! test2 10) (cdr test2)");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 10, result.vInt()));  

    result = eval(env, "(cdr (assq 'c (list (cons 'a 1) (cons 'b 2) (cons 'c 3) (cons 'd 4))))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 3, result.vInt()));  

    result = eval(env, "(car (reverse '(1 2 3 4)))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 4, result.vInt()));  
  }

  // --------------------------------------------------------------------------------

  void
  strings()
  {
    Environment env = libraryEnvironment();
    Value result;

    // number.string
    result = eval(env, "(string-append \"abc\" \"def\")");
    assert(CHECK("Result is string", result.isString()));
    assert(CHECK_EQUAL("Correct value", "abcdef", result.vString()));
  }



  // --------------------------------------------------------------------------------

  void
  control_operations()
  {
    Environment env = libraryEnvironment();
    Value result;
  
    result = eval(env, "(apply + '(1 2 3))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 6, result.vInt()));

    result = eval(env, "(apply + (map car '((1 2 3) (1 2 3) (1 2 3))))");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 3, result.vInt()));

    result = eval(env, "(define a 10) (for-each (lambda (b) (set! a (+ a b))) '(10 20 30 40)) a");
    assert(CHECK("Result is integer", result.isNumber() && result.isExact()));
    assert(CHECK_EQUAL("Value is correct", 110, result.vInt()));
  }
 
}
