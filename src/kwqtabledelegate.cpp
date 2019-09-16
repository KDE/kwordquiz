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
  editor->setFont(index.data(Qt::FontRole).value<QFont>());

  //connect(editor, SIGNAL(returnPressed()), this, SLOT(commitAndCloseEditor()));
  return editor;
}

void KWQTableDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  lineEdit->setText(index.data(Qt::DisplayRole).toString());
}

void KWQTableDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  model->setData(index, lineEdit->text());
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

void KWQTableDelegate::drawDecoration(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QPixmap& pixmap) const
{
  if (pixmap.isNull() || !rect.isValid())
    return;
  QPoint p = QStyle::alignedRect(option.direction, option.decorationAlignment, pixmap.size(), rect).topLeft();
  QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if (option.state & QStyle::State_Selected) {
    painter->fillRect(rect, option.palette.brush(cg, option.state & QStyle::State_HasFocus ?
          QPalette::Base : QPalette::Highlight));
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    const QPixmap pm = selectedPixmap(pixmap, option.palette, option.state & QStyle::State_Enabled);
    painter->drawPixmap(p, pm);
#else
    QPixmap *pm = selected(pixmap, option.palette, option.state & QStyle::State_Enabled);
    painter->drawPixmap(p, *pm);
#endif
  } else {
    painter->drawPixmap(p, pixmap);
  }
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
    painter->drawText(textRect, option.displayAlignment | Qt::TextWordWrap, text);
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
  painter->save();
  drawBackground(painter, option, index);

  QStyleOptionViewItem opt = option;
  opt.decorationSize = QSize(0, 0);
  QPixmap decorationPixmap;
  QVariant decorationVariant = index.data(Qt::DecorationRole);
  if (!decorationVariant.isNull()) {
    decorationPixmap = decoration(option, decorationVariant);
    opt.decorationPosition = QStyleOptionViewItem::Right;
    opt.decorationAlignment = Qt::AlignRight | Qt::AlignVCenter;
    opt.decorationSize = QSize(decorationPixmap.width(), decorationPixmap.height());
    drawDecoration(painter, opt, opt.rect, decorationPixmap);
  }

  opt.rect.adjust(0, 0, -opt.decorationSize.width(), 0);
  const KWQTableModel *model = static_cast<const KWQTableModel *>(index.model());

  if (!model->checkBlanksSyntax(index.data(Qt::DisplayRole).toString())) {
    QPalette::ColorGroup cg = QPalette::Normal;
    opt.palette.setColor(cg, QPalette::Text, Qt::red);
  }
  opt.font = index.data(Qt::FontRole).value<QFont>();
  drawDisplay(painter, opt, opt.rect, index.data(Qt::DisplayRole).toString());
  if (!index.data(KWQTableModel::SoundRole).isNull())
    painter->fillRect(option.rect.right() - 3, option.rect.top(), 4, 4, Qt::red);
  drawFocus(painter, opt, option.rect);

  painter->restore();
}
