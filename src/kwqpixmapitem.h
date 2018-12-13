/***************************************************************************
                               kwqpixmapitem.h
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


#ifndef KWQPIXMAPITEM_H
#define KWQPIXMAPITEM_H

#include <QGraphicsPixmapItem>

class KWQPixmapItem: public QGraphicsPixmapItem
{
public:
    explicit KWQPixmapItem(const QPixmap &pixmap, QGraphicsItem *parentItem = nullptr);

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void setImageRect(const QRect &rect);

private:
    QRect m_imageRect;
};

#endif // KWQPIXMAPITEM_H
