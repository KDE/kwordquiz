/***************************************************************************
                          dlgspecchar.h  -  description
                             -------------------
   copyright       : (C) 2003-2009 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

signals:
  void insertChar(QChar);

private slots:
  void slotUser1();
  void slotDoubleClicked();

private:
  QChar chr();

  KCharSelect *m_charSelect;
};

#endif
