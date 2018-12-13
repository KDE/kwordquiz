/***************************************************************************
                          kwordquizprefs.h  -  description
                             -------------------
    begin                : Sun Aug 18 2002
    copyright            : (C) 2002-2008 by Peter Hedlund
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

protected slots:
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
