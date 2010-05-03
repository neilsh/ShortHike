//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// This is an implementation of dataFile saving using SQLite3
// When adding a new property check all the places tagged NEW PROPERTY and add
// as required. Also add your property to the test Items in TestData.hpp

#include "Common.hpp"

#include "SQLiteImpl1.hpp"
#include "DataFile.hpp"
#include "Item.hpp"
#include "Property.hpp"
#include "PrototypeManager.hpp"
#include "generated/build_info.hpp"

#define handleError(SQLiteCall) handleSQLiteError(SQLiteCall, __FILE__, __LINE__)

const string DATAFILE_CURRENT_VERSION = "1";


//--------------------------------------------------------------------------------

SQLiteImpl1::SQLiteImpl1(DataFile* _dataFile)
  : dataFile(_dataFile)
{
}


//--------------------------------------------------------------------------------

void
SQLiteImpl1::createTables(sqlite3* database)
{
  // NEW PROPERTY: Add your data layout to the file
  // Create the data layout
  handleError(sqlite3_exec(database, "CREATE TABLE info (name TEXT UNIQUE, value TEXT);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE item (item_ID INTEGER PRIMARY KEY, class TEXT, identifier TEXT, prototype_name TEXT);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE dictionary (member_name TEXT, member_ID INTEGER);", NULL, NULL, NULL));

  handleError(sqlite3_exec(database, "CREATE TABLE property_string (item_ID INTEGER, member_ID INTEGER, value TEXT);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_bool (item_ID INTEGER, member_ID INTEGER, value INTEGER);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_int (item_ID INTEGER, member_ID INTEGER, value INTEGER);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_real (item_ID INTEGER, member_ID INTEGER, value FLOAT);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_real_vector (item_ID INTEGER, member_ID INTEGER, position INTEGER, value FLOAT);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_item (item_ID INTEGER, member_ID INTEGER, value INTEGER);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_item_vector (item_ID INTEGER, member_ID INTEGER, position INTEGER, value INTEGER);",
                           NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_vector3 (item_ID INTEGER, member_ID INTEGER, value_x FLOAT, value_y FLOAT, value_z FLOAT);",
                           NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE TABLE property_quaternion (item_ID INTEGER, member_ID INTEGER, value_x FLOAT, value_y FLOAT, value_z FLOAT, value_w FLOAT);",
                           NULL, NULL, NULL));
}


