//
// C++ Interface: dlgsort
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#ifndef DLGSORT_H
#define DLGSORT_H

#include <kdialogbase.h>

#include "dlgsortbase.h"

/**
@author Peter Hedlund
*/
class DlgSort : public KDialogBase
{
Q_OBJECT
public:

  DlgSort(QWidget *parent = 0, const char *name = 0, bool modal=true);
  ~DlgSort();
  
  bool base();
  bool ascending();
  void setLanguage(int index, const QString &lang);

private:
  DlgSortBase *dlgBase;

};

#endif
