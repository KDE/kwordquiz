/* This file is part of KWordQuiz
  Copyright (C) 2003 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <qcheckbox.h>
#include <q3buttongroup.h>

#include <knuminput.h>

#include "prefquiz.h"

PrefQuiz::PrefQuiz(QWidget *parent) : QWidget(parent)
{
  setupUi(this);

  connect(kcfg_AutoFlip, SIGNAL(toggled(bool)), kcfg_FlipDelay, SLOT(setEnabled(bool)));
  connect(kcfg_AutoFlip, SIGNAL(toggled(bool)), kcfg_KeepDiscard, SLOT(setEnabled(bool)));

  kcfg_FlipDelay -> setEnabled(kcfg_AutoFlip->isChecked());
  kcfg_KeepDiscard->setEnabled(kcfg_AutoFlip->isChecked());
  //optCorrect -> setEnabled(kcfg_AutoFlip->isChecked());
  //optError -> setEnabled(kcfg_AutoFlip->isChecked());
}

#include "prefquiz.moc"
