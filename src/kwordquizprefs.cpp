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
#include <krestrictedline.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kconfigskeleton.h>

#include <qlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qlabel.h>

#include "kwordquizprefs.h"
#include "prefeditor.h"
#include "prefquiz.h"
#include "prefcharacter.h"
#include "kwordquiz.h"
#include "dlgspecchar.h"

KWordQuizPrefs::KWordQuizPrefs(QWidget *parent, const char *name,  KConfigSkeleton *config, DialogType dialogType, int dialogButtons, ButtonCode defaultButton, bool modal) 
  : KConfigDialog(parent, name, config, dialogType, Default|Ok|Apply|Cancel|Help, Ok, false) 
{
  m_config = config;

  m_prefEditor = new PrefEditor(0, "Editor Settings");
  addPage(m_prefEditor, i18n("Editor"), "editor", i18n("Editor Settings"), true);

  m_prefQuiz = new PrefQuiz(0, "Quiz Settings");
  addPage(m_prefQuiz, i18n("Quiz"),  "flash", i18n("Quiz Settings"), true);

  m_prefCharacter = new PrefCharacter(0, "Special Characters");
  addPage(m_prefCharacter, i18n("Special\nCharacters"), "kcharselect", i18n("Special Characters"), true);

  m_dlgSpecChar = 0L;

  connect(m_prefCharacter->lstCharacters, SIGNAL(selectionChanged()), this, SLOT(slotCharListSelectionChanged()));
  connect(m_prefCharacter->btnCharacter, SIGNAL(clicked()), this, SLOT(slotSelectSpecChar()));

  KWordQuizApp *win=(KWordQuizApp *) parent;
  int i=0;

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  QString ds = item->property().toString();

  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    it.current()->setText(2, (QString) ds[i++] ) ;
    it.current()->setText(1, win->actionCollection()->action(QString("char_" + QString::number(i)).latin1())->shortcut().toString());
  }

  m_prefCharacter->lstCharacters->setSelected(m_prefCharacter->lstCharacters->firstChild(), true);
  m_prefCharacter->lstCharacters->setItemMargin(2);

  m_hasChanged = false;
}

KWordQuizPrefs::~KWordQuizPrefs(){
}

void KWordQuizPrefs::slotCharListSelectionChanged( )
{
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
}

void KWordQuizPrefs::slotSelectSpecChar( )
{
  KConfigSkeletonItem * item = m_config->findItem("EditorFont");
  QString f = item->property().toFont().family();
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
  m_hasChanged = true;
  updateButtons();
}

bool KWordQuizPrefs::hasChanged( )
{
  kdDebug() << "hasChanged" << endl;
  bool result = false;
  if (m_hasChanged)
    result = true;
  else
    result = KConfigDialog::hasChanged();
  kdDebug() << result << endl;
  return result;
}

bool KWordQuizPrefs::isDefault( )
{
  kdDebug() << "isDefault" << endl;
  bool bUseDefaults = m_config->useDefaults(true);
  bool result /*= !hasChanged()*/;

  QString s;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  QString ds = item->property().toString();

  if (ds == s.stripWhiteSpace())
    result = KConfigDialog::isDefault();
  else
    result = false;

  m_config->useDefaults(bUseDefaults);
  return result;
}

void KWordQuizPrefs::updateSettings( )
{
  kdDebug() << "updateSettings" << endl;
  QString s;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  item->setProperty(QVariant(s));

  emit settingsChanged();
  m_hasChanged = false;
}

void KWordQuizPrefs::updateWidgetsDefault( )
{
  bool bUseDefaults = m_config->useDefaults(true);

  QString s;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  QString ds = item->property().toString();

  int i=0;
  for ( QListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    it.current()->setText(2, (QString) ds[i++] ) ;
  }
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
  m_hasChanged = (ds != s);
  m_config->useDefaults(bUseDefaults);
}

#include "kwordquizprefs.moc"
