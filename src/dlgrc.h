/***************************************************************************
                          dlglrc.h  -  description
                             -------------------
   copyright            : (C) 2003 by Peter Hedlund
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

#include <kdialogbase.h>

#include "dlgrcbase.h"

/**
@author Peter Hedlund
*/
class DlgRC : public KDialogBase
{
Q_OBJECT
public:
  DlgRC(QWidget *parent = 0, const char *name = 0, bool modal=true);

  ~DlgRC();
  void setNumRows(int r);
  void setRowHeight(int h);
  void setColWidth(int w);
  int numRows();
  int rowHeight();
  int colWidth();

private:
  DlgRCBase *dlgBase;

};

#endif
