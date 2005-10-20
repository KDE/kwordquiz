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

#include "prefcharacter.h"

PrefCharacter::PrefCharacter(QWidget *parent) : QWidget(parent)
{
  setupUi(this);
  QStringList sl;
  sl << i18n("Action");
  sl << i18n("Shortcut");
  sl << i18n("Character");
  CharacterTree->setHeaderLabels(sl);

  QTreeWidgetItem * twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 1"));
  twi->setText(1, i18n("Ctrl+1"));
  twi->setText(2, i18n("A"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 2"));
  twi->setText(1, i18n("Ctrl+2"));
  twi->setText(2, i18n("B"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 3"));
  twi->setText(1, i18n("Ctrl+3"));
  twi->setText(2, i18n("C"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 4"));
  twi->setText(1, i18n("Ctrl+4"));
  twi->setText(2, i18n("D"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 5"));
  twi->setText(1, i18n("Ctrl+5"));
  twi->setText(2, i18n("E"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 6"));
  twi->setText(1, i18n("Ctrl+6"));
  twi->setText(2, i18n("F"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 7"));
  twi->setText(1, i18n("Ctrl+7"));
  twi->setText(2, i18n("G"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 8"));
  twi->setText(1, i18n("Ctrl+8"));
  twi->setText(2, i18n("H"));

  twi = new QTreeWidgetItem(CharacterTree);
  twi->setText(0, i18n("Special Character 9"));
  twi->setText(1, i18n("Ctrl+9"));
  twi->setText(2, i18n("I"));

}

#include "prefcharacter.moc"
