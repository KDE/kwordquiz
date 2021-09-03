/*
    SPDX-FileCopyrightText: 2008 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef PREFGENERAL_H
#define PREFGENERAL_H

#include "ui_prefgeneralbase.h"

/**
@author Peter Hedlund
*/
class PrefGeneral : public QWidget, public Ui::PrefGeneralBase
{
Q_OBJECT
public:
  explicit PrefGeneral(QWidget *parent);
};

#endif
