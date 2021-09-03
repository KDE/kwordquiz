/* This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2004 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef PREFCHARACTER_H
#define PREFCHARACTER_H

#include <KActionCollection>

#include "ui_prefcharacterbase.h"

class DlgSpecChar;

/**
@author Peter Hedlund
*/
class PrefCharacter :  public QWidget, public Ui::PrefCharacterBase
{
Q_OBJECT
public:
  explicit PrefCharacter(QWidget *parent, KActionCollection * ac);
  void updateWidgets();
  bool hasChanged();
  bool isDefault();
  void updateSettings();

Q_SIGNALS:
  void widgetModified();

private Q_SLOTS:
  void fillWidgets();
  void slotCharListSelectionChanged();
  void slotDlgSpecCharClosed();
  void slotSelectSpecChar();
  void slotSpecChar(QChar);

private:
  KActionCollection * m_actionCollection;
  DlgSpecChar* m_dlgSpecChar;
};

#endif
