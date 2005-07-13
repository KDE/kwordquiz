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

	QString& systemName();			//returns the systems name
	void setSystemName( const QString& name );		//sets the systems name

	LeitnerBox* boxWithNumber( int number );	//returns box by number
	LeitnerBox* boxWithName( const QString& name );//returns box by name
	int number( LeitnerBox* box );
	const QString& box( int i );

	const QString& nextBox( QString& previousBox );	//returns the next box for the next question

	const QString& correctBox( int box );	//returns the correct word box of "int box"
	const QString& wrongBox( int box );		//returns the wrong word box of "int box"
	const QString& correctBox( QString& box );
	const QString& wrongBox( QString& box );

	int wrongBoxNumber( int box );
	int correctBoxNumber( int box );

	void setCorrectBox( const QString& box, const QString& correctWordBox );
	void setWrongBox( const QString& box, const QString& wrongWordBox );

	bool setBoxName( int box, const QString& name );
	bool setBoxName( LeitnerBox* box, const QString& name );

	void setBoxVocabCount( QString& box, int vocabCount );
	int boxVocabCount( QString& box );

	void incrementBoxVocabCount( QString& box );
 	void decrementBoxVocabCount( QString& box );

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
