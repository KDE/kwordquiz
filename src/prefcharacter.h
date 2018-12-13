/* This file is part of KWordQuiz
  Copyright (C) 2004 Peter Hedlund <peter.hedlund@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
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

signals:
  void widgetModified();

private slots:
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
