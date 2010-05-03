//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
//
// Design decisions and assumptions:
// - Each Item is linked to a single datafile.
// - All nodes in any graph of items created by references using the property system
//   must also belong to the same DataFile.
// - Data can be anonymous or part of the prototype hierarchy. Prototype definitions,
//   if found within the data hierarchy, are all loaded at startup time. As anonymous
//   data prototypes need to be self contained
//--------------------------------------------------------------------------------

#ifndef PROPERTY_SQLITE_IMPL_1
#define PROPERTY_SQLITE_IMPL_1

class DataFile;
class Item;
class Property;
class StringProperty;
class WStringProperty;
class BoolProperty;
class IntProperty;
class RealProperty;
class RealVectorProperty;
class ItemProperty;
class ItemVectorProperty;
class ItemSetProperty;
class Vector3Property;
class QuaternionProperty;


class SQLiteImpl1
{
public:
  SQLiteImpl1(DataFile* dataFile);

  void save();
  void load();  
private:
  void createTables(sqlite3* database);
  void createIndices(sqlite3* database);
  void prepareStatements(sqlite3* database);
  void finalizeStatements(sqlite3* database);

  string getInfo(string name);
  void setInfo(string name, string value);

  template<class PropertyClass> bool writePropertyStub(Property* property, Item* target);
  void writeProperty(StringProperty* property, Item* currentItem);
  void writeProperty(WStringProperty* property, Item* currentItem);
  void writeProperty(BoolProperty* property, Item* currentItem);
  void writeProperty(IntProperty* property, Item* currentItem);
  void writeProperty(RealProperty* property, Item* currentItem);
  void writeProperty(RealVectorProperty* property, Item* currentItem);
  void writeProperty(ItemProperty* property, Item* currentItem);
  void writeProperty(ItemVectorProperty* property, Item* currentItem);
  void writeProperty(ItemSetProperty* property, Item* currentItem);
  void writeProperty(Vector3Property* property, Item* currentItem);
  void writeProperty(QuaternionProperty* property, Item* currentItem);

  template<class PropertyClass> bool readPropertyStub(Property* property, Item* target);
  void readProperty(StringProperty* property, Item* currentItem);
  void readProperty(WStringProperty* property, Item* currentItem);
  void readProperty(BoolProperty* property, Item* currentItem);
  void readProperty(IntProperty* property, Item* currentItem);
  void readProperty(RealProperty* property, Item* currentItem);
  void readProperty(RealVectorProperty* property, Item* currentItem);
  void readProperty(ItemProperty* property, Item* currentItem);
  void readProperty(ItemVectorProperty* property, Item* currentItem);
  void readProperty(ItemSetProperty* property, Item* currentItem);
  void readProperty(Vector3Property* property, Item* currentItem);
  void readProperty(QuaternionProperty* property, Item* currentItem);

  void setCurrentDatabase(sqlite3* database);
  int handleSQLiteError(int errorCode, string file, int line);

  DataFile* dataFile;

  sqlite3* errorDatabase;
  bool sqliteErrors;

  sqlite3_stmt* selectInfoStatement;
  sqlite3_stmt* insertInfoStatement;
  sqlite3_stmt* selectItemStatement;
  sqlite3_stmt* insertItemStatement;
  sqlite3_stmt* selectDictionaryStatement;
  sqlite3_stmt* insertDictionaryStatement;

  sqlite3_stmt* insertStringProp;
  sqlite3_stmt* insertBoolProp;
  sqlite3_stmt* insertIntProp;
  sqlite3_stmt* insertRealProp;
  sqlite3_stmt* insertRealVectorProp;
  sqlite3_stmt* insertItemProp;
  sqlite3_stmt* insertItemVectorProp;
  sqlite3_stmt* insertVector3Prop;
  sqlite3_stmt* insertQuaternionProp;

  sqlite3_stmt* selectStringProp;
  sqlite3_stmt* selectBoolProp;
  sqlite3_stmt* selectIntProp;
  sqlite3_stmt* selectRealProp;
  sqlite3_stmt* selectRealVectorProp;
  sqlite3_stmt* selectItemProp;
  sqlite3_stmt* selectItemVectorProp;
  sqlite3_stmt* selectVector3Prop;
  sqlite3_stmt* selectQuaternionProp;

  map<Item*, int> itemToID;
  map<int, Item*> IDToItem;  
  map<string, int> dictionary;
};


//--------------------------------------------------------------------------------

#endif
