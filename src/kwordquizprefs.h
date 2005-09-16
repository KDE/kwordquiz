/***************************************************************************
                          kwordquizprefs.h  -  description
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

#ifndef KWORDQUIZPREFS_H
#define KWORDQUIZPREFS_H

#include <kconfigdialog.h>

/**
  *@author Peter Hedlund
*/

class PrefEditor;
class PrefQuiz;
class PrefCharacter;
class PrefCardAppearance;
class DlgSpecChar;

class KWordQuizPrefs : public KConfigDialog
{
  Q_OBJECT
public:
  KWordQuizPrefs(QWidget *parent=0, const char *name=0, KConfigSkeleton *config=0, DialogType dialogType=IconList,
    int dialogButtons=Default|Ok|Apply|Cancel|Help, ButtonCode defaultButton=Ok, bool modal=false);

protected slots:
  void updateSettings();
  void updateWidgetsDefault();

private slots:
  void slotCharListSelectionChanged();
  void slotDlgSpecCharClosed();
  void slotSelectSpecChar();
  void slotSpecChar(QChar);

protected:
  bool hasChanged();
  bool isDefault();

private:
  PrefEditor *m_prefEditor;
  PrefQuiz *m_prefQuiz;
  PrefCharacter* m_prefCharacter;
  DlgSpecChar* m_dlgSpecChar;
  PrefCardAppearance* m_prefCardAppearance;
  KConfigSkeleton * m_config;
};

#endif
