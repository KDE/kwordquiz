/***************************************************************************
                               kwqsortfiltermodel.cpp
                             -------------------

    copyright            : (C) 2007 by Peter Hedlund
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

#include "kwqsortfiltermodel.h"
#include "kwqtablemodel.h"

KWQSortFilterModel::KWQSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_sourceModel = 0;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    m_restoreNativeOrder = false;
}

void KWQSortFilterModel::setSourceModel(KWQTableModel * sourceModel)
{
    m_sourceModel = sourceModel;
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

KWQTableModel * KWQSortFilterModel::sourceModel() const
{
    return m_sourceModel;
}

bool KWQSortFilterModel::lessThan(const QModelIndex & left, const QModelIndex & right) const
{
    if (m_restoreNativeOrder)
        return sourceModel()->index(right.row(),  right.column(),  QModelIndex()).row() <
               sourceModel()->index(left.row(), left.column(), QModelIndex()).row();
    else
        return QSortFilterProxyModel::lessThan(left, right);
}

void KWQSortFilterModel::restoreNativeOrder()
{
    m_restoreNativeOrder = true;
    sort(-1, Qt::AscendingOrder);
    invalidate();
    m_restoreNativeOrder = false;
}

bool KWQSortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index;
    for (int i = 0 ; i < 2; i++) {
        index = sourceModel()->index(sourceRow, i, sourceParent);
        if (sourceModel()->data(index, Qt::DisplayRole).toString().contains(filterRegExp()))
            return true;
    }
    return false;
}

#include "kwqsortfiltermodel.moc"
