/***************************************************************************
                          qaview.cpp  -  description
                             -------------------
   copyright            : (C) 2003-2007 Peter Hedlund
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

#include "qaview.h"

#include <QLabel>

#include <KActionCollection>
#include <KIconLoader>
#include <KLocale>
#include <KLineEdit>
#include <KNotification>

#include "prefs.h"
#include "kwordquiz.h"
#include "wqscore.h"

QString highlightError(const QString & c, const QString & e)
{
  if (c == e)
    return c;

  QString s = c;
  if (s.left(4) == "<qt>" && e.left(4) != "<qt>")
      s = s.mid(4, s.length() - 9);

  if (s == e)
    return s;

  QString result = "<qt>";
  int i = 0;
  while (i < e.length() && s[i] == e[i])
    result.append(e[i++]);
  result.append("<b>");
  QString result2 = "</qt>";
  int j = s.length() - 1;
  int k = e.length() - 1;
  while (j >= 0 && k >= 0 && s[j] == e[k])
  {
    result2.prepend(e[k]);
    j--;
    k--;
  }
  result2.prepend("</b>");

  for (int m = i; m <= k; m++)
    result.append(e[m]);

  result.append(result2);
  return result;
}

QAView::QAView(QWidget *parent) : QWidget(parent)
{
  setupUi(this);

  connect(txtAnswer, SIGNAL(returnPressed()), this, SLOT(slotCheck()));

  m_score = new WQScore();
}

void QAView::setQuiz(KWQQuiz *quiz)
{
  m_quiz = quiz;
}

void QAView::init()
{
  m_score -> setQuestionCount(m_quiz->questionCount());
  m_score -> setAsPercent(Prefs::percent());
  m_hintUsed = false;
  m_question = 0;
  m_error = 0;
  picAnswered -> clear();
  picCorrect -> clear();
  picError -> clear();

  QFont f = Prefs::editorFont();
  f.setWeight(QFont::Normal);
  lblQuestion -> setFont(f);
  lblAnswerBlank -> setFont(f);
  txtAnswer -> setFont(f);
  lblPreviousQuestion -> setFont(f);
  lblYourAnswer -> setFont(f);
  lblCorrect -> setFont(f);

  lblPreviousQuestionHeader->clear();
  lblPreviousQuestion->clear();
  lblYourAnswerHeader->clear();
  lblYourAnswer->clear();
  lblCorrectHeader->clear();
  lblCorrect->clear();

  picPrevious->clear();
  picYourAnswer->clear();
  picCorrectAnswer->clear();

  KWordQuizApp *win=KWordQuizApp::self();
  win->actionCollection()->action("quiz_check")->setEnabled(true);
  win->actionCollection()->action("qa_hint")->setEnabled(true);
  win->actionCollection()->action("quiz_repeat_errors")->setEnabled(false);

  updateScore();
  showQuestion(0);
  txtAnswer->show();
  txtAnswer->setFocus();
}

void QAView::slotCheck()
{
  KWordQuizApp *win=KWordQuizApp::self();
  if (win->actionCollection()->action("quiz_check")->isEnabled())
  {
    bool fIsCorrect;

    if (m_hintUsed && Prefs::hintError())
    {
      //Force an Error
      fIsCorrect = m_quiz->checkAnswer(m_question, "");
    }
    else
    {
      //Really check the answer
      fIsCorrect = m_quiz->checkAnswer(m_question, txtAnswer->text());
    }
    //Reset for next question
    m_hintUsed = false;

    if (fIsCorrect)
    {

      picYourAnswer->setPixmap(KIconLoader::global()->loadIcon("check", KIconLoader::Panel));
      lblYourAnswer->setText(m_quiz->yourAnswer(txtAnswer->text()));
      lblCorrectHeader->clear();
      picCorrectAnswer->clear();
      lblCorrect->clear();
      m_score->countIncrement(WQScore::cdCorrect);
      updateScore();
      KNotification::event("QuizCorrect", i18n("Your answer was correct!"));
    }
    else
    {
      m_error++;

      picYourAnswer->setPixmap(KIconLoader::global()->loadIcon("error", KIconLoader::Panel));
      lblYourAnswer->setText(highlightError(m_quiz->answer(m_question), m_quiz->yourAnswer(txtAnswer->text())));
      lblCorrect->setText(m_quiz->answer(m_question));
      picCorrectAnswer->setPixmap(KIconLoader::global()->loadIcon("check", KIconLoader::Panel));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      m_score->countIncrement(WQScore::cdError);
      updateScore();
      KNotification::event("QuizError", i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question(m_question));
    //lblPreviousQuestion->setFont(m_quiz->fontQuestion(m_question));
    picPrevious->setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));

    lblYourAnswerHeader->setText(i18n("Your Answer"));

    if (++m_question < m_quiz->questionCount())
    {
      showQuestion(m_question);
    }
    else
    {
      m_quiz->finish();
      win->actionCollection()->action("quiz_check")->setEnabled(false);
      win->actionCollection()->action("qa_hint")->setEnabled(false);
      win->actionCollection()->action("quiz_repeat_errors")->setEnabled((m_error > 0));

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      lblAnswerBlank->hide();
      txtAnswer->hide();
      picQuestion->setPixmap(KIconLoader::global()->loadIcon("kwordquiz", KIconLoader::Panel));
      picAnswer->clear();
    }
  }
}

void QAView::slotHint()
{
  QString answer = txtAnswer->text();
  QString correctAnswer = m_quiz->hint(m_question);
  if (correctAnswer.left(4) == "<qt>")
  {
    correctAnswer = correctAnswer.remove("<qt>");
    correctAnswer = correctAnswer.remove("</qt>");
  }
  int minLength = qMin(answer.length(), correctAnswer.length());

  int correctCharCount = 1;

  if (answer.length() > 0)
  {
    for(int i = 0; i < minLength; i++)
    {
    if (answer.at(i) == correctAnswer.at(i))
      correctCharCount++;
    }
  }

  txtAnswer->setText(correctAnswer.left(correctCharCount));
  txtAnswer->setFocus();
  txtAnswer->setCursorPosition(txtAnswer->text().length());

  m_hintUsed = true;
}

void QAView::slotRestart()
{
  m_quiz->activateBaseList();
  init();
}

void QAView::slotRepeat()
{
  m_quiz->activateErrorList();
  init();
}

void QAView::updateScore()
{
  QString s;
  s = s.setNum(m_quiz->questionCount(), 10);
  lblScoreCount->setText(s);
  picCount->setPixmap(KIconLoader::global()->loadIcon("kwordquiz", KIconLoader::Panel));

  s = m_score->answerText();
  lblScoreAnswered->setText(s);
  if (!s.isEmpty())
    picAnswered->setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));

  s = m_score->correctText();
  lblScoreCorrect->setText(s);
  if (!s.isEmpty())
    picCorrect->setPixmap(KIconLoader::global()->loadIcon("check", KIconLoader::Panel));

  s = m_score->errorText();
  lblScoreError->setText(s);
  if (!s.isEmpty())
    picError->setPixmap(KIconLoader::global()->loadIcon("error", KIconLoader::Panel));
}


/*!
    \fn QAView::showQuestion(int i)
 */
