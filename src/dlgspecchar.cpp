/***************************************************************************
                          dlglspecchar.cpp  -  description
                             -------------------
   copyright            : (C) 2003 by Peter Hedlund
   email                : peter.hedlund@kdemail.net
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

#include <tqlayout.h>

#include <klocale.h>
#include <kcharselect.h>
#include <kdebug.h>

DlgSpecChar::DlgSpecChar( TQWidget *parent, const char *name, const TQString &_font, const TQChar &_chr, bool _modal )
    : KDialogBase( Plain, i18n("Select Character"), User1 | Cancel, User1 , parent, name, _modal )
{
    initDialog(_chr,_font,true);

    setButtonText( User1, i18n("&Select") );
    setButtonTip( User1, i18n("Select this character") );

}

void DlgSpecChar::initDialog(const TQChar &_chr, const TQString &_font, bool /*_enableFont*/)
{
   TQWidget *page = plainPage();

    grid = new TQGridLayout( page, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    int t = (_chr.unicode()/256);
    
    charSelect = new KCharSelect( page, "", _font, _chr, t);
    connect(charSelect, TQT_SIGNAL(doubleClicked()),this, TQT_SLOT(slotDoubleClicked()));
    charSelect->resize( charSelect->sizeHint() );
    charSelect->enableFontCombo( false );
    grid->addWidget( charSelect, 0, 0 );

    grid->addColSpacing( 0, charSelect->width() );
    grid->addRowSpacing( 0, charSelect->height() );
    grid->setRowStretch( 0, 0 );
    charSelect->setFocus();
}

DlgSpecChar::~DlgSpecChar()
{
}

void DlgSpecChar::closeDialog()
{
  KDialogBase::close();
}

TQChar DlgSpecChar::chr()
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
