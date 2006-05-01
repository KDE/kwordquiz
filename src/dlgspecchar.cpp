/***************************************************************************
                          dlgspecchar.cpp  -  description
                             -------------------
   copyright       : (C) 2003-2005 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGridLayout>

#include <klocale.h>
#include <kcharselect.h>
#include <kdebug.h>

#include "dlgspecchar.h"

DlgSpecChar::DlgSpecChar( QWidget *parent, const char *name, const QString &_font, const QChar &_chr, bool _modal )
    : KDialogBase( Plain, i18n("Select Character"), User1 | Cancel, User1 , parent, name, _modal )
{
  initDialog(_chr,_font,true);

  setButtonText( User1, i18n("&Select") );
  setButtonTip( User1, i18n("Select this character") );

}

void DlgSpecChar::initDialog(const QChar &_chr, const QString &_font, bool /*_enableFont*/)
{
  QWidget *page = plainPage();

  grid = new QGridLayout( page );
  grid->setMargin( KDialog::marginHint() );
  grid->setSpacing( KDialog::spacingHint() );

  int t = (_chr.unicode()/256);

  charSelect = new KCharSelect( page,  _font, _chr, t);
  connect(charSelect, SIGNAL(doubleClicked()),this, SLOT(slotDoubleClicked()));
  charSelect->resize( charSelect->sizeHint() );
  charSelect->enableFontCombo( false );
  grid->addWidget( charSelect, 0, 0 );

  grid->addItem( new QSpacerItem( charSelect->width(), 0 ), 0, 0 );
  grid->addItem( new QSpacerItem( 0, charSelect->height() ), 0, 0 );
  grid->setRowStretch( 0, 0 );
  charSelect->setFocus();
}

void DlgSpecChar::closeDialog()
{
  KDialogBase::close();
}

QChar DlgSpecChar::chr()
{
  return charSelect->chr();
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
