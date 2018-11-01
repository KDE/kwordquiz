/***************************************************************************
                              kwqcardscene.cpp
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

#include "kwqcardscene.h"

#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QTextOption>
#include <QTextDocument>

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
    m_identifier = addSimpleText(QLatin1String(""));

    QFont f = m_identifier->font();
    f.setPixelSize(cardMargin);
    m_identifier->setFont(f);
    m_identifier->setPos(cardMargin, cardMargin);

    m_textArea = addRect(textMargin, cardMargin * 4, cardWidth - (textMargin * 2), cardHeight - (cardMargin * 5));
    m_textArea->setPen(Qt::NoPen);
    m_textArea->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    m_textArea->setZValue(4);

    m_text = addText(QLatin1String(""));
    m_text->setTextWidth(cardWidth - (textMargin * 2));
    f.setPointSize(12);
    m_text->setParentItem(m_textArea);

    m_pixmap = new KWQPixmapItem(QPixmap(), m_card);
    m_pixmap->setImageRect(QRect(textMargin, cardMargin * 4, cardWidth - (textMargin * 2), cardHeight - (cardMargin * 5)));

    setIdentifier(QLatin1String(""));
    setText(QLatin1String(""));
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
    QTextOption option = m_text->document()->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    m_text->document()->setDefaultTextOption(option);
    m_text->setPlainText(text);
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

void KWQCardScene::setImage(const QPixmap &image)
{
    realign(image.isNull());
    m_pixmap->setPixmap(image);
    repositionText();
}

void KWQCardScene::realign(bool textOnly)
{
    if (textOnly) {
        m_textArea->setRect(textMargin, cardMargin * 4, cardWidth - (textMargin * 2), cardHeight - (cardMargin * 5));
    } else {
        m_pixmap->setImageRect(QRect(textMargin, cardMargin * 4, (cardWidth / 2) - textMargin,  cardHeight - (cardMargin * 5)));
        m_textArea->setRect((cardWidth / 2), cardMargin * 4, (cardWidth / 2) - textMargin, cardHeight - (cardMargin * 5));
    }
}

void KWQCardScene::repositionText()
{
    int h = 0;

    if (m_pixmap->pixmap().isNull()) {
        h = ((cardMargin * 4) + ((cardHeight - (cardMargin * 5)) - m_text->boundingRect().height()) / 2);
        m_text->setTextWidth(cardWidth - (textMargin * 2));
        m_text->setPos(textMargin, h);
    } else {
        h = ((cardMargin * 4) + ((cardHeight - (cardMargin * 5)) - m_textArea->boundingRect().height()) / 2);
        m_pixmap->setPos(textMargin, h);
        h = ((cardMargin * 4) + ((cardHeight - (cardMargin * 5)) - m_text->boundingRect().height()) / 2);
        m_text->setTextWidth((cardWidth / 2) - textMargin);
        m_text->setPos((cardWidth + (cardWidth / 2) - m_text->boundingRect().width() - textMargin) / 2, h);
    }
}

void KWQCardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItem *item = itemAt(mouseEvent->scenePos(), QTransform());
    if (item != 0)
        emit cardClicked();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
