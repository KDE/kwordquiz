//
// C++ Implementation: prefquiz
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

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
