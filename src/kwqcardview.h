/***************************************************************************
                                kwqcardview.h
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

#ifndef KWQCARDVIEW_H
#define KWQCARDVIEW_H

#include <QGraphicsView>

#include "kwqcardscene.h"

class KWQCardScene;

class KWQCardView : public QGraphicsView
{
Q_OBJECT
public:

    explicit KWQCardView(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override {if(scene()==nullptr) return QSize(); else return qobject_cast<KWQCardScene*>(scene())->minimumSizeHint();}

    void setIdentifier(const QString &);
    void setText(const QString &);
    void setTextColor(const QColor &);
    void setTextFont(const QFont &);
    void setCardColor(const QColor &);
    void setFrameColor(const QColor &);
    void setImage(const QPixmap &);

signals:
    void cardClicked();

protected:
    ///Overloaded to resize card.
    void resizeEvent(QResizeEvent* event) override;

private:
    KWQCardScene *m_scene;
};

#endif // KWQCARDVIEW_H
