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

#include <qcheckbox.h>
#include <qradiobutton.h>

#include <krestrictedline.h>

#include "prefquiz.h"


PrefQuiz::PrefQuiz(QWidget *parent, const char *name, WFlags f)
 : PrefQuizBase(parent, name, f)
{
}


PrefQuiz::~PrefQuiz()
{
}

/*!
    \fn PrefQuiz::slotAutoFlipClicked()
 */
void PrefQuiz::slotAutoFlipClicked()
{
  txtSeconds -> setEnabled(chkAutoFlip->isChecked());
  optCorrect -> setEnabled(chkAutoFlip->isChecked());
  optError -> setEnabled(chkAutoFlip->isChecked());
}

#include "prefquiz.moc"
