/*
    This file is part of KWordQuiz
    Copyright (C) 2003-2010 Peter Hedlund <peter.hedlund@kdemail.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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

public slots:
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
