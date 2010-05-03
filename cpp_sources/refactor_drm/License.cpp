//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Iphlpapi.h"
#include "Iptypes.h"

#include "License.hpp"
#include "LimitDialog.hpp"
#include "property/Property.hpp"
#include "util/imath.hpp"
#include "config/ConfigManager.hpp"
#include "generated/build_info.hpp"

//--------------------------------------------------------------------------------

const char* MAGIC_RSA_E = "FIXME:REMOVED";
const char* MAGIC_RSA_N = "FIXME:REMOVED";
// const char* DEBUG_RSA_D = "FIXME:REMOVED";

const string MAGIC_PRODUCT_ID_VERSION = "10";
const string MAGIC_LICENSE_KEY_VERSION = "10";
const float MAGIC_MINIMUM_TRIAL_MINUTES = 5.0;
const float MAGIC_MAXIMUM_TRIAL_MINUTES = 60.0;



//--------------------------------------------------------------------------------

START_PROPERTY_DEFINITION(License)
{
  addProperty(new RealProperty(L"Trial minutes left", SetMember(mTrialMinutes)));

  addProperty(new StringProperty(L"Player full name", SetMember(mPlayerName)));
  addProperty(new StringProperty(L"Player email address", SetMember(mPlayerEmail)));
  addProperty(new IntProperty(L"License expiration", SetMember(mExpiration)));

  addProperty(new StringProperty(L"Product ID", SetMember(mProductID)));
  addProperty(new StringProperty(L"Source for product ID", SetMember(mIdentifierSource)));
  addProperty(new StringProperty(L"License Key", SetMember(mLicenseKey)));
  addProperty(new BoolProperty(L"Has a hardware key", SetMember(mHasHardwareID)));
}
END_PROPERTY_DEFINITION

//--------------------------------------------------------------------------------
// Constructor

License::License(DataFile* dataFile)
  : Item(dataFile),
    mType(LICENSE_TRIAL), mTrialMinutes(5), mPlayerName(""), mPlayerEmail(""), mExpiration(0),
    mProductID(""), mIdentifierSource(""), mLicenseKey(""), mSecondsSinceSaved(0)
{
}


//--------------------------------------------------------------------------------

bool
License::initialize()
{
  bool saveConfig = false;
  // Create identifier both if completely new or using a hardware identifier.
  // In case of complete hardware removal switch to a softwareID
  if(mProductID == "" || mHasHardwareID == true) {
    ostringstream ossLicenseID;
    ossLicenseID << MAGIC_PRODUCT_ID_VERSION << createUniqueIdentifier();
    mProductID = ossLicenseID.str();
    transform(mProductID.begin(), mProductID.end(), mProductID.begin(), tolower);
    saveConfig = true;
  }

  if(mTrialMinutes < MAGIC_MINIMUM_TRIAL_MINUTES)
    mTrialMinutes = MAGIC_MINIMUM_TRIAL_MINUTES;
  if(mTrialMinutes > MAGIC_MAXIMUM_TRIAL_MINUTES)
    mTrialMinutes = MAGIC_MAXIMUM_TRIAL_MINUTES;

  validateKey();

  return saveConfig;
}

//--------------------------------------------------------------------------------

