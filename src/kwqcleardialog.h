/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWQCLEARDIALOG_H
#define KWQCLEARDIALOG_H

#include <QDialog>

#include "ui_cleardialogbase.h"

class KWQClearDialog : public QDialog, private Ui::ClearDialogBase {
  Q_OBJECT
  public:
    explicit KWQClearDialog(QWidget *parent = nullptr);

  public Q_SLOTS:
    void accept() override;

  private Q_SLOTS:
    void buttonGroupChanged(int );

  private:
    void updateCheckBoxes();

};

#endif // KWQCLEARDIALOG_H
