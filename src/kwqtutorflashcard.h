/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWQTUTORFLASHCARD_H
#define KWQTUTORFLASHCARD_H

#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>

#include "kwqcardview.h"

class KWQTutorFlashCard : public QWidget
{
  Q_OBJECT

  public:
    explicit KWQTutorFlashCard(QWidget* parent = nullptr);
    void init();
    /* reimplemented func to save flashcard geometry before closing */
    void closeEvent (QCloseEvent* event) override;
    void setText(const QString &);

  private:
    QVBoxLayout* m_cardLayout;
    KWQCardView* m_card;
    QLabel* m_label;
    QString m_text;
};

#endif // KWQTUTORFLASHCARD_H
