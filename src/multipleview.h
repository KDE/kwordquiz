//
// C++ Interface: multipleview
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef MULTIPLEVIEW_H
#define MULTIPLEVIEW_H

#include <multipleviewbase.h>

#include "wqquiz.h"
#include "wqscore.h"

/**
@author Peter Hedlund
*/
class MultipleView : public MultipleViewBase
{
Q_OBJECT
public:
    MultipleView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

    ~MultipleView();

    void setQuiz(WQQuiz *quiz);
    void init();

public slots:
    void slotCheck();
    void slotRepeat();
    void slotRestart();
    void slotOpt1Clicked();
    void slotOpt2Clicked();
    void slotOpt3Clicked();
    void slotApplySettings();

private:
    WQQuiz *m_quiz;
    WQScore *m_score;

    int m_question;
    int m_error;

    void updateScore();
    void showQuestion(int i);
};

#endif
