/***************************************************************************
                          kwordquizprefs.cpp  -  description
                             -------------------
    begin                : Sun Aug 18 2002
    copyright            : (C) 2002-2005 by Peter Hedlund
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

#include <tqwidgetstack.h>
#include <tqlayout.h>
#include <tqradiobutton.h>
#include <tqcheckbox.h>
#include <tqlistview.h>
#include <tqlabel.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <krestrictedline.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kconfigskeleton.h>
#include <kcolorbutton.h>
#include <kfontrequester.h>

#include "kwordquizprefs.h"
#include "prefeditor.h"
#include "prefquiz.h"
#include "prefcharacter.h"
#include "prefcardappearance.h"
#include "kwordquiz.h"
#include "dlgspecchar.h"

KWordQuizPrefs::KWordQuizPrefs(TQWidget *parent, const char *name,  KConfigSkeleton *config, DialogType dialogType, int /*dialogButtons*/, ButtonCode /*defaultButton*/, bool /*modal*/)
  : KConfigDialog(parent, name, config, dialogType, Default|Ok|Apply|Cancel|Help, Ok, false)
{
  m_config = config;

  m_prefEditor = new PrefEditor(0, "Editor Settings");
  addPage(m_prefEditor, i18n("Editor"), "editor", i18n("Editor Settings"), true);

  m_prefQuiz = new PrefQuiz(0, "Quiz Settings");
  addPage(m_prefQuiz, i18n("Quiz"),  "qa", i18n("Quiz Settings"), true);

  m_prefCardAppearance = new PrefCardAppearance(0, "Flashcard Appearance");
  addPage(m_prefCardAppearance, i18n("Flashcard\nAppearance"), "flash", i18n("Flashcard Appearance Settings"), true);

  m_prefCharacter = new PrefCharacter(0, "Special Characters");
  addPage(m_prefCharacter, i18n("Special\nCharacters"), "kcharselect", i18n("Special Characters"), true);

  m_dlgSpecChar = 0L;

  connect(m_prefCharacter->lstCharacters, TQT_SIGNAL(selectionChanged()), this, TQT_SLOT(slotCharListSelectionChanged()));
  connect(m_prefCharacter->btnCharacter, TQT_SIGNAL(clicked()), this, TQT_SLOT(slotSelectSpecChar()));

  KWordQuizApp *win=(KWordQuizApp *) parent;
  int i=0;

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  TQString ds = item->property().toString();

  for ( TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    it.current()->setText(2, (TQString) ds[i++] ) ;
    it.current()->setText(1, win->actionCollection()->action(TQString("char_" + TQString::number(i)).latin1())->shortcut().toString());
  }

  m_prefCharacter->lstCharacters->setSelected(m_prefCharacter->lstCharacters->firstChild(), true);
  m_prefCharacter->lstCharacters->setItemMargin(2);

  kapp->processEvents();
}

void KWordQuizPrefs::slotCharListSelectionChanged()
{
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
}

void KWordQuizPrefs::slotSelectSpecChar( )
{
  KConfigSkeletonItem * item = m_config->findItem("EditorFont");
  TQString f = item->property().toFont().family();
  TQString s = m_prefCharacter->lstCharacters->currentItem()->text(2);
  TQChar c = s[0];

  if (m_dlgSpecChar == 0)
  {
    m_dlgSpecChar = new DlgSpecChar( this, "insert special char", f, c, true );
    connect(m_dlgSpecChar, TQT_SIGNAL(insertChar(TQChar)), this, TQT_SLOT(slotSpecChar(TQChar)));
    connect(m_dlgSpecChar, TQT_SIGNAL(finished()), this, TQT_SLOT(slotDlgSpecCharClosed()));
  }
  m_dlgSpecChar->show();
}

void KWordQuizPrefs::slotDlgSpecCharClosed()
{
  if ( m_dlgSpecChar )
  {
    disconnect(m_dlgSpecChar, TQT_SIGNAL(insertChar(TQChar)), this, TQT_SLOT(slotSpecChar(TQChar)));
    disconnect(m_dlgSpecChar, TQT_SIGNAL(finished()), this, TQT_SLOT(slotDlgSpecCharClosed()));
    m_dlgSpecChar->deleteLater();
    m_dlgSpecChar = 0L;
  }
}

void KWordQuizPrefs::slotSpecChar(TQChar c)
{
  m_prefCharacter->lstCharacters->currentItem()->setText(2, c);
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
  updateButtons();
}

bool KWordQuizPrefs::hasChanged()
{
  bool result;

  TQString s;
  for (TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  TQString ds = item->property().toString();

  if (ds == s.stripWhiteSpace())
    result = KConfigDialog::hasChanged();
  else
    result = true;

  return result;
}

bool KWordQuizPrefs::isDefault()
{
  bool bUseDefaults = m_config->useDefaults(true);
  bool result;

  TQString s;
  for (TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  TQString ds = item->property().toString();

  if (ds == s.stripWhiteSpace())
    result = KConfigDialog::isDefault();
  else
    result = false;

  m_config->useDefaults(bUseDefaults);
  return result;
}

void KWordQuizPrefs::updateSettings( )
{
  TQString s;
  for (TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  item->setProperty(TQVariant(s));

  emit settingsChanged();
}

void KWordQuizPrefs::updateWidgetsDefault( )
{
  bool bUseDefaults = m_config->useDefaults(true);

  TQString s;
  for (TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    s.append(it.current()->text(2));
  }

  KConfigSkeletonItem * item = m_config->findItem("SpecialCharacters");
  TQString ds = item->property().toString();

  int i=0;
  for (TQListViewItemIterator it = m_prefCharacter->lstCharacters; it.current(); ++it)
  {
    it.current()->setText(2, (TQString) ds[i++] ) ;
  }
  m_prefCharacter->lblPreview->setText(m_prefCharacter->lstCharacters->currentItem()->text(2));
  m_config->useDefaults(bUseDefaults);
}

#include "kwordquizprefs.moc"
