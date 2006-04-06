/***************************************************************************
                          dlgsort.cpp  -  description
                             -------------------
   copyright       : (C) 2003-2006 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QRadioButton>

#include <klocale.h>

#include "dlgsort.h"

DlgSort::DlgSort(QWidget *parent): KDialog(parent, i18n("Sort"), Ok|Cancel)
{
  QWidget * w = new QWidget();
  setMainWidget(w);
  dlgBase = new Ui::DlgSortBase();
  dlgBase->setupUi(mainWidget());
  dlgBase->optLang1->setChecked(true);
  dlgBase->optAscending->setChecked(true);
  dlgBase->optLang1->setFocus();
}

bool DlgSort::base()
{
  return dlgBase->optLang1->isChecked();
}

bool DlgSort::ascending()
{
  return dlgBase->optAscending->isChecked();
}

void DlgSort::setLanguage(int index, const QString & lang)
{
  if (index == 1)
    dlgBase->optLang1 -> setText("&1 " + lang);
  else
    dlgBase->optLang2 -> setText("&2 " + lang);
}


#include "dlgsort.moc"
