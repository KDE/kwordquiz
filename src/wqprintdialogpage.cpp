/* This file is part of KWordQuiz
  Copyright (C) 2004 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <tqbuttongroup.h>
#include <tqradiobutton.h>
#include <tqlayout.h>
#include <tqwhatsthis.h>

#include <klocale.h>

#include "wqprintdialogpage.h"

WQPrintDialogPage::WQPrintDialogPage(TQWidget *parent, const char *name )
 : KPrintDialogPage( parent, name )
{
  setTitle(i18n("Vocabulary Options"));
  
  TQGridLayout * l = new TQGridLayout( this, 1, 1, 11, 6);

  g = new TQButtonGroup(i18n("Select Type of Printout"), this );
  g->setColumnLayout(0, Qt::Vertical );
  g->layout()->setSpacing( 6 );
  g->layout()->setMargin( 11 );
  TQGridLayout * v = new TQGridLayout( g->layout() );
  v->setAlignment( Qt::AlignTop );

  rb0 = new TQRadioButton(i18n("Vocabulary &list"), g );
  v->addWidget( rb0, 0, 0 );
  rb1 = new TQRadioButton(i18n("Vocabulary e&xam"), g);
  v->addWidget( rb1, 1, 0 );
  rb2 = new TQRadioButton(i18n("&Flashcards"), g);
  v->addWidget( rb2, 2, 0 );
  l->addWidget( g, 0, 0 );
  
  TQWhatsThis::add(g, i18n("Specify type of printout to make"));
  TQWhatsThis::add(rb0, i18n("Select to print the vocabulary as displayed in the editor"));
  TQWhatsThis::add(rb1, i18n("Select to print the vocabulary as a vocabulary exam"));
  TQWhatsThis::add(rb2, i18n("Select to print flashcards"));
}


WQPrintDialogPage::~WQPrintDialogPage()
{
}

void WQPrintDialogPage::getOptions(TQMap< TQString, TQString > & opts, bool incldef)
{
  opts[ "kde-kwordquiz-type" ] = TQString::number(g->selectedId());
}

void WQPrintDialogPage::setOptions(const TQMap< TQString, TQString > & opts )
{
  TQString type = opts[ "kde-kwordquiz-type" ];
  switch (type.toInt())
  {
    case 0: rb0->setChecked(true); break;
    case 1: rb1->setChecked(true); break;
    case 2: rb2->setChecked(true); break;
  }
}

bool WQPrintDialogPage::isValid(const TQString & msg )
{
  return true;
}


#include "wqprintdialogpage.moc"
