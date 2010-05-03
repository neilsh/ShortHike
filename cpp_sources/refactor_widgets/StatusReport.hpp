//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_STATUS_REPORT_HPP
#define WIDGETS_STATUS_REPORT_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Dialog.hpp"
#include "guilib/List.hpp"

class List;
class Label;
class Button;

class StatusReport : public Dialog, public ActionListener, public ListListener, public Singleton<StatusReport>
{
public:
  StatusReport();

  virtual void show();
  
  void clear();
  void pushObjective(wstring shortDescription, wstring longDescription, wstring progress, float progressPercentage, bool primary = false);

  virtual void actionPerformed(Widget* source);
  virtual void itemSelected(Widget* source, unsigned num);
private:
  struct Objective
  {
    wstring shortDescription;
    wstring longDescription;
    wstring progress;
    float progressPercentage;
  };
  
  List* mPrimaryObjectivesList;
  List* mSecondaryObjectivesList;
  Label* mDescription;
  Button* mCloseButton;

  vector<Objective> mPrimaryObjectives;
  vector<Objective> mSecondaryObjectives;
};



#endif