void
SQLiteImpl1::createIndices(sqlite3* database)
{
  // NEW PROPERTY: Add index for your data layout
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_string ON property_string (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_bool ON property_bool (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_int ON property_int (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_real ON property_real (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_real_vector ON property_real_vector (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_item ON property_item (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_item_vector ON property_item_vector (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_vector3 ON property_vector3 (item_ID, member_ID);", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "CREATE INDEX index_property_quaternion ON property_quaternion (item_ID, member_ID);", NULL, NULL, NULL));
}


void
SQLiteImpl1::prepareStatements(sqlite3* database)
{
  handleError(sqlite3_prepare(database, "SELECT value FROM info WHERE name=?", -1, &selectInfoStatement, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO info(name, value) VALUES(?, ?);", -1, &insertInfoStatement, NULL));

  handleError(sqlite3_prepare(database, "SELECT item_ID, class, identifier, prototype_name FROM item;", -1, &selectItemStatement, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO item(item_ID, class, identifier, prototype_name) VALUES(?, ?, ?, ?);", -1, &insertItemStatement, NULL));

  handleError(sqlite3_prepare(database, "SELECT member_name, member_ID FROM dictionary;", -1, &selectDictionaryStatement, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO dictionary(member_name, member_ID) VALUES(?, ?);", -1, &insertDictionaryStatement, NULL));

  // NEW PROPERTY: Add statmenet for insert and select
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_string(item_ID, member_ID, value) VALUES(?, ?, ?);", -1, &insertStringProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_bool(item_ID, member_ID, value) VALUES(?, ?, ?);", -1, &insertBoolProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_int(item_ID, member_ID, value) VALUES(?, ?, ?);", -1, &insertIntProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_real(item_ID, member_ID, value) VALUES(?, ?, ?);", -1, &insertRealProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_real_vector(item_ID, member_ID, position, value) VALUES(?, ?, ?, ?);",
                              -1, &insertRealVectorProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_item(item_ID, member_ID, value) VALUES(?, ?, ?);", -1, &insertItemProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_item_vector(item_ID, member_ID, position, value) VALUES(?, ?, ?, ?);",
                              -1, &insertItemVectorProp, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_vector3(item_ID, member_ID, value_x, value_y, value_z) VALUES(?, ?, ?, ?, ?);",
                              -1, &insertVector3Prop, NULL));
  handleError(sqlite3_prepare(database, "INSERT OR REPLACE INTO property_quaternion(item_ID, member_ID, value_x, value_y, value_z, value_w) VALUES(?, ?, ?, ?, ?, ?);",
                              -1, &insertQuaternionProp, NULL));

  handleError(sqlite3_prepare(database, "SELECT value FROM property_string WHERE item_ID=? AND member_ID=?;", -1, &selectStringProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_bool WHERE item_ID=? AND member_ID=?;", -1, &selectBoolProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_int WHERE item_ID=? AND member_ID=?;", -1, &selectIntProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_real WHERE item_ID=? AND member_ID=?;", -1, &selectRealProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_real_vector WHERE item_ID=? AND member_ID=? ORDER BY position ASC;",
                              -1, &selectRealVectorProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_item WHERE item_ID=? AND member_ID=?;", -1, &selectItemProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value FROM property_item_vector WHERE item_ID=? AND member_ID=? ORDER BY position ASC;",
                              -1, &selectItemVectorProp, NULL));
  handleError(sqlite3_prepare(database, "SELECT value_x, value_y, value_z FROM property_vector3 WHERE item_ID=? AND member_ID=?;",
                              -1, &selectVector3Prop, NULL));
  handleError(sqlite3_prepare(database, "SELECT value_x, value_y, value_z, value_w FROM property_quaternion WHERE item_ID=? AND member_ID=?;",
                              -1, &selectQuaternionProp, NULL));
}

void
SQLiteImpl1::finalizeStatements(sqlite3* database)
{
  handleError(sqlite3_finalize(selectInfoStatement));
  handleError(sqlite3_finalize(insertInfoStatement));
  
  handleError(sqlite3_finalize(selectItemStatement));
  handleError(sqlite3_finalize(insertItemStatement));

  handleError(sqlite3_finalize(selectDictionaryStatement));
  handleError(sqlite3_finalize(insertDictionaryStatement));
  
  // NEW PROPERTY: Add finalization to the statements
  handleError(sqlite3_finalize(insertStringProp));
  handleError(sqlite3_finalize(insertBoolProp));
  handleError(sqlite3_finalize(insertIntProp));
  handleError(sqlite3_finalize(insertRealProp));
  handleError(sqlite3_finalize(insertRealVectorProp));
  handleError(sqlite3_finalize(insertItemProp));
  handleError(sqlite3_finalize(insertItemVectorProp));
  handleError(sqlite3_finalize(insertVector3Prop));
  handleError(sqlite3_finalize(insertQuaternionProp));

  handleError(sqlite3_finalize(selectStringProp));
  handleError(sqlite3_finalize(selectBoolProp));
  handleError(sqlite3_finalize(selectIntProp));
  handleError(sqlite3_finalize(selectRealProp));
  handleError(sqlite3_finalize(selectRealVectorProp));
  handleError(sqlite3_finalize(selectItemProp));
  handleError(sqlite3_finalize(selectItemVectorProp));
  handleError(sqlite3_finalize(selectVector3Prop));
  handleError(sqlite3_finalize(selectQuaternionProp));
}

//--------------------------------------------------------------------------------

string
SQLiteImpl1::getInfo(string name)
{
  handleError(sqlite3_reset(selectInfoStatement));
  handleError(sqlite3_bind_text(selectInfoStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT));

  if(sqlite3_step(selectInfoStatement) != SQLITE_ROW) return "";

  return string(reinterpret_cast<const char*>(sqlite3_column_text(selectInfoStatement, 0)));
}


void
SQLiteImpl1::setInfo(string name, string value)
{
  handleError(sqlite3_reset(insertInfoStatement));
  handleError(sqlite3_bind_text(insertInfoStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT));
  handleError(sqlite3_bind_text(insertInfoStatement, 2, value.c_str(), -1, SQLITE_TRANSIENT));
  sqlite3_step(insertInfoStatement);
}



//--------------------------------------------------------------------------------

int
SQLiteImpl1::handleSQLiteError(int value, string file, int line)
{
  if(value == SQLITE_DONE || value == SQLITE_ROW) return value;
  if(value) {
    if(dataFile->silentErrors == false) {
      ostringstream errorStream;
      errorStream << file << "(" << line << ") : SQLite3 error: " << sqlite3_errmsg(errorDatabase);
      logEngineError(errorStream.str());
    }
    sqliteErrors = true;
  }  
  return value;
}

void
SQLiteImpl1::setCurrentDatabase(sqlite3* _database)
{
  errorDatabase = _database;
}


//--------------------------------------------------------------------------------

void
SQLiteImpl1::save()
{
  //--------------------------------------------------------------------------------
  // Raw and quick file deletion.
  fclose(fopen(wstringToString(dataFile->getFileName()).c_str(), "w"));

  //--------------------------------------------------------------------------------
  // Write the actual database

  sqlite3* database;
  handleError(sqlite3_open16(dataFile->getFileName().c_str(), &database));
  setCurrentDatabase(database);

  handleError(sqlite3_exec(database, "PRAGMA synchronous=OFF;", NULL, NULL, NULL));
  handleError(sqlite3_exec(database, "BEGIN TRANSACTION;", NULL, NULL, NULL));

  createTables(database);
  prepareStatements(database);

  setInfo("version", DATAFILE_CURRENT_VERSION);
  setInfo("shorthike_revision", BUILD_REVISION);
  setInfo("shorthike_build_date", BUILD_DATE);

  // Build item_ID cross reference
  itemToID.clear();
  // Start counting from 1000 to catch dodgy 0 index problems now and in the future
  int currentID = 1000;
  for(set<Item*>::const_iterator itemI = dataFile->items.begin(); itemI != dataFile->items.end(); ++itemI) {
    Item* currentItem = *itemI;
    string className = currentItem->getClassName();
    itemToID[currentItem] = currentID;
    CHECK("Has valid Datafile", currentItem->getDataFile() == dataFile);

    handleError(sqlite3_reset(insertItemStatement));
    handleError(sqlite3_bind_int(insertItemStatement, 1, currentID));
    handleError(sqlite3_bind_text(insertItemStatement, 2, currentItem->getClassName().c_str(), -1, SQLITE_TRANSIENT));
    handleError(sqlite3_bind_text(insertItemStatement, 3, currentItem->getIdentifier().c_str(), -1, SQLITE_TRANSIENT));
    handleError(sqlite3_bind_text(insertItemStatement, 4, currentItem->getPrototypeName().c_str(), -1, SQLITE_TRANSIENT));
    handleError(sqlite3_step(insertItemStatement));

    currentID++;
  }

  // Store link to root item
  ostringstream rootItemIDStream;
  rootItemIDStream << itemToID[dataFile->rootItem];
  setInfo("root_item", rootItemIDStream.str());

  // Save dictionary
  dictionary.clear();
  dictionary = Item::getMemberNameDictionary();
  for(map<string, int>::const_iterator nameI = dictionary.begin(); nameI != dictionary.end(); ++nameI) {
    string name = (*nameI).first;
    int nameID = (*nameI).second;

    handleError(sqlite3_reset(insertDictionaryStatement));
    handleError(sqlite3_bind_text(insertDictionaryStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT));
    handleError(sqlite3_bind_int(insertDictionaryStatement, 2, nameID));
    handleError(sqlite3_step(insertDictionaryStatement));
  }  

  // Start saving data from each item
  for(set<Item*>::const_iterator itemI = dataFile->items.begin(); itemI != dataFile->items.end(); ++itemI) {
    Item* currentItem = *itemI;
    PropertyMap properties = currentItem->getDifferingProperties();
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      // NEW PROPERTY: Add a stub handler here
      if(writePropertyStub<StringProperty>(property, currentItem)) continue;
      if(writePropertyStub<WStringProperty>(property, currentItem)) continue;
      if(writePropertyStub<BoolProperty>(property, currentItem)) continue;
      if(writePropertyStub<IntProperty>(property, currentItem)) continue;
      if(writePropertyStub<RealProperty>(property, currentItem)) continue;
      if(writePropertyStub<RealVectorProperty>(property, currentItem)) continue;
      if(writePropertyStub<ItemProperty>(property, currentItem)) continue;
      if(writePropertyStub<ItemVectorProperty>(property, currentItem)) continue;
      if(writePropertyStub<ItemSetProperty>(property, currentItem)) continue;
      if(writePropertyStub<Vector3Property>(property, currentItem)) continue;
      if(writePropertyStub<QuaternionProperty>(property, currentItem)) continue;
      CHECK_FALSE("Trying to save non-supported Property type");
    }
  }

  handleError(sqlite3_exec(database, "COMMIT TRANSACTION;", NULL, NULL, NULL));
  createIndices(database);

  finalizeStatements(database);
  handleError(sqlite3_close(database));
}


//--------------------------------------------------------------------------------

template<class PropertyClass>
bool
SQLiteImpl1::writePropertyStub(Property* property, Item* target)
{
  CHECK("Current item is in dictionary", itemToID.find(target) != itemToID.end());
  PropertyClass* castedProperty = dynamic_cast<PropertyClass*>(property);
  if(castedProperty != NULL) {
    writeProperty(castedProperty, target);
    return true;
  }
  else {
    return false;
  }
}


//--------------------------------------------------------------------------------
// Actual write implementations

void
SQLiteImpl1::writeProperty(StringProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertStringProp));
  handleError(sqlite3_bind_int(insertStringProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertStringProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_text(insertStringProp, 3, property->getString(currentItem).c_str(), -1, SQLITE_TRANSIENT));
  handleError(sqlite3_step(insertStringProp));
}

void
SQLiteImpl1::writeProperty(WStringProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertStringProp));
  handleError(sqlite3_bind_int(insertStringProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertStringProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_text16(insertStringProp, 3, property->getString(currentItem).c_str(), -1, SQLITE_TRANSIENT));
  handleError(sqlite3_step(insertStringProp));
}


void
SQLiteImpl1::writeProperty(BoolProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertBoolProp));
  handleError(sqlite3_bind_int(insertBoolProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertBoolProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_int(insertBoolProp, 3, property->getBool(currentItem)));
  handleError(sqlite3_step(insertBoolProp));
}


void
SQLiteImpl1::writeProperty(IntProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertIntProp));
  handleError(sqlite3_bind_int(insertIntProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertIntProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_int(insertIntProp, 3, property->getInt(currentItem)));
  handleError(sqlite3_step(insertIntProp));
}


void
SQLiteImpl1::writeProperty(RealProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertRealProp));
  handleError(sqlite3_bind_int(insertRealProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertRealProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_double(insertRealProp, 3, property->getReal(currentItem)));
  handleError(sqlite3_step(insertRealProp));
}


void
SQLiteImpl1::writeProperty(RealVectorProperty* property, Item* currentItem)
{
  vector<Real> numbers = property->getRealVector(currentItem);
  int position = 0;
  for(vector<Real>::const_iterator numberI = numbers.begin(); numberI != numbers.end(); ++numberI) {
    Real targetNumber = *numberI;
    handleError(sqlite3_reset(insertRealVectorProp));
    handleError(sqlite3_bind_int(insertRealVectorProp, 1, itemToID[currentItem]));
    handleError(sqlite3_bind_int(insertRealVectorProp, 2, dictionary[property->getMemberName()]));
    handleError(sqlite3_bind_int(insertRealVectorProp, 3, position));
    handleError(sqlite3_bind_double(insertRealVectorProp, 4, targetNumber));
    handleError(sqlite3_step(insertRealVectorProp));
    position++;
  }
}



void
SQLiteImpl1::writeProperty(ItemProperty* property, Item* currentItem)
{
  Item* target = property->getItem(currentItem);
  if(target == NULL) return;
  CHECK("Target item is in dictionary", itemToID.find(target) != itemToID.end());

  handleError(sqlite3_reset(insertItemProp));
  handleError(sqlite3_bind_int(insertItemProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertItemProp, 2, dictionary[property->getMemberName()]));
  handleError(sqlite3_bind_int(insertItemProp, 3, itemToID[target]));
  handleError(sqlite3_step(insertItemProp));
}

void
SQLiteImpl1::writeProperty(ItemVectorProperty* property, Item* currentItem)
{
  vector<Item*> items = property->getItems(currentItem);
  int position = 0;
  for(vector<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
    Item* targetItem = *itemI;
    CHECK("Target item is in dictionary", itemToID.find(targetItem) != itemToID.end());
    handleError(sqlite3_reset(insertItemVectorProp));
    handleError(sqlite3_bind_int(insertItemVectorProp, 1, itemToID[currentItem]));
    handleError(sqlite3_bind_int(insertItemVectorProp, 2, dictionary[property->getMemberName()]));
    handleError(sqlite3_bind_int(insertItemVectorProp, 3, position));
    handleError(sqlite3_bind_int(insertItemVectorProp, 4, itemToID[targetItem]));
    handleError(sqlite3_step(insertItemVectorProp));
    position++;
  }
}

void
SQLiteImpl1::writeProperty(ItemSetProperty* property, Item* currentItem)
{
  set<Item*> items = property->getItems(currentItem);
  int position = 0;
  for(set<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
    Item* targetItem = *itemI;
    CHECK("Target item is in dictionary", itemToID.find(targetItem) != itemToID.end());
    handleError(sqlite3_reset(insertItemVectorProp));
    handleError(sqlite3_bind_int(insertItemVectorProp, 1, itemToID[currentItem]));
    handleError(sqlite3_bind_int(insertItemVectorProp, 2, dictionary[property->getMemberName()]));
    handleError(sqlite3_bind_int(insertItemVectorProp, 3, position));
    handleError(sqlite3_bind_int(insertItemVectorProp, 4, itemToID[targetItem]));
    handleError(sqlite3_step(insertItemVectorProp));
    position++;
  }
}


void
SQLiteImpl1::writeProperty(Vector3Property* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertVector3Prop));
  handleError(sqlite3_bind_int(insertVector3Prop, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertVector3Prop, 2, dictionary[property->getMemberName()]));
  Vector3 target = property->getVector3(currentItem);
  handleError(sqlite3_bind_double(insertVector3Prop, 3, target[0]));
  handleError(sqlite3_bind_double(insertVector3Prop, 4, target[1]));
  handleError(sqlite3_bind_double(insertVector3Prop, 5, target[2]));
  handleError(sqlite3_step(insertVector3Prop));
}

void
SQLiteImpl1::writeProperty(QuaternionProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(insertQuaternionProp));
  handleError(sqlite3_bind_int(insertQuaternionProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(insertQuaternionProp, 2, dictionary[property->getMemberName()]));
  Quaternion target = property->getQuaternion(currentItem);
  handleError(sqlite3_bind_double(insertQuaternionProp, 3, target.x));
  handleError(sqlite3_bind_double(insertQuaternionProp, 4, target.y));
  handleError(sqlite3_bind_double(insertQuaternionProp, 5, target.z));
  handleError(sqlite3_bind_double(insertQuaternionProp, 6, target.w));
  handleError(sqlite3_step(insertQuaternionProp));
}

// NEW PROPERTY: Add a write handler



//================================================================================
// Loading

void
SQLiteImpl1::load()
{
  sqliteErrors = false;
  sqlite3* database;
  handleError(sqlite3_open16(dataFile->getFileName().c_str(), &database));
  setCurrentDatabase(database);

  prepareStatements(database);
  if(sqliteErrors == true || getInfo("version") != DATAFILE_CURRENT_VERSION) {
    finalizeStatements(database);
    handleError(sqlite3_close(database));
    return;
  }

  IDToItem.clear();
  itemToID.clear();

  // Create items
  handleError(sqlite3_reset(selectItemStatement));
  while(handleError(sqlite3_step(selectItemStatement)) == SQLITE_ROW) {
    int itemID = sqlite3_column_int(selectItemStatement, 0);
    string className = reinterpret_cast<const char*>(sqlite3_column_text(selectItemStatement, 1));
    string identifier = reinterpret_cast<const char*>(sqlite3_column_text(selectItemStatement, 2));
    string prototypeName = reinterpret_cast<const char*>(sqlite3_column_text(selectItemStatement, 3));
    Item* item;
    if(prototypeName != "")
      item = PrototypeManager::getSingleton().createClone(dataFile, prototypeName);
    else 
      item = Item::createItem(className, dataFile);
    CHECK("Item creation succeeded", item != NULL);
    IDToItem[itemID] = item;
    itemToID[item] = itemID;
    dataFile->items.insert(item);
  }

  // Find root
  stringstream rootIDStream (getInfo("root_item"));
  int rootID;
  rootIDStream >> rootID;
  dataFile->setRootItem(IDToItem[rootID]);
  CHECK("Valid root item", dataFile->getRootItem() != NULL);

  // Load dictionary
  dictionary.clear();
  handleError(sqlite3_reset(selectDictionaryStatement));
  while(handleError(sqlite3_step(selectDictionaryStatement)) == SQLITE_ROW) {
    string memberName = reinterpret_cast<const char*>(sqlite3_column_text(selectDictionaryStatement, 0));
    int memberID = sqlite3_column_int(selectDictionaryStatement, 1);
    dictionary[memberName] = memberID;
  }

  // Load properties
  for(set<Item*>::const_iterator itemI = dataFile->items.begin(); itemI != dataFile->items.end(); ++itemI) {
    Item* currentItem = *itemI;

    PropertyMap properties = currentItem->getProperties();
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      // NEW PROPERTY: Add a stub read handler here
      if(readPropertyStub<StringProperty>(property, currentItem)) continue;
      if(readPropertyStub<WStringProperty>(property, currentItem)) continue;
      if(readPropertyStub<BoolProperty>(property, currentItem)) continue;
      if(readPropertyStub<IntProperty>(property, currentItem)) continue;
      if(readPropertyStub<RealProperty>(property, currentItem)) continue;
      if(readPropertyStub<RealVectorProperty>(property, currentItem)) continue;
      if(readPropertyStub<ItemProperty>(property, currentItem)) continue;
      if(readPropertyStub<ItemVectorProperty>(property, currentItem)) continue;
      if(readPropertyStub<ItemSetProperty>(property, currentItem)) continue;
      if(readPropertyStub<Vector3Property>(property, currentItem)) continue;
      if(readPropertyStub<QuaternionProperty>(property, currentItem)) continue;
      CHECK_FALSE("Trying to load non-supported Property type");
    }
  }

  finalizeStatements(database);
  handleError(sqlite3_close(database));
}




//--------------------------------------------------------------------------------

template<class PropertyClass>
bool
SQLiteImpl1::readPropertyStub(Property* property, Item* target)
{
  PropertyClass* castedProperty = dynamic_cast<PropertyClass*>(property);
  if(castedProperty != NULL) {
    readProperty(castedProperty, target);
    return true;
  }
  else {
    return false;
  }
}


//--------------------------------------------------------------------------------
// Actual read implementations


void
SQLiteImpl1::readProperty(StringProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectStringProp));
  handleError(sqlite3_bind_int(selectStringProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectStringProp, 2, dictionary[property->getMemberName()]));

  // We go with default value if not set
  if(handleError(sqlite3_step(selectStringProp)) == SQLITE_ROW) {
    property->setString(currentItem, reinterpret_cast<const char*>(sqlite3_column_text(selectStringProp, 0)));
  }
}


