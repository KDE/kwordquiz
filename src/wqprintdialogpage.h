/*
    SPDX-FileCopyrightText: 2004-2008 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef WQPRINTDIALOGPAGE_H
#define WQPRINTDIALOGPAGE_H

#include <QGroupBox>
#include <QButtonGroup>

#include "prefs.h"

/**
Print dialog page to provide specific print options for KWordQuiz

@author Peter Hedlund
*/
class WQPrintDialogPage : public QWidget
{
Q_OBJECT
public:
  explicit WQPrintDialogPage(QWidget *parent = nullptr);

  int printStyle();
  void setPrintStyle(int style);

private:
  QGroupBox *g;
  QButtonGroup *bg;
};

#endif
