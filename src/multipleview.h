/*
    This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2003-2010 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef MULTIPLEVIEW_H
#define MULTIPLEVIEW_H

#include "ui_multipleviewbase.h"

#include <KActionCollection>
#include "kwqquizview.h"

class QActionGroup;
class QButtonGroup;

/**
@author Peter Hedlund
*/
class MultipleView : public KWQQuizView, public Ui::MultipleViewBase
{
Q_OBJECT
public:
    explicit MultipleView(QWidget *parent, KActionCollection *actionCollection);
    void init() override;

public Q_SLOTS:
    void slotCheck() override;
    void slotChoiceClicked(int );
    void slotChoiceActionTriggered(QAction *);
    void slotApplySettings() override;

private:
    QActionGroup *m_choicesActions;
    QButtonGroup *m_choicesButtons;
    QStringList m_choices;

    void showQuestion();
};

#endif