void
SQLiteImpl1::readProperty(WStringProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectStringProp));
  handleError(sqlite3_bind_int(selectStringProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectStringProp, 2, dictionary[property->getMemberName()]));

  // We go with default value if not set
  if(handleError(sqlite3_step(selectStringProp)) == SQLITE_ROW) {
    property->setString(currentItem, reinterpret_cast<const wchar_t*>(sqlite3_column_text16(selectStringProp, 0)));
  }
}


void
SQLiteImpl1::readProperty(BoolProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectBoolProp));
  handleError(sqlite3_bind_int(selectBoolProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectBoolProp, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectBoolProp)) == SQLITE_ROW) {
    property->setBool(currentItem, sqlite3_column_int(selectBoolProp, 0) != 0);
  }
}


void
SQLiteImpl1::readProperty(IntProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectIntProp));
  handleError(sqlite3_bind_int(selectIntProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectIntProp, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectIntProp)) == SQLITE_ROW) {
    property->setInt(currentItem, sqlite3_column_int(selectIntProp, 0));
  }
}


void
SQLiteImpl1::readProperty(RealProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectRealProp));
  handleError(sqlite3_bind_int(selectRealProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectRealProp, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectRealProp)) == SQLITE_ROW) {
    property->setReal(currentItem, sqlite3_column_double(selectRealProp, 0));
  }
}


