/*
    This file is part of KWordQuiz
    Copyright (C) 2010 Peter Hedlund <peter.hedlund@kdemail.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KWQCLEARDIALOG_H
#define KWQCLEARDIALOG_H

#include <QDialog>

#include "ui_cleardialogbase.h"

class KWQClearDialog : public QDialog, private Ui::ClearDialogBase {
  Q_OBJECT
  public:
    explicit KWQClearDialog(QWidget *parent = nullptr);

  public slots:
    void accept() override;

  private slots:
    void buttonGroupChanged(int );

  private:
    void updateCheckBoxes();

};

#endif // KWQCLEARDIALOG_H
