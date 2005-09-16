/***************************************************************************
                          flashview.cpp  -  description
                             -------------------
   copyright            : (C) 2003 by Peter Hedlund
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
 
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>
#include <knotifyclient.h>

#include "kwordquiz.h"
#include "flashview.h"
#include "prefs.h"


FlashView::FlashView(QWidget *parent, const char *name, WFlags f)
    : FlashViewBase(parent, name, f)
{
  m_score = new WQScore();
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
}


FlashView::~FlashView()
{
}


void FlashView::setQuiz(WQQuiz *quiz)
{
  m_quiz = quiz;
}

void FlashView::init()
{
  m_score ->setQuestionCount(m_quiz->questionCount());
  m_score ->setAsPercent(Prefs::percent());

  m_question = 0;
  m_error = 0;
  picAnswered->clear();
  picCorrect->clear();
  picError->clear();

  updateScore();

  KWordQuizApp *win=(KWordQuizApp *) parent();
  win->actionCollection()->action("quiz_check")->setEnabled(true);
  win->actionCollection()->action("flash_know")->setEnabled(true);
  win->actionCollection()->action("flash_dont_know")->setEnabled(true);
  win->actionCollection()->action("quiz_repeat_errors")->setEnabled(false);

  m_showFirst = true;
  slotFlip();
}

void FlashView::showFront(int i)
{
  lblLanguageQuestion -> setText(m_quiz ->langQuestion(i));
  lblQuestion -> setText(m_quiz -> question(i));
}

void FlashView::showBack(int i)
{
  lblLanguageQuestion -> setText(m_quiz->langAnswer(i));
  lblQuestion -> setText(m_quiz->answer(i));
}


void FlashView::keepDiscardCard(bool keep)
{
  if (!keep)
  {
    m_score->countIncrement(WQScore::cdCorrect);
    updateScore();
    KNotifyClient::event(winId(), "QuizCorrect", i18n("Your answer was correct!"));
  }
  else
  {
    m_error++;
    m_quiz->checkAnswer(m_question, "");
    m_score->countIncrement(WQScore::cdError);
    updateScore();
    KNotifyClient::event(winId(), "QuizError", i18n("Your answer was incorrect."));
  }

  m_showFirst = true;

  if (++m_question < m_quiz->questionCount())
  {
    slotFlip();
  }
  else
  {
    m_quiz->finish();
    KWordQuizApp *win=(KWordQuizApp *) parent();
    win->actionCollection()->action("quiz_check")->setEnabled(false);
    win->actionCollection()->action("flash_know")->setEnabled(false);
    win->actionCollection()->action("flash_dont_know")->setEnabled(false);
    win->actionCollection()->action("quiz_repeat_errors")->setEnabled((m_error > 0));
  }
}

void FlashView::slotFlip()
{
  if (m_showFirst)
  {
    lblLanguageQuestion->setPaletteBackgroundColor(Prefs::frontCardColor());
    lblQuestion->setFont(Prefs::frontFont());
    lblQuestion->setPaletteForegroundColor(Prefs::frontTextColor());
    lblQuestion->setPaletteBackgroundColor(Prefs::frontCardColor());
    fraCard->setPaletteBackgroundColor(Prefs::frontCardColor());
    fraCard->setPaletteForegroundColor(Prefs::frontFrameColor());
    linFlash->setPaletteForegroundColor(Prefs::frontFrameColor());
    linFlash->setPaletteBackgroundColor(Prefs::frontCardColor());
    //linFlash->setPaletteForegroundColor(QColor(255, 0, 0));
    showFront(m_question);
    m_showFirst = false;
  }
  else
  {
    lblLanguageQuestion->setPaletteBackgroundColor(Prefs::backCardColor());
    lblQuestion->setFont(Prefs::backFont());
    lblQuestion->setPaletteForegroundColor(Prefs::backTextColor());
    lblQuestion->setPaletteBackgroundColor(Prefs::backCardColor());
    fraCard->setPaletteBackgroundColor(Prefs::backCardColor());
    fraCard->setPaletteForegroundColor(Prefs::backFrameColor());
    linFlash->setPaletteForegroundColor(Prefs::backFrameColor());
    linFlash->setPaletteBackgroundColor(Prefs::backCardColor());
    //linFlash->setPaletteForegroundColor(QColor(0, 0, 255));
    showBack(m_question);
    m_showFirst = true;
  }

  if (Prefs::autoFlip())
    m_timer->start(Prefs::flipDelay() * 1000, true);
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

/*!
    \fn FlashView::updateScore
 */
void FlashView::updateScore()
{
  QString s;
  s = s.setNum(m_quiz->questionCount(), 10);
  lblScoreCount->setText(s);
  picCount->setPixmap(KGlobal::iconLoader()->loadIcon("kwordquiz", KIcon::Panel));

  s = m_score->answerText();
  lblScoreAnswered->setText(s);
  if (!s.isEmpty())
    picAnswered->setPixmap(KGlobal::iconLoader()->loadIcon("question", KIcon::Panel));

  s = m_score->correctText();
  lblScoreCorrect->setText(s);
  if (!s.isEmpty())
    picCorrect->setPixmap(KGlobal::iconLoader()->loadIcon("check", KIcon::Panel));

  s = m_score->errorText();
  lblScoreError->setText(s);
  if (!s.isEmpty())
    picError->setPixmap(KGlobal::iconLoader()->loadIcon("error", KIcon::Panel));
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
    lblLanguageQuestion->setPaletteBackgroundColor(Prefs::frontCardColor());
    lblQuestion->setFont(Prefs::frontFont());
    lblQuestion->setPaletteForegroundColor(Prefs::frontTextColor());
    lblQuestion->setPaletteBackgroundColor(Prefs::frontCardColor());
    fraCard->setPaletteBackgroundColor(Prefs::frontCardColor());
    fraCard->setPaletteForegroundColor(Prefs::frontFrameColor());
    linFlash->setPaletteForegroundColor(Prefs::frontFrameColor());
    linFlash->setPaletteBackgroundColor(Prefs::frontCardColor());
  }
  else
  {
    lblLanguageQuestion->setPaletteBackgroundColor(Prefs::backCardColor());
    lblQuestion->setFont(Prefs::backFont());
    lblQuestion->setPaletteForegroundColor(Prefs::backTextColor());
    lblQuestion->setPaletteBackgroundColor(Prefs::backCardColor());
    fraCard->setPaletteBackgroundColor(Prefs::backCardColor());
    fraCard->setPaletteForegroundColor(Prefs::backFrameColor());
    linFlash->setPaletteForegroundColor(Prefs::backFrameColor());
    linFlash->setPaletteBackgroundColor(Prefs::backCardColor());
  }
  if (Prefs::autoFlip())
    m_timer->start(Prefs::flipDelay() * 1000, true);
  else
    m_timer->stop();

  m_score ->setAsPercent(Prefs::percent());
  updateScore();
}


#include "flashview.moc"
