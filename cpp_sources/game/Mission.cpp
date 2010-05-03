//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Mission.hpp"
#include "Aspects.hpp"
#include "Station.hpp"
#include "Location.hpp"
#include "Prototype.hpp"

//--------------------------------------------------------------------------------

// FIXME: Hardcoded location

Mission::Mission()
  : mMonth(0), mYear(2020), mActionPointsMax(10), mActionPointsCurrent(10), mCredits(0)
{
  mAspects = new Aspects();
  mLocation = new Location();
  string fileName = "assets/cislunar/low_earth_orbit.shd";
  mLocation->load(fileName);
  
  mStation = new Station(this);
  setCredits(1000.0);

  // DEBUG

  mAspects->enableCredits();
  mAspects->enableIncome();
  mAspects->enableExpenses();
  mAspects->enableEnergy();
  mAspects->enableThermal();

//    simulation = {
//       mass = 11.6,
//       cost = 8.70,
//       maintenance = 0.36,

//       energy = {
// 	 flow = 400
//       };

//       thermal = {
// 	 flow = 150
//       };

//       lifesupport = {
// 	 flow = 12,
//       };

//       crew = {
// 	 flow = 10,
//       };
//    }

  Prototype* node = new Prototype("assets/cislunar/node.shd", false);
  mPrototypes.push_back(node);
  node->setSimFloat(SIM_MASS, 11.6);
  node->setSimFloat(SIM_COST_HARDWARE, 8.7);
  node->setSimFloat(SIM_COST_MAINTENANCE, 0.36);
  node->setSimFloat(SIM_ENERGY, -4);
  node->setSimFloat(SIM_THERMAL, -2);
  node->setSimFloat(SIM_PROFIT_MAX, 5);

  node->setSimFloat(SIM_LIFESUPPORT_BASE, frand() * 10);
  node->setSimFloat(SIM_FLOW_CAPACITY, 3);
  node->setSimFloat(SIM_FLOW_PASSENGERS, static_cast<int>(fnrand() * 10.0) + 5);
  node->setSimFloat(SIM_FLOW_VISITORS, static_cast<int>(fnrand() * 10.0) + 5);
  node->setSimFloat(SIM_FLOW_CREW, static_cast<int>(fnrand() * 10.0) + 5);
  node->setSimFloat(SIM_FLOW_CARGO, static_cast<int>(fnrand() * 10.0) + 5);
  node->setSimFloat(SIM_FLOW_COMMODITIES, static_cast<int>(fnrand() * 10.0) + 5);
  node->setSimFloat(SIM_FLOW_SERVICE, static_cast<int>(fnrand() * 10.0) + 5);

//   // DEBUG:
//   setLifeSupportBase(frand() * 10);
//   setFlowCapacity(3);
//   float flowClass = frand() * 6;
//   if(flowClass < 1)
//     setFlowLocal(DISCRETE_PASSENGERS, static_cast<int>(fnrand() * 10.0) + 5);
//   else if(flowClass < 2)
//     setFlowLocal(DISCRETE_VISITORS, static_cast<int>(fnrand() * 10.0) + 5);
//   else if(flowClass < 3)
//     setFlowLocal(DISCRETE_CREW, static_cast<int>(fnrand() * 10.0) + 5);
//   else if(flowClass < 4)
//     setFlowLocal(DISCRETE_CARGO, static_cast<int>(fnrand() * 10.0) + 5);
//   else if(flowClass < 5)
//     setFlowLocal(DISCRETE_COMMODITIES, static_cast<int>(fnrand() * 10.0) + 5);
//   else
//     setFlowLocal(DISCRETE_SERVICE, static_cast<int>(fnrand() * 10.0) + 5);


//    simulation = {
//       mass = 56.6, -- tons
//       cost = 52.0,
//       maintenance = 2.5,

//       energy = {
// 	 output = 6.8, -- kWe
// 	 flow = 100.0,
//       };

//       thermal = {
// 	 output = 4.5, -- kWt
// 	 flow = 45,
//       };

//       lifesupport = {
// 	 flow = 12,
//       };

//       crew = {
// 	 output = 2,
// 	 flow = 10,
//       };
//    };

  Prototype* service = new Prototype("assets/cislunar/service_module.shd", false);  
  mPrototypes.push_back(service);
  service->setSimFloat(SIM_MASS, 56.6);
  service->setSimFloat(SIM_COST_HARDWARE, 52.0);
  service->setSimFloat(SIM_COST_MAINTENANCE, 2.5);
  service->setSimFloat(SIM_ENERGY, 20);
  service->setSimFloat(SIM_THERMAL, 10);
  service->setSimFloat(SIM_LIFESUPPORT_BASE, frand() * 10);

  service->setSimFloat(SIM_FLOW_CAPACITY, 3);
  service->setSimFloat(SIM_FLOW_PASSENGERS, static_cast<int>(fnrand() * 10.0 + 5));
  service->setSimFloat(SIM_FLOW_VISITORS, static_cast<int>(fnrand() * 10.0 + 5));
  service->setSimFloat(SIM_FLOW_CREW, static_cast<int>(fnrand() * 10.0 + 5));
  service->setSimFloat(SIM_FLOW_CARGO, static_cast<int>(fnrand() * 10.0 + 5));
  service->setSimFloat(SIM_FLOW_COMMODITIES, static_cast<int>(fnrand() * 10.0 + 5));
  service->setSimFloat(SIM_FLOW_SERVICE, static_cast<int>(fnrand() * 10.0 + 5));

  mStation->addModule(mPrototypes[0]->createModule());
}

Mission::~Mission()
{
}

// void
// Mission::initScriptFile(string scriptFile)
// {
//   mScriptFile = scriptFile;
//   reload();
// }


//--------------------------------------------------------------------------------



