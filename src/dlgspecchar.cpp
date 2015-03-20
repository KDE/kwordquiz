/***************************************************************************
                          dlgspecchar.cpp  -  description
                             -------------------
   copyright       : (C) 2003-2009 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dlgspecchar.h"

#include <KLocalizedString>
#include <KCharSelect>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

DlgSpecChar::DlgSpecChar(QWidget *parent, const QFont &font, const QChar &chr) : QDialog(parent)
{
  setWindowTitle( i18nc("@title:window select character dialog", "Select Character") );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  QPushButton *user1Button = new QPushButton;
  buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  //PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
  mainLayout->addWidget(buttonBox);
  user1Button->setDefault(true);
  setModal(true);

  m_charSelect = new KCharSelect(this, 0);
  m_charSelect->setCurrentChar(chr);
  m_charSelect->setCurrentFont(font);
  connect(m_charSelect, SIGNAL(charSelected(QChar)),this, SLOT(slotDoubleClicked()));
  m_charSelect->resize(m_charSelect->sizeHint());
  m_charSelect->setFocus();
  mainLayout->addWidget(m_charSelect);

  user1Button->setText(i18nc("@action:button", "&Select"));
  user1Button->setToolTip(i18nc("@info:tooltip", "Select this character"));
  connect(user1Button,SIGNAL(clicked()),this,SLOT(slotUser1()));
}

void DlgSpecChar::closeDialog()
{
  QDialog::close();
}

QChar DlgSpecChar::chr()
{
  return m_charSelect->currentChar();
}

void DlgSpecChar::slotDoubleClicked()
{
  emit insertChar(chr());
  closeDialog();
}

void DlgSpecChar::slotUser1( )
{
  emit insertChar(chr());
  closeDialog();
}
