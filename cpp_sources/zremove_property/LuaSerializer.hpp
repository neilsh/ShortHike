//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_LUA_SERIALIZER
#define PROPERTY_LUA_SERIALIZER

class Item;
class Property;

class StringProperty;
class RealProperty;
class ItemProperty;
class ItemVectorProperty;


class LuaSerializer
{
public:
  LuaSerializer();
  void write(ostream& stream, Item* target);
  Item* read(LuaObject table);

private:
  void reset();
  string getNewIDString();

  void scanTableToDictionary(LuaObject itemlist);
  void readItems(LuaObject itemList);
  template<class PropertyClass> bool readPropertyStub(LuaObject data, Item* target, Property* property);

  void readProperty(LuaObject data, Item* target, StringProperty* property);
  void readProperty(LuaObject data, Item* target, RealProperty* property);
  void readProperty(LuaObject data, Item* target, ItemProperty* property);
  void readProperty(LuaObject data, Item* target, ItemVectorProperty* property);

  void writeItem(ostream& stream, Item* target);
  template<class PropertyClass> bool writePropertyStub(ostream& stream, Item* target, Property* property);
  set<Item*> scanItemsToDictionary(Item* target);

  void writeProperty(ostream& stream, Item* target, StringProperty* property);
  void writeProperty(ostream& stream, Item* target, RealProperty* property);
  void writeProperty(ostream& stream, Item* target, ItemProperty* property);
  void writeProperty(ostream& stream, Item* target, ItemVectorProperty* property);
  
  unsigned runningID;
  map<Item*, string> itemToString;
  map<string, Item*> stringToItem;
};

//--------------------------------------------------------------------------------

class TestLuaSerializer : public TestFixture
{
public:
  TEST_FIXTURE_START(TestLuaSerializer);
  TEST_CASE(simple_structure);
  TEST_FIXTURE_END();
  
  void setUp();
  void tearDown();

  void simple_structure();
private:
  LuaState* luaState;
  static LuaObject currentTable;

  static int createTableImpl(LuaState* state);
  LuaObject createTable(string tableSpec);
};



#endif
