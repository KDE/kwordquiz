/* This file is part of KWordQuiz
  Copyright (C) 2003-2007 Peter Hedlund <peter.hedlund@kdemail.net>

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

#ifndef KWQQUIZ_H
#define KWQQUIZ_H

#include <QAbstractItemModel>
#include <QList>

/**
@author Peter Hedlund
*/

class KWQListItem
{
public:
  /**
   * Constructs an empty KWQListItem
   */
  KWQListItem(){}
  /**
   * Constructs a KWQListItem
   * @param question the column containing the question
   * @param first the row containing the correct answer (also the first choice for multiple choice)
   * @param second the row containing the second choice for multiple choice
   * @param third the row containing the third choice for multiple choice
   */
  KWQListItem(int question, int first, int second, int third) : m_question(question), m_firstChoice(first), m_secondChoice(second), m_thirdChoice(third) {}

  int question()     {return m_question;}
  int firstChoice()  {return m_firstChoice;}
  int secondChoice() {return m_secondChoice;}
  int thirdChoice()  {return m_thirdChoice;}

private:
  int m_question;
  int m_firstChoice;
  int m_secondChoice;
  int m_thirdChoice;
};


class KWQQuiz : public QObject
{
Q_OBJECT
public:
  enum QuizType {QuizEditor, QuizFlashCard, QuizMultipleChoice, QuizQuestionAnswer};
  enum QuizIcon {IconLeftCol, IconRightCol, IconQuestion, IconCorrect, IconError};

  KWQQuiz(QObject *parent);

  void setModel(QAbstractItemModel *);

  void activateErrorList();
  void activateBaseList();

  bool init();
  void finish();
  bool checkAnswer(int i, const QString & );
  QStringList multiOptions(int i);
  QString quizIcon(int i, QuizIcon ico);
  QString yourAnswer(const QString & s);
  QString hint(int i);

  QuizType quizType() const {return m_quizType;}
  void setQuizType(QuizType qt);

  int quizMode() const {return m_quizMode;}
  void setQuizMode(int qm);

  int questionCount();
  QString question(int i);
  QString blankAnswer(int i);
  QString answer(int i);
  QString langQuestion(int i);
  QString langAnswer(int i);

  int kbAnswer(int i);

signals:
  void checkingAnswer(int );

private:
  QAbstractItemModel *m_model;
  int m_quizMode;
  int m_questionCount;
  QList<KWQListItem> m_list;
  QList<KWQListItem> m_errorList;
  QList<KWQListItem> m_quizList;

  QuizType m_quizType;
  QString m_correctBlank;
  QString m_answerBlank;

  void buildList(int );
};

#endif
