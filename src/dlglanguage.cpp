/***************************************************************************
                          dlglanguage.cpp  -  description
                             -------------------
   copyright       : (C) 2004-2005 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QLabel>

#include <klocale.h>
#include <klineedit.h>
#include <kiconloader.h>

#include "prefs.h"
#include "dlglanguage.h"

DlgLanguage::DlgLanguage(QWidget *parent, const char *name, bool modal): KDialogBase(parent, name, modal, i18n("Column Titles"), Ok|Cancel, Ok, true)
{
  dlgBase = new Ui::DlgLanguageBase();
  dlgBase->setupUi(makeMainWidget());

  dlgBase -> picLanguage1 -> setPixmap(KGlobal::iconLoader()->loadIcon("question", K3Icon::Panel));
  dlgBase -> picLanguage2 -> setPixmap(KGlobal::iconLoader()->loadIcon("answer", K3Icon::Panel));

  completion1 = new KCompletion();
  completion1->setItems(Prefs::columnTitles1());
  dlgBase -> txtLanguage1->setCompletionObject(completion1);

  completion2 = new KCompletion();
  completion2->setItems(Prefs::columnTitles2());
  dlgBase -> txtLanguage2->setCompletionObject(completion2);

  dlgBase->txtLanguage1->setFocus();
}


DlgLanguage::~DlgLanguage()
{
  delete completion1;
  delete completion2;
}


void DlgLanguage::setLanguage(int index, const QString &lang) {
  if (index == 1) {
    dlgBase->txtLanguage1 -> setText(lang);
  }
  else
  {
    dlgBase->txtLanguage2 -> setText(lang);
  }
}


QString DlgLanguage::Language(int index){
  if (index == 1) {
    completion1->addItem(dlgBase->txtLanguage1 -> text());
    Prefs::setColumnTitles1(completion1->items());
    return dlgBase->txtLanguage1 -> text();
  }
  else
  {
    completion2->addItem(dlgBase->txtLanguage2 -> text());
    Prefs::setColumnTitles2(completion2->items());
    return dlgBase->txtLanguage2 -> text();
  }
}



#include "dlglanguage.moc"
