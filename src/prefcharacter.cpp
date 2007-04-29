/* This file is part of KWordQuiz
  Copyright (C) 2004, 2007 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <kaction.h>
#include <klocale.h>

#include "prefs.h"
#include "dlgspecchar.h"

PrefCharacter::PrefCharacter(QWidget *parent, KActionCollection * ac) : QWidget(parent)
{
  setupUi(this);
  m_actionCollection = ac;

  connect(CharacterTree, SIGNAL(itemSelectionChanged()), this, SLOT(slotCharListSelectionChanged()));
  connect(btnCharacter, SIGNAL(clicked()), this, SLOT(slotSelectSpecChar()));

  fillWidgets();
  updateWidgets();

  m_dlgSpecChar = 0L;
}

void PrefCharacter::fillWidgets()
{
  QStringList sl;
  sl << i18n("Action");
  sl << i18n("Shortcut");
  sl << i18n("Character");
  CharacterTree->setHeaderLabels(sl);

  QString sc = Prefs::specialCharacters();
  QTreeWidgetItem * twi;

  for (int i = 0; i < 9; i++)
  {
    twi = new QTreeWidgetItem(CharacterTree);
    twi->setText(0, i18n("Special Character %1", i + 1));
    twi->setText(1, m_actionCollection->action(QString("char_" + QString::number(i + 1)))->shortcut().toString());
    twi->setText(2, QString(sc[i]));
  }
  CharacterTree->resizeColumnToContents(0);
  CharacterTree->setItemSelected(CharacterTree->topLevelItem(0), true);
  CharacterTree->setCurrentItem(CharacterTree->topLevelItem(0));
  lblPreview->setText(CharacterTree->topLevelItem(0)->text(2));
}

void PrefCharacter::updateWidgets()
{
  QString sc = Prefs::specialCharacters();
  for (int i = 0; i < 9; i++)
    CharacterTree->topLevelItem(i)->setText(2, QString(sc[i]));
}

void PrefCharacter::slotCharListSelectionChanged( )
{
  if (CharacterTree->currentItem())
    lblPreview->setText(CharacterTree->currentItem()->text(2));
}

bool PrefCharacter::hasChanged()
{
  QString sc = Prefs::specialCharacters();
  QString s;
  for (int i = 0; i < 9; i++)
    s.append(CharacterTree->topLevelItem(i)->text(2));

  return (sc.simplified() != s.simplified());
}

bool PrefCharacter::isDefault()
{
  QString def = "abcdefghi";
  QString s;
  for (int i = 0; i < 9; i++)
    s.append(CharacterTree->topLevelItem(i)->text(2));

  return (def == s.simplified());
}

void PrefCharacter::updateSettings( )
{
  QString s;
  for (int i = 0; i < 9; i++)
    s.append(CharacterTree->topLevelItem(i)->text(2));

  Prefs::setSpecialCharacters(s.simplified());
}

void PrefCharacter::slotDlgSpecCharClosed()
{
  if (m_dlgSpecChar)
  {
    disconnect(m_dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    disconnect(m_dlgSpecChar, SIGNAL(finished()), this, SLOT(slotDlgSpecCharClosed()));
    m_dlgSpecChar->deleteLater();
    m_dlgSpecChar = 0L;
  }
}

void PrefCharacter::slotSelectSpecChar()
{
  QString f = Prefs::editorFont().family();
  QString s = CharacterTree->currentItem()->text(2);
  QChar c = s[0];

  if (m_dlgSpecChar == 0)
  {
    m_dlgSpecChar = new DlgSpecChar( this, "insert special char", f, c, true );
    connect(m_dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    connect(m_dlgSpecChar, SIGNAL(finished()), this, SLOT(slotDlgSpecCharClosed()));
  }
  m_dlgSpecChar->show();
}

void PrefCharacter::slotSpecChar(QChar c)
{
  CharacterTree->currentItem()->setText(2, QString(c));
  lblPreview->setText(CharacterTree->currentItem()->text(2));
  emit widgetModified();
}



#include "prefcharacter.moc"
