//
// C++ Interface: wqquiz
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#ifndef WQQUIZ_H
#define WQQUIZ_H

#include <qtable.h>
#include <qptrlist.h>

#include "wqlistitem.h"
#include "kwordquizview.h"

/**
@author Peter Hedlund
*/

class WQQuiz{

  public:
    enum QuizType {qtEditor, qtFlash, qtMultiple, qtQA};
    enum QuizIcon {qiLeftCol, qiRightCol, qiQuestion, qiCorrect, qiError};

    WQQuiz(KWordQuizView *t);
    ~WQQuiz();
    void activateErrorList();
    void activateBaseList();


    bool init();
    bool checkAnswer(int i, QString ans);
    QStringList multiOptions(int i);
    QString quizIcon(int i, QuizIcon ico);
    QString yourAnswer(int i, QString s);
    QString hint(int i);

    QuizType quizType() const {return m_quizType;}
    void setQuizType(QuizType qt);

    int quizMode() const {return m_quizMode;}
    void setQuizMode(int qm);

    //int questionCount() const {return m_questionCount;}
    int questionCount();
		QString question(int i);
    QString blankAnswer(int i);
    QString answer(int i);
    QString langQuestion(int i);
    QString langAnswer(int i);
    QFont fontQuestion(int i);
    QFont fontAnswer(int i);
    int kbAnswer(int i);

    int column() const {return m_column;}
    void setColumn(int i);

    bool enableBlanks() const {return m_enableBlanks;}
		void setEnableBlanks(bool b);

  private:
    KWordQuizView *m_table;
    int m_quizMode;
    int m_questionCount;
    static QPtrList<WQListItem> *m_list;
    //static QPtrList<WQListItem> *m_tempList;
    static QPtrList<WQListItem> *m_errorList;
    static QPtrList<WQListItem> *m_quizList;

    QuizType m_quizType;
    int m_column;
    QString m_correctBlank;
    QString m_answerBlank;
    bool m_enableBlanks;

    void addToList(int aCol, int bCol);
    void listRandom();

};

#endif
