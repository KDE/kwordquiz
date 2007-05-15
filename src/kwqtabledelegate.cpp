/***************************************************************************
                               kwqtabledelegate.h
                             -------------------

    begin                : Wed Mar 1 19:17:30 PST 2006
    copyright            : (C) 2006-2007 by Peter Hedlund
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

#include "kwqtabledelegate.h"

#include <QLineEdit>
#include <QPainter>

#include "kwqtablemodel.h"

KWQTableDelegate::KWQTableDelegate(QObject * parent) : QItemDelegate(parent)
{
}

QWidget * KWQTableDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  Q_UNUSED(option);
  QLineEdit *editor = new QLineEdit(parent);
  editor->setFrame(false);
  editor->setFont(index.model()->data(index, Qt::FontRole).value<QFont>());
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

void KWQTableDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  Q_UNUSED(index);
  editor->setGeometry(option.rect);
}

void KWQTableDelegate::commitAndCloseEditor()
{
  QWidget *editor = qobject_cast<QWidget *>(sender());

  emit commitData(editor);
  emit closeEditor(editor, QAbstractItemDelegate::NoHint);
}

void KWQTableDelegate::drawDisplay(QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text) const
{
    QPen pen = painter->pen();
    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(rect, option.palette.brush(cg, option.state & QStyle::State_HasFocus ?
          QPalette::Base : QPalette::Highlight));
        painter->setPen(option.palette.color(cg, option.state & QStyle::State_HasFocus ?
          QPalette::Text : QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }

    if (option.state & QStyle::State_Editing) {
        painter->save();
        painter->setPen(option.palette.color(cg, QPalette::Text));
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        painter->restore();
    }

    QFont font = painter->font();
    painter->setFont(option.font);
    QRect textRect = rect.adjusted(3, 0, -3, 0); // remove width padding
    QString str = text;

    painter->drawText(textRect, option.displayAlignment | Qt::TextWordWrap, str);
    painter->setFont(font);
    painter->setPen(pen);
}


void KWQTableDelegate::drawFocus(QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect) const
{
  if (option.state & QStyle::State_HasFocus) {
    painter->save();
    QPen pen = painter->pen();
    pen.setColor(Qt::black);
    pen.setWidth(0);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
    painter->drawRect(rect.adjusted(1, 1, -2, -2));
    painter->restore();
  }
}


void KWQTableDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QItemDelegate::paint(painter, option, index);
}

#include "kwqtabledelegate.moc"
