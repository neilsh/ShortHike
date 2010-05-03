//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Location;
private import Prototype;
private import Station;
private import Defs;
private import std.string;

struct Message
{
  wchar[] mFrom;
  wchar[] mSubject;
  wchar[] mBody;
};

enum Aspect {
  CREDITS,
  INCOME,
  EXPENSES,
  ENERGY,
  THERMAL
};


class Mission
{
public:

  Location getLocation() {return mLocation;}


  int getActionPointsCurrent() {return mActionPointsCurrent;}
  int getActionPointsMax() {return mActionPointsMax;}

  int getSimulationMonth() {return mMonth;}
  int getSimulationYear() {return mYear;}
  void setTime(int month, int year) {mMonth = month; mYear = year;}

  Prototype[] getPrototypes() {return mPrototypes;}

  //--------------------------------------------------------------------------------

  // FIXME: Hardcoded location

  this()
  {
    mMonth = 0;
    mYear = 2020;
    mActionPointsMax = 10;
    mActionPointsCurrent = 10;
    mCredits = 0;

    mLocation = new Location();
    char[] fileName = "assets/locations/low_earth_orbit.shd";
    mLocation.load(fileName);
  
    mStation = new Station(this);
    setCredits(1000.0);

    // DEBUG
    enableAspect(Aspect.CREDITS);
    enableAspect(Aspect.INCOME);
    enableAspect(Aspect.EXPENSES);
    enableAspect(Aspect.ENERGY);
    enableAspect(Aspect.THERMAL);

    createPrototypes(this);
    mStation.addModule(mPrototypes[0].createModule());
  }

  ~this()
  {
  }

  // --------------------------------------------------------------------------------

  void
  setActionPointsMax(int maxPoints)
  {
    mActionPointsMax = maxPoints;
    if(mActionPointsCurrent > mActionPointsMax)
      mActionPointsCurrent = mActionPointsMax;
  }

  // --------------------------------------------------------------------------------

  void
  update(float timestep)
  {
    mLocation.update(timestep);
    mStation.updateSimulation();
    //   doString("base_update()");
  }

  void
  advanceTurn()
  {
    mMonth++;
    if(12 == mMonth) {
      mMonth = 0;
      mYear++;
    }

    mStation.stabilizeSimulation();
    mStation.advanceTurn();
    float profit = 0;
    if(isAspect(Aspect.INCOME))
      profit += mStation.getTotalIncome();
    if(isAspect(Aspect.EXPENSES))
      profit += mStation.getTotalExpenses();
    if(isAspect(Aspect.CREDITS))
      creditTransaction(profit);

    mActionPointsCurrent = mActionPointsMax;
  }


  void
  useActionPoint()
  {
    mActionPointsCurrent--;
    if(mActionPointsCurrent < 1)
      advanceTurn();
  }

  // --------------------------------------------------------------------------------

  wchar[]
  getSimulationDateString()
  {
    wchar[] dateString;
    if(mMonth == 0)
      dateString ~= "Jan";
    else if(mMonth == 1)
      dateString ~= "Feb";
    else if(mMonth == 2)
      dateString ~= "Mar";
    else if(mMonth == 3)
      dateString ~= "Apr";
    else if(mMonth == 4)
      dateString ~= "May";
    else if(mMonth == 5)
      dateString ~= "Jun";
    else if(mMonth == 6)
      dateString ~= "Jul";
    else if(mMonth == 7)
      dateString ~= "Aug";
    else if(mMonth == 8)
      dateString ~= "Sep";
    else if(mMonth == 9)
      dateString ~= "Oct";
    else if(mMonth == 10)
      dateString ~= "Nov";
    else if(mMonth == 11)
      dateString ~= "Dec";
    char[] year = std.string.toString(mYear);
    // DEBUG
    //   dateString ~= " " ~ year;
    return dateString;
  }

  //--------------------------------------------------------------------------------
  // Credits

  float
  getCredits()
  {
    return mCredits;
  }

  void
  setCredits(float _credits)
  {
    mCredits = _credits;
  }


  void
  creditTransaction(float _creditsChange)
  {
    mCredits += _creditsChange;
  }



  const int MAGIC_MAXIMUM_DEBT = -100;

  bool
  isSolvent()
  {
    // DEBUG
    return true;
  
    //   if(mAspects.isCredits())
    //     return mCredits > MAGIC_MAXIMUM_DEBT;
    //   else
    //     return true;
  }


  //--------------------------------------------------------------------------------

  const uint MAXIMUM_MESSAGE_NUM = 200;

  void
  postMessage(wchar[] from, wchar[] subject, wchar[] text)
  {
    if(mMessages.length == MAXIMUM_MESSAGE_NUM) return;
    Message newMessage;
    newMessage.mFrom = from;
    newMessage.mSubject = subject;
    newMessage.mBody = text;
    mMessages ~= newMessage;
  }

  Message[]
  getMessages()
  {
    return mMessages;
  }



  //--------------------------------------------------------------------------------

  Station
  getStation()
  {
    return mStation;
  }


  // --------------------------------------------------------------------------------

  bool
  isAspect(Aspect aspect)
  {
    return mAspects[aspect];
  }

  void
  enableAspect(Aspect aspect)
  {
    mAspects[aspect] = true;
  }

  void
  disableAspect(Aspect aspect)
  {
    mAspects[aspect] = true;
  }


  //-------------------------------------------------------------------------------

  Prototype[] mPrototypes;

private:

  int mMonth;
  int mYear;
  int mActionPointsMax;
  int mActionPointsCurrent;

  bool mAspects[Aspect.max + 1];

  float mCredits;
  Station mStation;
  Message[] mMessages;


  Location mLocation;
}
