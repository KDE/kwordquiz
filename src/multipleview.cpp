//
// C++ Implementation: multipleview
//
// Description:
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#include <qlabel.h>
#include <qradiobutton.h>

#include <kstandarddirs.h> //locate
#include <klocale.h>
#include <knotifyclient.h>

#include "kwordquiz.h"
#include "multipleview.h"
#include "configuration.h"

MultipleView::MultipleView(QWidget *parent, const char *name, WFlags f)
    : MultipleViewBase(parent, name, f)
{
  m_score = new WQScore();
}


MultipleView::~MultipleView()
{}

void MultipleView::setQuiz(WQQuiz *quiz)
{
  m_quiz = quiz;
}

void MultipleView::init()
{

  m_score ->setQuestionCount(m_quiz->questionCount());
  m_score ->setAsPercent(Config().m_percent);
  m_question = 0;
  m_error = 0;

  opt1->show();
  opt2->show();
  opt3->show();

  lblQuestion -> setFont(Config().m_editorFont);
  lblPreviousQuestion -> setFont(Config().m_editorFont);
  lblYourAnswer -> setFont(Config().m_editorFont);
  lblCorrect -> setFont(Config().m_editorFont); 
  opt1->setFont(Config().m_editorFont);
  opt2->setFont(Config().m_editorFont);
  opt3->setFont(Config().m_editorFont);
  
  picAnswered->clear();
  picCorrect->clear();
  picError->clear();

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
  win->actionCollection()->action("quiz_repeat_errors")->setEnabled(false);

  updateScore();
  showQuestion(0);
}

void MultipleView::slotCheck()
{
  KWordQuizApp *win=(KWordQuizApp *) parent();
  if (win->actionCollection()->action("quiz_check")->isEnabled())
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
    lblYourAnswer->setText(m_quiz->yourAnswer(m_question, ans));
    //lblYourAnswer->setFont(m_quiz->fontAnswer(m_question));

    if (++m_question < m_quiz->questionCount())
    {
      showQuestion(m_question);
    }
    else
    {

      win->actionCollection()->action("quiz_check")->setEnabled(false);
      win->actionCollection()->action("quiz_repeat_errors")->setEnabled((m_error > 0));

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      opt1->hide();
      opt2->hide();
      opt3->hide();
      picQuestion->setPixmap(QPixmap(locate("data", "kwordquiz/pics/appicon.png")));
      picAnswer->clear();
    }
  }
}

void MultipleView::slotOpt1Clicked()
{
  if (Config().m_autoCheck)
    slotCheck();
  else
  {
    opt2->setChecked(false);
    opt3->setChecked(false);
  }
}

void MultipleView::slotOpt2Clicked()
{
  if (Config().m_autoCheck)
    slotCheck();
  else
  {
    opt1->setChecked(false);
    opt3->setChecked(false);
  }
}

void MultipleView::slotOpt3Clicked()
{
  if (Config().m_autoCheck)
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

void MultipleView::updateScore()
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
    \fn MultipleView::showQuestion(int i)
 */
void MultipleView::showQuestion(int i)
{

  lblQuestionLanguage -> setText(m_quiz ->langQuestion(i));
  lblQuestion -> setText(m_quiz ->question(i));
  //lblQuestion -> setFont(m_quiz->fontQuestion(i));

  picQuestion->setPixmap(QPixmap(locate("data", "kwordquiz/pics/" + m_quiz->quizIcon(i, WQQuiz::qiLeftCol) + ".png")));

  lblAnswerLanguage -> setText(m_quiz ->langAnswer(i));

  //opt1->setFont(m_quiz->fontAnswer(i));
  //opt2->setFont(m_quiz->fontAnswer(i));
  //opt3->setFont(m_quiz->fontAnswer(i));

  QStringList sl = m_quiz->multiOptions(i);
  QString s[10];
  int j = 0;
  for(QStringList::Iterator it = sl.begin(); it != sl.end(); ++it)
  {
    s[j] = *it;
    j++;
  }

  opt1->setText("&1 " + s[0]);
  opt2->setText("&2 " + s[1]);
  opt3->setText("&3 " + s[2]);

  opt1->setChecked(false);
  opt2->setChecked(false);
  opt3->setChecked(false);

  picAnswer->setPixmap(QPixmap(locate("data", "kwordquiz/pics/" + m_quiz->quizIcon(i, WQQuiz::qiRightCol) + ".png")));
}

void MultipleView::slotApplySettings( )
{
  m_score ->setAsPercent(Config().m_percent);
  updateScore();
}

#include "multipleview.moc"
