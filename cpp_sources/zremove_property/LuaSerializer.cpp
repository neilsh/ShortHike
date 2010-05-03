//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "LuaSerializer.hpp"
#include "property/Item.hpp"
#include "property/Property.hpp"
#include "generated/build_info.hpp"

//--------------------------------------------------------------------------------

LuaSerializer::LuaSerializer()
{
  reset();
}

void
LuaSerializer::reset()
{
  runningID = 0;
  itemToString.clear();
  stringToItem.clear();
}


//--------------------------------------------------------------------------------

Item*
LuaSerializer::read(LuaObject table)
{
  reset();

  try {
    LuaObject rootItemSpec = table["rootItem"];
    CHECK("RootItem is string", rootItemSpec.IsString());
    string rootItemName = string(rootItemSpec.GetString());

    LuaObject itemList = table["items"];
    CHECK("Itemlist is table", itemList.IsTable());
    scanTableToDictionary(itemList);
    readItems(itemList);

    Item* rootItem = stringToItem[rootItemName];
    return rootItem;
  }
  catch(Exception& e) {
    logEngineError("LuaSerializer: Error reading item from table" + e.getFullDescription());
    return NULL;
  }  
}

void
LuaSerializer::scanTableToDictionary(LuaObject table)
{
  int itemCount = table.GetCount();
  for(int itemI = 1; itemI <= itemCount; ++itemI) {
    LuaObject itemSpec = table[itemI];
    CHECK("Item spec is table", itemSpec.IsTable());

    LuaObject nameSpec = itemSpec["itemName"];
    CHECK("Item name is string", nameSpec.IsString());
    string itemName = string(nameSpec.GetString());
    
    LuaObject classSpec = itemSpec["itemClass"];
    CHECK("Item class is string", classSpec.IsString());
    string className = string(classSpec.GetString());
    
    Item* newItem = Item::createItem(className, NULL);

    itemToString[newItem] = itemName;
    stringToItem[itemName] = newItem;
  }
}

void
LuaSerializer::readItems(LuaObject table)
{
  int itemCount = table.GetCount();
  for(int itemI = 1; itemI <= itemCount; ++itemI) {
    LuaObject itemSpec = table[itemI];

    LuaObject nameSpec = itemSpec["itemName"];
    CHECK("Item name is string", nameSpec.IsString());
    string itemName = string(nameSpec.GetString());

    Item* currentItem = stringToItem[itemName];
    PropertyMap properties = currentItem->getProperties();

    LuaObject propertiesSpec = itemSpec["properties"];
    CHECK("Properties are a table", propertiesSpec.IsTable());
    int propertyCount = propertiesSpec.GetCount();
    for(int propertyI = 1; propertyI <= propertyCount; ++propertyI) {
      LuaObject currentPropertySpec = propertiesSpec[propertyI];
      CHECK("Property specification is table", currentPropertySpec.IsTable());

      LuaObject memberNameSpec = currentPropertySpec["memberName"];
      CHECK("MemberName is string", memberNameSpec.IsString());
      
      string memberName = string(memberNameSpec.GetString());
      Property* property = currentItem->getProperty(memberName);
      LuaObject dataSpec = currentPropertySpec["value"];
      
      if(readPropertyStub<StringProperty>(dataSpec, currentItem, property)) continue;
      if(readPropertyStub<RealProperty>(dataSpec, currentItem, property)) continue;
      if(readPropertyStub<ItemProperty>(dataSpec, currentItem, property)) continue;
      if(readPropertyStub<ItemVectorProperty>(dataSpec, currentItem, property)) continue;
    }
  }
}

template<class PropertyClass>
bool
LuaSerializer::readPropertyStub(LuaObject data, Item* target, Property* property)
{
  PropertyClass* castedProperty = dynamic_cast<PropertyClass*>(property);
  if(castedProperty != NULL) {
    readProperty(data, target, castedProperty);
    return true;
  }
  else {
    return false;
  }  
}

//--------------------------------------------------------------------------------
// Functions to parse individual Properties

void
LuaSerializer::readProperty(LuaObject data, Item* target, StringProperty* property)
{
  CHECK("StringProperty value is string", data.IsString());
  string value = string(data.ToString());
  property->setString(target, value);
}

void
LuaSerializer::readProperty(LuaObject data, Item* target, RealProperty* property)
{
  CHECK("RealProperty value is number", data.IsConvertibleToNumber());
  Real value = static_cast<Real>(data.GetDouble());
  property->setReal(target, value);
}


void
LuaSerializer::readProperty(LuaObject data, Item* target, ItemProperty* property)
{
  CHECK("ItemProperty value is string", data.IsString());
  string itemName = string(data.GetString());
  Item* value = stringToItem[itemName];
  property->setItem(target, value);  
}

void
LuaSerializer::readProperty(LuaObject data, Item* target, ItemVectorProperty* property)
{
  CHECK("ItemVectorProperty is table", data.IsTable());
  int itemCount = data.GetCount();
  vector<Item*> items;
  
  for(int itemI = 1; itemI <= itemCount; ++itemI) {
    LuaObject itemSpec = data[itemI];
    CHECK("ItemSpec is string", itemSpec.IsString());

    string itemName = string(itemSpec.GetString());
    Item* currentItem = stringToItem[itemName];
    CHECK("Item is found in dictionary", currentItem != NULL);
    items.push_back(currentItem);    
  }
  property->setItems(target, items);
}


//--------------------------------------------------------------------------------


