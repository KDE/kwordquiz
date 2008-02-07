/***************************************************************************
                          dlgspecchar.cpp  -  description
                             -------------------
   copyright       : (C) 2003-2008 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <QGridLayout>

#include <KLocale>
#include <KCharSelect>

DlgSpecChar::DlgSpecChar(QWidget *parent, const QFont &font, const QChar &chr) : KDialog(parent)
{
  setCaption( i18nc("@title:window select character dialog", "Select Character") );
  setButtons(User1 | Cancel);
  setDefaultButton(User1);
  setModal(true);

  initDialog(font, chr);

  setButtonText( User1, i18nc("@action:button select", "&Select") );
  setButtonToolTip( User1, i18nc("@info:tooltip select this character", "Select this character") );
  connect(this,SIGNAL(user1Clicked()),this,SLOT(slotUser1()));
}

void DlgSpecChar::initDialog(const QFont &font, const QChar &chr)
{
  QWidget *page = new QWidget(this);
  setMainWidget(page);

  m_layout = new QGridLayout(page);
  m_layout->setMargin(KDialog::marginHint());
  m_layout->setSpacing(KDialog::spacingHint());

  m_charSelect = new KCharSelect(page);
  m_charSelect->setCurrentChar(chr);
  m_charSelect->setCurrentFont(font);
  connect(m_charSelect, SIGNAL(charSelected(QChar)),this, SLOT(slotDoubleClicked()));
  m_charSelect->resize(m_charSelect->sizeHint());
  m_layout->addWidget(m_charSelect, 0, 0);
  m_charSelect->setFocus();
}

void DlgSpecChar::closeDialog()
{
  KDialog::close();
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


#include "dlgspecchar.moc"
