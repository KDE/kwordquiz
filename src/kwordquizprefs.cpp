/*
    SPDX-FileCopyrightText: 2002-2008 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kwordquizprefs.h"

#include <QCheckBox>

#include <KConfigSkeleton>
#include <KLocalizedString>

#include "prefgeneral.h"
#include "prefeditor.h"
#include "prefquiz.h"
#include "prefcharacter.h"
#include "prefcardappearance.h"

KWordQuizPrefs::KWordQuizPrefs(QWidget *parent, const QString &name, KConfigSkeleton *config, KActionCollection *actionCollection) : KConfigDialog(parent, name, config)
{
  m_config = config;

  m_prefGeneral = new PrefGeneral(0);
  addPage(m_prefGeneral, i18nc("@title:group general settings", "General"), QStringLiteral("kwordquiz"), i18nc("@title:group general settings", "General Settings"), true);

  m_prefEditor = new PrefEditor(0);
  addPage(m_prefEditor, i18nc("@title:group editor settings", "Editor"), QStringLiteral("editor"), i18nc("@title:group editor settings", "Editor Settings"), true);

  m_prefQuiz = new PrefQuiz(0);
  addPage(m_prefQuiz, i18nc("@title:group quiz settings", "Quiz"), QStringLiteral("qa"), i18nc("@title:group quiz settings", "Quiz Settings"), true);

  m_prefCardAppearance = new PrefCardAppearance(0);
  addPage(m_prefCardAppearance, i18nc("@title:group flash appearance settings", "Flashcard Appearance"), QStringLiteral("flash"), i18nc("@title:group flash appearance settings", "Flashcard Appearance Settings"), true);

  m_prefCharacter = new PrefCharacter(0, actionCollection);
  addPage(m_prefCharacter, i18nc("@title:group special character settings", "Special Characters"), QStringLiteral("accessories-character-map"), i18nc("@title:group special character settings", "Special Characters"), true);
  setHelp(QString(), QStringLiteral("kwordquiz"));
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
  Q_EMIT settingsChanged(QLatin1String(""));
}

void KWordQuizPrefs::updateWidgetsDefault()
{
  bool bUseDefaults = m_config->useDefaults(true);
  m_prefCharacter->updateWidgets();
  m_config->useDefaults(bUseDefaults);
}
