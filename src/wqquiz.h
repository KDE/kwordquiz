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

#ifndef WQQUIZ_H
#define WQQUIZ_H

#include <tqtable.h>

#include "wqlistitem.h"
#include "kwordquizview.h"

/**
@author Peter Hedlund
*/

typedef TQValueList<WQListItem> QuizList;

class WQQuiz : public QObject
{
  Q_OBJECT
  public:
    enum QuizType {qtEditor, qtFlash, qtMultiple, qtQA};
    enum QuizIcon {qiLeftCol, qiRightCol, qiQuestion, qiCorrect, qiError};

    WQQuiz(KWordQuizView * parent, const char * name=0);

    void activateErrorList();
    void activateBaseList();

    bool init();
    void finish();
    bool checkAnswer(int i, const TQString & );
    TQStringList multiOptions(int i);
    TQString quizIcon(int i, QuizIcon ico);
    TQString yourAnswer(int i, const TQString & s);
    TQString hint(int i);

    QuizType quizType() const {return m_quizType;}
    void setQuizType(QuizType qt);

    int quizMode() const {return m_quizMode;}
    void setQuizMode(int qm);

    int questionCount();
    TQString question(int i);
    TQString blankAnswer(int i);
    TQString answer(int i);
    TQString langQuestion(int i);
    TQString langAnswer(int i);

    int kbAnswer(int i);

  signals:
    void checkingAnswer(int );

  private:
    KWordQuizView *m_table;
    int m_quizMode;
    int m_questionCount;
    QuizList m_list;
    QuizList m_errorList;
    QuizList m_quizList;

    QuizType m_quizType;
    TQString m_correctBlank;
    TQString m_answerBlank;

    void addToList(int aCol, int bCol);
    void listRandom();
};

#endif
