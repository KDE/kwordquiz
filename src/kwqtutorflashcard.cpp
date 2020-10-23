/***************************************************************************
                             kwqtutorflashcard.cpp
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

#include "kwqtutorflashcard.h"


#include "prefs.h"

KWQTutorFlashCard::KWQTutorFlashCard(QWidget* parent) : QWidget(parent)
{
  m_label = 0;
  m_card = 0;

  m_cardLayout = new QVBoxLayout(this);
  m_cardLayout->setContentsMargins(0, 0, 0, 0);
  m_cardLayout->setSpacing(0);
  setLayout(m_cardLayout);
  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
  init();
}

void KWQTutorFlashCard::init()
{
  if (m_label != 0) {
    m_cardLayout->removeWidget(m_label);
    delete m_label;
    m_label = 0;
  }

  if (m_card != 0) {
    m_cardLayout->removeWidget(m_card);
    delete m_card;
    m_card = 0;
  }

  if (Prefs::tutorCardAppearance() == Prefs::EnumTutorCardAppearance::Minimalistic) {
    m_label = new QLabel(this);
    m_label->setMinimumWidth(80);
    m_label->setAlignment(Qt::AlignCenter);
    m_cardLayout->addWidget(m_label);
  } else {
    m_card = new KWQCardView(this);
    m_card->setTextFont(Prefs::frontFont());
    m_cardLayout->addWidget(m_card);
  }

  setText(m_text);
}

void KWQTutorFlashCard::closeEvent (QCloseEvent* event)
{
  Prefs::setTutorFlashCardGeometry(this->geometry());  //save flashcard window coords and geometry before closing it
  event->accept();
}

void KWQTutorFlashCard::setText(const QString & text)
{
  m_text = text;
  if (m_label != 0)
    m_label->setText(text);
  if (m_card != 0)
    m_card->setText(text);
}
