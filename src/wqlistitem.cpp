//
// C++ Implementation: wqlistitem
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wqlistitem.h"

WQListItem::WQListItem()
{
}


WQListItem::~WQListItem()
{
}


void WQListItem::setQuestion(int i)
{
  m_question = i;
}

void WQListItem::setCorrect(int c)
{
  m_correct = c;
}

void WQListItem::setOneOp(int i)
{
  m_oneOp = i;
}

void WQListItem::setTwoOp(int i)
{
  m_twoOp = i;
}

void WQListItem::setThreeOp(int i)
{
  m_threeOp = i;
}
