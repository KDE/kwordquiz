//
// C++ Interface: kwqsortfiltermodel
//
// Description:
//
//
// Author:  (C) 2007 Peter Hedlund <peter.hedlund@kdemail.net>
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
    KWQSortFilterModel(QObject *parent = 0);

    void setSourceModel(KWQTableModel * sourceModel);
    KWQTableModel * sourceModel() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public slots:
    void restoreNativeOrder();

protected:
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const;

private:
    KWQTableModel * m_sourceModel;

    bool m_restoreNativeOrder;
};

#endif
