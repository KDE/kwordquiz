/*
    This file is part of KWordQuiz
    Copyright (C) 2003-2010 Peter Hedlund <peter.hedlund@kdemail.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "flashview.h"

#include <QTimer>

#include <KLocalizedString>
#include <KNotification>

#include "kwqquizmodel.h"
#include "kwqscorewidget.h"
#include "prefs.h"

FlashView::FlashView(QWidget *parent, KActionCollection *actionCollection) : KWQQuizView(parent, actionCollection)
{
  setupUi(this);
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &FlashView::slotTimer);
}

void FlashView::init()
{
  score->clear();
  score->setQuestionCount(m_quiz->questionCount());
  score->setAsPercent(Prefs::percent());

  m_actionCollection->action(QStringLiteral("quiz_check"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("flash_know"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("flash_dont_know"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);

  // reset last file
  audioPlayFile(QUrl(), true);

  connect(flashcard, SIGNAL(cardClicked()), this, SLOT(slotCheck()), Qt::UniqueConnection);

  m_showFirst = true;
  slotCheck();
}

void FlashView::keepDiscardCard(bool keep)
{
  if (!keep) {
    score->countIncrement(KWQScoreWidget::cdCorrect);
    KNotification::event(QStringLiteral("QuizCorrect"), i18n("Your answer was correct!"));
  }
  else {
    m_quiz->checkAnswer(QLatin1String(""));
    score->countIncrement(KWQScoreWidget::cdError);
    KNotification::event(QStringLiteral("QuizError"), i18n("Your answer was incorrect."));
  }

  m_showFirst = true;

  m_quiz->toNext();
  if (!m_quiz->atEnd()) {
    slotCheck();
  }
  else {
    m_quiz->finish();
    m_actionCollection->action(QStringLiteral("quiz_check"))->setEnabled(false);
    m_actionCollection->action(QStringLiteral("flash_know"))->setEnabled(false);
    m_actionCollection->action(QStringLiteral("flash_dont_know"))->setEnabled(false);
    m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(m_quiz->hasErrors());
    m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(m_quiz->hasErrors());
    m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);
    disconnect(flashcard, SIGNAL(cardClicked()), 0, 0);
  }
}

void FlashView::slotCheck()
{
  if (m_showFirst) {
    flashcard->setCardColor(Prefs::frontCardColor());
    flashcard->setTextColor(Prefs::frontTextColor());
    flashcard->setFrameColor(Prefs::frontFrameColor());
    flashcard->setTextFont(Prefs::frontFont());
    flashcard->setIdentifier(m_quiz ->langQuestion());
    flashcard->setImage(m_quiz->imageQuestion());
    flashcard->setText(m_quiz->question());
    audioPlayQuestion();
    m_showFirst = false;
  }
  else {
    flashcard->setCardColor(Prefs::backCardColor());
    flashcard->setTextColor(Prefs::backTextColor());
    flashcard->setFrameColor(Prefs::backFrameColor());
    flashcard->setTextFont(Prefs::backFont());
    flashcard->setIdentifier(m_quiz->langAnswer());
    flashcard->setImage(m_quiz->imageAnswer());
    flashcard->setText(m_quiz->answer());
    audioPlayAnswer();
    m_showFirst = true;
  }

  if (Prefs::autoFlip()) {
    m_timer->setSingleShot(true);
    m_timer->start(Prefs::flipDelay() * 1000);
  }
  else
    m_timer->stop();
}

void FlashView::slotKnow()
{
  keepDiscardCard(false);
}

void FlashView::slotDontKnow()
{
  keepDiscardCard(true);
}

void FlashView::slotTimer( )
{
  if (!m_showFirst)
    slotCheck();
  else
    if (Prefs::keepDiscard())
      slotDontKnow();
    else
      slotKnow();
}

void FlashView::slotApplySettings( )
{
  if (!m_showFirst) {
    flashcard->setCardColor(Prefs::frontCardColor());
    flashcard->setTextColor(Prefs::frontTextColor());
    flashcard->setFrameColor(Prefs::frontFrameColor());
    flashcard->setTextFont(Prefs::frontFont());
  }
  else {
    flashcard->setCardColor(Prefs::backCardColor());
    flashcard->setTextColor(Prefs::backTextColor());
    flashcard->setFrameColor(Prefs::backFrameColor());
    flashcard->setTextFont(Prefs::backFont());
  }

  if (Prefs::autoFlip()) {
    m_timer->setSingleShot(true);
    m_timer->start(Prefs::flipDelay() * 1000);
  }
  else
    m_timer->stop();

  score ->setAsPercent(Prefs::percent());
}
