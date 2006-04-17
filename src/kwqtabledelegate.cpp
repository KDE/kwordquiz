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

#include "kdebug.h"

#include "kwqtabledelegate.h"
#include "kwqtablemodel.h"

KWQTableDelegate::KWQTableDelegate(QObject * parent) : QItemDelegate(parent)
{
}

QWidget * KWQTableDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QLineEdit *editor = new QLineEdit(parent);
  editor->setFrame(false);
  editor->setFont(index.model()->data(index, Qt::FontRole).value<QFont>());
  //editor->installEventFilter(const_cast<KWQTableDelegate*>(this));
  connect(editor, SIGNAL(returnPressed()), this, SLOT(commitAndCloseEditor()));
  return editor;
}

void KWQTableDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QString value = index.model()->data(index, Qt::DisplayRole).toString();

  if (value == "@empty@")
    value = "";

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

void KWQTableDelegate::commitAndCloseEditor()
{
  QWidget *editor = qobject_cast<QWidget *>(sender());
  kDebug() << "Commiting and closing";
  emit commitData(editor);
  emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
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
    if (str == "@empty@")
      str = "";

    if (painter->fontMetrics().width(text) > textRect.width() && !text.contains(QLatin1Char('\n')))
        str = elidedText(option.fontMetrics, textRect.width(), option.textElideMode, text);
#warning a troll told me that function is not public so you can NOT use it (moreover it does not link when using it) FIX !! It links just fine for me. I will rewrite this later
    qt_format_text(option.font, textRect, option.displayAlignment, str, 0, 0, 0, 0, painter);
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
    painter->drawRect(rect.adjusted(0,0,-1,-1));
    painter->drawRect(rect.adjusted(1,1,-2,-2));
    painter->restore();
  }
}

QSize KWQTableDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  return QSize(300, 25);
}

void KWQTableDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QItemDelegate::paint(painter, option, index);
}

#include "kwqtabledelegate.moc"
