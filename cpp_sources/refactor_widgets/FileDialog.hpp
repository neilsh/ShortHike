//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_FILE_DIALOG
#define USER_INTERFACE_FILE_DIALOG

#include "guilib/Dialog.hpp"
#include "guilib/ActionListener.hpp"
#include "guilib/List.hpp"

class Label;
class List;
class Button;
class TextField;

class FileDialog : public Dialog, public Singleton<FileDialog>, public ActionListener, public ListListener
{
public:
  FileDialog();  

  void show(ActionListener* listener, wstring title = L"Select File", wstring actionTitle = L"OK");
  void show(wstring title, wstring actionTitle, string luaFunction);

  virtual void actionPerformed(Widget* source);

  virtual void itemSelected(Widget* source, unsigned num);
  virtual void itemActivated(Widget* source, unsigned num);

  wstring getFileName();
private:
  void fireAction();

  string mLuaFunction;
  ActionListener* listener;

  string path;

  Label* heading;
  Label* currentDirectory;
  
  List* fileList;

  Label* fileNameHeading;
  TextField* fileNameEntry;
  
  Button* actionButton;
  Button* cancelButton;

  vector<string> fileNames;
};


#endif
