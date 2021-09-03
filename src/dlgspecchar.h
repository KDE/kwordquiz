/*
    dlgspecchar.h  -  description
    -------------------
    SPDX-FileCopyrightText: 2003-2009 Peter Hedlund <peter.hedlund@kdemail.net>

*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

#ifndef DLGSPECCHAR_H
#define DLGSPECCHAR_H

#include <QDialog>

class QWidget;
class KCharSelect;

/**
@author Peter Hedlund
*/
class DlgSpecChar : public QDialog
{
Q_OBJECT
public:
  explicit DlgSpecChar(QWidget *parent, const QFont &font, const QChar &chr);
  void closeDialog();

Q_SIGNALS:
  void insertChar(QChar);

private Q_SLOTS:
  void slotUser1();
  void slotDoubleClicked();

private:
  QChar chr();

  KCharSelect *m_charSelect;
};

#endif
