/* This file is part of KWordQuiz
  Copyright (C) 2003 Peter Hedlund <peter@peterandlinda.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#ifndef PREFQUIZ_H
#define PREFQUIZ_H

#include "prefquizbase.h"

/**
@author Peter Hedlund
*/
class PrefQuiz : public PrefQuizBase
{
Q_OBJECT
public:

  PrefQuiz(QWidget *parent, const char *name=0, WFlags f=0);
  ~PrefQuiz();

public slots:
    void slotAutoFlipClicked();
};

#endif
