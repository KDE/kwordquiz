/***************************************************************************
                               kwqsortfiltermodel.cpp
                             -------------------

    copyright            : (C) 2007-2010 by Peter Hedlund
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

#include <KRandomSequence>

KWQSortFilterModel::KWQSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_sourceModel = 0;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    m_restoreNativeOrder = false;
    m_shuffle = false;
}

void KWQSortFilterModel::setTableModel(KWQTableModel * sourceModel)
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
    if (m_shuffle)
        return m_shuffleList.at(right.row()) < m_shuffleList.at(left.row());
    else if (m_restoreNativeOrder)
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

void KWQSortFilterModel::shuffle()
{
    m_shuffleList.clear();
    for (int i = 0; i < rowCount(QModelIndex()); ++i)
        m_shuffleList.append(i);

    KRandomSequence rs;
    rs.randomize(m_shuffleList);
    m_shuffle = true;
    sort(0, Qt::AscendingOrder);
    invalidate();
    m_shuffle = false;
}
