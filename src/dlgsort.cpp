//
// C++ Implementation: dlgsort
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#include <klocale.h>

#include <qradiobutton.h>
#include <qlayout.h>

#include "dlgsort.h"


DlgSort::DlgSort(QWidget *parent, const char *name, bool modal): KDialogBase(Plain, i18n("Sort"), Ok|Cancel, Ok, parent, name, modal, true)
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
