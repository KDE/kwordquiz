/***************************************************************************
                          flashview.cpp  -  description
                             -------------------
   copyright            : (C) 2003-2009 Peter Hedlund
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

  m_showFirst = true;
  slotFlip();
}

void FlashView::showFront()
{
  lblLanguageQuestion->setText(m_quiz ->langQuestion());
  lblQuestion->setText(m_quiz->question());
}

void FlashView::showBack()
{
  lblLanguageQuestion->setText(m_quiz->langAnswer());
  lblQuestion->setText(m_quiz->answer());
}


void FlashView::keepDiscardCard(bool keep)
{
  if (!keep)
  {
    score->countIncrement(KWQScoreWidget::cdCorrect);
    KNotification::event("QuizCorrect", i18n("Your answer was correct!"));
  }
  else
  {
    m_quiz->checkAnswer("");
    score->countIncrement(KWQScoreWidget::cdError);
    KNotification::event("QuizError", i18n("Your answer was incorrect."));
  }

  m_showFirst = true;

  m_quiz->toNext();
  if (!m_quiz->atEnd())
  {
    slotFlip();
  }
  else
  {
    m_quiz->finish();
    m_actionCollection->action("quiz_check")->setEnabled(false);
    m_actionCollection->action("flash_know")->setEnabled(false);
    m_actionCollection->action("flash_dont_know")->setEnabled(false);
    m_actionCollection->action("quiz_repeat_errors")->setEnabled(m_quiz->hasErrors());
    m_actionCollection->action("quiz_export_errors")->setEnabled(m_quiz->hasErrors());
  }
}

void FlashView::slotFlip()
{
  if (m_showFirst)
  {
    QPalette pal;
    pal.setColor(lblLanguageQuestion->backgroundRole(), Prefs::frontCardColor());
    lblLanguageQuestion->setPalette(pal);

    pal = QPalette();
    pal.setColor(lblQuestion->foregroundRole(), Prefs::frontTextColor());
    pal.setColor(lblQuestion->backgroundRole(), Prefs::frontCardColor());
    lblQuestion->setPalette(pal);
    lblQuestion->setFont(Prefs::frontFont());

    pal = QPalette();
    pal.setColor(fraCard->foregroundRole(), Prefs::frontFrameColor());
    pal.setColor(fraCard->backgroundRole(), Prefs::frontCardColor());
    fraCard->setPalette(pal);

    pal = QPalette();
    pal.setColor(linFlash->foregroundRole(), Prefs::frontFrameColor());
    pal.setColor(linFlash->backgroundRole(), Prefs::frontCardColor());
    linFlash->setPalette(pal);
    //linFlash->setPaletteForegroundColor(QColor(255, 0, 0));
    showFront();
    m_showFirst = false;
  }
  else
  {
    QPalette pal;
    pal.setColor(lblLanguageQuestion->backgroundRole(), Prefs::backCardColor());
    lblLanguageQuestion->setPalette(pal);

    pal = QPalette();
    pal.setColor(lblQuestion->foregroundRole(), Prefs::backTextColor());
    pal.setColor(lblQuestion->backgroundRole(), Prefs::backCardColor());
    lblQuestion->setPalette(pal);
    lblQuestion->setFont(Prefs::backFont());

    pal = QPalette();
    pal.setColor(fraCard->foregroundRole(), Prefs::backFrameColor());
    pal.setColor(fraCard->backgroundRole(), Prefs::backCardColor());
    fraCard->setPalette(pal);

    pal = QPalette();
    pal.setColor(linFlash->foregroundRole(), Prefs::backFrameColor());
    pal.setColor(linFlash->backgroundRole(), Prefs::backCardColor());
    linFlash->setPalette(pal);

    showBack();
    m_showFirst = true;
  }

  if (Prefs::autoFlip())
  {
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
  if (!m_showFirst)
  {
    QPalette pal;
    pal.setColor(lblQuestion->backgroundRole(), Prefs::frontCardColor());
    lblLanguageQuestion->setPalette(pal);

    pal = QPalette();
    pal.setColor(lblQuestion->foregroundRole(), Prefs::frontTextColor());
    pal.setColor(lblQuestion->backgroundRole(), Prefs::frontCardColor());
    lblQuestion->setPalette(pal);
    lblQuestion->setFont(Prefs::frontFont());

    pal = QPalette();
    pal.setColor(fraCard->foregroundRole(), Prefs::frontFrameColor());
    pal.setColor(fraCard->backgroundRole(), Prefs::frontCardColor());
    fraCard->setPalette(pal);

    pal = QPalette();
    pal.setColor(linFlash->foregroundRole(), Prefs::frontFrameColor());
    pal.setColor(linFlash->backgroundRole(), Prefs::frontCardColor());
    linFlash->setPalette(pal);
  }
  else
  {
    QPalette pal;
    pal.setColor(lblLanguageQuestion->backgroundRole(), Prefs::backCardColor());
    lblLanguageQuestion->setPalette(pal);

    pal = QPalette();
    pal.setColor(lblQuestion->foregroundRole(), Prefs::backTextColor());
    pal.setColor(lblQuestion->backgroundRole(), Prefs::backCardColor());
    lblQuestion->setPalette(pal);
    lblQuestion->setFont(Prefs::backFont());

    pal = QPalette();
    pal.setColor(fraCard->foregroundRole(), Prefs::backFrameColor());
    pal.setColor(fraCard->backgroundRole(), Prefs::backCardColor());
    fraCard->setPalette(pal);

    pal = QPalette();
    pal.setColor(linFlash->foregroundRole(), Prefs::backFrameColor());
    pal.setColor(linFlash->backgroundRole(), Prefs::backCardColor());
    linFlash->setPalette(pal);
  }

  if (Prefs::autoFlip())
  {
    m_timer->setSingleShot(true);
    m_timer->start(Prefs::flipDelay() * 1000);
  }
  else
    m_timer->stop();

  score ->setAsPercent(Prefs::percent());
}

#include "flashview.moc"
