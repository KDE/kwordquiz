/***************************************************************************
                              kwqcardscene.cpp
                             -------------------
   copyright            : (C) 2009 by Peter Hedlund
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

#include "kwqcardscene.h"

#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>

static const int cardWidth = 500;
static const int cardHeight = 300;
static const int cardMargin = 20;
static const int textMargin = 30;
static const int shadowOffset = 2;

KWQCardScene::KWQCardScene(QObject *parent) : QGraphicsScene(parent)
{
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(5);
    shadowEffect->setOffset(shadowOffset);

    m_card = addRect(0, 0, cardWidth, cardHeight);
    m_card->setGraphicsEffect(shadowEffect);
    
    m_line = addLine(cardMargin, cardMargin * 3, cardWidth - cardMargin, cardMargin * 3);
    m_identifier = addSimpleText("");

    QFont f = m_identifier->font();
    f.setPixelSize(cardMargin);
    m_identifier->setFont(f);
    m_identifier->setPos(cardMargin, cardMargin);

    m_textArea = addRect(textMargin, cardMargin * 4, cardWidth - (textMargin * 2), cardHeight - (cardMargin * 5));
    m_textArea->setPen(Qt::NoPen);
    m_textArea->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    m_textArea->setZValue(2);

    m_text = addText("");
    m_text->setTextWidth(cardWidth - (textMargin * 2));
    f.setPointSize(12);
    m_text->setParentItem(m_textArea);

    setIdentifier("");
    setText("");
    setTextColor(Qt::blue);
    setTextFont(f);
    setCardColor(Qt::white);
    setFrameColor(Qt::red);
}

const QSize KWQCardScene::minimumSizeHint() const
{
    int width = cardWidth / 10;
    int height = cardHeight / 10;
    return QSize((width) + 10, (height) + 10);	// the +10 is to provide padding and to avoid scrollbars
}

void KWQCardScene::setIdentifier(const QString &identifier)
{
    QFontMetrics fm(m_identifier->font());
    QString s = fm.elidedText(identifier, Qt::ElideRight, cardWidth - (textMargin * 2));
    m_identifier->setText(identifier);
}

void KWQCardScene::setText(const QString &text)
{
    QString t = QString("<html><div align=center>%1</div></html>").arg(text);
    m_text->setHtml(t);
    repositionText();
}

void KWQCardScene::setTextColor(const QColor &textColor)
{
    m_text->setDefaultTextColor(textColor);
}

void KWQCardScene::setTextFont(const QFont &font)
{
    m_text->setFont(font);
    repositionText();
}

void KWQCardScene::setCardColor(const QColor &cardColor)
{
    m_card->setBrush(QBrush(cardColor));
    m_textArea->setBrush(QBrush(cardColor));
}

void KWQCardScene::setFrameColor(const QColor &frameColor)
{
    m_card->setPen(QPen(frameColor));
    m_line->setPen(QPen(frameColor));
}

void KWQCardScene::repositionText()
{
    int h = ((cardMargin * 4) + ((cardHeight - (cardMargin * 5)) - m_text->boundingRect().height())/2);
    m_text->setPos(textMargin, h);
}

void KWQCardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItem *item = itemAt(mouseEvent->scenePos());
    if (item != 0)
        emit cardClicked();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

#include "kwqcardscene.moc"
