/* This file is part of KWordQuiz
  Copyright (C) 2010 Peter Hedlund <peter.hedlund@kdemail.net>

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
