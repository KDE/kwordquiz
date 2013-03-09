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

private slots:
  void timingChanged(int);
  void minChanged(int );
  void maxChanged(int );

};

#endif
