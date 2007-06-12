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

#include <klocale.h>

#include "prefs.h"
#include "keduvocexpression.h"

KWQTableModel::KWQTableModel(QObject * parent) : QAbstractTableModel(parent)
{
  m_doc = 0;
}

int KWQTableModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_doc->entryCount();
}

int KWQTableModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
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
    result = m_doc->entry(index.row())->translation(0).translation();
  else
    result = m_doc->entry(index.row())->translation(1).translation();

  return result;
}

QVariant KWQTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (role == Qt::DisplayRole) {
      if (section == 0)
        return m_doc->originalIdentifier();
      else
        return m_doc->identifier(1);
    }
    if (role == Qt::SizeHintRole)
      return QSize(m_doc->sizeHint(section), 25);
    return QVariant();
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
      m_doc->entry(index.row())->setTranslation(0, value.toString());
    else
      m_doc->entry(index.row())->setTranslation(1, value.toString());

    emit dataChanged(index, index);
    m_doc->setModified(true);
    return true;
  }
  return false;
}

bool KWQTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
  if (orientation == Qt::Horizontal) {
    if (role == Qt::EditRole) {
      if (section == 0)
        m_doc->setOriginalIdentifier(value.toString());
      if (section == 1)
        m_doc->setIdentifier(1, value.toString());
    }
    if (role == Qt::SizeHintRole)
      m_doc->setSizeHint(section, qvariant_cast<QSize>(value).width());

    emit headerDataChanged(orientation, section, section);
    m_doc->setModified(true);
    return true;
  }
  return false;
}

bool KWQTableModel::insertRows(int row, int count, const QModelIndex & parent)
{
  Q_UNUSED(parent);
  if (count < 1 || row < 0 || row > m_doc->entryCount())
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
  if (count < 1 || row < 0 || row + count > m_doc->entryCount() || count >= m_doc->entryCount())
    return false;

  int bottomRow = row + count -1;
  beginRemoveRows(QModelIndex(), row, row + count - 1);

  for (int i = bottomRow; i >= row; i--)
    m_doc->removeEntry(i);

  endRemoveRows();
  m_doc->setModified(true);
  return true;
}

void KWQTableModel::shuffle( )
{
  m_doc->shuffle();
  reset();
}

void KWQTableModel::setDocument(KEduVocDocument * doc)
{
  m_doc = doc;
  reset();
}

bool KWQTableModel::isEmpty()
{
  if (m_doc->url().fileName() == i18n("Untitled")){
    int rc = rowCount(QModelIndex());
    for (int i = 0; i < rc; i++)
      if (!data(index(i, 0), Qt::DisplayRole).toString().isEmpty() || !data(index(i, 1), Qt::DisplayRole).toString().isEmpty())
        return false;

    return true;
  }
  else
    return false;
}

#include "kwqtablemodel.moc"
