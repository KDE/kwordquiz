//
// C++ Interface: prefquiz
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PREFQUIZ_H
#define PREFQUIZ_H

#include <prefquizbase.h>

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
