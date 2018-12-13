/***************************************************************************
                              kwqtabledelegate.h
                             -------------------

    begin                : Wed Mar 1 19:17:30 PST 2006
    copyright            : (C) 2006-2010 by Peter Hedlund
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

#ifndef KWQTABLEDELEGATE_H
#define KWQTABLEDELEGATE_H

#include <QItemDelegate>

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/

class KWQTableDelegate : public QItemDelegate
{
  Q_OBJECT
  public:
    explicit KWQTableDelegate(QObject *parent = nullptr);

    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

  protected:
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const override;
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const override;
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap ) const override;

  private slots:
    void commitAndCloseEditor();
};

#endif
