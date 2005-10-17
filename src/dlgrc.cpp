/***************************************************************************
                          dlglrc.cpp  -  description
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

#include <klocale.h>
#include <krestrictedline.h>

#include "dlgrc.h"

DlgRC::DlgRC(QWidget *parent, const char *name, bool modal): KDialogBase(Swallow, i18n("Rows & Columns"), Ok|Cancel, Ok, parent, name, modal, true)
{
  QWidget w;
  //Ui::DlgRCBase ui;
  dlgBase->setupUi(&w);

  //dlgBase = new DlgRCBase( this, "Dlg" );
  setMainWidget(&w /*dlgBase*/);
  dlgBase->txtNumRows->setFocus();
}

void DlgRC::setNumRows( int r )
{
  dlgBase->txtNumRows->setValue(r);
}

void DlgRC::setRowHeight( int h )
{
  dlgBase->txtRowHeight->setValue(h);
}

void DlgRC::setColWidth( int w )
{
  dlgBase->txtColWidth->setValue(w);
}

int DlgRC::numRows( )
{
  return dlgBase->txtNumRows->value();
}

int DlgRC::rowHeight( )
{
  return dlgBase->txtRowHeight->value();
}

int DlgRC::colWidth( )
{
  return dlgBase->txtColWidth->value();
}


#include "dlgrc.moc"
