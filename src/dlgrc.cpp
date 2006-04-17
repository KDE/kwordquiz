/***************************************************************************
                          dlgrc.cpp  -  description
                             -------------------
   copyright       : (C) 2003-2006 Peter Hedlund <peter.hedlund@kdemail.net>

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

DlgRC::DlgRC(QWidget *parent): KDialog(parent, i18n("Rows & Columns"), Ok|Cancel)
{
  QWidget * w = new QWidget();
  setMainWidget(w);
  dlgBase = new Ui::DlgRCBase();
  dlgBase->setupUi(mainWidget());
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
