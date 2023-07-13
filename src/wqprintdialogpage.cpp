/*
    SPDX-FileCopyrightText: 2004-2008 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "wqprintdialogpage.h"

#include <QGridLayout>
#include <QRadioButton>
#include <QSpacerItem>

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

#include "moc_wqprintdialogpage.cpp"
