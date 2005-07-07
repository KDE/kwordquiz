//
// C++ Implementation: leitnerbox
//
// Description: 
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "leitnerbox.h"

LeitnerBox::LeitnerBox()
{
}


LeitnerBox::~LeitnerBox()
{
}

int LeitnerBox::getVocabCount()
{
	return m_vocabCount;
}

void LeitnerBox::setVocabCount( int count )
{
	m_vocabCount = count;
}

const QString& LeitnerBox::getBoxName() const
{
	return m_name;
}

LeitnerBox* LeitnerBox::getCorrectWordBox()
{
	return m_correctWordBox;
}

LeitnerBox* LeitnerBox::getWrongWordBox()
{
	return m_wrongWordBox;
}

void LeitnerBox::setBoxName( const QString& name )
{
	m_name = name;
}

void LeitnerBox::setCorrectWordBox( LeitnerBox* correctWordBox )
{
	m_correctWordBox = correctWordBox;
}

void LeitnerBox::setWrongWordBox( LeitnerBox* wrongWordBox )
{
	m_wrongWordBox = wrongWordBox;
}

bool LeitnerBox::operator ==( const LeitnerBox& a ) const
{
	if( a.getBoxName() == getBoxName() )
		return true;
	else
		return false;
}
