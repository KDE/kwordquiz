/*
    SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kwqsortfiltermodel.h"
#include "kwqtablemodel.h"

#include <KRandom>

KWQSortFilterModel::KWQSortFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_sourceModel = nullptr;
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

    KRandom::shuffle(m_shuffleList, QRandomGenerator::global());
    m_shuffle = true;
    sort(0, Qt::AscendingOrder);
    invalidate();
    m_shuffle = false;
}

#include "moc_kwqsortfiltermodel.cpp"
