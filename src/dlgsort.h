/***************************************************************************
                          dlglsort.h  -  description
                             -------------------
   copyright            : (C) 2003 by Peter Hedlund
   email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
