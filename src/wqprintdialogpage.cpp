//
// C++ Implementation: wqprintdialogpage
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <klocale.h>

#include "wqprintdialogpage.h"

WQPrintDialogPage::WQPrintDialogPage(QWidget *parent, const char *name )
 : KPrintDialogPage( parent, name )
{
  setTitle(i18n("Vocabulary Options"));
  
  QGridLayout * l = new QGridLayout( this, 1, 1, 11, 6);

  g = new QButtonGroup(i18n("Select type of printout"), this );
  g->setColumnLayout(0, Qt::Vertical );
  g->layout()->setSpacing( 6 );
  g->layout()->setMargin( 11 );
  QGridLayout * v = new QGridLayout( g->layout() );
  v->setAlignment( Qt::AlignTop );

  rb0 = new QRadioButton(i18n("Vocabulary &List"), g );
  v->addWidget( rb0, 0, 0 );
  rb1 = new QRadioButton(i18n("Vocabulary E&xam"), g);
  v->addWidget( rb1, 1, 0 );
  rb2 = new QRadioButton(i18n("&Flashcards"), g);
  v->addWidget( rb2, 2, 0 );
  l->addWidget( g, 0, 0 );
  
  QWhatsThis::add(g, i18n("Specify type of printout to make"));
  QWhatsThis::add(rb0, i18n("Select to print the vocabulary as displayed in the editor"));
  QWhatsThis::add(rb1, i18n("Select to print the vocabulary as a vocabulary exam"));
  QWhatsThis::add(rb2, i18n("Select to print flashcards"));      
}


WQPrintDialogPage::~WQPrintDialogPage()
{
}

void WQPrintDialogPage::getOptions( QMap< QString, QString > & opts, bool incldef )
{
  opts[ "kde-kwordquiz-type" ] = QString::number(g->selectedId());
}

void WQPrintDialogPage::setOptions( const QMap< QString, QString > & opts )
{
  QString type = opts[ "kde-kwordquiz-type" ];
  switch (type.toInt())
  {
    case 0: rb0->setChecked(true); break;
    case 1: rb1->setChecked(true); break;
    case 2: rb2->setChecked(true); break;
  }
}

bool WQPrintDialogPage::isValid( QString & msg )
{
  return true;
}


#include "wqprintdialogpage.moc"
