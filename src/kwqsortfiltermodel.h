/*
    SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWQSORTFILTERMODEL_H
#define KWQSORTFILTERMODEL_H

#include <QSortFilterProxyModel>

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/

class KWQTableModel;

class KWQSortFilterModel : public QSortFilterProxyModel
{
  Q_OBJECT
  public:
    explicit KWQSortFilterModel(QObject *parent = nullptr);

    void setTableModel(KWQTableModel * sourceModel);
    KWQTableModel * sourceModel() const;

  public Q_SLOTS:
    void restoreNativeOrder();
    void shuffle();

  protected:
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const override;

    // Disable from public calls; use setTableModel instead.
    //void setSourceModel(QAbstractItemModel *sourceModel);

  private:
    KWQTableModel * m_sourceModel;

    bool m_restoreNativeOrder;
    bool m_shuffle;
    QList<int> m_shuffleList;
};

#endif