void QAView::showQuestion(int i)
{
  lblQuestionLanguage -> setText(m_quiz ->langQuestion(i));
  lblQuestion -> setText(m_quiz ->question(i));

  picQuestion->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(i, KWQQuiz::IconLeftCol), KIconLoader::Panel));

  lblAnswerLanguage -> setText(m_quiz ->langAnswer(i));

  if (!QString(m_quiz ->blankAnswer(i)).isEmpty())
  {
    lblAnswerBlank->show();
    lblAnswerBlank->setText(m_quiz->blankAnswer(i));
  }
  else
    lblAnswerBlank->hide();

  txtAnswer -> setText("");

  picAnswer->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(i, KWQQuiz::IconRightCol), KIconLoader::Panel));

  ///@todo handle keyboard layouts
}

void QAView::slotApplySettings( )
{
  m_score ->setAsPercent(Prefs::percent());
  updateScore();
}

void QAView::slotSpecChar( const QChar & c)
{
  if (txtAnswer->hasFocus())
  {
    if (txtAnswer->hasSelectedText())
    {
      QString ls = txtAnswer->text();
      QString s = txtAnswer->selectedText();
      int len = s.length();
      int ss = txtAnswer->selectionStart();
      ls = ls.replace(ss, len, c);
      txtAnswer->setText(ls);
      txtAnswer->setSelection(ss, 1);
    }
    else
    {
      QString s = txtAnswer->text();
      int i = txtAnswer->cursorPosition();
      s.insert(i, c);
      txtAnswer->setText(s);
      txtAnswer->setCursorPosition(i + 1);
    }
  }
}

#include "qaview.moc"
