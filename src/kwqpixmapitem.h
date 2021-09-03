/*
    kwqpixmapitem.h
    -------------------
    SPDX-FileCopyrightText: 2011 Peter Hedlund
    email                : peter.hedlund@kdemail.net
*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
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
