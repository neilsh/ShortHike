//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "MessageReader.hpp"
#include "game/Mission.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/List.hpp"
#include "guilib/Label.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> MessageReader*
Singleton<MessageReader>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_MESSAGE_READER_WIDTH = 650;
const int MAGIC_MESSAGE_READER_HEIGHT = 600;

MessageReader::MessageReader()
  : Dialog("MessageReader"), mMessageList(NULL), mMessagePane(NULL), mCloseButton(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_MESSAGE_READER_WIDTH, MAGIC_MESSAGE_READER_HEIGHT);
  centerOnScreen();

  mMessageList = new List(3, this, true);
  mMessageList->setBackground(new FrameBackground());
  mMessageList->setColumnWidth(0, 0.15);
  mMessageList->setColumnWidth(1, 0.30);
  mMessageList->setColumnWidth(2, 0.55);
  
  mMessagePane = new Label();
  mMessagePane->setBackground(new FrameBackground());

  mCloseButton = new Button(L"Close", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(new Label(L"Messages"), 0, 0, 26, 1);
  gridLayout->add(mCloseButton, 26, 0, 6, 1);
  gridLayout->add(mMessageList, 0, 1, 32, 11);
  gridLayout->add(mMessagePane, 0, 12, 32, 12);
  layout();
}


void
MessageReader::selectNewestMessage()
{
  Mission* mission = rMain()->getMission();
  const MessageList& messages = mission->getMessages();
  if(messages.size() > 0) {
    mMessagePane->setLabel(messages[messages.size() - 1].mBody);
    mMessageList->setSelected(messages.size());
  }
  update(0);
}


//--------------------------------------------------------------------------------

void
MessageReader::update(float)
{
  Mission* mission = rMain()->getMission();
  const MessageList& messages = mission->getMessages();
  if(static_cast<int>(messages.size() + 1) == mMessageList->size()) return;

  unsigned oldIndex = mMessageList->getSelected();

  mMessageList->removeAllItems();

  vector<wstring> headingLine;
  headingLine.push_back(L"Date");
  headingLine.push_back(L"From");
  headingLine.push_back(L"Subject");
  mMessageList->addItem(headingLine, false);

  for(MessageList::const_iterator iMessage = messages.begin(); iMessage != messages.end(); ++iMessage) {
    const Message& message = *iMessage;
    vector<wstring> newLine;
    // FIXME: Message times
//     newLine.push_back(message.mTime.getSimulationDateString());
    newLine.push_back(L"fix date");
    newLine.push_back(message.mFrom);
    newLine.push_back(message.mSubject);
    mMessageList->addItem(newLine);
  }

  mMessageList->setSelected(oldIndex);
  if(0 != oldIndex && (oldIndex - 1) < messages.size())
    mMessagePane->setLabel(messages[oldIndex - 1].mBody);
  else
    mMessagePane->setLabel(L"");
}


//--------------------------------------------------------------------------------


void
MessageReader::actionPerformed(Widget* source)
{
  if(source == mCloseButton) {
    hide();
  }
}


void
MessageReader::itemSelected(Widget*, unsigned num)
{
  Mission* mission = rMain()->getMission();
  const MessageList& messages = mission->getMessages();
  mMessagePane->setLabel(messages[num - 1].mBody);
  mMessageList->setSelected(num);
}

