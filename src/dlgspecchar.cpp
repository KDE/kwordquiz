//
// C++ Implementation: dlgspecchar
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dlgspecchar.h"

#include <qlayout.h>
#include <qclipboard.h>

#include <kapplication.h>
#include <klocale.h>
#include <kcharselect.h>
#include <kdebug.h>

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

    grid = new QGridLayout( page, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    charSelect = new KCharSelect( page, "", _font, _chr );
    connect(charSelect, SIGNAL(doubleClicked()),this, SLOT(slotDoubleClicked()));
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
