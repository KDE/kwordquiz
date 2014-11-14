/***************************************************************************
                               kwqpixmapitem.cpp
                             -------------------
   copyright            : (C) 2011 by Peter Hedlund
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
