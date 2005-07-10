//
// C++ Implementation: prefleitner
//
// Description: the part of the preferences to change the settings for the LeitnerSystem
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "prefleitner.h"

#include "leitnersystemview.h"
#include "leitnersystem.h"

#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <kdebug.h>

PrefLeitner::PrefLeitner(QWidget * parent, const char* name, WFlags f, LeitnerSystem* system)
	: PrefLeitnerBase( parent, name, f )
{
	m_selectedSystem = system;
	m_selectedBox = 0;

	//create new leitnersystemview, show and connect it
	m_leitnerSystemView = new LeitnerSystemView( this, 0 );
	PrefLeitnerBaseLayout->addWidget( m_leitnerSystemView, 1, 0 );
	PrefLeitnerBaseLayout->setOrigin( QGridLayout::BottomLeft );
	
	connect( m_leitnerSystemView, SIGNAL( boxClicked( int ) ), this, SLOT( slotBoxClicked( int ) ) );
	/*
	connect( cmbCorrect, SIGNAL( activated(const QString&) ), this, SLOT( slotCorrectWord(const QString&) ) );
	connect( cmbWrong, SIGNAL( activated(const QString&) ), this, SLOT( slotWrongWord(const QString&) ) );
	connect( lndBoxName, SIGNAL( textChanged(const QString&) ), this, SLOT( slotBoxName(const QString&) ) );
	connect( btnAddBox, SIGNAL( clicked() ), this, SLOT( slotAddBox() ) );
	connect( btnDeleteBox, SIGNAL( clicked() ), this, SLOT( slotDeleteBox() ) );*/
// 	connect( btnApply, SIGNAL( clicked() ), this, SLOT( slotApply() ) );
// 	connect( btnDiscard, SIGNAL( clicked() ), this, SLOT( slotDiscard() ) );
	
	//insert the list of box' names in the comboboxes
	cmbWrong->insertStringList( m_selectedSystem->getBoxNameList() );
	cmbCorrect->insertStringList( m_selectedSystem->getBoxNameList() );
	
	//show leitnersystem
	m_leitnerSystemView->setSystem( m_selectedSystem );
}


PrefLeitner::~PrefLeitner()
{
}

void PrefLeitner::slotCorrectWord( const QString& newBox )
{
	if( m_selectedBox == 0 )
		return;
	
	//when the correct word box was changed in the combobox
	m_selectedBox->setCorrectWordBox( m_selectedSystem->getBoxWithName( newBox ) );
	refreshSystemView();
}

void PrefLeitner::slotWrongWord( const QString& newBox )
{
	if( m_selectedBox == 0 )
		return;
	
	//when the wrong word box was changed in the combobox
	m_selectedBox->setWrongWordBox( m_selectedSystem->getBoxWithName( newBox ) );
	refreshSystemView();
}

void PrefLeitner::slotBoxName( const QString& newName )
{
	//when the boxes name was changed
	m_selectedSystem->setBoxName( m_selectedBox, newName );
}

void PrefLeitner::newSystem()
{
	cmbCorrect->insertStringList( m_selectedSystem->getBoxNameList() );
	cmbWrong->insertStringList( m_selectedSystem->getBoxNameList() );
	
	refreshSystemView();
}

void PrefLeitner::refreshSystemView()
{
	m_leitnerSystemView->setSystem( m_selectedSystem );
}

void PrefLeitner::slotBoxClicked( int box )
{
	if( box == -1 )
	{
		kdDebug() << "outside" << endl;
		m_selectedBox = 0;
		lndBoxName->setText( "" );
	}
	else
	{
		kdDebug() << "inside" << endl;
		m_selectedBox = m_selectedSystem->getBoxWithNumber( box );
		
		cmbCorrect->setCurrentItem( m_selectedSystem->getCorrectBoxNumber( box ) );
		cmbWrong->setCurrentItem( m_selectedSystem->getWrongBoxNumber( box ) );
		lndBoxName->setText( m_selectedBox->getBoxName() );
	}
}

void PrefLeitner::slotAddBox()
{
	m_selectedSystem->insertBox( "Neue Box", 1, 1 );
	refreshSystemView();
}

void PrefLeitner::slotDeleteBox()
{
	m_selectedSystem->deleteBox( m_selectedBox );
	
	refreshSystemView();
}

void PrefLeitner::slotApply()
{
	kdDebug() << "apply" << endl;
	setResult( QDialog::Accepted );
	close();
}

void PrefLeitner::slotDiscard()
{
	kdDebug() << "discard" << endl;
	setResult( QDialog::Rejected );
	close();
}

LeitnerSystem* PrefLeitner::getSystem()
{
	return m_selectedSystem;
}

//#include "prefleitner.moc"
