//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_ITEM
#define PROPERTY_ITEM

class Property;
class DataFile;
class Item;

typedef Item* (*ItemCreatorFunction)(DataFile* dataFile);
typedef map<string, Property*> PropertyMap;

class Item : public UserDefinedObject
{
  friend bool compare(Item* item1, Item* item2, bool verbose = false);
public:
  Item(DataFile* dataFile);

  Property* getProperty(string name);

  DataFile* getDataFile();
  void setDataFile(DataFile* dataFile);

  const string& getIdentifier() const;
  void setIdentifier(string identifier);
  const string& getPrototypeName() const;
  void setPrototypeName(string prototypeName);
  PropertyMap getDifferingProperties();

  Item* createInstance(DataFile* newDataFile);
  Item* clone(DataFile* newDataFile);
  set<Item*> findConnectedItems();

  static void destroyItem(Item* target);
  static Item* createItem(string className, DataFile* dataFile);
  static void registerItemClass(string className, ItemCreatorFunction creatorFunction);

  static void addMemberName(string memberName);
  static map<string, int>& getMemberNameDictionary();

  // This can be overriden by subclasses to do additional copying
  virtual void copyFrom(Item* ) {}  

  //--------------------------------------------------------------------------------
  // This function is called after each property change
  virtual void propertyChanged(string propertyName) {}

  //--------------------------------------------------------------------------------
  // Save and loading hook functions

  virtual void preSave() {}  
  virtual void postLoad() {}  

  //--------------------------------------------------------------------------------
  // Internal methods implemented direct or by the macro

  virtual PropertyMap& getProperties() = 0;
  virtual const string& getClassName() const = 0;
  virtual long getTypeID() const { return 0; }

  //--------------------------------------------------------------------------------
  // Template methods

  template<class ItemClass> ItemClass* castToClass()
  {
    if(this->getClassName() != ItemClass::getStaticClassName())
      return NULL;
    else
      return static_cast<ItemClass*>(this);
  }
  
protected:
  virtual ~Item();
  static void addPropertyImpl(PropertyMap& props, Property* newProperty);
private:
  DataFile* dataFile;
  string identifier;
  string prototypeName;

  static map<string, ItemCreatorFunction>& creatorFunctions();
};

extern bool compare(Item* item1, Item* item2, bool verbose);

//--------------------------------------------------------------------------------
// These macros are used for declaring properties

#define GetMemberPtr(Member) (&(((thisClass*) 0)->Member))
#define GetPointerMemberPtr(Member) ((((thisClass*) 0)->Member))
#define GetMemberOffset(Member) ((ulong) &(((thisClass*) 0)->Member))
#define GetMemberSize(Member) (sizeof (((thisClass*)0)->Member))

#define SetMember(Member) #Member, GetMemberOffset(Member)

//----------------------------------------
// Declaration is used inside class

#define PROPERTY_DECLARATION(CLASSNAME)                             \
public:                                                             \
typedef CLASSNAME thisClass;                                        \
                                                                    \
class AutoItemRegister                                              \
{                                                                   \
public:                                                             \
  AutoItemRegister()                                                \
  {                                                                 \
    CLASSNAME::registerItemClass(#CLASSNAME, &CLASSNAME::itemCreatorFunction); \
    CLASSNAME::registerProperties();                                \
  }                                                                 \
};                                                                  \
                                                                    \
static AutoItemRegister autoItemRegister;                           \
                                                                    \
static Item*                                                        \
itemCreatorFunction(DataFile* dataFile)                             \
{                                                                   \
  return new CLASSNAME(dataFile);                                   \
}                                                                   \
                                                                    \
virtual const string&                                               \
getClassName() const                                                \
{                                                                   \
  return CLASSNAME::getStaticClassName();                           \
}                                                                   \
                                                                    \
virtual const string&                                               \
getTypeName() const                                                 \
{                                                                   \
  return CLASSNAME::getStaticClassName();                           \
}                                                                   \
                                                                    \
public:                                                             \
virtual PropertyMap&                                                \
getProperties()                                                     \
{                                                                   \
  return properties;                                                \
}                                                                   \
                                                                    \
static const string&                                                \
getStaticClassName()                                                \
{                                                                   \
  static const string classNameString = #CLASSNAME;                 \
  return classNameString;                                           \
}                                                                   \
                                                                    \
static void                                                         \
addProperty(Property* newProperty)                                  \
{                                                                   \
  addPropertyImpl(properties, newProperty);                         \
}                                                                   \
                                                                    \
static PropertyMap properties;                                      \
                                                                    \
static void registerProperties();


//----------------------------------------
// Definition is used in the cpp file

#define START_PROPERTY_DEFINITION(CLASSNAME)                        \
                                                                    \
PropertyMap CLASSNAME::properties;                                  \
CLASSNAME::AutoItemRegister CLASSNAME::autoItemRegister = CLASSNAME::AutoItemRegister(); \
                                                                    \
void CLASSNAME::registerProperties()                                \
{

#define END_PROPERTY_DEFINITION }


//--------------------------------------------------------------------------------

class TestItem : public TestFixture
{
public:
  TEST_FIXTURE_START(TestItem);
  TEST_CASE(item_creation);
  TEST_CASE(deep_compare);
  TEST_CASE(find_connected);
  TEST_CASE(create_instance);
  TEST_FIXTURE_END();

  void item_creation();
  void deep_compare();
  void find_connected();
  void create_instance();
};

#endif
