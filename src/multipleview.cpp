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

#include "multipleview.h"

#include <QtGui/QButtonGroup>

#include <KIconLoader>
#include <KLocale>
#include <KNotification>
#include <KActionCollection>

#include "kwqquizmodel.h"
#include "kwqscorewidget.h"
#include "prefs.h"

MultipleView::MultipleView(QWidget *parent, KActionCollection *actionCollection) : KWQQuizView(parent, actionCollection)
{
  setupUi(this);
  m_choicesButtons = new QButtonGroup(this);
  m_choicesButtons->addButton(opt1, 1);
  m_choicesButtons->addButton(opt2, 2);
  m_choicesButtons->addButton(opt3, 3);
  connect(m_choicesButtons, SIGNAL(buttonClicked(int )), this, SLOT(slotChoiceClicked(int )));
  m_choicesActions = new QActionGroup(this);
  connect(m_choicesActions, SIGNAL(triggered(QAction *)), this, SLOT(slotChoiceActionTriggered(QAction *)));
  m_choicesActions->addAction(m_actionCollection->action("quiz_Opt1"));
  m_choicesActions->addAction(m_actionCollection->action("quiz_Opt2"));
  m_choicesActions->addAction(m_actionCollection->action("quiz_Opt3"));
  m_actionCollection->action("quiz_Opt1")->setData(1);
  m_actionCollection->action("quiz_Opt2")->setData(2);
  m_actionCollection->action("quiz_Opt3")->setData(3);
}

void MultipleView::init()
{
  m_choices.clear();

  score->clear();
  score->setQuestionCount(m_quiz->questionCount());
  score->setAsPercent(Prefs::percent());

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
  m_actionCollection->action("quiz_audio_play")->setEnabled(false);
  m_actionCollection->action("quiz_Opt1")->setEnabled(true);
  m_actionCollection->action("quiz_Opt2")->setEnabled(true);
  m_actionCollection->action("quiz_Opt3")->setEnabled(true);

  showQuestion();
}

void MultipleView::slotCheck()
{
  if (m_actionCollection->action("quiz_check")->isEnabled())
  {
    if (m_choicesButtons->checkedId() == -1)
        return;

    QString ans = m_choices[m_choicesButtons->checkedId() - 1];

    bool fIsCorrect = m_quiz->checkAnswer(ans);

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
      picYourAnswer->setPixmap(KIconLoader::global()->loadIcon("error", KIconLoader::Panel));
      lblCorrect->setText(m_quiz->answer());
      picCorrectAnswer->setPixmap(KIconLoader::global()->loadIcon("answer-correct", KIconLoader::Panel));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      score->countIncrement(KWQScoreWidget::cdError);
      KNotification::event("QuizError", i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question());
    picPrevious->setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));

    lblYourAnswerHeader->setText(i18n("Your Answer"));
    lblYourAnswer->setText(m_quiz->yourAnswer(ans));

    m_quiz->toNext();
    if (!m_quiz->atEnd())
    {
      showQuestion();
    }
    else
    {
      m_quiz->finish();
      m_actionCollection->action("quiz_check")->setEnabled(false);
      m_actionCollection->action("quiz_Opt1")->setEnabled(false);
      m_actionCollection->action("quiz_Opt2")->setEnabled(false);
      m_actionCollection->action("quiz_Opt3")->setEnabled(false);
      m_actionCollection->action("quiz_repeat_errors")->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action("quiz_export_errors")->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action("quiz_audio_play")->setEnabled(false);

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

void MultipleView::slotChoiceActionTriggered(QAction *choice)
{
  slotChoiceClicked(choice->data().toInt());
}

void MultipleView::slotChoiceClicked(int choice)
{
  m_choicesButtons->button(choice)->setChecked(true);
  if (Prefs::autoCheck())
    slotCheck();
}

/*!
    \fn MultipleView::showQuestion()
 */
void MultipleView::showQuestion()
{
  lblQuestionLanguage->setText(m_quiz ->langQuestion());
  lblQuestion->setText(m_quiz ->question());

  picQuestion->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(KWQQuizModel::IconLeftCol), KIconLoader::Panel));

  lblAnswerLanguage->setText(m_quiz ->langAnswer());

  m_choices = m_quiz->multiOptions();

  opt1->setText("&1 " + m_choices[0]);
  opt2->setText("&2 " + m_choices[1]);
  opt3->setText("&3 " + m_choices[2]);

  m_choicesButtons->setExclusive(false);
  opt1->setChecked(false);
  opt2->setChecked(false);
  opt3->setChecked(false);
  m_choicesButtons->setExclusive(true);
  setFocus();
  picAnswer->setPixmap(KIconLoader::global()->loadIcon(m_quiz->quizIcon(KWQQuizModel::IconRightCol), KIconLoader::Panel));
}

void MultipleView::slotApplySettings()
{
  score->setAsPercent(Prefs::percent());
}

#include "multipleview.moc"
