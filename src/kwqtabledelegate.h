/*
    SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

  private Q_SLOTS:
    void commitAndCloseEditor();
};

#endif
