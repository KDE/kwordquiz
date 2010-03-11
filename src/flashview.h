/***************************************************************************
                                 flashview.h
                             -------------------
   copyright            : (C) 2003-2010 by Peter Hedlund
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

#include "ui_flashviewbase.h"

#include <KActionCollection>

class KWQQuizModel;

/**
@author Peter Hedlund
*/
class FlashView : public QWidget, public Ui::FlashViewBase
{
Q_OBJECT
public:
    FlashView(QWidget *parent, KActionCollection *actionCollection);

    void setQuiz(KWQQuizModel *quiz);
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
    KWQQuizModel *m_quiz;
    KActionCollection *m_actionCollection;

    QTimer* m_timer;

    bool m_showFirst;
    bool m_soundFeedBack;
    bool m_flipAuto;
    bool m_keepDiscard;

    void keepDiscardCard(bool keep);
};

#endif
