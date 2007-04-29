/* This file is part of KWordQuiz
  Copyright (C) 2004-2005 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <QtGui/QRadioButton>
#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>

#include <klocale.h>
#include <kdialog.h>

WQPrintDialogPage::WQPrintDialogPage(QWidget *parent) : KPrintDialogPage(parent)
{
  setTitle(i18n("Vocabulary Options"));

  QGridLayout * l = new QGridLayout(this);
  l->setSpacing(KDialog::spacingHint());
  l->setMargin(KDialog::marginHint());
  QSpacerItem * s = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  l->addItem(s, 1, 0, 1, 1);

  g = new QGroupBox(i18n("Select Type of Printout"), this );
  QVBoxLayout * vboxLayout = new QVBoxLayout(g);
  vboxLayout->setSpacing(KDialog::spacingHint());
  vboxLayout->setMargin(KDialog::marginHint());

  rb0 = new QRadioButton(i18n("Vocabulary &list"),g);
  rb0->setWhatsThis(i18n("Select to print the vocabulary as displayed in the editor"));
  vboxLayout->addWidget(rb0);

  rb1 = new QRadioButton(i18n("Vocabulary e&xam"),g);
  rb1->setWhatsThis(i18n("Select to print the vocabulary as a vocabulary exam"));
  vboxLayout->addWidget(rb1);

  rb2 = new QRadioButton(i18n("&Flashcards"),g);
  rb2->setWhatsThis(i18n("Select to print flashcards"));
  vboxLayout->addWidget(rb2);

  l->addWidget(g, 0, 0, 1, 1);
}

void WQPrintDialogPage::getOptions(QMap< QString, QString > & opts, bool /*incldef*/)
{
  if (rb0->isChecked())
    opts[ "kde-kwordquiz-type" ] = "0";
  if (rb1->isChecked())
    opts[ "kde-kwordquiz-type" ] = "1";
  if (rb2->isChecked())
    opts[ "kde-kwordquiz-type" ] = "2";
}

void WQPrintDialogPage::setOptions(const QMap< QString, QString > & opts )
{
  QString type = opts[ "kde-kwordquiz-type" ];
  switch (type.toInt())
  {
    case 0: rb0->setChecked(true); break;
    case 1: rb1->setChecked(true); break;
    case 2: rb2->setChecked(true); break;
  }
}

#include "wqprintdialogpage.moc"
