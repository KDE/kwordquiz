/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWQTUTORPREFS_H
#define KWQTUTORPREFS_H

#include <KActionCollection>
#include <KConfigDialog>
#include <KShortcutsEditor>

class KConfigSkeleton;

/**
  *@author Peter Hedlund
*/

class PrefTutor;

class KWQTutorPrefs : public KConfigDialog
{
  Q_OBJECT
public:
  KWQTutorPrefs(QWidget *parent, const QString & name, KConfigSkeleton *config,
		KActionCollection *actionCollection);

protected Q_SLOTS:
  void updateSettings() override;
  void updateWidgetsDefault() override;
  void slotEnableApplyButton();

protected:
  bool hasChanged() override;

private:
  PrefTutor *m_prefTutor;
  KConfigSkeleton * m_config;
  KShortcutsEditor* m_shortcutsWidget;
};

#endif
