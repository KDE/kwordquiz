/***************************************************************************
                          kwordquizprefs.cpp  -  description
                             -------------------
    begin                : Sun Aug 18 2002
    copyright            : (C) 2002-2003 by Peter Hedlund
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
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <krestrictedline.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include "kwordquizprefs.h"
#include "prefeditor.h"
#include "prefquiz.h"
#include "configuration.h"

KWordQuizPrefs::KWordQuizPrefs(QWidget *parent, const char *name, bool modal) : KDialogBase(IconList, i18n("Configure"), Default|Ok|Apply|Cancel, Ok, parent, name, modal, true) {

  QFrame* fraEditor = addPage( i18n("Editor"), i18n("Editor Settings"), locate("data", "kwordquiz/pics/lang1.png"));
  QVBoxLayout *frameLayout = new QVBoxLayout( fraEditor, 0, 0 );
  m_prefEditor = new PrefEditor(fraEditor);
  frameLayout->addWidget(m_prefEditor);  
    
  QFrame* fraQuiz = addPage( i18n("Quiz"), i18n("Quiz Settings"), locate("data", "kwordquiz/pics/pref_flash.png"));
  frameLayout = new QVBoxLayout( fraQuiz, 0, 0 );
  m_prefQuiz = new PrefQuiz(fraQuiz);
  frameLayout->addWidget(m_prefQuiz);

  // connect interactive widgets and selfmade signals to the enableApply slotDefault
  connect(m_prefEditor->optDown, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefEditor->optRight, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefEditor->optNoMove, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefEditor->chkEnableBlanks, SIGNAL(clicked()), this, SLOT(enableApply()));

  connect(m_prefQuiz->chkAutoFlip, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefQuiz->txtSeconds, SIGNAL(textChanged(const QString&)), this, SLOT(enableApply()));
  connect(m_prefQuiz->optCorrect, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefQuiz->optError, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefQuiz->chkAutoCheck, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefQuiz->chkHintError, SIGNAL(clicked()), this, SLOT(enableApply()));
  connect(m_prefQuiz->chkPercent, SIGNAL(clicked()), this, SLOT(enableApply()));
}

KWordQuizPrefs::~KWordQuizPrefs(){
}


/** Read settings */
void KWordQuizPrefs::updateDialog()
{
  m_prefEditor->optDown->setChecked(Config().m_enterMove == 1);
  m_prefEditor->optRight->setChecked(Config().m_enterMove == 2);
  m_prefEditor->optNoMove->setChecked(Config().m_enterMove == 3);
  m_prefEditor->chkEnableBlanks->setChecked(Config().m_enableBlanks);

  m_prefQuiz->chkAutoFlip->setChecked(Config().m_autoFlip);
  QString s;
  s.setNum(Config().m_flipDelay);
  m_prefQuiz->txtSeconds->setText(s);
  m_prefQuiz->optError->setChecked(Config().m_keepDiscard);
  m_prefQuiz->optCorrect->setChecked(!Config().m_keepDiscard);
  m_prefQuiz->txtSeconds -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());
  m_prefQuiz->optCorrect -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());
  m_prefQuiz->optError -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());

  m_prefQuiz->chkAutoCheck->setChecked(Config().m_autoCheck);

  m_prefQuiz->chkHintError->setChecked(Config().m_hintError);

  m_prefQuiz->chkPercent->setChecked(Config().m_percent);
  enableButtonApply(false);   // disable apply button

}

/** Write settings */
void KWordQuizPrefs::updateConfiguration()
{
  if (m_prefEditor->optDown->isChecked())
    Config().m_enterMove = 1;
  if (m_prefEditor->optRight->isChecked())
    Config().m_enterMove = 2;
  if (m_prefEditor->optNoMove->isChecked())
    Config().m_enterMove = 3;    
  Config().m_enableBlanks = m_prefEditor->chkEnableBlanks->isChecked();

  Config().m_autoFlip = m_prefQuiz->chkAutoFlip->isChecked();
  QString s = m_prefQuiz->txtSeconds->text();
  Config().m_flipDelay = s.toInt();
  Config().m_keepDiscard = m_prefQuiz->optError->isChecked();

  Config().m_autoCheck = m_prefQuiz->chkAutoCheck->isChecked();

  Config().m_hintError = m_prefQuiz->chkHintError->isChecked();

  Config().m_percent = m_prefQuiz->chkPercent->isChecked();
  enableButtonApply(false);   // disable apply button
}

void KWordQuizPrefs::slotDefault() {
  if (KMessageBox::warningContinueCancel(this, i18n("This will set the default options "
    "in ALL pages of the preferences dialog! Continue?"), i18n("Set default options?"),
    i18n("Set defaults"))==KMessageBox::Continue)
  {
    m_prefEditor->optDown->setChecked(Config().m_defaultEnterMove == 1);
    m_prefEditor->optRight->setChecked(Config().m_defaultEnterMove == 2);
    m_prefEditor->optNoMove->setChecked(Config().m_defaultEnterMove == 3);
    m_prefEditor->chkEnableBlanks->setChecked(Config().m_defaultEnableBlanks);

    m_prefQuiz->chkAutoFlip->setChecked(Config().m_defaultAutoFlip);
    QString s;
    s.setNum(Config().m_defaultFlipDelay);
    m_prefQuiz->txtSeconds->setText(s);
    m_prefQuiz->optError->setChecked(Config().m_defaultKeepDiscard);
    m_prefQuiz->optCorrect->setChecked(!Config().m_defaultKeepDiscard);
    m_prefQuiz->txtSeconds -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());
    m_prefQuiz->optCorrect -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());
    m_prefQuiz->optError -> setEnabled(m_prefQuiz->chkAutoFlip->isChecked());

    m_prefQuiz->chkAutoCheck->setChecked(Config().m_defaultAutoCheck);

    m_prefQuiz->chkHintError->setChecked(Config().m_defaultHintError);

    m_prefQuiz->chkPercent->setChecked(Config().m_defaultPercent);
    enableApply();   // enable apply button
  }
};

void KWordQuizPrefs::slotApply() {
    updateConfiguration();      // transfer settings to configuration object
    emit settingsChanged();     // apply the preferences
    enableButtonApply(false);   // disable apply button again
};


void KWordQuizPrefs::enableApply() {
    enableButtonApply(true);   // enable apply button
};

