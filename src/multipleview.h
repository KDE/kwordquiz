/* This file is part of KWordQuiz
  Copyright (C) 2003-2008 Peter Hedlund <peter.hedlund@kdemail.net>

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

#ifndef MULTIPLEVIEW_H
#define MULTIPLEVIEW_H

#include "ui_multipleviewbase.h"

#include <KActionCollection>

class KWQQuiz;
class WQScore;

/**
@author Peter Hedlund
*/
class MultipleView : public QWidget, public Ui::MultipleViewBase
{
Q_OBJECT
public:
    MultipleView(QWidget *parent, KActionCollection *actionCollection);

    void setQuiz(KWQQuiz *quiz);
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
    KWQQuiz *m_quiz;
    WQScore *m_score;
    KActionCollection *m_actionCollection;

    int m_question;
    int m_error;

    void updateScore();
    void showQuestion(int i);
};

#endif
