//
// C++ Implementation: dlglanguage
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <klocale.h>
#include <klineedit.h>
#include <kstandarddirs.h> //locate

#include <qlabel.h>

#include "configuration.h"
#include "dlglanguage.h"
#include "dlglanguagebase.h"

DlgLanguage::DlgLanguage(QWidget *parent, const char *name, bool modal): KDialogBase(Plain, i18n("Column Titles"), Ok|Cancel, Ok, parent, name, modal, true)
{
  dlgBase = new DlgLanguageBase( this, "Dlg" );
  setMainWidget(dlgBase);
  dlgBase -> picLanguage1 -> setPixmap(locate("data", "kwordquiz/pics/lang1.png"));
  dlgBase -> picLanguage2 -> setPixmap(locate("data", "kwordquiz/pics/lang2.png"));
  
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
