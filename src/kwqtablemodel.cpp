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
#include "prefs.h"

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
  else if (role == Qt::FontRole)
    return QVariant(Prefs::editorFont());
  else if (role != Qt::DisplayRole)
    return QVariant();

  QVariant result;
  if (index.column() == 0)
    result = m_doc->entry(index.row())->original();
  else
    result = m_doc->entry(index.row())->translation(1);

  if (result.toString().isEmpty())
    result = "@empty@";

  return result;
}

QVariant KWQTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (role != Qt::DisplayRole)
      return QVariant();
    else if (section == 0)
      return m_doc->originalIdentifier();
    else
      return m_doc->identifier(1);
  }
  else
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags KWQTableModel::flags(const QModelIndex & index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool KWQTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (index.isValid() && role == Qt::EditRole) {
    if (index.column() == 0)
      m_doc->entry(index.row())->setOriginal(value.toString());
    else
      m_doc->entry(index.row())->setTranslation(1, value.toString());

    emit dataChanged(index, index);
    m_doc->setModified(true);
    return true;
  }
  return false;
}

int KWQTableModel::columnWidth(int column) const
{
  return 500 /*m_doc->sizeHint(column)*/;
}

bool KWQTableModel::insertRows(int row, int count, const QModelIndex & parent)
{
  Q_UNUSED(parent);
  if (count < 1 || row < 0 || row > m_doc->numEntries())
    return false;

  beginInsertRows(QModelIndex(), row, row + count - 1);

  for (int i = row; i < row + count; i++)
    m_doc->insertEntry(new KEduVocExpression, i);

  endInsertRows();
  m_doc->setModified(true);
  return true;
}

bool KWQTableModel::removeRows(int row, int count, const QModelIndex & parent)
{
  Q_UNUSED(parent);
  if (count < 1 || row < 0 || row + count > m_doc->numEntries() || count >= m_doc->numEntries())
    return false;

  int bottomRow = row + count -1;
  beginRemoveRows(QModelIndex(), row, row + count - 1);

  for (int i = bottomRow; i >= row; i--)
    m_doc->removeEntry(i);

  endRemoveRows();
  m_doc->setModified(true);
  return true;
}

#include "kwqtablemodel.moc"
