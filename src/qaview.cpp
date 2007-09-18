/* This file is part of KWordQuiz
  Copyright (C) 2003 Peter Hedlund <peter.hedlund@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>
#include <klineedit.h>
#include <knotifyclient.h>

#include "qaview.h"
#include "prefs.h"
#include "kwordquiz.h"

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
  while (s[i] == e[i])
    result.append(e[i++]);
  result.append("<b>");
  QString result2 = "</qt>";
  int j = s.length() - 1;
  int k = e.length() - 1;
  while (s[j] == e[k] && k > i)
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


QAView::QAView(QWidget *parent, const char *name, WFlags f):QAViewBase(parent, name, f)
{
  m_score = new WQScore();
}


QAView::~QAView()
{
  delete txtAnswer; //Fixes #122110 on PCLOS and Ubuntu
}

void QAView::setQuiz(WQQuiz *quiz)
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
  //lblQuestion -> font().setBold(false);
  lblAnswerBlank -> setFont(f);
  //lblAnswerBlank -> font().setBold(false);
  txtAnswer -> setFont(f);
  //txtAnswer -> font().setBold(false);
  lblPreviousQuestion -> setFont(f);
  //lblPreviousQuestion -> font().setBold(false);
  lblYourAnswer -> setFont(f);
  //lblYourAnswer -> font().setBold(false);
  lblCorrect -> setFont(f);
  //lblCorrect -> font().setBold(false);

  lblPreviousQuestionHeader->clear();
  lblPreviousQuestion->clear();
  lblYourAnswerHeader->clear();
  lblYourAnswer->clear();
  lblCorrectHeader->clear();
  lblCorrect->clear();

  picPrevious->clear();
  picYourAnswer->clear();
  picCorrectAnswer->clear();

  KWordQuizApp *win=(KWordQuizApp *) parent();
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
  KWordQuizApp *win=(KWordQuizApp *) parent();
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

      picYourAnswer->setPixmap(KGlobal::iconLoader()->loadIcon("check", KIcon::Panel));
      lblYourAnswer->setText(m_quiz->yourAnswer(m_question, txtAnswer->text()));
      lblCorrectHeader->clear();
      picCorrectAnswer->clear();
      lblCorrect->clear();
      m_score->countIncrement(WQScore::cdCorrect);
      updateScore();
      KNotifyClient::event(winId(), "QuizCorrect", i18n("Your answer was correct!"));
    }
    else
    {
      m_error++;

      picYourAnswer->setPixmap(KGlobal::iconLoader()->loadIcon("error", KIcon::Panel));
      lblYourAnswer->setText(highlightError(m_quiz->answer(m_question), m_quiz->yourAnswer(m_question, txtAnswer->text())));
      lblCorrect->setText(m_quiz->answer(m_question));
      picCorrectAnswer->setPixmap(KGlobal::iconLoader()->loadIcon("check", KIcon::Panel));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      m_score->countIncrement(WQScore::cdError);
      updateScore();
      KNotifyClient::event(winId(), "QuizError", i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question(m_question));
    picPrevious->setPixmap(KGlobal::iconLoader()->loadIcon("question", KIcon::Panel));
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
      picQuestion->setPixmap(KGlobal::iconLoader()->loadIcon("kwordquiz", KIcon::Panel));
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

  int correctCharCount = 1;

  if (answer.length() > 0)
  {
    for(uint i = 0; i <= answer.length(); i++)
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


/*!
    \fn QAView::showQuestion(int i)
 */
void QAView::showQuestion(int i)
{
  //m_quiz->setColumn(i);
  lblQuestionLanguage -> setText(m_quiz ->langQuestion(i));
  lblQuestion -> setText(m_quiz ->question(i));
  //lblQuestion -> setFont(m_quiz->fontQuestion(i));
  
  picQuestion->setPixmap(KGlobal::iconLoader()->loadIcon(m_quiz->quizIcon(i, WQQuiz::qiLeftCol), KIcon::Panel));

  lblAnswerLanguage -> setText(m_quiz ->langAnswer(i));

  if (!QString(m_quiz ->blankAnswer(i)).isEmpty())
  {
    lblAnswerBlank->show();
    //lblAnswerBlank->setFont(m_quiz->fontAnswer(i));
    lblAnswerBlank->setText(m_quiz->blankAnswer(i));
  }
  else
    lblAnswerBlank->hide();

  //txtAnswer->setFont(m_quiz->fontAnswer(i));
  txtAnswer -> setText("");

  picAnswer->setPixmap(KGlobal::iconLoader()->loadIcon(m_quiz->quizIcon(i, WQQuiz::qiRightCol), KIcon::Panel));  

  //@todo handle keyboard layouts
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
