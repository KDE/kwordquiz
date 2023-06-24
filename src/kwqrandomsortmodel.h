// SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QSortFilterProxyModel>

class KWQCardModel;

/// Randomize the order of the items contained inside the KWQCardModel
///
/// \author Peter Hedlund <peter.hedlund@kdemail.net>
class KWQRandomSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(KWQCardModel *cardModel READ cardModel WRITE setCardModel NOTIFY cardModelChanged)

public:
    explicit KWQRandomSortModel(QObject *parent = nullptr);

    KWQCardModel *cardModel() const;
    void setCardModel(KWQCardModel * sourceModel);

public Q_SLOTS:
    void restoreNativeOrder();
    void shuffle();

Q_SIGNALS:
    void cardModelChanged();

protected:
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const override;

    // Disable from public calls; use setTableModel instead.
    //void setSourceModel(QAbstractItemModel *sourceModel);

private:
    KWQCardModel * m_sourceModel;

    bool m_restoreNativeOrder;
    bool m_shuffle;
    QList<int> m_shuffleList;
};
