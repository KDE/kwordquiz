/*
    kwqcardview.h
    -------------------
    SPDX-FileCopyrightText: 2009-2011 Peter Hedlund
    email                : peter.hedlund@kdemail.net
*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
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

Q_SIGNALS:
    void cardClicked();

protected:
    ///Overloaded to resize card.
    void resizeEvent(QResizeEvent* event) override;

private:
    KWQCardScene *m_scene;
};

#endif // KWQCARDVIEW_H
