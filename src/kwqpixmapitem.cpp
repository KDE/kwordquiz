/*
    kwqpixmapitem.cpp
    -------------------
    SPDX-FileCopyrightText: 2011 Peter Hedlund
    email                : peter.hedlund@kdemail.net
*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/


#include "kwqpixmapitem.h"

#include <QPainter>

KWQPixmapItem::KWQPixmapItem(const QPixmap &pixmap, QGraphicsItem *parentItem) : QGraphicsPixmapItem(pixmap, parentItem)
{
    setCacheMode(NoCache);
    setTransformationMode(Qt::SmoothTransformation);
}


QRectF KWQPixmapItem::boundingRect() const
{
    return m_imageRect;
}


void KWQPixmapItem::setImageRect(const QRect& rect)
{
    m_imageRect = rect;
}


void KWQPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
    Q_UNUSED(options);
    Q_UNUSED(widget);
    QRect viewRect = painter->combinedTransform().mapRect(boundingRect().toRect());
    QPixmap pm = pixmap().scaled(viewRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QSize pixmapSize = pm.size();
    pixmapSize.scale(QSizeF(boundingRect().width(), boundingRect().height()).toSize(), Qt::KeepAspectRatio);
    painter->drawPixmap((boundingRect().width() - pixmapSize.width()) / 2, (boundingRect().height() - pixmapSize.height()) / 2, pixmapSize.width(), pixmapSize.height(), pm);
}
