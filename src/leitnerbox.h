//
// C++ Interface: leitnerbox
//
// Description:
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LEITNERBOX_H
#define LEITNERBOX_H

#include <QString>

/**
 * A box in a LeitnerSystem - a LeitnerBox
 * This box has a name and a certain number of vocabulars in it. This number changes continuosly
 * as the vocab cards increase or decrease in box position. There is a determined order of LeitnerBoxes
 * in a LeitnerSystem, therefore each LeitnerBox helds pointers to the LeitnerBox for wrong and correct
 * cards of this LeitnerBox
 * @author Martin Pfeiffer
 */
class LeitnerBox
{
public:
	/**
	 * The constructor
	 */
	LeitnerBox();

	~LeitnerBox();

	/**
	 * Sets the LeitnerBox's pointer to the correct word LeitnerBox
	 * @param correctWordBox a pointer to the LeitnerBox to be set
	 */
	void setCorrectWordBox( LeitnerBox* correctWordBox );

	/**
	 * Sets the LeitnerBox's pointer to the wrong word LeitnerBox
	 * @param wrongWordBox a pointer to the LeitnerBox to be set
	 */
	void setWrongWordBox( LeitnerBox* wrongWordBox );
	void setBoxName( const QString& );	//sets the boxes name

	void setVocabCount( int count );
	int vocabCount();

	LeitnerBox* correctWordBox();
	LeitnerBox* wrongWordBox();
	const QString& boxName() const;

	bool operator == ( const LeitnerBox& a ) const;

private:
	QString m_name;

	int m_vocabCount;

	LeitnerBox* m_wrongWordBox;
	LeitnerBox* m_correctWordBox;
};

#endif