void
LuaSerializer::write(ostream& stream, Item* target)
{
  reset();
  set<Item*> processed = scanItemsToDictionary(target);
  
  stream << "{" << endl;
  stream << " -- Created using LuaSerializer build: " << BUILD_DATE << " " << BUILD_REVISION << endl;  
  stream << " rootItem = \"" << itemToString[target] << "\"," << endl;
  stream << " items = {" << endl;
  for(set<Item*>::const_iterator itemI = processed.begin(); itemI != processed.end(); ++itemI) {
    Item* nextToSave = *itemI;
    writeItem(stream, nextToSave);
  }
  stream << " }," << endl;
  stream << "}" << endl;
}


void
LuaSerializer::writeItem(ostream& stream, Item* nextToSave)
{
  stream << "  {" << endl;
  stream << "   itemName = \"" << itemToString[nextToSave] << "\"," << endl;
  stream << "   itemClass = \"" << nextToSave->getClassName() << "\"," << endl;
  stream << "   properties = {" << endl;
  PropertyMap properties = nextToSave->getProperties();
  for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
    Property* property = (*propertyI).second;
    if(writePropertyStub<StringProperty>(stream, nextToSave, property)) continue;
    if(writePropertyStub<RealProperty>(stream, nextToSave, property)) continue;
    if(writePropertyStub<ItemProperty>(stream, nextToSave, property)) continue;
    if(writePropertyStub<ItemVectorProperty>(stream, nextToSave, property)) continue;
  }
  stream << "   }," << endl;
  stream << "  }," << endl;
}


template<class PropertyClass>
bool
LuaSerializer::writePropertyStub(ostream& stream, Item* target, Property* property)
{
  PropertyClass* castedProperty = dynamic_cast<PropertyClass*>(property);
  if(castedProperty != NULL) {
    stream << "    {" << endl
           << "     memberName = \"" << property->getMemberName() << "\"," << endl;
    writeProperty(stream, target, castedProperty);
    stream << "    }," << endl;
    return true;
  }
  else {
    return false;
  }  
}

//--------------------------------------------------------------------------------
// Specific routines for writing different types of properties

void
LuaSerializer::writeProperty(ostream& stream, Item* target, StringProperty* property)
{
  stream << "     value = \"" << property->getString(target) << "\"," << endl;
}

void
LuaSerializer::writeProperty(ostream& stream, Item* target, RealProperty* property)
{
  stream << "     value = " << property->getReal(target) << "," << endl;
}

void
LuaSerializer::writeProperty(ostream& stream, Item* target, ItemProperty* property)
{
  stream << "     value = \"" << itemToString[property->getItem(target)] << "\"," << endl;
}

void
LuaSerializer::writeProperty(ostream& stream, Item* target, ItemVectorProperty* property)
{  
  stream << "     value = {" << endl;
  vector<Item*> items = property->getItems(target);
  for(vector<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
    Item* item = *itemI;
    stream << "     \"" << itemToString[item] << "\"," << endl;
  }
  stream << "     }," << endl;  
}


//--------------------------------------------------------------------------------
// Scanning in all items into the dictionary

set<Item*>
LuaSerializer::scanItemsToDictionary(Item* target)
{
  set<Item*> processed;
  queue<Item*> tagged;
  tagged.push(target);
  while(true != tagged.empty()) {
    Item* nextItem = tagged.front();
    processed.insert(nextItem);

    PropertyMap properties = nextItem->getProperties();
    // Tag non processed pointers
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      if(property->hasItemPointers(nextItem)) {
        vector<Item*> pointers = property->getItemPointers(nextItem);
        for(vector<Item*>::const_iterator pointerI = pointers.begin(); pointerI != pointers.end(); ++pointerI) {
          Item* pointer = *pointerI;
          if(processed.find(pointer) == processed.end()) {
            tagged.push(pointer);
          }          
        }
      }
    }
    
    string IDString = getNewIDString();
    itemToString[nextItem] = IDString;
    stringToItem[IDString] = nextItem;
    tagged.pop();
  }  
  return processed;
}

string
LuaSerializer::getNewIDString()
{
  ostringstream IDStream;
  IDStream << "ID" << setfill('0') << setw(6) << ++runningID;
  return IDStream.str();
}


//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/property/TestLuaSerializer", TestLuaSerializer);

#include "TestData.hpp"

//--------------------------------------------------------------------------------

LuaObject TestLuaSerializer::currentTable;

void
TestLuaSerializer::setUp()
{
  luaState = LuaState::Create();

  LuaObject globals = luaState->GetGlobals();

  globals.Register("createTable", &TestLuaSerializer::createTableImpl);
}

void
TestLuaSerializer::tearDown()
{
  LuaState::Destroy(luaState);
}


//--------------------------------------------------------------------------------


void
TestLuaSerializer::simple_structure()
{
  SampleItem* item1 = createSampleTree();

  LuaSerializer serializer;
  ostringstream output1;
  
  serializer.write(output1, item1);
//   cout << output1.str() << endl;
  LuaObject tableSpec = createTable(output1.str());
  Item* rootItem = serializer.read(tableSpec);
  SampleItem* item1Copy = static_cast<SampleItem*>(rootItem);

  ostringstream output2;
  serializer.write(output2, item1Copy);
  //  cout << endl << endl << "Copy of item1" << endl << output2.str() << endl; 
}


//--------------------------------------------------------------------------------
// Utility functions


int
TestLuaSerializer::createTableImpl(LuaState* state)
{
  LuaStack stack(state);
  currentTable = stack[1];
  return 0;
}

LuaObject
TestLuaSerializer::createTable(string tableSpec)
{
  string fullSpec("createTable");
  fullSpec += tableSpec;
  luaState->DoString(fullSpec.c_str());
  return currentTable;
}