void
SQLiteImpl1::readProperty(RealVectorProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectRealVectorProp));
  handleError(sqlite3_bind_int(selectRealVectorProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectRealVectorProp, 2, dictionary[property->getMemberName()]));
  int result = handleError(sqlite3_step(selectRealVectorProp));
  // We need to skip reseting the RealVector if there is no data.
  if(result == SQLITE_ROW) {
    vector<Real> numberVector;
    while(result == SQLITE_ROW) {
      Real number = sqlite3_column_double(selectRealVectorProp, 0);
      numberVector.push_back(number);
      result = handleError(sqlite3_step(selectRealVectorProp));    
    }
    property->setRealVector(currentItem, numberVector);
  }
}


void
SQLiteImpl1::readProperty(ItemProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectItemProp));
  handleError(sqlite3_bind_int(selectItemProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectItemProp, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectItemProp)) == SQLITE_ROW) {
    Item* target = IDToItem[sqlite3_column_int(selectItemProp, 0)];
    CHECK("Valid target ID", target != NULL);
    if(target->getClassName() == property->getClassName())
      property->setItem(currentItem, target);
    else {
      ostringstream message;
      message << "Warning: ItemProperty: ClassName mismatch when loading file " << wstringToString(dataFile->getFileName())
              << ": expected " << property->getClassName() << " found " << target->getClassName();
#ifndef _DEBUG
      LogManager::getSingleton().logMessage(message.str());
#endif
    }    
  }
}

