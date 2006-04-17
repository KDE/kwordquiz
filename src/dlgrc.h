/***************************************************************************
                          dlglrc.h  -  description
                             -------------------
   copyright            : (C) 2003-2006 by Peter Hedlund
   email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGRC_H
#define DLGRC_H

#include <kdialog.h>

#include "ui_dlgrcbase.h"

/**
@author Peter Hedlund
*/
class DlgRC : public KDialog
{
Q_OBJECT
public:
  DlgRC(QWidget *parent = 0);

  void setNumRows(int r);
  void setRowHeight(int h);
  void setColWidth(int w);
  int numRows();
  int rowHeight();
  int colWidth();

private:
  Ui::DlgRCBase *dlgBase;

};

#endif
