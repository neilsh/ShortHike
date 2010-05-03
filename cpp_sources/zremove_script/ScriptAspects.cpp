//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "ScriptUtil.hpp"
#include "game/Aspects.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------

int
aspects_enable_credits(LuaState*)
{
  rMain()->getAspects()->enableCredits();
  return 0;
}

int
aspects_disable_credits(LuaState*)
{
  rMain()->getAspects()->disableCredits();
  return 0;
}


int
aspects_is_credits(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isCredits());
  return 1;
}

//--------------------------------------------------------------------------------

int
aspects_enable_income(LuaState*)
{
  rMain()->getAspects()->enableIncome();
  return 0;
}

int
aspects_disable_income(LuaState*)
{
  rMain()->getAspects()->disableIncome();
  return 0;
}


int
aspects_is_income(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isIncome());
  return 1;
}

//--------------------------------------------------------------------------------

int
aspects_enable_expenses(LuaState*)
{
  rMain()->getAspects()->enableExpenses();
  return 0;
}

int
aspects_disable_expenses(LuaState*)
{
  rMain()->getAspects()->disableExpenses();
  return 0;
}


int
aspects_is_expenses(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isExpenses());
  return 1;
}

//--------------------------------------------------------------------------------

int
aspects_enable_energy(LuaState*)
{
  rMain()->getAspects()->enableEnergy();
  return 0;
}

int
aspects_disable_energy(LuaState*)
{
  rMain()->getAspects()->disableEnergy();
  return 0;
}


int
aspects_is_energy(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isEnergy());
  return 1;
}


//--------------------------------------------------------------------------------

int
aspects_enable_thermal(LuaState*)
{
  rMain()->getAspects()->enableThermal();
  return 0;
}

int
aspects_disable_thermal(LuaState*)
{
  rMain()->getAspects()->disableThermal();
  return 0;
}


int
aspects_is_thermal(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isThermal());
  return 1;
}


//--------------------------------------------------------------------------------

int
aspects_enable_shop(LuaState*)
{
  rMain()->getAspects()->enableShop();
  return 0;
}

int
aspects_disable_shop(LuaState*)
{
  rMain()->getAspects()->disableShop();
  return 0;
}


int
aspects_is_shop(LuaState* state)
{
  state->PushBoolean(rMain()->getAspects()->isShop());
  return 1;
}

//--------------------------------------------------------------------------------

void
registerAspectsFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("aspects_enable_credits", &aspects_enable_credits);
  globals.Register("aspects_disable_credits", &aspects_disable_credits);
  globals.Register("aspects_is_credits", &aspects_is_credits);

  globals.Register("aspects_enable_income", &aspects_enable_income);
  globals.Register("aspects_disable_income", &aspects_disable_income);
  globals.Register("aspects_is_income", &aspects_is_income);

  globals.Register("aspects_enable_expenses", &aspects_enable_expenses);
  globals.Register("aspects_disable_expenses", &aspects_disable_expenses);
  globals.Register("aspects_is_expenses", &aspects_is_expenses);

  globals.Register("aspects_enable_energy", &aspects_enable_energy);
  globals.Register("aspects_disable_energy", &aspects_disable_energy);
  globals.Register("aspects_is_energy", &aspects_is_energy);

  globals.Register("aspects_enable_thermal", &aspects_enable_thermal);
  globals.Register("aspects_disable_thermal", &aspects_disable_thermal);
  globals.Register("aspects_is_thermal", &aspects_is_thermal);

  globals.Register("aspects_enable_shop", &aspects_enable_shop);
  globals.Register("aspects_disable_shop", &aspects_disable_shop);
  globals.Register("aspects_is_shop", &aspects_is_shop);
}
