/*
    SPDX-FileCopyrightText: 2004-2007 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

public Q_SLOTS:
  void slotFlipButtonClicked();
  void slotFontChanged(const QFont &);
  void slotTextColorChanged(const QColor &);
  void slotCardColorChanged(const QColor &);
  void slotFrameColorChanged(const QColor &);
  void slotCurrentChanged(int);
};

#endif
