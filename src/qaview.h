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

public slots:
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
