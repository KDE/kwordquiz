/* This file is part of KWordQuiz
  Copyright (C) 2004-2005 Peter Hedlund <peter.hedlund@kdemail.net>

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

#ifndef WQPRINTDIALOGPAGE_H
#define WQPRINTDIALOGPAGE_H

#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>

/**
Print dialog page to provide specific print options for KWordQuiz

@author Peter Hedlund
*/
class WQPrintDialogPage : public QWidget
{
Q_OBJECT
public:
  WQPrintDialogPage(QWidget *parent = 0);

  enum PrintStyle
  {
    List,
    Exam,
    Flashcard
  };

  WQPrintDialogPage::PrintStyle printStyle();
  void setPrintStyle(WQPrintDialogPage::PrintStyle style);

private:
  QGroupBox * g;
  QRadioButton * rb0;
  QRadioButton * rb1;
  QRadioButton * rb2;
};

#endif
