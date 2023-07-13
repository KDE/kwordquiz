/*
    SPDX-FileCopyrightText: 2009-2011 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    if (scene() != nullptr)
        fitInView(scene()->sceneRect().adjusted(0, 0, 30, 30), Qt::KeepAspectRatio);

    QGraphicsView::resizeEvent(event);
}

void KWQCardView::setIdentifier(const QString &identifier)
{
    if (scene() != nullptr)
        m_scene->setIdentifier(identifier);
}

void KWQCardView::setText(const QString &text)
{
    if (scene() != nullptr)
        m_scene->setText(text);
}

void KWQCardView::setTextColor(const QColor &textColor)
{
    if (scene() != nullptr)
        m_scene->setTextColor(textColor);
}

void KWQCardView::setTextFont(const QFont &font)
{
    if (scene() != nullptr)
        m_scene->setTextFont(font);
}

void KWQCardView::setCardColor(const QColor &cardColor)
{
    if (scene() != nullptr)
        m_scene->setCardColor(cardColor);
}

void KWQCardView::setFrameColor(const QColor &frameColor)
{
    if (scene() != nullptr)
        m_scene->setFrameColor(frameColor);
}

void KWQCardView::setImage(const QPixmap &image)
{
    if (scene() != nullptr)
        m_scene->setImage(image);
}

#include "moc_kwqcardview.cpp"
