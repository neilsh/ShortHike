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
//
// NB. Datafiles are automatically loaded on initialization if a valid filename
// is provided. The creation of a new datafile is the only way to load. Save as
// changes the filename permanently while save just saves to the current name.
//
//--------------------------------------------------------------------------------


#ifndef PROPERTY_DATA_FILE
#define PROPERTY_DATA_FILE

class Item;
class SQLiteImpl1;

class DataFile
{
  friend class Item;
  friend class SQLiteImpl1;
public:
  DataFile(wstring fileName = L"", bool silentErrors = false);
  ~DataFile();

  wstring getFileName();

  void save();
  void save(wstring fileName);

  Item* getRootItem();  
  void setRootItem(Item* rootItem);  

  bool isPartOfFile(Item* item);
  int getNumberOfItems() {return items.size();}

  void setSilentErrors() { silentErrors = true; }
private:

  void registerItem(Item* target);
  void unRegisterItem(Item* target);

  wstring fileName;
  Item* rootItem;
  set<Item*> items;

  bool silentErrors;
  SQLiteImpl1* dataFileImpl;
};


//--------------------------------------------------------------------------------

class TestDataFile : public TestFixture
{
public:
  TEST_FIXTURE_START(TestDataFile);
  TEST_CASE(save_load);
  TEST_CASE(save_modify);
  TEST_CASE(item_destroy);
  TEST_CASE(item_clone);
  TEST_CASE(prototype_save_load);
  TEST_CASE(corrupted_database);
  TEST_FIXTURE_END();
  
  void save_load();
  void save_modify();
  void item_destroy();
  void item_clone();
  void prototype_save_load();
  void corrupted_database();
};


class TestDataFileStress : public TestFixture
{
public:
  TEST_FIXTURE_START(TestDataFileStress);
//   TEST_CASE(save_stress);
  TEST_CASE(prototype_save_stress);
  TEST_FIXTURE_END();

  void save_stress();
  void prototype_save_stress();
};


#endif
