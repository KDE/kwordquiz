//
// C++ Implementation: leitnersystem
//
// Description:
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "leitnersystem.h"

#include <kdebug.h>

LeitnerSystem::LeitnerSystem(QList<LeitnerBox>& boxes, QString name)
{
	if( !boxes.empty() )
		m_boxes = boxes;

	if( name.isNull() )
		m_systemName = name;
}

LeitnerSystem::LeitnerSystem()
{
}

LeitnerSystem::~LeitnerSystem()
{
}

QStringList LeitnerSystem::getBoxNameList()
{
	QStringList boxNameList;

	foreach( LeitnerBox box, m_boxes )
		boxNameList.append( box.boxName() );
	
	return boxNameList;
}

int LeitnerSystem::getNumberOfBoxes() const
{
	return m_boxes.count();
}

LeitnerBox* LeitnerSystem::boxWithNumber( int number )
{
	return &m_boxes[ number ];
}

LeitnerBox* LeitnerSystem::boxWithName( const QString& name ) const
{
	foreach( LeitnerBox box, m_boxes )
	{
		if ( box.boxName() == name )
			return &box;
	}

	return 0;
}

QString& LeitnerSystem::systemName()
{
	return m_systemName;
}

const QString& LeitnerSystem::nextBox( QString& previousBox )
{
	for( int i = 0; i < m_boxes.count(); i++ )
	{
		if( m_boxes[i].vocabCount() > 0 )
			return box( i );
	}

	return QString::null;
}

const QString& LeitnerSystem::correctBox( int box )
{
	return m_boxes[ box ].correctWordBox()->boxName();
}

const QString& LeitnerSystem::wrongBox( int box )
{
	return m_boxes[ box ].wrongWordBox()->boxName();
}

const QString& LeitnerSystem::correctBox( QString& box )
{
	return boxWithName( box )->correctWordBox()->boxName();
}

const QString& LeitnerSystem::wrongBox( QString& box )
{
	return boxWithName( box )->wrongWordBox()->boxName();
}

int LeitnerSystem::wrongBoxNumber( int box )
{
	return number( m_boxes[ box ].wrongWordBox() );
}

int LeitnerSystem::correctBoxNumber( int box )
{
	return number( m_boxes[ box ].correctWordBox() );
}

void LeitnerSystem::deleteBox( int box )
{
	m_boxes.removeAt( box );
}

void LeitnerSystem::deleteBox( LeitnerBox* box )
{
	for( int i = 0; i < m_boxes.count(); i++ )
	{
		if( m_boxes[i].correctWordBox() == box )
			m_boxes[i].setCorrectWordBox( 0 );

		if( m_boxes[i].wrongWordBox() == box )
			m_boxes[i].setWrongWordBox( 0 );
	}

	m_boxes.removeAll( *box );
}

bool LeitnerSystem::insertBox( const QString& name, int correctWordBox, int wrongWordBox )
{
	if( getBoxNameList().contains( name ) != 0 )
		return false;

	LeitnerBox tmpBox;
	tmpBox.setBoxName( name );
	tmpBox.setCorrectWordBox( boxWithNumber( correctWordBox ) );
	tmpBox.setWrongWordBox( boxWithNumber( wrongWordBox ) );

	m_boxes.append( tmpBox );
	return true;
}

void LeitnerSystem::setSystemName( const QString& name )
{
	m_systemName = name;
}

int LeitnerSystem::number( LeitnerBox* box ) const
{
	if( box == 0 )
		return -1;

	return m_boxes.findIndex( *box );
}

bool LeitnerSystem::setBoxName( int box, const QString& name )
{
	if( boxWithName( name ) == 0 || boxWithName( name ) == boxWithNumber( box ) )
	{
		boxWithNumber( box )->setBoxName( name );

		return true;
	}
	else
		return false;
}

bool LeitnerSystem::setBoxName( LeitnerBox* box, const QString& name )
{
	if( boxWithName( name ) == 0 || boxWithName( name ) == box )
	{
		box->setBoxName( name );

		return true;
	}
	else
		return false;
}

bool LeitnerSystem::insertBox( const QString& name )
{
	if( getBoxNameList().contains( name ) != 0 )
		return false;

	LeitnerBox tmpBox;
	tmpBox.setBoxName( name );

	m_boxes.append( tmpBox );
	return true;
}

void LeitnerSystem::setCorrectBox( const QString& box, const QString& correctWordBox )
{
	boxWithName( box )->setCorrectWordBox( boxWithName( correctWordBox ) );
}

void LeitnerSystem::setWrongBox( const QString& box, const QString& wrongWordBox )
{
	boxWithName( box )->setWrongWordBox( boxWithName( wrongWordBox ) );
}

const QString& LeitnerSystem::box( int i ) const
{
	return m_boxes[ i ].boxName();
}

void LeitnerSystem::setBoxVocabCount( QString& box, int vocabCount )
{
	boxWithName( box )->setVocabCount( vocabCount );
}

int LeitnerSystem::boxVocabCount( QString& box )
{
	return boxWithName( box )->vocabCount();
}

void LeitnerSystem::incrementBoxVocabCount( QString& box )
{
	int tmp = boxWithName( box )->vocabCount();
	boxWithName( box )->setVocabCount( tmp++ );
}

void LeitnerSystem::decrementBoxVocabCount( QString& box )
{
	int tmp = boxWithName( box )->vocabCount();
	boxWithName( box )->setVocabCount( tmp-- );
}

