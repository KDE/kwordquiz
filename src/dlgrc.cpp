//
// C++ Implementation: dlgrc
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#include <klocale.h>
#include <krestrictedline.h>

#include "dlgrc.h"

DlgRC::DlgRC(QWidget *parent, const char *name, bool modal): KDialogBase(Swallow, i18n("Rows & Columns"), Ok|Cancel, Ok, parent, name, modal, true)
{
  dlgBase = new DlgRCBase( this, "Dlg" );
  setMainWidget(dlgBase);
  dlgBase->txtNumRows->setFocus();
}


DlgRC::~DlgRC()
{
}

void DlgRC::setNumRows( int r )
{
  dlgBase->txtNumRows->setText(QString::number(r, 10));
}

void DlgRC::setRowHeight( int h )
{
  dlgBase->txtRowHeight->setText(QString::number(h, 10));
}

void DlgRC::setColWidth( int w )
{
  dlgBase->txtColWidth->setText(QString::number(w, 10));
}

int DlgRC::numRows( )
{
  return dlgBase->txtNumRows->text().toInt();
}

int DlgRC::rowHeight( )
{
  return dlgBase->txtRowHeight->text().toInt();
}

int DlgRC::colWidth( )
{
  return dlgBase->txtColWidth->text().toInt();
}


#include "dlgrc.moc"
