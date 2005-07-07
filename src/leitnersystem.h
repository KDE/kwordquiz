//
// C++ Interface: leitnersystem
//
// Description: 
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include "leitnerbox.h"

#ifndef LEITNERSYSTEM_H
#define LEITNERSYSTEM_H


/**
@author Martin Pfeiffer
*/
class LeitnerSystem
{
public:
	LeitnerSystem();
	LeitnerSystem( QValueList<LeitnerBox>& boxes, QString name );

	~LeitnerSystem();

	int getNumberOfBoxes();				//returns the number of boxes in the system
	QStringList getBoxNameList();			//returns a list with the boxes names

	QString& getSystemName();			//returns the systems name
	void setSystemName( const QString& name );		//sets the systems name

	LeitnerBox* getBoxWithNumber( int number );	//returns box by number
	LeitnerBox* getBoxWithName( const QString& name );//returns box by name
	int getNumber( LeitnerBox* box );
	const QString& getBox( int i );
	
	const QString& getNextBox( QString& previousBox );	//returns the next box for the next question
	
	const QString& getCorrectBox( int box );	//returns the correct word box of "int box"
	const QString& getWrongBox( int box );		//returns the wrong word box of "int box"
	int getWrongBoxNumber( int box );
	int getCorrectBoxNumber( int box );
	void setCorrectBox( const QString& box, const QString& correctWordBox );
	void setWrongBox( const QString& box, const QString& wrongWordBox );
	bool setBoxName( int box, const QString& name );
	bool setBoxName( LeitnerBox* box, const QString& name );
	
	//inserts a box with number, name, correct and wrong word box
	bool insertBox( const QString& name, int correctWordBox, int wrongWordBox );
	bool insertBox( const QString& name );
	void deleteBox( int box );
	void deleteBox( LeitnerBox* box );

private:
	QString m_systemName;				//the systems name

	QValueList<LeitnerBox> m_boxes;
};

#endif
