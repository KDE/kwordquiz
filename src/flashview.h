//
// C++ Interface: flashview
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef FLASHVIEW_H
#define FLASHVIEW_H



#include "flashviewbase.h"

#include "wqscore.h"

class WQQuiz;

/**
@author Peter Hedlund
*/
class FlashView : public FlashViewBase
{
Q_OBJECT
public:
    FlashView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

    ~FlashView();

    void setQuiz(WQQuiz *quiz);
    void init();

private slots:
    void slotTimer();

public slots:
    void slotFlip();
    void slotKnow();
    void slotDontKnow();
    void slotRepeat();
    void slotRestart();
    void slotApplySettings();

private:
    WQQuiz *m_quiz;
    WQScore *m_score;

    KAction* actFlip;
    KAction* actKnow;
    KAction* actDontKnow;
    KAction* actRepeat;
    KAction* actRestart;

    QTimer* m_timer;

    int m_question;
    int m_error;
    bool m_showFirst;
    bool m_soundFeedBack;
    bool m_flipAuto;
    bool m_keepDiscard;

    void showFront(int i);
    void showBack(int i);
    void keepDiscardCard(bool keep);
    void updateScore();
};

#endif
