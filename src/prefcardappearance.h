/***************************************************************************
                          prefcardappearance.h
                             -------------------

    copyright            : (C) 2004-2007 by Peter Hedlund
    email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
  explicit PrefCardAppearance(QWidget *parent);

public slots:
  void slotFlipButtonClicked();
  void slotFontChanged(const QFont &);
  void slotTextColorChanged(const QColor &);
  void slotCardColorChanged(const QColor &);
  void slotFrameColorChanged(const QColor &);
  void slotCurrentChanged(int);
};

#endif
