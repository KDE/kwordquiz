//
// C++ Implementation: qaview
//
// Description:
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#include <qlabel.h>

#include <kstandarddirs.h> //locate
#include <klocale.h>
#include <klineedit.h>
#include <knotifyclient.h>

#include "qaview.h"
#include "configuration.h"
#include "kwordquiz.h"

QAView::QAView(QWidget *parent, const char *name, WFlags f):QAViewBase(parent, name, f)
{
  m_score = new WQScore();
}


QAView::~QAView()
{
}

void QAView::setQuiz(WQQuiz *quiz)
{
  m_quiz = quiz;
}

void QAView::init()
{
  m_score -> setQuestionCount(m_quiz->questionCount());
  m_score -> setAsPercent(Config().m_percent);
  m_hintUsed = false;
  m_question = 0;
  m_error = 0;
  picAnswered -> clear();
  picCorrect -> clear();
  picError -> clear();

  lblQuestion -> setFont(Config().m_editorFont);
  lblAnswerBlank -> setFont(Config().m_editorFont);
  txtAnswer -> setFont(Config().m_editorFont);
  lblPreviousQuestion -> setFont(Config().m_editorFont);
  lblYourAnswer -> setFont(Config().m_editorFont);
  lblCorrect -> setFont(Config().m_editorFont);
      
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

    if (m_hintUsed && Config().m_hintError)
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

      picYourAnswer->setPixmap(QPixmap(locate("data", "kwordquiz/pics/correct.png")));
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

      picYourAnswer->setPixmap(QPixmap(locate("data", "kwordquiz/pics/error.png")));

      lblCorrect->setText(m_quiz->answer(m_question));
      //lblCorrect->setFont(m_quiz->fontAnswer(m_question));
      picCorrectAnswer->setPixmap(QPixmap(locate("data", "kwordquiz/pics/correct.png")));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      m_score->countIncrement(WQScore::cdError);
      updateScore();
      KNotifyClient::event(winId(), "QuizError", i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question(m_question));
    //lblPreviousQuestion->setFont(m_quiz->fontQuestion(m_question));
    picPrevious->setPixmap(QPixmap(locate("data", "kwordquiz/pics/question.png")));

    lblYourAnswerHeader->setText(i18n("Your Answer"));
    lblYourAnswer->setText(m_quiz->yourAnswer(m_question, txtAnswer->text()));
    //lblYourAnswer->setFont(m_quiz->fontAnswer(m_question));

    if (++m_question < m_quiz->questionCount())
    {
      showQuestion(m_question);
    }
    else
    {

      win->actionCollection()->action("quiz_check")->setEnabled(false);
      win->actionCollection()->action("qa_hint")->setEnabled(false);
      win->actionCollection()->action("quiz_repeat_errors")->setEnabled((m_error > 0));

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      lblAnswerBlank->hide();
      txtAnswer->hide();
      picQuestion->setPixmap(QPixmap(locate("data", "kwordquiz/pics/appicon.png")));
      picAnswer->clear();
    }
  }
}

void QAView::slotHint()
{
  QString answer = txtAnswer->text();
  QString correctAnswer = m_quiz->hint(m_question);

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


/*!
    \fn QAView::showQuestion(int i)
 */
void QAView::showQuestion(int i)
{
  //m_quiz->setColumn(i);
  lblQuestionLanguage -> setText(m_quiz ->langQuestion(i));
  lblQuestion -> setText(m_quiz ->question(i));
  //lblQuestion -> setFont(m_quiz->fontQuestion(i));
  
  picQuestion->setPixmap(QPixmap(locate("data", "kwordquiz/pics/" + m_quiz->quizIcon(i, WQQuiz::qiLeftCol) + ".png")));

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

  picAnswer->setPixmap(QPixmap(locate("data", "kwordquiz/pics/" + m_quiz->quizIcon(i, WQQuiz::qiRightCol) + ".png")));  

  //@todo handle keyboard layouts
}

void QAView::slotApplySettings( )
{
  m_score ->setAsPercent(Config().m_percent);
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
