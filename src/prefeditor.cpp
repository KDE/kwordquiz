/*
    SPDX-FileCopyrightText: 2003 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "prefeditor.h"

PrefEditor::PrefEditor(QWidget *parent) : QWidget(parent)
{
  setupUi(this);
}

#include "moc_prefeditor.cpp"
