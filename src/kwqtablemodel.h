/***************************************************************************
                               kwqtablemodel.h
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

#ifndef KWQTABLEMODEL_H
#define KWQTABLEMODEL_H

#include <QAbstractTableModel>

#include <keduvocdocument.h>

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/
class KWQTableModel : public QAbstractTableModel
{
Q_OBJECT
public:
  KWQTableModel(KEduVocDocument * doc, QObject * parent);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  //QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
  KEduVocDocument * m_doc;
};

#endif
