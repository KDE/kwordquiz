#ifndef LEITNERSYSTEM_H
#define LEITNERSYSTEM_H

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

#include <QString>
#include <QStringList>

#include "leitnerbox.h"

/**
@author Martin Pfeiffer
*/

class LeitnerSystem
{
public:
	/**Constructor without arguments*/
	LeitnerSystem();

	/**Constructor with arguments
	 * @param boxes reference to a QList of LeitnerBox
	 * @param name the new LeitnerSystems name
	 */
	LeitnerSystem( QList<LeitnerBox>& boxes, QString name );

	/**Destructor*/
	~LeitnerSystem();

	/**Returns the number of LeitnerBoxes contained by this LeitnerSystem
	 * @return the number of LeitnerBoxes as integer
	 */
	int getNumberOfBoxes() const;

	/**Returns a QStringList with all the LeitnerBoxes' names
	 * @return a QStringList with all names
	 */
	QStringList getBoxNameList();

	/**Returns the LeitnerSystem's name
	 * @return the LeitnerSystem's name as reference to QString
	 */
	QString& systemName();

	/**Sets the LeitnerSystem's name
	 * @param the new name as QString
	 */
	void setSystemName( const QString& name );

	/**Returns a LeitnerBox by number
	 * @param number the number of the LeitnerBox to be returned
	 * @return a pointer to the LeitnerBox with the number
	 */
	LeitnerBox* boxWithNumber( int number );

	/**Returns a LeitnerBox by name
	 * @param name the name of the LeitnerBox to be returned
	 * @return a pointer to the LeitnerBox with the name,
	 *         or 0 if no such box exists.
	 */
	LeitnerBox* boxWithName( const QString& name ) const;

	/**Returns the number of the given LeitnerBox
	 * @param box a pointer to the LeitnerBox
	 * @return the number of the given LeitnerBox
	 */
	int number( LeitnerBox* box ) const;

	/**Returns the name of the LeitnerBox with number @p i
	 * @param i the LeitnerBox's number
	 * @return the name of the LeitnerBox with number @p i
	 */
	const QString& box( int i ) const;

	/**Returns the LeitnerBox following @p previousBox
	 * @param previousBox the name of the LeitnerBox
	 * @return the name of the LeitnerBox following previousBox
	 */
	const QString& nextBox( QString& previousBox );

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

	QList<LeitnerBox> m_boxes;
};

#endif
