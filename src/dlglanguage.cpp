/***************************************************************************
                          dlglanguage.cpp  -  description
                             -------------------
   copyright            : (C) 2004 by Peter Hedlund
    email                : peter@peterandlinda.com
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
#include <klineedit.h>
#include <kiconloader.h>

#include <qlabel.h>

#include "configuration.h"
#include "dlglanguage.h"

DlgLanguage::DlgLanguage(QWidget *parent, const char *name, bool modal): KDialogBase(Swallow, i18n("Column Titles"), Ok|Cancel, Ok, parent, name, modal, true)
{
  dlgBase = new DlgLanguageBase( this, "Dlg" );
  setMainWidget(dlgBase);
  dlgBase -> picLanguage1 -> setPixmap(KGlobal::iconLoader()->loadIcon("rowcol", KIcon::Panel));
  dlgBase -> picLanguage2 -> setPixmap(KGlobal::iconLoader()->loadIcon("language2", KIcon::Panel));
  
  completion1 = new KCompletion();
  completion1->setItems(Config().m_columnTitles1);
  dlgBase -> txtLanguage1->setCompletionObject(completion1);

  completion2 = new KCompletion();
  completion2->setItems(Config().m_columnTitles2);
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
};


QString DlgLanguage::Language(int index){
  if (index == 1) {
    completion1->addItem(dlgBase->txtLanguage1 -> text());
    Config().m_columnTitles1 = completion1->items();
    return dlgBase->txtLanguage1 -> text();
  }
  else
  {
    completion2->addItem(dlgBase->txtLanguage2 -> text());
    Config().m_columnTitles2 = completion2->items();
    return dlgBase->txtLanguage2 -> text();
  }
}



#include "dlglanguage.moc"
