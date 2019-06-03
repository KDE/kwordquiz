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

#ifndef FLASHVIEW_H
#define FLASHVIEW_H

#include "ui_flashviewbase.h"

#include <KActionCollection>
#include "kwqquizview.h"


/**
@author Peter Hedlund
*/
class FlashView : public KWQQuizView, public Ui::FlashViewBase
{
Q_OBJECT
public:
    explicit FlashView(QWidget *parent, KActionCollection *actionCollection);

    void init() override;

private slots:
    void slotTimer();

public slots:
    void slotCheck() override;
    void slotKnow();
    void slotDontKnow();
    void slotApplySettings() override;

private:
    QTimer* m_timer;
    bool m_showFirst;
    bool m_soundFeedBack;
    bool m_flipAuto;
    bool m_keepDiscard;

    void keepDiscardCard(bool keep);
};

#endif
