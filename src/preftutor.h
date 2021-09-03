/* This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef PREFTUTOR_H
#define PREFTUTOR_H

#include "ui_preftutorbase.h"

/**
@author Peter Hedlund
*/
class PrefTutor : public QWidget, public Ui::PrefTutorBase
{
Q_OBJECT
public:
  explicit PrefTutor(QWidget *parent);

private Q_SLOTS:
  void timingChanged(int);
  void minChanged(int );
  void maxChanged(int );

};

#endif
