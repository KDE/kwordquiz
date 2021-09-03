/*
    SPDX-FileCopyrightText: 2003-2007 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "prefquiz.h"

#include <QCheckBox>

PrefQuiz::PrefQuiz(QWidget *parent) : QWidget(parent)
{
  setupUi(this);

  connect(kcfg_AutoFlip, &QAbstractButton::toggled, kcfg_FlipDelay, &QWidget::setEnabled);
  connect(kcfg_AutoFlip, &QAbstractButton::toggled, kcfg_KeepDiscard, &QWidget::setEnabled);

  kcfg_FlipDelay -> setEnabled(kcfg_AutoFlip->isChecked());
  kcfg_KeepDiscard->setEnabled(kcfg_AutoFlip->isChecked());
}
