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

LeitnerSystem::LeitnerSystem(QValueList<LeitnerBox>& boxes, QString name)
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
	QValueList<LeitnerBox>::iterator it;	


	for(it = m_boxes.begin(); it != m_boxes.end(); ++it)
	{
		boxNameList.append((*it).getBoxName());
	}

	return boxNameList;
}

int LeitnerSystem::getNumberOfBoxes()
{
	return m_boxes.count();
}

LeitnerBox* LeitnerSystem::getBoxWithNumber( int number )
{
	return &m_boxes[ number ];
}

LeitnerBox* LeitnerSystem::getBoxWithName( const QString& name )
{
	QValueList<LeitnerBox>::iterator it;

	for(it = m_boxes.begin(); it != m_boxes.end(); ++it)
	{
		if((*it).getBoxName() == name)
			return &(*it);
	}

	return 0;
}

QString& LeitnerSystem::getSystemName()
{
	return m_systemName;
}

const QString& LeitnerSystem::getNextBox( QString& previousBox )
{
	for( int i = 0; i < m_boxes.count(); i++ )
	{
		if( m_boxes[i].getVocabCount() > 0 )
			return getBox( i );
	}
	
	return QString::null;
}

const QString& LeitnerSystem::getCorrectBox( int box )
{
	return m_boxes[ box ].getCorrectWordBox()->getBoxName();
}

const QString& LeitnerSystem::getWrongBox( int box )
{
	return m_boxes[ box ].getWrongWordBox()->getBoxName();
}

int LeitnerSystem::getWrongBoxNumber( int box )
{
	return getNumber( m_boxes[ box ].getWrongWordBox() );
}

int LeitnerSystem::getCorrectBoxNumber( int box )
{
	return getNumber( m_boxes[ box ].getCorrectWordBox() );
}

void LeitnerSystem::deleteBox( int box )
{
	m_boxes.remove( m_boxes.at( box ) );
}

void LeitnerSystem::deleteBox( LeitnerBox* box )
{
	m_boxes.remove( *box ); 
}

bool LeitnerSystem::insertBox( const QString& name, int correctWordBox, int wrongWordBox )
{
	if( getBoxNameList().contains( name ) != 0 )
		return false;
	
	LeitnerBox tmpBox;
	tmpBox.setBoxName( name );
	tmpBox.setCorrectWordBox( getBoxWithNumber( correctWordBox ) );
	tmpBox.setWrongWordBox( getBoxWithNumber( wrongWordBox ) );
	
	m_boxes.append( tmpBox );
	return true;
}

void LeitnerSystem::setSystemName( const QString& name )
{
	m_systemName = name;
}

int LeitnerSystem::getNumber( LeitnerBox* box )
{
	if( m_boxes.findIndex( *box ) == -1 )
		kdDebug() << "muhaha" << endl;
	return m_boxes.findIndex( *box );
}

bool LeitnerSystem::setBoxName( int box, const QString& name )
{
	if( getBoxWithName( name ) == 0 || getBoxWithName( name ) == getBoxWithNumber( box ) )
	{
		getBoxWithNumber( box )->setBoxName( name );
		
		return true;
	}
	else
		return false;
}

bool LeitnerSystem::setBoxName( LeitnerBox* box, const QString& name )
{
	if( getBoxWithName( name ) == 0 || getBoxWithName( name ) == box )
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
	//tmpBox.setVocabCount( count );
		
	m_boxes.append( tmpBox );
	return true;
}

void LeitnerSystem::setCorrectBox( const QString& box, const QString& correctWordBox )
{
	getBoxWithName( box )->setCorrectWordBox( getBoxWithName( correctWordBox ) );
}

void LeitnerSystem::setWrongBox( const QString& box, const QString& wrongWordBox )
{
	getBoxWithName( box )->setWrongWordBox( getBoxWithName( wrongWordBox ) );
}

const QString& LeitnerSystem::getBox( int i )
{
	return getBoxWithNumber( i )->getBoxName();
}