void
License::updateLicenseKey(bool forceNetwork)
{
  if(forceNetwork == true)
    if(ERROR_SUCCESS != InternetAttemptConnect(0)) return;
  
  HINTERNET internetBase = InternetOpen("ShortHike", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
  if(NULL == internetBase) return;
  
  // Prompt for email if not working
  downloadLicenseKey(internetBase);

  CHECK("Closing internet succeeded", TRUE == InternetCloseHandle(internetBase));  
}


//--------------------------------------------------------------------------------
// Connects to the server and updates our license key

const int MAGIC_BUFFER_SIZE = 2000;

bool
License::downloadLicenseKey(HINTERNET internetBase)
{  
  ostringstream ossLicenseURL;
  ossLicenseURL << "https://secure.shorthike.com/drm/update_license.php?product_ID=" << mProductID << "&email=" << mPlayerEmail;
  HINTERNET serverFile = InternetOpenUrl(internetBase, ossLicenseURL.str().c_str(), NULL, NULL,
                               INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD
                               | INTERNET_FLAG_SECURE, NULL);
  char buffer[MAGIC_BUFFER_SIZE];
  ostringstream ossTextFile;
  
  BOOL readStatus = FALSE;
  DWORD bytesRead = 0;
  do {
    readStatus = InternetReadFile(serverFile, buffer, MAGIC_BUFFER_SIZE, &bytesRead);
    if(bytesRead > 0) {
      ossTextFile << string(buffer, bytesRead);
    }
  } while(readStatus == TRUE && bytesRead != 0);
  if(readStatus == FALSE) {
    return true;
  }
  
  CHECK("Closing handle succeeded", TRUE == InternetCloseHandle(serverFile));
  
  if(ossTextFile.str() == "-1") {
    return false;
  }
  
  // Valid license from the server
  istringstream issDataStream(ossTextFile.str());

  bool hasChanged = false;
  string newLicenseKey;
  string newPlayerName;
  string newPlayerEmail;

  getline(issDataStream, newLicenseKey);
  getline(issDataStream, newPlayerName);
  getline(issDataStream, newPlayerEmail);
  
  if(newLicenseKey != mLicenseKey) {
    mLicenseKey = newLicenseKey;
    hasChanged = true;
    validateKey();
  }
  if(newPlayerName != mPlayerName) {
    mPlayerName = newPlayerName;
    hasChanged = true;
  }  
  if(newPlayerEmail != mPlayerEmail) {
    mPlayerEmail = newPlayerEmail;
    hasChanged = true;
  }
  
  if(hasChanged)
    ConfigManager::getSingleton().saveConfig();

  return true;
}


//--------------------------------------------------------------------------------

const int MAGIC_LICENSE_KEY_RADIX = 32;
const int MAGIC_PRODUCT_ID_RADIX = 16;
const int MAGIC_RSA_KEY_RADIX = 16;

void
License::validateKey()
{
  mpz_t cipherText;
  mpz_t clearText;
  mpz_t rsaE;
  mpz_t rsaN;
  const int BUFFER_SIZE = 10000;
  char buffer[BUFFER_SIZE];

  mp_int_init(&cipherText);
  mp_int_init(&clearText);
  mp_int_init(&rsaE);
  mp_int_init(&rsaN);
  mp_int_read_string(&rsaE, MAGIC_RSA_KEY_RADIX, MAGIC_RSA_E);
  mp_int_read_string(&rsaN, MAGIC_RSA_KEY_RADIX, MAGIC_RSA_N);
  
  mp_int_read_string(&cipherText, MAGIC_LICENSE_KEY_RADIX, mLicenseKey.c_str());
  mp_int_exptmod(&cipherText, &rsaE, &rsaN, &clearText);
  mp_int_to_string(&clearText, MAGIC_PRODUCT_ID_RADIX, buffer, BUFFER_SIZE);
  
  string compoundKey(buffer);
  if(MAGIC_LICENSE_KEY_VERSION == compoundKey.substr(0, 2)) {
    string productID = compoundKey.substr(10, compoundKey.size() - 10);
    transform(productID.begin(), productID.end(), productID.begin(), tolower);
    if(productID == mProductID) {
      istringstream expirationStream(compoundKey.substr(2, 8));
      expirationStream >> mExpiration;
      if((mExpiration) > BUILD_DATE_INTEGER) {
        mType = LICENSE_ACTIVE;
        return;
      }
    }
  }
  
  mType = LICENSE_TRIAL;
  if(mTrialMinutes > MAGIC_MAXIMUM_TRIAL_MINUTES) mTrialMinutes = MAGIC_MAXIMUM_TRIAL_MINUTES;
}

//--------------------------------------------------------------------------------
// Managing the trial license expiration process

const float MAGIC_SECONDS_BETWEEN_SAVES = 60;

void
License::update(float timeStep)
{
  if(mType == LICENSE_TRIAL) {
    // DEBUG
    //    timeStep *= 30;

    mTrialMinutes -= (timeStep / 60);
    if(mTrialMinutes < 0) {
      mTrialMinutes = 0;
      if(false == LimitDialog::getSingleton().getVisible()) {
        showLimitDialog(true);
      }
    }
    
    mSecondsSinceSaved += timeStep;
    if(mSecondsSinceSaved > MAGIC_SECONDS_BETWEEN_SAVES) {
      mSecondsSinceSaved = 0;
      ConfigManager::getSingleton().saveConfig();
    }  
  }
}



//--------------------------------------------------------------------------------
// Get hardware identifier. This is either the MAC address or a randomly generated
// sequence.

string
License::createUniqueIdentifier()
{
  string hardwareID = getHardwareID();
  if(hardwareID != "") {
    mHasHardwareID = true;
    return hardwareID;
  }
  
  // No valid MAC addresses found. Generating random
  mHasHardwareID = false;
  srand(GetTickCount());
  ostringstream ossRandomKey;
  ossRandomKey.setf(ios_base::hex, ios_base::basefield);
  for(int iByte = 0; iByte < 16; ++iByte) {
    int byte = static_cast<int>((frand() * 255.0f));
    ossRandomKey.width(2);
    ossRandomKey.fill('0');
    ossRandomKey << static_cast<int>(byte);
  }
  mIdentifierSource = "Software Random Key";
  
  return ossRandomKey.str();
}

//--------------------------------------------------------------------------------
string
License::getHardwareID()
{
  IP_ADAPTER_INFO AdapterInfo[32];
  DWORD dwBufLen = sizeof(AdapterInfo);

  // The first option is to scan for a valid MAC address. This makes reinstalls easier.
  if(ERROR_SUCCESS == GetAdaptersInfo(AdapterInfo, &dwBufLen)) {
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    do {
      if(pAdapterInfo->Type != MIB_IF_TYPE_PPP && pAdapterInfo->Type != MIB_IF_TYPE_SLIP && pAdapterInfo->Type != MIB_IF_TYPE_LOOPBACK) {
        ostringstream ossMACAddress;
        ossMACAddress.setf(ios_base::hex, ios_base::basefield);
        bool nonZero = false;
        for(int iByte = 0; iByte < MAX_ADAPTER_ADDRESS_LENGTH; ++iByte) {
          // Skip zero bytes after the first six
          if(iByte < 6 || pAdapterInfo->Address[iByte] != 0) {
            ossMACAddress.width(2);
            ossMACAddress.fill('0');
            ossMACAddress << static_cast<int>(pAdapterInfo->Address[iByte]);
          }
          if(pAdapterInfo->Address[iByte] != 0)
            nonZero = true;
        }
        if(nonZero) {
          mIdentifierSource = pAdapterInfo->Description;
          return ossMACAddress.str();
        }        
      }
      pAdapterInfo = pAdapterInfo->Next;
    }
    while(pAdapterInfo);
  }
  return "";
}

//--------------------------------------------------------------------------------


bool
License::hasExpired() const
{
  SYSTEMTIME systemTime;
  GetSystemTime(&systemTime);
  unsigned int systemTimeInteger = systemTime.wYear * 10000 + systemTime.wMonth * 100 + systemTime.wDay;
  return systemTimeInteger > mExpiration;
}


//--------------------------------------------------------------------------------

wstring
License::getExpirationString()
{
  wostringstream wossExpiration;
  wossExpiration << mExpiration;

  wstring expirationString = wossExpiration.str();
  wostringstream wossTextual;  
  wstring month = expirationString.substr(4, 2);
  if(L"01" == month)
    wossTextual << L"Jan";
  else if(L"02" == month)
    wossTextual << L"Feb";
  else if(L"03" == month)
    wossTextual << L"Mar";
  else if(L"04" == month)
    wossTextual << L"Apr";
  else if(L"05" == month)
    wossTextual << L"May";
  else if(L"06" == month)
    wossTextual << L"Jun";
  else if(L"07" == month)
    wossTextual << L"Jul";
  else if(L"08" == month)
    wossTextual << L"Aug";
  else if(L"09" == month)
    wossTextual << L"Sep";
  else if(L"10" == month)
    wossTextual << L"Oct";
  else if(L"11" == month)
    wossTextual << L"Nov";
  else if(L"12" == month)
    wossTextual << L"Dec";
  wossTextual << L" " << expirationString.substr(6, 2) << L" " << expirationString.substr(0, 4);
  return wossTextual.str();
}


//--------------------------------------------------------------------------------
// Output

ostream& operator<< (ostream& out, const License* license)
{
  out << "--------------------------------------------------------------------------------" << endl;
  out << "License" << endl;  

  if(license->mType == LICENSE_TRIAL) {
    out << "Evaluation License" << endl;
  }
  else if(license->mType == LICENSE_ACTIVE) {
    out << "Full License (" << license->mExpiration << ")";
  }
  else if(license->mType == LICENSE_EXPIRED) {
    out << "Full License " << "[SUBSCRIPTION EXPIRED]" << endl;
  }
  
  out << "--------------------------------------------------------------------------------" << endl;
  
  return out;
}


