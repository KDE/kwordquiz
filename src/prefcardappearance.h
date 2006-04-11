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

#include <ui_prefcardappearancebase.h>

/**
@author Peter Hedlund
*/
class PrefCardAppearance : public QWidget, public Ui::PrefCardAppearanceBase
{
Q_OBJECT
public:
  PrefCardAppearance(QWidget *parent);

public slots:
  void slotFlipButtonClicked();
  void slotFontChanged(const QFont &);
  void slotTextColorChanged(const QColor &);
  void slotCardColorChanged(const QColor &);
  void slotFrameColorChanged(const QColor &);
  void slotCurrentChanged(int);
};

#endif
