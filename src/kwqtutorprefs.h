/***************************************************************************
                             kwqtutorprefs.h
                             -------------------

    copyright            : (C) 2010by Peter Hedlund
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

protected slots:
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
