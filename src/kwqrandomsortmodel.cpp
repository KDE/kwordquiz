// SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqrandomsortmodel.h"
#include "kwqcardmodel.h"

#include <KRandom>

KWQRandomSortModel::KWQRandomSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_sourceModel = 0;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    m_restoreNativeOrder = false;
    m_shuffle = false;
}

void KWQRandomSortModel::setCardModel(KWQCardModel *sourceModel)
{
    if (m_sourceModel == sourceModel) {
        return;
    }
    m_sourceModel = sourceModel;
    QSortFilterProxyModel::setSourceModel(sourceModel);
    Q_EMIT cardModelChanged();
}

KWQCardModel *KWQRandomSortModel::cardModel() const
{
    return m_sourceModel;
}

bool KWQRandomSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (m_shuffle) {
        return m_shuffleList.at(right.row()) < m_shuffleList.at(left.row());
    } else if (m_restoreNativeOrder) {
        return sourceModel()->index(right.row(), right.column()).row() <
               sourceModel()->index(left.row(), left.column()).row();
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool KWQRandomSortModel::filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const
{
    if (!m_showErrorsOnly) {
        return true;
    }

    return m_errors.contains(sourceRow);
}

void KWQRandomSortModel::restoreNativeOrder()
{
    m_errors.clear();
    m_restoreNativeOrder = true;
    sort(-1, Qt::AscendingOrder);
    invalidate();
    m_restoreNativeOrder = false;
}

void KWQRandomSortModel::shuffle()
{
    m_errors.clear();
    m_shuffleList.clear();
    for (int i = 0; i < rowCount({}); ++i)
        m_shuffleList.append(i);

    KRandom::shuffle(m_shuffleList, QRandomGenerator::global());
    m_shuffle = true;
    sort(0, Qt::AscendingOrder);
    invalidate();
    m_shuffle = false;
}

void KWQRandomSortModel::markAsError(const int row)
{
    m_errors << sourceModel()->index(row, 0).row();
}

void KWQRandomSortModel::unMarkAsError(const int row)
{
    m_errors.removeAll(sourceModel()->index(row, 0).row());
}

bool KWQRandomSortModel::showErrorsOnly() const
{
    return m_showErrorsOnly;
}

void KWQRandomSortModel::setShowErrorsOnly(const bool showErrorsOnly)
{
    if (m_showErrorsOnly == showErrorsOnly) {
        return;
    }
    m_showErrorsOnly = showErrorsOnly;
    Q_EMIT showErrorsOnlyChanged();
    invalidateFilter();
}
