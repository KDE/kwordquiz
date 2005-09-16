//
// C++ Interface:
//
// Description: KWordQuiz flashcard appearance preferences
//
//
// Author: Peter Hedlund <peter.hedlund@kdemail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PREFCARDAPPEARANCE_H
#define PREFCARDAPPEARANCE_H

#include <prefcardappearancebase.h>

/**
@author Peter Hedlund
*/
class PrefCardAppearance : public PrefCardAppearanceBase
{
Q_OBJECT
public:
  PrefCardAppearance(QWidget *parent = 0, const char *name = 0);
  ~PrefCardAppearance();

public slots:
  void slotFlipButtonClicked();
  void slotFontChanged(const QFont &);
  void slotTextColorChanged(const QColor &);
  void slotCardColorChanged(const QColor &);
  void slotFrameColorChanged(const QColor &);
  void slotAboutToShowWidget(QWidget *);
};

#endif
