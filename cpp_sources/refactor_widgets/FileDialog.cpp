//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "FileDialog.hpp"
#include "game/Mission.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/List.hpp"
#include "guilib/Label.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/TextField.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> FileDialog*
Singleton<FileDialog>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_FILE_DIALOG_SIZE = 512;

FileDialog::FileDialog()
  : Dialog("FileDialog"), listener(NULL), path("database")
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_FILE_DIALOG_SIZE, MAGIC_FILE_DIALOG_SIZE);
  centerOnScreen();

  heading = new Label(L"Select file");
  currentDirectory = new Label(stringToWString(path));
  
  fileList = new List(1, this);
  fileList->setBackground(new FrameBackground());
  
  fileNameHeading = new Label(L"File Name:");
  fileNameEntry = new TextField(40);

  actionButton = new Button(L"Load", this);
  cancelButton = new Button(L"Cancel", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(heading, 0, 0, 32, 1);
  gridLayout->add(currentDirectory, 0, 1, 32, 1);

  gridLayout->add(fileList, 0, 2, 32, 20);

  gridLayout->add(fileNameHeading, 0, 22, 28, 1);
  gridLayout->add(fileNameEntry, 0, 23, 28, 1);

  gridLayout->add(actionButton, 28, 22, 4, 1);
  gridLayout->add(cancelButton, 28, 23, 4, 1);

  layout();
}

//--------------------------------------------------------------------------------

void
FileDialog::show(ActionListener* _listener, wstring _title, wstring _actionTitle)
{
  mLuaFunction = "";
  listener = _listener;
  heading->setLabel(_title);
  actionButton->setLabel(_actionTitle);
  fileNameEntry->setLabel(L"");
  fileNames.clear();

  fileList->removeAllItems();
  FileSystemArchive* fileArchive = static_cast<FileSystemArchive*>(ArchiveManager::getSingleton().load(path, "FileSystem"));
  StringVectorPtr files = fileArchive->list(false);
  for(StringVector::const_iterator fileNameI = files->begin(); fileNameI != files->end(); ++fileNameI) {
    string fileName = *fileNameI;    
    fileList->addItem(stringToWString(fileName));
    fileNames.push_back(fileName);
  }
  rGUIManager()->pushDialog(this);
}

void
FileDialog::show(wstring _title, wstring _actionTitle, string luaFunction)
{
  mLuaFunction = luaFunction;
  listener = NULL;
  
  heading->setLabel(_title);
  actionButton->setLabel(_actionTitle);
  fileNameEntry->setLabel(L"");
  fileNames.clear();
  
  fileList->removeAllItems();
  FileSystemArchive* fileArchive = static_cast<FileSystemArchive*>(ArchiveManager::getSingleton().load(path, "FileSystem"));
  StringVectorPtr files = fileArchive->list(false);
  for(StringVector::const_iterator fileNameI = files->begin(); fileNameI != files->end(); ++fileNameI) {
    string fileName = *fileNameI;    
    fileList->addItem(stringToWString(fileName));
    fileNames.push_back(fileName);
  }
  rGUIManager()->pushDialog(this);
  
}



//--------------------------------------------------------------------------------

void
FileDialog::actionPerformed(Widget* source)
{
  if(source == cancelButton) {
    rGUIManager()->popAllDialogs();
  }  
  else if(source == actionButton) {
    fireAction();
  }
}

void
FileDialog::itemSelected(Widget*, unsigned num)
{
  fileNameEntry->setLabel(fileList->getItem(num));
}

void
FileDialog::itemActivated(Widget* source, unsigned num)
{
  itemSelected(source, num);
  fireAction();
}

//--------------------------------------------------------------------------------

void
FileDialog::fireAction()
{
  if(listener != NULL) {
    listener->actionPerformed(this);
  }
  listener = NULL;

  if(mLuaFunction != "") {
    ostringstream ossLuaCall;
    ossLuaCall << mLuaFunction << "(\"" << wstringToString(getFileName()) << "\")";
    rMain()->getMission()->doString(ossLuaCall.str());
  }
  
  rGUIManager()->popAllDialogs();
}


wstring
FileDialog::getFileName()
{
  return stringToWString(path) + L"/" + fileNameEntry->getLabel();
}


