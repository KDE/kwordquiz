/***************************************************************************
                          flashview.h -  description
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
