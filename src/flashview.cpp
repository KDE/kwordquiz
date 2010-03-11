/***************************************************************************
                                flashview.cpp
                             -------------------
   copyright            : (C) 2003-2010 Peter Hedlund
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

#include "flashview.h"

#include <QTimer>

#include <KIconLoader>
#include <KLocale>
#include <KNotification>

#include "kwqquizmodel.h"
#include "kwqscorewidget.h"
#include "prefs.h"

FlashView::FlashView(QWidget *parent, KActionCollection *actionCollection) : QWidget(parent), m_actionCollection(actionCollection)
{
  setupUi(this);
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
}

void FlashView::setQuiz(KWQQuizModel *quiz)
{
  m_quiz = quiz;
}

void FlashView::init()
{
  score->clear();
  score->setQuestionCount(m_quiz->questionCount());
  score->setAsPercent(Prefs::percent());

  m_actionCollection->action("quiz_check")->setEnabled(true);
  m_actionCollection->action("flash_know")->setEnabled(true);
  m_actionCollection->action("flash_dont_know")->setEnabled(true);
  m_actionCollection->action("quiz_repeat_errors")->setEnabled(false);
  m_actionCollection->action("quiz_export_errors")->setEnabled(false);
  connect(flashcard, SIGNAL(cardClicked()), this, SLOT(slotFlip()), Qt::UniqueConnection);

  m_showFirst = true;
  slotFlip();
}

void FlashView::keepDiscardCard(bool keep)
{
  if (!keep) {
    score->countIncrement(KWQScoreWidget::cdCorrect);
    KNotification::event("QuizCorrect", i18n("Your answer was correct!"));
  }
  else {
    m_quiz->checkAnswer("");
    score->countIncrement(KWQScoreWidget::cdError);
    KNotification::event("QuizError", i18n("Your answer was incorrect."));
  }

  m_showFirst = true;

  m_quiz->toNext();
  if (!m_quiz->atEnd()) {
    slotFlip();
  }
  else {
    m_quiz->finish();
    m_actionCollection->action("quiz_check")->setEnabled(false);
    m_actionCollection->action("flash_know")->setEnabled(false);
    m_actionCollection->action("flash_dont_know")->setEnabled(false);
    m_actionCollection->action("quiz_repeat_errors")->setEnabled(m_quiz->hasErrors());
    m_actionCollection->action("quiz_export_errors")->setEnabled(m_quiz->hasErrors());
    disconnect(flashcard, SIGNAL(cardClicked()), 0, 0);
  }
}

void FlashView::slotFlip()
{
  if (m_showFirst) {
    flashcard->setCardColor(Prefs::frontCardColor());
    flashcard->setTextColor(Prefs::frontTextColor());
    flashcard->setFrameColor(Prefs::frontFrameColor());
    flashcard->setTextFont(Prefs::frontFont());
    flashcard->setIdentifier(m_quiz ->langQuestion());
    flashcard->setImage(m_quiz->imageQuestion());
    flashcard->setText(m_quiz->question());
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

void FlashView::slotRestart()
{
  m_quiz->activateBaseList();
  init();
}

void FlashView::slotRepeat()
{
  m_quiz->activateErrorList();
  init();
}

void FlashView::slotTimer( )
{
  if (!m_showFirst)
    slotFlip();
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

#include "flashview.moc"
