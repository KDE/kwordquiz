//
// C++ Interface: qaview
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QAVIEW_H
#define QAVIEW_H

#include <kaction.h>

#include "qaviewbase.h"

#include "wqquiz.h"
#include "wqscore.h"

/**
@author Peter Hedlund
*/
class QAView : public QAViewBase
{
Q_OBJECT
public:
    QAView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

    ~QAView();

    void setQuiz(WQQuiz *quiz);
    void init();

public slots:
    void slotCheck();
    void slotHint();
    void slotRepeat();
    void slotRestart();
    void slotApplySettings();
    void slotSpecChar(const QChar &);
    
private:
    WQQuiz *m_quiz;
    WQScore *m_score;

    KAction* actCheck;
    KAction* actHint;
    KAction* actRepeat;
    KAction* actRestart;

    int m_question;
    int m_error;

    bool m_hintUsed;

    void updateScore();
    void showQuestion(int i);
};

#endif
