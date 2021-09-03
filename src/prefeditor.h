/*
    SPDX-FileCopyrightText: 2003 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef PREFEDITOR_H
#define PREFEDITOR_H

#include "ui_prefeditorbase.h"

/**
@author Peter Hedlund
*/
class PrefEditor : public QWidget, public Ui::PrefEditorBase
{
Q_OBJECT
public:
  explicit PrefEditor(QWidget *parent);

};

#endif
