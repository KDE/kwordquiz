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
#include <kapplication.h>
#include <kpushbutton.h>

#include <qlayout.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qlabel.h>

#include "kwordquizprefs.h"
#include "prefeditor.h"
#include "prefquiz.h"
#include "prefcharacter.h"
#include "configuration.h"
#include "kwordquiz.h"
#include "dlgspecchar.h"

KWordQuizPrefs::KWordQuizPrefs(QWidget *parent, const char *name, bool modal) : KDialogBase(IconList, i18n("Configure"), Default|Ok|Apply|Cancel, Ok, parent, name, modal, true) {

  QFrame* fraEditor = addPage( i18n("Editor"), i18n("Editor Settings"), locate("data", "kwordquiz/pics/lang1.png"));
  QVBoxLayout *frameLayout = new QVBoxLayout( fraEditor, 0, 0 );
  m_prefEditor = new PrefEditor(fraEditor);
  frameLayout->addWidget(m_prefEditor);  
    
  QFrame* fraQuiz = addPage( i18n("Quiz"), i18n("Quiz Settings"), locate("data", "kwordquiz/pics/pref_flash.png"));
  frameLayout = new QVBoxLayout( fraQuiz, 0, 0 );
  m_prefQuiz = new PrefQuiz(fraQuiz);
  frameLayout->addWidget(m_prefQuiz);

  QFrame* fraCharacter = addPage( i18n("Special\nCharacters"), i18n("Special Characters"), DesktopIcon("kcharselect", 32));
  frameLayout = new QVBoxLayout( fraCharacter, 0, 0 );
  m_prefCharacter = new PrefCharacter(fraCharacter);
  frameLayout->addWidget(m_prefCharacter);  
  
  m_dlgSpecChar = 0L;
  
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
  
  connect(m_prefCharacter->lstCharacters, SIGNAL(selectionChanged()), this, SLOT(slotCharListSelectionChanged()));
  connect(m_prefCharacter->btnCharacter, SIGNAL(clicked()), this, SLOT(slotSelectSpecChar()));
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
  
  KWordQuizApp *win=(KWordQuizApp *) parent();
  int i=0;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    it.current()->setText(2, (QString) Config().m_specialCharacters[i++] ) ;
    it.current()->setText(1, win->actionCollection()->action(QString("char_" + QString::number(i)).latin1())->shortcut().toString());
  }
  
  m_prefCharacter->lstCharacters->setSelected(m_prefCharacter->lstCharacters->firstChild(), true);
  m_prefCharacter->lstCharacters->setItemMargin(2);
  
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
  
  int i=0;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    Config().m_specialCharacters.replace(i++, 1, it.current()->text(2));
  }
  
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
    
    int i = 0;
    for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
    {
      it.current()->setText(2, (QString) Config().m_defaultSpecialCharacters[i++] ) ;
    }
      
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

void KWordQuizPrefs::slotCharListSelectionChanged( )
{
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
}

void KWordQuizPrefs::slotSelectSpecChar( )
{
  QString f = Config().m_editorFont.family();
  QString s = m_prefCharacter->lstCharacters->currentItem()->text(2);
  QChar c = s[0];
  
  if (m_dlgSpecChar == 0)
  {
    m_dlgSpecChar = new DlgSpecChar( this, "insert special char", f, c, true );
    connect( m_dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    connect( m_dlgSpecChar, SIGNAL(finished()), this, SLOT( slotDlgSpecCharClosed() ) );
  }
  m_dlgSpecChar->show();
}

void KWordQuizPrefs::slotDlgSpecCharClosed( )
{
  if ( m_dlgSpecChar )
  {
    disconnect( m_dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    disconnect( m_dlgSpecChar, SIGNAL( finished() ), this, SLOT( slotDlgSpecCharClosed() ) );
    m_dlgSpecChar->deleteLater();
    m_dlgSpecChar = 0L;
  }
}

void KWordQuizPrefs::slotSpecChar(QChar c)
{
  m_prefCharacter->lstCharacters->currentItem()->setText(2, c);
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));  
  enableButtonApply(true);
}
