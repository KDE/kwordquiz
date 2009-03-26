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
  m_charSelect = new KCharSelect(this, 0);
  m_charSelect->setCurrentChar(chr);
  m_charSelect->setCurrentFont(font);
  connect(m_charSelect, SIGNAL(charSelected(QChar)),this, SLOT(slotDoubleClicked()));
  m_charSelect->resize(m_charSelect->sizeHint());
  m_charSelect->setFocus();
  setMainWidget(m_charSelect);
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
