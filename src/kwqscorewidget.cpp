/***************************************************************************
                               kwqscorewidget.cpp
                             -------------------

    copyright            : (C) 2009-2010 by Peter Hedlund
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

#include "kwqscorewidget.h"

#include <QIcon>

KWQScoreWidget::KWQScoreWidget(QWidget *parent) : QWidget(parent)
{
  setupUi(this);
  m_percent = false;
  clear();
}

void KWQScoreWidget::clear()
{
  m_questionCount = 0;
  m_error = 0;
  m_correct = 0;
  m_answerCount = 0;
  picAnswered->clear();
  picCorrect->clear();
  picError->clear();
  update();
}

void KWQScoreWidget::setAsPercent(bool p)
{
  m_percent = p;
  update();
}

void KWQScoreWidget::setQuestionCount(int c)
{
  clear();
  m_questionCount = c;
  update();
}

void KWQScoreWidget::countIncrement(CountDirection d)
{
  m_answerCount++;
  if (d == cdCorrect)
    m_correct++;
  if (d == cdError)
    m_error++;
  update();
}

/*!
    \fn KWQScoreWidget::answerText()
 */
QString KWQScoreWidget::answerText()
{
  QString s;
  if (m_percent) {
    float f = ((m_answerCount * 100.0) / m_questionCount);
    if (m_answerCount > 0) {
      if (f < 1)
        s = s.setNum(f, 'f', 1) + '%'; //for long lists (i.e. each question is less than 1%) we show one decimal
      else
        s = s.setNum(f, 'f', 0) + '%';
    }
    else
      s = valueToString(m_answerCount);
  }
  else
    s = valueToString(m_answerCount);
  return s;
}

/*!
    \fn KWQScoreWidget::correctText()
 */
QString KWQScoreWidget::correctText()
{
  QString s;
  if (m_percent) {
    if (m_correct > 0) {
      float f = ((m_correct * 100.0) / m_answerCount);
      s = s.setNum(f, 'f', 0) + '%';
    }
    else
      s = valueToString(m_correct);
  }
  else
    s = valueToString(m_correct);
  return s;
}

/*!
    \fn KWQScoreWidget::errorText()
 */
QString KWQScoreWidget::errorText()
{
  QString s;
  if (m_percent) {
    if (m_error > 0) {
      float f = ((m_error * 100.0) / m_answerCount);
      s = s.setNum(f, 'f', 0) + '%';
    }
    else
      s = valueToString(m_error);
  }
  else
    s = valueToString(m_error);
  return s;
}

/*!
    \fn KWQScoreWidget::valueToString(int i)
 */
QString KWQScoreWidget::valueToString(int i)
{
  QString s;
  s.clear(); //return empty string for 0

  if (i > 0)
    s = s.setNum(i, 10);

  return s;
}

void KWQScoreWidget::update()
{
  QString s;
  s = s.setNum(m_questionCount, 10);
  lblScoreCount->setText(s);
  picCount->setPixmap(QIcon::fromTheme(QStringLiteral("kwordquiz")).pixmap(32));

  s = answerText();
  lblScoreAnswered->setText(s);
  if (!s.isEmpty())
    picAnswered->setPixmap(QIcon::fromTheme(QStringLiteral("question")).pixmap(32));

  s = correctText();
  lblScoreCorrect->setText(s);
  if (!s.isEmpty())
    picCorrect->setPixmap(QIcon::fromTheme(QStringLiteral("answer-correct")).pixmap(32));

  s = errorText();
  lblScoreError->setText(s);
  if (s.isEmpty())
    picError->setPixmap(QPixmap());
  else
    picError->setPixmap(QIcon::fromTheme(QStringLiteral("error")).pixmap(32));
}

void KWQScoreWidget::swapCount()
{
  m_error--;
  m_correct++;
  update();
}
