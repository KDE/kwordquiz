/***************************************************************************
                          kwordquizprefs.cpp  -  description
                             -------------------
    begin                : Sun Aug 18 2002
    copyright            : (C) 2002-2007 by Peter Hedlund
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

#include "kwordquizprefs.h"

#include <QLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>

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

#include "prefeditor.h"
#include "prefquiz.h"
#include "prefcharacter.h"
#include "prefcardappearance.h"
#include "kwordquiz.h"

KWordQuizPrefs::KWordQuizPrefs(QWidget *parent, const QString &name, KConfigSkeleton *config) : KConfigDialog(parent, name, config)
{
  m_config = config;

  m_prefEditor = new PrefEditor(0);
  addPage(m_prefEditor, i18nc("@title:group editor settings", "Editor"), "editor", i18n("Editor Settings"), true);

  m_prefQuiz = new PrefQuiz(0);
  addPage(m_prefQuiz, i18nc("@title:group quiz settings", "Quiz"), "qa", i18n("Quiz Settings"), true);

  m_prefCardAppearance = new PrefCardAppearance(0);
  addPage(m_prefCardAppearance, i18nc("@title:group flash appearance settings", "Flashcard\nAppearance"), "flash", i18n("Flashcard Appearance Settings"), true);

  KWordQuizApp *win = KWordQuizApp::self();
  m_prefCharacter = new PrefCharacter(0, win->actionCollection());
  addPage(m_prefCharacter, i18nc("@title:group special character settings", "Special\nCharacters"), "accessories-character-map", i18n("Special Characters"), true);
}

bool KWordQuizPrefs::hasChanged()
{
  return m_prefCharacter->hasChanged();
}

bool KWordQuizPrefs::isDefault()
{
  return m_prefCharacter->isDefault();
}

void KWordQuizPrefs::updateSettings( )
{
  m_prefCharacter->updateSettings();
  emit settingsChanged("");
}

void KWordQuizPrefs::updateWidgetsDefault()
{
  bool bUseDefaults = m_config->useDefaults(true);
  m_prefCharacter->updateWidgets();
  m_config->useDefaults(bUseDefaults);
}

#include "kwordquizprefs.moc"
