//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef DRM_LICENSE_HPP
#define DRM_LICENSE_HPP

#include "Wininet.h"

enum LicenseType {
  LICENSE_TRIAL,
  LICENSE_ACTIVE,
  LICENSE_EXPIRED
};


#include "property/Item.hpp"

class License : public Item
{
  friend ostream& operator<< (ostream& out, const License* license);
public:
  PROPERTY_DECLARATION(License);

  License(DataFile* dataFile);

  bool initialize();
  void updateLicenseKey(bool forceNetwork = false);
  
  void update(float timeStep);

  LicenseType getType() {return mType;}
  bool isActive() { return mType == LICENSE_ACTIVE; }

  float getTrialMinutes() const {return mTrialMinutes;}

  string getPlayerName() const {return mPlayerName;}
  string getPlayerEmail() const {return mPlayerEmail;}
  int getExpiration() const {return mExpiration;}
  wstring getExpirationString();

  void setPlayerEmail(string _email) {mPlayerEmail = _email;}

  bool hasExpired() const;

  string getProductID() {return mProductID;}
  string getLicenseKey() {return mLicenseKey;}
private:
  bool downloadLicenseKey(HINTERNET internetBase);
  string createUniqueIdentifier();
  string getHardwareID();
  void validateKey();

  LicenseType mType;

  // Used by trial license
  float mTrialMinutes;
  float mSecondsSinceSaved;

  // Used by full license
  string mPlayerName;
  string mPlayerEmail;
  unsigned int mExpiration;

  string mProductID;
  string mIdentifierSource;
  bool mHasHardwareID;
  string mLicenseKey;
};


ostream& operator<< (ostream& out, const License* license);


#endif
