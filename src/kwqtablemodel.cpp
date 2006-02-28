/***************************************************************************
                              kwqtablemodel.cpp
                             -------------------

    begin                : Mon Feb 27 18:27:30 PST 2006
    copyright            : (C) 2006 by Peter Hedlund
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

#include "kwqtablemodel.h"

KWQTableModel::KWQTableModel(KEduVocDocument * doc, QObject * parent) : QAbstractTableModel(parent)
{
  m_doc = doc;
}

int KWQTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return m_doc->numEntries();
}

int KWQTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2;
}

QVariant KWQTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();
  else if (role != Qt::DisplayRole)
    return QVariant();

  if (index.column() == 0)
    return m_doc->entry(index.row())->original();
  else
    return m_doc->entry(index.row())->translation(1);
}


