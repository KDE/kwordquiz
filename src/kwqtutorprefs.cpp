/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kwqtutorprefs.h"

#include <QPushButton>

#include <KConfigSkeleton>
#include <KLocalizedString>

#include "preftutor.h"

KWQTutorPrefs::KWQTutorPrefs(QWidget *parent, const QString &name, KConfigSkeleton *config,
			     KActionCollection *actionCollection)
  : KConfigDialog(parent, name, config)
{
  m_config = config;

  m_prefTutor = new PrefTutor(0);
  addPage(m_prefTutor, i18nc("@title:group tutor settings", "Tutor"), QStringLiteral("kwordquiz"), i18nc("@title:group tutor settings", "Tutor Settings"), true);

  m_shortcutsWidget = new KShortcutsEditor( actionCollection, 0, KShortcutsEditor::GlobalAction );
  addPage(m_shortcutsWidget, i18nc("Shortcuts Config", "Shortcuts"), QStringLiteral("configure-shortcuts"), i18n("Shortcuts Settings"));

  connect(m_shortcutsWidget, &KShortcutsEditor::keyChange, this, &KWQTutorPrefs::slotEnableApplyButton);

  setHelp(QString(), QStringLiteral("kwordquiz"));
}

bool KWQTutorPrefs::hasChanged()
{
  return m_shortcutsWidget->isModified();
}

void KWQTutorPrefs::updateSettings( )
{
  m_shortcutsWidget->save();
}

void KWQTutorPrefs::updateWidgetsDefault()
{
  bool bUseDefaults = m_config->useDefaults(true);
  m_shortcutsWidget->allDefault();
  m_config->useDefaults(bUseDefaults);
}

void KWQTutorPrefs::slotEnableApplyButton()
{
  buttonBox()->button(QDialogButtonBox::Apply)->setEnabled(true);
}
