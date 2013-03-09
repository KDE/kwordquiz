/***************************************************************************
                             kwqtutorflashcard.h
                             -------------------

    copyright            : (C) 2010 by Peter Hedlund
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

#ifndef KWQTUTORFLASHCARD_H
#define KWQTUTORFLASHCARD_H

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QCloseEvent>

#include "kwqcardview.h"

class KWQTutorFlashCard : public QWidget
{
  Q_OBJECT

  public:
    explicit KWQTutorFlashCard(QWidget* parent = 0);
    void init();
    /* reimplemented func to save flashcard geometry before closing */
    void closeEvent (QCloseEvent* event);
    void setText(const QString &);

  private:
    QVBoxLayout* m_cardLayout;
    KWQCardView* m_card;
    QLabel* m_label;
    QString m_text;
};

#endif // KWQTUTORFLASHCARD_H
