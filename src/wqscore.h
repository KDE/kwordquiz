/***************************************************************************
                          wqscore.h  -  description
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

#ifndef WQSCORE_H
#define WQSCORE_H

#include <qwidget.h>

/**
@author Peter Hedlund
*/
class WQScore{
public:
    enum CountDirection {cdCorrect, cdError, cdNone};
    WQScore();

    ~WQScore();

    void setAsPercent(bool p);
    void setQuestionCount(int c);
    void countIncrement(CountDirection d);

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
};

#endif
