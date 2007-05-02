/***************************************************************************
                               kwqsortfiltermodel.h
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
