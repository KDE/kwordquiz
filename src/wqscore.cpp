/***************************************************************************
                          wqscore.cpp  -  description
                             -------------------
   copyright            : (C) 2003 by Peter Hedlund
   email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "wqscore.h"

WQScore::WQScore()
{
  m_questionCount = 0;
  m_percent = false;
  m_error = 0;
  m_correct = 0;
  m_answerCount = 0;
}


WQScore::~WQScore()
{}


void WQScore::setAsPercent(bool p)
{
  m_percent = p;
}

void WQScore::setQuestionCount(int c)
{
  m_questionCount = c;
  //m_percent = false;
  m_error = 0;
  m_correct = 0;
  m_answerCount = 0;
}

void WQScore::countIncrement(CountDirection d)
{

  if (d != cdNone)
    m_answerCount++;
  if (d == cdCorrect)
    m_correct++;
  if (d == cdError)
    m_error++;
}

/*!
    \fn WQScore::answerText()
 */
QString WQScore::answerText()
{
  QString s;
  if (m_percent)
  {
    float f = ((m_answerCount * 100.0) / m_questionCount);
    if (m_answerCount > 0)
    {
      if (f < 1)
        s = s.setNum(f, 'f', 1) + "%"; //for long lists (i.e. each question is less than 1%) we show one decimal 
      else
        s = s.setNum(f, 'f', 0) + "%";
    }
    else
      s= valueToString(m_answerCount);
  }
  else
    s = valueToString(m_answerCount);
  return s;
}

/*!
    \fn WQScore::correctText()
 */
QString WQScore::correctText()
{
  QString s;
  if (m_percent)
  {
    if (m_correct > 0)
    {
      float f = ((m_correct * 100.0) / m_answerCount);
      s = s.setNum(f, 'f', 0) + "%";
    }
    else
      s = valueToString(m_correct);

  }
  else
    s = valueToString(m_correct);
  return s;
}

/*!
    \fn WQScore::errorText()
 */
QString WQScore::errorText()
{
  QString s;
  if (m_percent)
  {
    if (m_error > 0)
    {
      float f = ((m_error * 100.0) / m_answerCount);
      s = s.setNum(f, 'f', 0) + "%";
    }
    else
      s = valueToString(m_error);

  }
  else
    s = valueToString(m_error);
  return s;
}

/*!
    \fn WQScore::valueToString(int i)
 */
QString WQScore::valueToString(int i)
{
  QString s = ""; //return empty string for 0

  if (i > 0)
    s = s.setNum(i, 10);

  return s;
}




