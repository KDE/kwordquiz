/***************************************************************************
                               kwqtabledelegate.h
                             -------------------

    begin                : Wed Mar 1 19:17:30 PST 2006
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

#include <QtGui>

#include "kwqtabledelegate.h"

KWQTableDelegate::KWQTableDelegate(QObject * parent) : QItemDelegate(parent)
{
}

QWidget * KWQTableDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index */) const
{
  QLineEdit *editor = new QLineEdit(parent);
  editor->setFrame(false);
  //editor->installEventFilter(const_cast<KWQTableDelegate*>(this));
  connect(editor, SIGNAL(returnPressed()), this, SLOT(commitAndCloseEditor()));
  return editor;
}

void KWQTableDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QString value = index.model()->data(index, Qt::DisplayRole).toString();

  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  lineEdit->setText(value);
}

void KWQTableDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  QString value = lineEdit->text();

  model->setData(index, value);
}

void KWQTableDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & /*index*/) const
{
  editor->setGeometry(option.rect);
}

void KWQTableDelegate::commitAndCloseEditor( )
{
  QWidget *editor = qobject_cast<QWidget *>(sender());
  emit commitData(editor);
  emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
}

#include "kwqtabledelegate.moc"
