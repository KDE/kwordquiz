/***************************************************************************
                               kwqcardview.cpp
                             -------------------
   copyright            : (C) 2009-2011 by Peter Hedlund
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

#include "kwqcardview.h"


KWQCardView::KWQCardView(QWidget *parent) : QGraphicsView(parent)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setInteractive(true);
    setStyleSheet(QStringLiteral("background: transparent; border: none"));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scene = new KWQCardScene(this);
    setScene(m_scene);
    connect(m_scene, &KWQCardScene::cardClicked, this, &KWQCardView::cardClicked);
}

void KWQCardView::resizeEvent(QResizeEvent* event)
{
    if (scene() != 0)
        fitInView(scene()->sceneRect().adjusted(0, 0, 30, 30), Qt::KeepAspectRatio);

    QGraphicsView::resizeEvent(event);
}

void KWQCardView::setIdentifier(const QString &identifier)
{
    if (scene() != 0)
        m_scene->setIdentifier(identifier);
}

void KWQCardView::setText(const QString &text)
{
    if (scene() != 0)
        m_scene->setText(text);
}

void KWQCardView::setTextColor(const QColor &textColor)
{
    if (scene() != 0)
        m_scene->setTextColor(textColor);
}

void KWQCardView::setTextFont(const QFont &font)
{
    if (scene() != 0)
        m_scene->setTextFont(font);
}

void KWQCardView::setCardColor(const QColor &cardColor)
{
    if (scene() != 0)
        m_scene->setCardColor(cardColor);
}

void KWQCardView::setFrameColor(const QColor &frameColor)
{
    if (scene() != 0)
        m_scene->setFrameColor(frameColor);
}

void KWQCardView::setImage(const QPixmap &image)
{
    if (scene() != 0)
        m_scene->setImage(image);
}
