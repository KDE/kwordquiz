/***************************************************************************
                          dlglsort.cpp  -  description
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

#include <klocale.h>

#include <qradiobutton.h>

#include "dlgsort.h"


DlgSort::DlgSort(QWidget *parent, const char *name, bool modal): KDialogBase(Swallow, i18n("Sort"), Ok|Cancel, Ok, parent, name, modal, true)
{
  dlgBase = new DlgSortBase( this, "Dlg" );
  setMainWidget(dlgBase);

  dlgBase->optLang1->setChecked(true);
  dlgBase->optAscending->setChecked(true);
  dlgBase->optLang1->setFocus();
}

DlgSort::~DlgSort()
{
}

bool DlgSort::base( )
{
  return dlgBase->optLang1->isChecked();
}

bool DlgSort::ascending( )
{
  return dlgBase->optAscending->isChecked();
}

void DlgSort::setLanguage( int index, const QString & lang )
{
  if (index == 1) {
    dlgBase->optLang1 -> setText("&1 " + lang);
  }
  else
  {
    dlgBase->optLang2 -> setText("&2 " + lang);
  }
}


#include "dlgsort.moc"
