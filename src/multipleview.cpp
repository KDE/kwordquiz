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

#include <QButtonGroup>

#include <KLocalizedString>
#include <KNotification>

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
  connect(m_choicesButtons, SIGNAL(buttonClicked(int)), this, SLOT(slotChoiceClicked(int)));
  m_choicesActions = new QActionGroup(this);
  connect(m_choicesActions, &QActionGroup::triggered, this, &MultipleView::slotChoiceActionTriggered);
  m_choicesActions->addAction(m_actionCollection->action(QStringLiteral("quiz_Opt1")));
  m_choicesActions->addAction(m_actionCollection->action(QStringLiteral("quiz_Opt2")));
  m_choicesActions->addAction(m_actionCollection->action(QStringLiteral("quiz_Opt3")));
  m_actionCollection->action(QStringLiteral("quiz_Opt1"))->setData(1);
  m_actionCollection->action(QStringLiteral("quiz_Opt2"))->setData(2);
  m_actionCollection->action(QStringLiteral("quiz_Opt3"))->setData(3);
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

  m_actionCollection->action(QStringLiteral("quiz_check"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_Opt1"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("quiz_Opt2"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("quiz_Opt3"))->setEnabled(true);

  showQuestion();
}

void MultipleView::slotCheck()
{
  if (m_actionCollection->action(QStringLiteral("quiz_check"))->isEnabled())
  {
    if (m_choicesButtons->checkedId() == -1)
        return;

    QString ans = m_choices[m_choicesButtons->checkedId() - 1];

    bool fIsCorrect = m_quiz->checkAnswer(ans);

    if (fIsCorrect)
    {
      picYourAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("answer-correct")).pixmap(32));
      lblCorrectHeader->clear();
      picCorrectAnswer->clear();
      lblCorrect->clear();
      score->countIncrement(KWQScoreWidget::cdCorrect);
      KNotification::event(QStringLiteral("QuizCorrect"), i18n("Your answer was correct!"));
    }
    else
    {
      picYourAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("error")).pixmap(32));
      lblCorrect->setText(m_quiz->answer());
      picCorrectAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("answer-correct")).pixmap(32));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      score->countIncrement(KWQScoreWidget::cdError);
      KNotification::event(QStringLiteral("QuizError"), i18n("Your answer was incorrect."));
    }

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question());
    picPrevious->setPixmap(QIcon::fromTheme(QStringLiteral("question")).pixmap(32));

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
      m_actionCollection->action(QStringLiteral("quiz_check"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("quiz_Opt1"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("quiz_Opt2"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("quiz_Opt3"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      opt1->hide();
      opt2->hide();
      opt3->hide();
      picQuestion->setPixmap(QIcon::fromTheme(QStringLiteral("kwordquiz")).pixmap(32));
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

  picQuestion->setPixmap(QIcon::fromTheme(m_quiz->quizIcon(KWQQuizModel::IconLeftCol)).pixmap(32));

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
  picAnswer->setPixmap(QIcon::fromTheme(m_quiz->quizIcon(KWQQuizModel::IconRightCol)).pixmap(32));
}

void MultipleView::slotApplySettings()
{
  score->setAsPercent(Prefs::percent());
}
