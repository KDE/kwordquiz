/*
    This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2003-2010 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef QAVIEW_H
#define QAVIEW_H

#include "ui_qaviewbase.h"

#include <KActionCollection>
#include <phonon/MediaObject>
#include <phonon/Path>
#include <phonon/AudioOutput>
#include <phonon/Global>
#include "kwqquizview.h"


/**
@author Peter Hedlund
*/
class QAView : public KWQQuizView, public Ui::QAViewBase
{
Q_OBJECT
public:
    QAView(QWidget *parent, KActionCollection * actionCollection);

    void init() override;

public Q_SLOTS:
    void slotCheck() override;
    void slotHint();
    void slotApplySettings() override;
    void slotSpecChar(const QChar &);
    void slotMarkLastCorrect();

private:
    bool m_hintUsed;

    void showQuestion();
};

#endif
