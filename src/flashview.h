/*
    SPDX-FileCopyrightText: 2003-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
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

private Q_SLOTS:
    void slotTimer();

public Q_SLOTS:
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
