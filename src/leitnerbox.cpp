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

int LeitnerBox::vocabCount()
{
	return m_vocabCount;
}

void LeitnerBox::setVocabCount( int count )
{
	m_vocabCount = count;
}

const QString& LeitnerBox::boxName() const
{
	return m_name;
}

LeitnerBox* LeitnerBox::correctWordBox()
{
	return m_correctWordBox;
}

LeitnerBox* LeitnerBox::wrongWordBox()
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
	if( a.boxName() == boxName() )
		return true;
	else
		return false;
}
