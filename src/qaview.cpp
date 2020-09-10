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

#include "qaview.h"

#include <QDBusInterface>
#include <QUrl>

#include <KLocalizedString>
#include <KNotification>

#include "prefs.h"
#include "kwqquizmodel.h"
#include "kwqscorewidget.h"

QString highlightError(const QString & c, const QString & e)
{
  if (c == e)
    return c;

  QString s = c;
  if (s.left(4) == QLatin1String("<qt>") && e.left(4) != QLatin1String("<qt>"))
      s = s.mid(4, s.length() - 9);

  if (s == e)
    return s;

  QString result = QStringLiteral("<qt>");
  int i = 0;
  while (i < e.length() && s[i] == e[i])
    result.append(e[i++]);
  result.append("<b>");
  QString result2 = QStringLiteral("</qt>");
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

QAView::QAView(QWidget *parent, KActionCollection * actionCollection) : KWQQuizView(parent, actionCollection)
{
  setupUi(this);

  connect(txtAnswer, SIGNAL(returnPressed()), this, SLOT(slotCheck()));
}

void QAView::init()
{
  score->clear();
  score->setQuestionCount(m_quiz->questionCount());
  score->setAsPercent(Prefs::percent());

  m_hintUsed = false;

  QFont f = Prefs::editorFont();
  f.setWeight(QFont::Normal);
  lblQuestion->setFont(f);
  lblAnswerBlank->setFont(f);
  txtAnswer->setFont(f);
  lblPreviousQuestion->setFont(f);
  lblYourAnswer->setFont(f);
  lblCorrect->setFont(f);

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
  m_actionCollection->action(QStringLiteral("qa_mark_last_correct"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("qa_hint"))->setEnabled(true);
  m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(false);
  m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);

  // reset last file
  audioPlayFile(QUrl(), true);

  showQuestion();
  txtAnswer->show();
  txtAnswer->setFocus();
}

void QAView::slotCheck()
{
  if (m_actionCollection->action(QStringLiteral("quiz_check"))->isEnabled())
  {
    bool fIsCorrect;

    if (m_hintUsed && Prefs::hintError())
    {
      //Force an Error
      fIsCorrect = m_quiz->checkAnswer(QLatin1String(""));
    }
    else
    {
      //Really check the answer
      fIsCorrect = m_quiz->checkAnswer(txtAnswer->text());
    }
    //Reset for next question
    m_hintUsed = false;

    if (fIsCorrect)
    {
      picYourAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("answer-correct")).pixmap(32));
      lblYourAnswer->setText(m_quiz->yourAnswer(txtAnswer->text()));
      lblCorrectHeader->clear();
      picCorrectAnswer->clear();
      lblCorrect->clear();
      score->countIncrement(KWQScoreWidget::cdCorrect);
      KNotification::event(QStringLiteral("QuizCorrect"), i18n("Your answer was correct!"));
      m_actionCollection->action(QStringLiteral("qa_mark_last_correct"))->setEnabled(false);
    }
    else
    {
      picYourAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("error")).pixmap(32));
      lblYourAnswer->setText(highlightError(m_quiz->answer(), m_quiz->yourAnswer(txtAnswer->text())));
      lblCorrect->setText(m_quiz->answer());
      picCorrectAnswer->setPixmap(QIcon::fromTheme(QStringLiteral("answer-correct")).pixmap(32));
      lblCorrectHeader->setText(i18n("Correct Answer"));
      score->countIncrement(KWQScoreWidget::cdError);
      KNotification::event(QStringLiteral("QuizError"), i18n("Your answer was incorrect."));
      m_actionCollection->action(QStringLiteral("qa_mark_last_correct"))->setEnabled(true);
    }

    audioPlayAnswer();

    lblPreviousQuestionHeader->setText(i18n("Previous Question"));
    lblPreviousQuestion->setText(m_quiz->question());
    //lblPreviousQuestion->setFont(m_quiz->fontQuestion(m_question));
    picPrevious->setPixmap(QIcon::fromTheme(QStringLiteral("question")).pixmap(32));

    lblYourAnswerHeader->setText(i18n("Your Answer"));

    m_quiz->toNext();
    if (!m_quiz->atEnd())
    {
      showQuestion();
    }
    else
    {
      m_quiz->finish();
      m_actionCollection->action(QStringLiteral("quiz_check"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("qa_hint"))->setEnabled(false);
      m_actionCollection->action(QStringLiteral("quiz_repeat_errors"))->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action(QStringLiteral("quiz_export_errors"))->setEnabled(m_quiz->hasErrors());
      m_actionCollection->action(QStringLiteral("qa_mark_last_correct"))->setEnabled(false);

      lblQuestionLanguage->setText(i18n("Summary"));
      lblQuestion->clear();
      lblAnswerLanguage->clear();
      lblAnswerBlank->hide();
      txtAnswer->hide();
      picQuestion->setPixmap(QIcon::fromTheme(QStringLiteral("kwordquiz")).pixmap(32));
      picAnswer->clear();
    }
  }
}

void QAView::slotHint()
{
  QString answer = txtAnswer->text();
  QString correctAnswer = m_quiz->hint();
  if (correctAnswer.left(4) == QLatin1String("<qt>"))
  {
    correctAnswer = correctAnswer.remove(QStringLiteral("<qt>"));
    correctAnswer = correctAnswer.remove(QStringLiteral("</qt>"));
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

/*!
    \fn QAView::showQuestion(int i)
 */
void QAView::showQuestion()
{
  lblQuestionLanguage->setText(m_quiz ->langQuestion());
  lblQuestion->setText(m_quiz ->question());
  //audioPlayQuestion();

  picQuestion->setPixmap(QIcon::fromTheme(m_quiz->quizIcon(KWQQuizModel::IconLeftCol)).pixmap(32));

  lblAnswerLanguage->setText(m_quiz ->langAnswer());

  if (!QString(m_quiz->blankAnswer()).isEmpty())
  {
    lblAnswerBlank->show();
    lblAnswerBlank->setText(m_quiz->blankAnswer());
  }
  else
    lblAnswerBlank->hide();

  txtAnswer->setText(QLatin1String(""));

  picAnswer->setPixmap(QIcon::fromTheme(m_quiz->quizIcon(KWQQuizModel::IconRightCol)).pixmap(32));

  QString layout = m_quiz->kbAnswer();
  if (!layout.isEmpty()) {
    QDBusInterface kxkb(QStringLiteral("org.kde.keyboard"), QStringLiteral("/Layouts"), QStringLiteral("org.kde.KeyboardLayouts"));
    if (kxkb.isValid())
      kxkb.call(QStringLiteral("setLayout"), layout);
  }
}

void QAView::slotApplySettings( )
{
  score->setAsPercent(Prefs::percent());
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

void QAView::slotMarkLastCorrect( )
{
  m_quiz->removeLastError();
  score->swapCount();
  m_actionCollection->action(QStringLiteral("qa_mark_last_correct"))->setEnabled(false);
}
