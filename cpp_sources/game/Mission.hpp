//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef GAME_MISSION_HPP
#define GAME_MISSION_HPP

class Station;
class Location;
class Aspects;
class Prototype;

struct Message
{
  wstring mFrom;
  wstring mSubject;
  wstring mBody;
};

typedef vector<Message> MessageList;


class Mission
{
public:
  Mission();
  ~Mission();

  Location* getLocation() {return mLocation;}

  void updateSimulation();
  void advanceTurn();
  void useActionPoint();  

  int getActionPointsCurrent() {return mActionPointsCurrent;}
  int getActionPointsMax() {return mActionPointsMax;}
  void setActionPointsMax(int maxActionPoints);

  int getSimulationMonth() const {return mMonth;}
  int getSimulationYear() const {return mYear;}
  void setTime(int month, int year) {mMonth = month; mYear = year;}
  wstring getSimulationDateString() const;
  
  Real getCredits() const;
  void setCredits(Real _credits);
  void creditTransaction(Real _creditsChange);
  bool isSolvent() const;

  void postMessage(wstring from, wstring subject, wstring body);
  const MessageList& getMessages();

  const vector<Prototype*> getPrototypes() {return mPrototypes;}

  Station* getStation();
  Aspects* getAspects();
private:

  int mMonth;
  int mYear;
  int mActionPointsMax;
  int mActionPointsCurrent;

  Aspects* mAspects;
  Real mCredits;
  Station* mStation;
  MessageList mMessages;

  vector<Prototype*> mPrototypes;

  Location* mLocation;
};


#endif
