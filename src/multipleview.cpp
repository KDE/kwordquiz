/***************************************************************************
                          multipleview.cpp  -  description
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

#include "multipleview.h"

#include <KIconLoader>
#include <KLocale>
#include <KNotification>
#include <KActionCollection>

#include "kwqquiz.h"
#include "kwqscorewidget.h"
#include "prefs.h"

MultipleView::MultipleView(QWidget *parent, KActionCollection *actionCollection) : QWidget(parent), m_actionCollection(actionCollection)
{
  setupUi(this);
  connect(opt1, SIGNAL(clicked()), this, SLOT(slotOpt1Clicked()));
  connect(opt2, SIGNAL(clicked()), this, SLOT(slotOpt2Clicked()));
  connect(opt3, SIGNAL(clicked()), this, SLOT(slotOpt3Clicked()));
}

void MultipleView::setQuiz(KWQQuiz *quiz)
{
  m_quiz = quiz;
}

void MultipleView::init()
{
  score->clear();
  score->setQuestionCount(m_quiz->questionCount());
  score->setAsPercent(Prefs::percent());

  m_question = 0;
  m_error = 0;

  opt1->show();
  opt2->show();
  opt3->show();

  lblQuestion->setFont(Prefs::editorFont());
  lblPreviousQuestion->setFont(Prefs::editorFont());
  lblYourAnswer->setFont(Prefs::editorFont());
  lblCorrect->setFont(Prefs::editorFont());
  opt1->setFont(Prefs::editorFont());
  opt2->setFont(Prefs::editorFont());
  opt3->setFont(Prefs::editorFont());

  lblPreviousQuestionHeader->clear();
  lblPreviousQuestion->clear();
  lblYourAnswerHeader->clear();
  lblYourAnswer->clear();
  lblCorrectHeader->clear();
  lblCorrect->clear();

  picPrevious->clear();
  picYourAnswer->clear();
  picCorrectAnswer->clear();

  m_actionCollection->action("quiz_check")->setEnabled(true);
  m_actionCollection->action("quiz_repeat_errors")->setEnabled(false);
  m_actionCollection->action("quiz_export_errors")->setEnabled(false);
  m_actionCollection->action("quiz_Opt1")->setEnabled(true);
  m_actionCollection->action("quiz_Opt2")->setEnabled(true);
  m_actionCollection->action("quiz_Opt3")->setEnabled(true);

  showQuestion(0);
}

void MultipleView::slotCheck()
{
  if (m_actionCollection->action("quiz_check")->isEnabled())
  {

    QString ans;
    bool oneIsChecked = false;

    if (opt1->isChecked())
    {
      ans = opt1->text().mid(3, opt1->text().length());
      oneIsChecked = true;
    }

    if (opt2->isChecked())
    {
      ans = opt2->text().mid(3, opt2->text().length());
      oneIsChecked = true;
    }

    if (opt3->isChecked())
    {
      ans = opt3->text().mid(3, opt3->text().length());
      oneIsChecked = true;
    }

    if (!oneIsChecked)
      return;

    bool fIsCorrect = m_quiz->checkAnswer(m_question, ans);

    if (fIsCorrect)
    {
      picYourAnswer->setPixmap(KIconLoader::global()->loadIcon("answer-correct", KIconLoader::Panel));
      lblCorrectHeader->clear();
      picCorrectAnswer->clear();
      lblCorrect->clear();
      score->countIncrement(KWQScoreWidget::cdCorrect);
      KNotification::event("QuizCorrect", i18n("Your answer was correct!"));
    }
    else
    {
      m_error++;

      picYourAnswer->setPixmap(KIconLoader::global()->loadIcon("error", KIconLoader::Panel));

      lblCorrect->setText(m_quiz->answer(m_question));
      picCorrectAnswer->setPixmap(KIconLoader::global()->loadIcon("answer-correct", KIconLoader::Panel));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      score->countIncrement(KWQScoreWidget::cdError);
      KNotification::event("QuizError", i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question(m_question));
    picPrevious->setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));

    lblYourAnswerHeader->setText(i18n("Your Answer"));
    lblYourAnswer->setText(m_quiz->yourAnswer(ans));

    if (++m_question < m_quiz->questionCount())
    {
      showQuestion(m_question);
    }
    else
    {
      m_quiz->finish();
      m_actionCollection->action("quiz_check")->setEnabled(false);
      m_actionCollection->action("quiz_Opt1")->setEnabled(false);
      m_actionCollection->action("quiz_Opt2")->setEnabled(false);
      m_actionCollection->action("quiz_Opt3")->setEnabled(false);
      m_actionCollection->action("quiz_repeat_errors")->setEnabled((m_error > 0));
      m_actionCollection->action("quiz_export_errors")->setEnabled((m_error > 0));

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      opt1->hide();
      opt2->hide();
      opt3->hide();
      picQuestion->setPixmap(KIconLoader::global()->loadIcon("kwordquiz", KIconLoader::Panel));
      picAnswer->clear();
    }
  }
}

void MultipleView::slotOpt1Clicked()
{
  opt1->setChecked(true);
  if (Prefs::autoCheck())
    slotCheck();
  else
  {
    opt2->setChecked(false);
    opt3->setChecked(false);
  }
}

void MultipleView::slotOpt2Clicked()
{
  opt2->setChecked(true);
  if (Prefs::autoCheck())
    slotCheck();
  else
  {
    opt1->setChecked(false);
    opt3->setChecked(false);
  }
}

void MultipleView::slotOpt3Clicked()
{
  opt3->setChecked(true);
  if (Prefs::autoCheck())
    slotCheck();
  else
  {
    opt1->setChecked(false);
    opt2->setChecked(false);
  }
}

void MultipleView::slotRestart()
{
  m_quiz->activateBaseList();
  init();
}

void MultipleView::slotRepeat()
{
  m_quiz->activateErrorList();
  init();
}

/*!
    \fn MultipleView::showQuestion(int i)
 */
void MultipleView::showQuestion(int i)
{
  lblQuestionLanguage->setText(m_quiz ->langQuestion(i));
  lblQuestion->setText(m_quiz ->question(i));

  picQuestion->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(i, KWQQuiz::IconLeftCol), KIconLoader::Panel));

  lblAnswerLanguage->setText(m_quiz ->langAnswer(i));

  QStringList sl = m_quiz->multiOptions(i);

  opt1->setText("&1 " + sl[0]);
  opt2->setText("&2 " + sl[1]);
  opt3->setText("&3 " + sl[2]);

  opt1->setChecked(false);
  opt2->setChecked(false);
  opt3->setChecked(false);

  picAnswer->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(i, KWQQuiz::IconRightCol), KIconLoader::Panel));
}

void MultipleView::slotApplySettings()
{
  score->setAsPercent(Prefs::percent());
}

#include "multipleview.moc"