void
SQLiteImpl1::readProperty(ItemVectorProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectItemVectorProp));
  handleError(sqlite3_bind_int(selectItemVectorProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectItemVectorProp, 2, dictionary[property->getMemberName()]));
  vector<Item*> itemVector;
  while(handleError(sqlite3_step(selectItemVectorProp)) == SQLITE_ROW) {
    Item* target = IDToItem[sqlite3_column_int(selectItemVectorProp, 0)];
    CHECK("Valid target ID", target != NULL);
    if(target->getClassName() == property->getClassName())
      itemVector.push_back(target);
    else {
      ostringstream message;
      message << "Warning: ItemVectorProperty: ClassName mismatch when loading file " << wstringToString(dataFile->getFileName())
              << ": expected " << property->getClassName() << " found " << target->getClassName();
#ifndef _DEBUG
      LogManager::getSingleton().logMessage(message.str());
#endif
    }    
  }
  property->setItems(currentItem, itemVector);
}


void
SQLiteImpl1::readProperty(ItemSetProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectItemVectorProp));
  handleError(sqlite3_bind_int(selectItemVectorProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectItemVectorProp, 2, dictionary[property->getMemberName()]));
  set<Item*> itemSet;
  while(handleError(sqlite3_step(selectItemVectorProp)) == SQLITE_ROW) {
    Item* target = IDToItem[sqlite3_column_int(selectItemVectorProp, 0)];
    CHECK("Valid target ID", target != NULL);
    if(target->getClassName() == property->getClassName())
      itemSet.insert(target);
    else {
      ostringstream message;
      message << "Warning: ItemSetProperty: ClassName mismatch when loading file " << wstringToString(dataFile->getFileName())
              << ": expected " << property->getClassName() << " found " << target->getClassName();
#ifndef _DEBUG
      LogManager::getSingleton().logMessage(message.str());
#endif
    }    
  }
  property->setItems(currentItem, itemSet);
}


