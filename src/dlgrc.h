//
// C++ Interface: dlgrc
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#ifndef DLGRC_H
#define DLGRC_H

#include <kdialogbase.h>

#include <dlgrcbase.h>

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
