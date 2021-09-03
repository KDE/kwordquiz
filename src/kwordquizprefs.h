/*
    kwordquizprefs.h  -  description
    -------------------
    begin                : Sun Aug 18 2002
    SPDX-FileCopyrightText: 2002-2008 Peter Hedlund
    email                : peter.hedlund@kdemail.net
*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

#ifndef KWORDQUIZPREFS_H
#define KWORDQUIZPREFS_H

#include <KConfigDialog>
#include <KActionCollection>

/**
  *@author Peter Hedlund
*/

class PrefGeneral;
class PrefEditor;
class PrefQuiz;
class PrefCharacter;
class PrefCardAppearance;
class KConfigSkeleton;

class KWordQuizPrefs : public KConfigDialog
{
  Q_OBJECT
public:
  KWordQuizPrefs(QWidget *parent, const QString & name, KConfigSkeleton *config, KActionCollection *actionCollection);

protected Q_SLOTS:
  void updateSettings() override;
  void updateWidgetsDefault() override;

protected:
  bool hasChanged() override;
  bool isDefault() override;

private:
  PrefGeneral *m_prefGeneral;
  PrefEditor *m_prefEditor;
  PrefQuiz *m_prefQuiz;
  PrefCharacter* m_prefCharacter;
  PrefCardAppearance* m_prefCardAppearance;
  KConfigSkeleton * m_config;
};

#endif
