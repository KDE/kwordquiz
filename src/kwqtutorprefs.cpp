/***************************************************************************
                              kwqtutorprefs.cpp
                             -------------------

    copyright            : (C) 2010 by Peter Hedlund
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

#include "kwqtutorprefs.h"

#include <QPushButton>

#include <KLocalizedString>
#include <KConfigSkeleton>
#include <KConfigGroup>

#include "preftutor.h"

KWQTutorPrefs::KWQTutorPrefs(QWidget *parent, const QString &name, KConfigSkeleton *config,
			     KActionCollection *actionCollection)
  : KConfigDialog(parent, name, config)
{
  m_config = config;

  m_prefTutor = new PrefTutor(0);
  addPage(m_prefTutor, i18nc("@title:group tutor settings", "Tutor"), "kwordquiz", i18nc("@title:group tutor settings", "Tutor Settings"), true);

  m_shortcutsWidget = new KShortcutsEditor( actionCollection, 0, KShortcutsEditor::GlobalAction );
  addPage(m_shortcutsWidget, i18nc("Shortcuts Config", "Shortcuts"), "configure-shortcuts", i18n("Shortcuts Settings"));

  connect(m_shortcutsWidget, SIGNAL(keyChange()), this, SLOT(slotEnableApplyButton()));

  setHelp(QString(), "kwordquiz");
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
