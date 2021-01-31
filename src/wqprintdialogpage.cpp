/* This file is part of KWordQuiz
  Copyright (C) 2004-2008 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include "wqprintdialogpage.h"

#include <QDialog>
#include <QGridLayout>
#include <QRadioButton>
#include <QSpacerItem>

#include <KConfigGroup>
#include <KLocalizedString>

WQPrintDialogPage::WQPrintDialogPage(QWidget *parent) : QWidget(parent)
{
  setWindowTitle(i18nc("@title:window", "Vocabulary Options"));

  QGridLayout * l = new QGridLayout(this);
  QSpacerItem * s = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  l->addItem(s, 1, 0, 1, 1);

  g = new QGroupBox(i18n("Select Type of Printout"), this );
  QVBoxLayout * vboxLayout = new QVBoxLayout(g);

  bg = new QButtonGroup(this);

  QRadioButton *rb = new QRadioButton(i18n("Vocabulary &list"),g);
  rb->setWhatsThis(i18n("Select to print the vocabulary as displayed in the editor"));
  vboxLayout->addWidget(rb);
  bg->addButton(rb, Prefs::EnumPrintStyle::List);

  rb = new QRadioButton(i18n("Vocabulary e&xam"),g);
  rb->setWhatsThis(i18n("Select to print the vocabulary as a vocabulary exam"));
  vboxLayout->addWidget(rb);
  bg->addButton(rb, Prefs::EnumPrintStyle::Exam);

  rb = new QRadioButton(i18n("&Flashcards"),g);
  rb->setWhatsThis(i18n("Select to print flashcards"));
  vboxLayout->addWidget(rb);
  bg->addButton(rb, Prefs::EnumPrintStyle::Flashcard);

  l->addWidget(g, 0, 0, 1, 1);
}

int WQPrintDialogPage::printStyle()
{
  return bg->checkedId();
}

void WQPrintDialogPage::setPrintStyle(int style)
{
  bg->button(style)->setChecked(true);
}
