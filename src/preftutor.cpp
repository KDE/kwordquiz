/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "preftutor.h"

PrefTutor::PrefTutor(QWidget *parent) : QWidget(parent)
{
  setupUi(this);
  connect(kcfg_TutorTiming, &QGroupBox::clicked, this, &PrefTutor::timingChanged);
}

void PrefTutor::timingChanged (int id)
{
  kcfg_TutorEvery->setEnabled(id == 0);
  kcfg_TutorMin->setEnabled(id != 0);
  kcfg_TutorMax->setEnabled(id != 0);
}

void PrefTutor::minChanged(int newValue)
{
 if (newValue >= kcfg_TutorMax->value())
   kcfg_TutorMin->setValue(kcfg_TutorMax->value()-1);
}

void PrefTutor::maxChanged(int newValue)
{
 if (newValue <= kcfg_TutorMin->value())
   kcfg_TutorMax->setValue(kcfg_TutorMin->value()+1);
}
