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
    : KDialog( parent )
{
  setCaption( i18n("Select Character") );
  setButtons( User1 | Cancel );
  setDefaultButton( User1 );
  setModal( _modal );
  setObjectName( QLatin1String(name) );

  initDialog(_chr,_font,true);

  setButtonText( User1, i18n("&Select") );
  setButtonToolTip( User1, i18n("Select this character") );
  connect(this,SIGNAL(user1Clicked()),this,SLOT(slotUser1()));

}

void DlgSpecChar::initDialog(const QChar &_chr, const QString &_font, bool /*_enableFont*/)
{
  QWidget *page = new QWidget( this );
  setMainWidget( page );

  grid = new QGridLayout( page );
  grid->setMargin( KDialog::marginHint() );
  grid->setSpacing( KDialog::spacingHint() );

  charSelect = new KCharSelect( page, _chr, _font);
  connect(charSelect, SIGNAL(charSelected(QChar)),this, SLOT(slotDoubleClicked()));
  charSelect->resize( charSelect->sizeHint() );
//   charSelect->enableFontCombo( false );
  grid->addWidget( charSelect, 0, 0 );

//   grid->addItem( new QSpacerItem( charSelect->width(), 0 ), 0, 0 );
//   grid->addItem( new QSpacerItem( 0, charSelect->height() ), 0, 0 );
//   grid->setRowStretch( 0, 0 );
  charSelect->setFocus();
}

void DlgSpecChar::closeDialog()
{
  KDialog::close();
}

QChar DlgSpecChar::chr()
{
  return charSelect->currentChar();
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
