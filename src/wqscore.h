//
// C++ Interface: wqscore
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
