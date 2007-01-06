/***************************************************************************
                          dlglanguage.cpp  -  description
                             -------------------
   copyright       : (C) 2004-2006 Peter Hedlund <peter.hedlund@kdemail.net>

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
#include <kapplication.h>

#include "prefs.h"
#include "dlglanguage.h"

DlgLanguage::DlgLanguage(QWidget *parent): KDialog(parent)
{
  setCaption(i18n("Column Titles"));
  setButtons(Ok|Cancel);

  QWidget * w = new QWidget();
  setMainWidget(w);

  dlgBase = new Ui::DlgLanguageBase();
  dlgBase->setupUi(mainWidget());

  dlgBase -> picLanguage1 -> setPixmap(kapp->iconLoader()->loadIcon("question", K3Icon::Panel));
  dlgBase -> picLanguage2 -> setPixmap(kapp->iconLoader()->loadIcon("answer", K3Icon::Panel));

  dlgBase->txtLanguage1->completionObject(true)->setItems(Prefs::columnTitles1());
  dlgBase->txtLanguage1->setCompletionMode(KGlobalSettings::CompletionAuto);

  dlgBase->txtLanguage2->completionObject(true)->setItems(Prefs::columnTitles2());
  dlgBase->txtLanguage2->setCompletionMode(KGlobalSettings::CompletionAuto);

  dlgBase->txtLanguage1->setFocus();
}

void DlgLanguage::setLanguage(int index, const QString &lang) {
  if (index == 1) {
    dlgBase->txtLanguage1->setText(lang);
  }
  else
  {
    dlgBase->txtLanguage2->setText(lang);
  }
}

QString DlgLanguage::Language(int index){
  if (index == 1) {
    dlgBase->txtLanguage1->completionObject(true)->addItem(dlgBase->txtLanguage1 -> text());
    Prefs::setColumnTitles1(dlgBase->txtLanguage1->completionObject(true)->items());
    return dlgBase->txtLanguage1->text();
  }
  else
  {
    dlgBase->txtLanguage2->completionObject(true)->addItem(dlgBase->txtLanguage2 -> text());
    Prefs::setColumnTitles2(dlgBase->txtLanguage2->completionObject(true)->items());
    return dlgBase->txtLanguage2->text();
  }
}

#include "dlglanguage.moc"