void
SQLiteImpl1::readProperty(Vector3Property* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectVector3Prop));
  handleError(sqlite3_bind_int(selectVector3Prop, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectVector3Prop, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectVector3Prop)) == SQLITE_ROW) {
    Vector3 target;
    target[0] = sqlite3_column_double(selectVector3Prop, 0);
    target[1] = sqlite3_column_double(selectVector3Prop, 1);
    target[2] = sqlite3_column_double(selectVector3Prop, 2);
    property->setVector3(currentItem, target);
  }
}


void
SQLiteImpl1::readProperty(QuaternionProperty* property, Item* currentItem)
{
  handleError(sqlite3_reset(selectQuaternionProp));
  handleError(sqlite3_bind_int(selectQuaternionProp, 1, itemToID[currentItem]));
  handleError(sqlite3_bind_int(selectQuaternionProp, 2, dictionary[property->getMemberName()]));
  if(handleError(sqlite3_step(selectQuaternionProp)) == SQLITE_ROW) {
    Quaternion target;
    target.x = sqlite3_column_double(selectQuaternionProp, 0);
    target.y = sqlite3_column_double(selectQuaternionProp, 1);
    target.z = sqlite3_column_double(selectQuaternionProp, 2);
    target.w = sqlite3_column_double(selectQuaternionProp, 3);
    property->setQuaternion(currentItem, target);
  }
}


// NEW PROPERTY: Add a read handler here

