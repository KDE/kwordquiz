/*
    kwqscorewidget.h
    -------------------

    SPDX-FileCopyrightText: 2009-2010 Peter Hedlund
    email                : peter.hedlund@kdemail.net

*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

#ifndef KWQSCOREWIDGET_H
#define KWQSCOREWIDGET_H

#include "ui_kwqscorewidgetbase.h"

#include <QWidget>

class KWQScoreWidget : public QWidget, public Ui::KWQScoreWidgetBase
{
public:
  enum CountDirection {cdCorrect, cdError};
  explicit KWQScoreWidget(QWidget *parent);

  void clear();

  void setAsPercent(bool p);
  void setQuestionCount(int c);
  void countIncrement(CountDirection d);
  /* if Mark Last Correct is selected */
  void swapCount();

  QString answerText();
  QString correctText();
  QString errorText();

private:
  bool m_percent;
  int m_questionCount;
  int m_error;
  int m_correct;
  int m_answerCount;
  QString valueToString(int i);
  void update();
};

#endif // KWQSCOREWIDGET_H
