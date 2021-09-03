/* This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2003 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef PREFQUIZ_H
#define PREFQUIZ_H

#include "ui_prefquizbase.h"

/**
@author Peter Hedlund
*/
class PrefQuiz : public QWidget, public Ui::PrefQuizBase
{
Q_OBJECT
public:
  explicit PrefQuiz(QWidget *parent);

};

#endif
