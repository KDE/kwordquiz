/*
    dlglanguage.h  -  description
    -------------------
    SPDX-FileCopyrightText: 2004-2009 Peter Hedlund <peter.hedlund@kdemail.net>

*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

#ifndef DLGLANGUAGE_H
#define DLGLANGUAGE_H

#include <QDialog>

#include "ui_columndialogbase.h"
#include "kwqcommands.h"

/**
@author Peter Hedlund
*/

class KWQTableModel;

class DlgLanguage : public QDialog, private Ui::ColumnDialogBase {
  Q_OBJECT
public:
  explicit DlgLanguage(KWQTableModel *model, QWidget *parent = nullptr);

  ColumnDataList columnData();
};

#endif
