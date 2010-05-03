//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// You can only play a single game at a time. The GameManager is the central location
// for accessing all game related data.
//--------------------------------------------------------------------------------


#ifndef GAME_ASPECTS_HPP
#define GAME_ASPECTS_HPP


class Aspects
{
public:
  Aspects();
  ~Aspects() {}

  void enableCredits() {mCredits = true;}
  void disableCredits() {mCredits = false;}
  bool isCredits() {return mCredits;}
  
  void enableIncome() {mIncome = true;}
  void disableIncome() {mIncome = false;}
  bool isIncome() {return mIncome;}

  void enableExpenses() {mExpenses = true;}
  void disableExpenses() {mExpenses = false;}
  bool isExpenses() {return mExpenses;}

  void enableEnergy() {mEnergy = true;}
  void disableEnergy() {mEnergy = false;}
  bool isEnergy() {return mEnergy;}

  void enableThermal() {mThermal = true;}
  void disableThermal() {mThermal = false;}
  bool isThermal() {return mThermal;}

  void enableShop() {mShop = true;}
  void disableShop() {mShop = false;}
  bool isShop() {return mShop;}
private:

  bool mCredits;
  bool mIncome;
  bool mExpenses;

  bool mEnergy;
  bool mThermal;

  bool mShop;
};


#endif
