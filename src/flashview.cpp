//
// C++ Implementation: flashview
//
// Description:
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
#include <qlabel.h>
#include <qpixmap.h>

#include <kstandarddirs.h> //locate
#include <klocale.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <knotifyclient.h>

#include "kwordquiz.h"
#include "flashview.h"
#include "wqquiz.h"
#include "configuration.h"


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
  m_score ->setAsPercent(Config().m_percent);

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
  lblLanguageQuestion ->setText(m_quiz ->langQuestion(i));
  lblQuestion->setFont(m_quiz->fontQuestion(i));  
  lblQuestion -> setText(m_quiz -> question(i));

}

void FlashView::showBack(int i)
{
  lblLanguageQuestion ->setText(m_quiz->langAnswer(i));
  lblQuestion->setFont(m_quiz->fontAnswer(m_question));
  lblQuestion -> setText(m_quiz->answer(i));
}


void FlashView::keepDiscardCard(bool keep)
{

    if (!keep)
    {
      m_score->countIncrement(WQScore::cdCorrect);
      updateScore();
      KNotifyClient::event("QuizCorrect", i18n("Your answer was correct!"));
    }
    else
    {
      m_error++;
      m_quiz->checkAnswer(m_question, "");
      m_score->countIncrement(WQScore::cdError);
      updateScore();
      KNotifyClient::event("QuizError", i18n("Your answer was incorrect."));
    }

    //m_question++;

    m_showFirst = true;


  if (++m_question < m_quiz->questionCount())
  {
    slotFlip();
  }
  else
  {
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
    linFlash->setPaletteForegroundColor(QColor(255, 0, 0));
    showFront(m_question);
    m_showFirst = false;
  }
  else
  {
    linFlash->setPaletteForegroundColor(QColor(0, 0, 255));
    showBack(m_question);
    m_showFirst = true;
  }

  if (Config().m_autoFlip)
    m_timer->start(Config().m_flipDelay * 1000, true);
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
  picCount->setPixmap(QPixmap(locate("data", "kwordquiz/pics/appicon.png")));

  s = m_score->answerText();
  lblScoreAnswered->setText(s);
  if (!s.isEmpty())
    picAnswered->setPixmap(QPixmap(locate("data", "kwordquiz/pics/question.png")));

  s = m_score->correctText();
  lblScoreCorrect->setText(s);
  if (!s.isEmpty())
    picCorrect->setPixmap(QPixmap(locate("data", "kwordquiz/pics/correct.png")));

  s = m_score->errorText();
  lblScoreError->setText(s);
  if (!s.isEmpty())
    picError->setPixmap(QPixmap(locate("data", "kwordquiz/pics/error.png")));
}

void FlashView::slotTimer( )
{

  if (!m_showFirst)
    slotFlip();
  else
    if (Config().m_keepDiscard)
      slotDontKnow();
    else
      slotKnow();
}

void FlashView::slotApplySettings( )
{

  if (Config().m_autoFlip)
    m_timer->start(Config().m_flipDelay * 1000, true);
  else
    m_timer->stop();

  m_score ->setAsPercent(Config().m_percent);
  updateScore();
}


#include "flashview.moc"