// void
// Mission::preSave()
// {
//   lua_State* cLuaState = luaState->GetCState();
//   LuaAutoBlock autoBlock(cLuaState);
//   lua_getglobal(cLuaState, "base_export");
//   if(lua_pcall(cLuaState, 0, 1, 0)) {
//     logEngineInfo(string("Lua: ") + lua_tostring(cLuaState, -1));
//     return;
//   }
  
//   if(!lua_isstring(cLuaState, -1)) return;
//   mScriptData = lua_tostring(cLuaState, -1);
// }


// void
// Mission::postLoad()
// {
//   Mission* prevMission = rMain()->getMission();
//   rMain()->setMission(this);

//   initLua();
  
//   if(mScriptFile != "")
//     loadFile(mScriptFile.c_str());

//   if(mScriptData != "") {
//     lua_State* cLuaState = luaState->GetCState();
//     LuaAutoBlock autoBlock(cLuaState);
//     lua_getglobal(cLuaState, "base_import");
//     lua_pushstring(cLuaState, mScriptData.c_str());
//     if(lua_pcall(cLuaState, 1, 0, 0)) {
//       logEngineInfo(string("Lua: ") + lua_tostring(cLuaState, -1));
//       return;
//     }
//   }
//   rMain()->setMission(prevMission);
// }


//--------------------------------------------------------------------------------
// This loads or reloads the script environment

// void
// Mission::reload()
// {
//   initLua();
  
//   mMessages.clear();

//   if(mScriptFile != "")
//     loadFile(mScriptFile.c_str());
//   doString("trigger_update()");
// }

// void
// Mission::initLua()
// {
//   if(luaState != NULL) LuaState::Destroy(luaState);
  
//   luaState = LuaState::Create();
//   LuaObject globals = luaState->GetGlobals();
//   registerAllFunctions(luaState);
//   luaState->DoFile("assets/scripts/safe_load.lua");
//   loadFile("assets/scripts/base_shorthike.lua");
// }


// --------------------------------------------------------------------------------

void
Mission::setActionPointsMax(int maxPoints)
{
  mActionPointsMax = maxPoints;
  if(mActionPointsCurrent > mActionPointsMax)
    mActionPointsCurrent = mActionPointsMax;
}

// --------------------------------------------------------------------------------

void
Mission::updateSimulation()
{
  mStation->updateSimulation();
//   doString("base_update()");
}

void
Mission::advanceTurn()
{
  mMonth++;
  if(12 == mMonth) {
    mMonth = 0;
    mYear++;
  }

  mStation->stabilizeSimulation();
  mStation->advanceTurn();
  float profit = 0;
  if(getAspects()->isIncome())
    profit += mStation->getTotalIncome();
  if(getAspects()->isExpenses())
    profit += mStation->getTotalExpenses();
  if(getAspects()->isCredits())
    creditTransaction(profit);

  mActionPointsCurrent = mActionPointsMax;
}


void
Mission::useActionPoint()
{
  mActionPointsCurrent--;
  if(mActionPointsCurrent < 1)
    advanceTurn();
}

// --------------------------------------------------------------------------------

wstring
Mission::getSimulationDateString() const
{
  wostringstream dateString;
  if(mMonth == 0)
    dateString << L"Jan";
  else if(mMonth == 1)
    dateString << L"Feb";
  else if(mMonth == 2)
    dateString << L"Mar";
  else if(mMonth == 3)
    dateString << L"Apr";
  else if(mMonth == 4)
    dateString << L"May";
  else if(mMonth == 5)
    dateString << L"Jun";
  else if(mMonth == 6)
    dateString << L"Jul";
  else if(mMonth == 7)
    dateString << L"Aug";
  else if(mMonth == 8)
    dateString << L"Sep";
  else if(mMonth == 9)
    dateString << L"Oct";
  else if(mMonth == 10)
    dateString << L"Nov";
  else if(mMonth == 11)
    dateString << L"Dec";
  dateString << L" " << mYear;
  return dateString.str();
}

//--------------------------------------------------------------------------------
// Credits

Real
Mission::getCredits() const
{
  return mCredits;
}

void
Mission::setCredits(Real _credits)
{
  mCredits = _credits;
}


void
Mission::creditTransaction(Real _creditsChange)
{
  mCredits += _creditsChange;
}



const MAGIC_MAXIMUM_DEBT = -100;

bool
Mission::isSolvent() const
{
  // DEBUG
  return true;
  
//   if(mAspects->isCredits())
//     return mCredits > MAGIC_MAXIMUM_DEBT;
//   else
//     return true;
}


//--------------------------------------------------------------------------------

const unsigned MAXIMUM_MESSAGE_NUM = 200;

void
Mission::postMessage(wstring from, wstring subject, wstring body)
{
  if(mMessages.size() == MAXIMUM_MESSAGE_NUM) return;
  Message newMessage;
  newMessage.mFrom = from;
  newMessage.mSubject = subject;
  newMessage.mBody = body;
  mMessages.push_back(newMessage);
}

const MessageList&
Mission::getMessages()
{
  return mMessages;
}



//--------------------------------------------------------------------------------

Station*
Mission::getStation()
{
  return mStation;
}

Aspects*
Mission::getAspects()
{
  return mAspects;
}



//-------------------------------------------------------------------------------

// void
// Mission::loadFile(string fileName)
// {
//   ostringstream ossLoadFile;
//   ossLoadFile << "safe_load_file(\"" << fileName << "\")";
//   luaState->DoString(ossLoadFile.str().c_str());
// }



// void
// Mission::doString(string scriptCommand)
// {
//   ostringstream ossDoString;
//   ossDoString << "safe_do_string('" << scriptCommand << "')";
//   luaState->DoString(ossDoString.str().c_str());
// }

