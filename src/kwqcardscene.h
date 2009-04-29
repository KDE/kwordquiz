/***************************************************************************
                               kwqcardscene.h
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

#ifndef KWQCARDSCENE_H
#define KWQCARDSCENE_H

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

class KWQCardScene : public QGraphicsScene
{
Q_OBJECT
public:

    KWQCardScene(QObject *parent = 0);

    /**
    * The smallest the view can be when 'auto-zoom' is off
    * @return the minimum size the view should be
    */
    const QSize minimumSizeHint() const;

    void setIdentifier(const QString &);
    void setText(const QString &);
    void setTextColor(const QColor &);
    void setTextFont(const QFont &);
    void setCardColor(const QColor &);
    void setFrameColor(const QColor &);

signals:
    void cardClicked();

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

private:
    QGraphicsRectItem *m_card;
    QGraphicsLineItem *m_line;
    QGraphicsSimpleTextItem *m_identifier;
    QGraphicsRectItem *m_textArea;
    QGraphicsTextItem *m_text;

    void repositionText();
};

#endif // KWQCARDSCENE_H