/* This file is part of KWordQuiz
  Copyright (C) 2003 Peter Hedlund <peter.hedlund@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

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
