/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kwqquizmodel.h"

#include "blankanswer.h"
#include "kwqsortfiltermodel.h"
#include "kwqtablemodel.h"

#include <QRandomGenerator>

#include <KRandom>

KWQQuizModel::KWQQuizModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_sourceModel = 0;
    m_currentQuestion = 0;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    m_list.clear();
    m_errorList.clear();
    m_quizList.clear();
}


void KWQQuizModel::setFilterModel(KWQSortFilterModel * sourceModel)
{
    m_sourceModel = sourceModel;
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

KWQSortFilterModel * KWQQuizModel::sourceModel() const
{
    return m_sourceModel;
}

bool KWQQuizModel::lessThan(const QModelIndex & left, const QModelIndex & right) const
{
    return m_list[left.row()] < m_list[right.row()];
}

bool KWQQuizModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QString text;
    text.clear();
    for (int i = 0 ; i < 2; i++)
        text.append(sourceModel()->data(sourceModel()->index(sourceRow, i, sourceParent), Qt::DisplayRole).toString());

    return !text.isEmpty();
}


int KWQQuizModel::column(int i)
{
    int result  = 0;
    if (isOdd(m_quizMode))
        result = 1;
    if ((m_quizMode == 5) && qAbs(i) == i)
        result = 0;
    return result;
}

bool KWQQuizModel::atEnd()
{
  return m_currentQuestion >= questionCount();
}

void KWQQuizModel::toNext()
{
  ++m_currentQuestion;
}

bool KWQQuizModel::hasErrors()
{
    if (m_quizType == Prefs::EnumStartSession::MultipleChoice)
        return m_errorList.count() > 2; //this is a hack until better multiple choice is written
    else
        return !m_errorList.isEmpty();
}

bool KWQQuizModel::init()
{
    if (m_sourceModel == 0)
        return false;

    invalidate();

    //check if enough in list
    bool result = false;
    switch (m_quizType)  {
        case Prefs::EnumStartSession::Editor:
            //
            break;
        case Prefs::EnumStartSession::Flashcard:
            result = (rowCount(QModelIndex()) > 0);
            break;
        case Prefs::EnumStartSession::QA:
            result = (rowCount(QModelIndex()) > 0);
            break;
        case Prefs::EnumStartSession::MultipleChoice:
            result = (rowCount(QModelIndex()) > 2);
            break;
    }

    if (!result)
        return false;

    m_quizList.clear();
    for (int i = 0; i < rowCount(QModelIndex()); ++i) {
        m_quizList.append(i);
        if (m_quizMode == 5)
            m_quizList.append(-i);
    }

    activateBaseList();
    return true;
}

void KWQQuizModel::activateBaseList()
{
    m_list.clear();
    m_errorList.clear();
    foreach(int l, m_quizList)
        m_list.append(l);

    if (m_quizMode > 2) {
        KRandom::shuffle(m_list, QRandomGenerator::global());
    };

    m_questionCount = m_list.count();
    m_currentQuestion = 0;
}

bool KWQQuizModel::checkAnswer(const QString & a)
{
    bool result = false;
    QString ans = a;

    int row =  m_list.at(m_currentQuestion);

    QString tTemp = data(index(qAbs(row), column(row), QModelIndex()), Qt::DisplayRole).toString();

    tTemp = tTemp.simplified();
    ans = ans.simplified();

    if (m_quizType == Prefs::EnumStartSession::QA) {
        if (!m_correctBlank.isEmpty()) {
            QStringList la, ls;
            if (ans.indexOf(';') > 0)
                ls = ans.split(';', Qt::SkipEmptyParts);
            else
                ls.append(ans);

            if (m_correctBlank.indexOf(';') > 0)
                la = m_correctBlank.split(';', Qt::SkipEmptyParts);
            else
                la.append(m_correctBlank);

            result = (ls.count() == la.count());
            if (result) {
                for (int counter = 0; counter < la.count(); counter++) {
                result = (ls[counter].simplified() == la[counter].simplified());
                if (!result)
                    break;
                }
            }
        }
        else
        {
          result = (ans == tTemp);
        }
      }
      else
      {
        if (m_quizType == Prefs::EnumStartSession::MultipleChoice) {
            if (Prefs::enableBlanks()) {
                tTemp.remove('[');
                tTemp.remove(']');
            }
            result = (ans == tTemp);
        }
        else
        {
          result = (ans == tTemp);
        }

      }
      ///@todo currently the Leitner stuff crashes KWQ
      //QString tmpLeitner( m_doc->entry(li.oneOp())->leitnerBox() );

      if (!result) {
          //m_doc->entry(li.oneOp())->setLeitnerBox(m_doc->leitnerSystem()->wrongBox( tmpLeitner ));
          m_errorList.append(row);
      }
      //else
          //m_doc->entry(li.oneOp())->setLeitnerBox(m_doc->leitnerSystem()->correctBox( tmpLeitner ));

      return result;
}


QStringList KWQQuizModel::multiOptions()
{
  QStringList ls;
  int a = -1;
  int b = -1;

  do
    a = QRandomGenerator::global()->bounded(m_questionCount);
  while(a == m_currentQuestion);

  do
    b = QRandomGenerator::global()->bounded(m_questionCount);
  while(b == m_currentQuestion || b == a);

  int row =  m_list.at(m_currentQuestion);
  int col  = column(row);
  if (col == 0)
    col = 0;
  else
    col = 1;
  ls.append(data(index(qAbs(row), col, QModelIndex()), Qt::DisplayRole).toString());
  row =  m_list.at(a);
  ls.append(data(index(qAbs(row), col, QModelIndex()), Qt::DisplayRole).toString());
  row =  m_list.at(b);
  ls.append(data(index(qAbs(row), col, QModelIndex()), Qt::DisplayRole).toString());

  if (Prefs::enableBlanks()) {
    for (int i = 0; i < ls.count(); i++) {
      ls[i].remove('[');
      ls[i].remove(']');
      }
  }

  KRandom::shuffle(ls, QRandomGenerator::global());

  return ls;
}


QString KWQQuizModel::quizIcon(QuizIcon ico)
{
    QString s = QStringLiteral("question");
    int col = column(m_list.at(m_currentQuestion));
    if (ico == IconLeftCol) {
        if (col == 0)
            s = QStringLiteral("answer");
    }

    if (ico == IconRightCol) {
        if (col != 0)
            s = QStringLiteral("answer");
    }
    return s;
}

QString KWQQuizModel::yourAnswer(const QString &givenAnswer) const
{
    return BlankAnswer::yourAnswerResult(givenAnswer, m_answerBlank);
}


QString KWQQuizModel::hint()
{
    if (!m_correctBlank.isEmpty())
        return m_correctBlank;
    else
        return answer();
}


QString KWQQuizModel::question()
{
    int row =  m_list.at(m_currentQuestion);
    int col  = column(row);
    if (col == 0)
        col = 1;
    else
        col = 0;

    QString s = data(index(qAbs(row), col, QModelIndex()), Qt::DisplayRole).toString();

    if (Prefs::enableBlanks()) {
        s.remove('[');
        s.remove(']');
    }

    if (m_quizType != Prefs::EnumStartSession::Flashcard && m_currentQuestion > 0)
        Q_EMIT checkingAnswer(m_list.at(m_currentQuestion - 1));
    else
        Q_EMIT checkingAnswer(row);

    return s;
}

QString KWQQuizModel::blankAnswer()
{
    m_correctBlank.clear();
    m_answerBlank.clear();

    if (m_quizType == Prefs::EnumStartSession::QA && Prefs::enableBlanks()) {
        int row = m_list.at(m_currentQuestion);
        const QString input = data(index(qAbs(row), column(row), QModelIndex()), Qt::DisplayRole).toString();

        const BlankAnswer::BlankResult result = BlankAnswer::blankAnswer(input);

        m_answerBlank = result.blankedAnswer;
        m_correctBlank = result.correctAnswer;
    }
    return m_answerBlank;
}

QString KWQQuizModel::answer()
{
    int row =  m_list.at(m_currentQuestion);
    QString s = data(index(qAbs(row), column(row), QModelIndex()), Qt::DisplayRole).toString();

    if (m_quizType == Prefs::EnumStartSession::QA) {
        if (Prefs::enableBlanks()) {
            s.replace('[', QLatin1String("<u>"));
            s.replace(']', QLatin1String("</u>"));
            s.prepend("<qt>");
            s.append("</qt>");
        }
    }
    else
    {
        if (Prefs::enableBlanks()) {
            s.remove('[');
            s.remove(']');
        }
    }
    return s;
}


QUrl KWQQuizModel::soundQuestion()
{
    int row =  m_list.at(m_currentQuestion);
    int col = column(row);
    col == 0 ? col = 1 : col = 0;
 
    QUrl soundUrl = QUrl::fromLocalFile(data(index(qAbs(row), col, QModelIndex()), KWQTableModel::SoundRole).toString());
    return soundUrl;
}
 

QUrl KWQQuizModel::soundAnswer()
{
    int row =  m_list.at(m_currentQuestion);
    QUrl soundUrl = QUrl::fromLocalFile(data(index(qAbs(row), column(row), QModelIndex()), KWQTableModel::SoundRole).toString());
    return soundUrl;
}


QPixmap KWQQuizModel::imageQuestion()
{
    int row =  m_list.at(m_currentQuestion);
    int col = column(row);
    col == 0 ? col = 1 : col = 0;

    QUrl imageUrl = QUrl::fromLocalFile(data(index(qAbs(row), col, QModelIndex()), KWQTableModel::ImageRole).toString());
    return QPixmap(imageUrl.toLocalFile());
}


QPixmap KWQQuizModel::imageAnswer()
{
    int row =  m_list.at(m_currentQuestion);
    QUrl imageUrl = QUrl::fromLocalFile(data(index(qAbs(row), column(row), QModelIndex()), KWQTableModel::ImageRole).toString());
    return QPixmap(imageUrl.toLocalFile());
}


QString KWQQuizModel::langQuestion()
{
    if (column(m_list.at(m_currentQuestion)) == 1)
        return headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
    else
        return headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();
}


QString KWQQuizModel::langAnswer()
{
    if (column(m_list.at(m_currentQuestion)) == 0)
        return headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
    else
        return headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();
}


QString KWQQuizModel::kbAnswer()
{
    int col = column(m_list.at(m_currentQuestion));
    QString result;

    if (col == 0)
        return headerData(0, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
    if (col == 1)
        return headerData(1, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();

    return result;
}


void KWQQuizModel::removeLastError()
{
    m_errorList.removeLast();
}


int KWQQuizModel::questionCount()
{
    return m_questionCount;
}


void KWQQuizModel::finish()
{
    Q_EMIT checkingAnswer(-1);
}


void KWQQuizModel::setQuizType(Prefs::EnumStartSession::type qt)
{
    m_quizType = qt;
}


void KWQQuizModel::setQuizMode(int qm)
{
    m_quizMode = qm;
}


void KWQQuizModel::activateErrorList()
{
    m_list.clear();
    foreach(int i, m_errorList)
        m_list.append(i);

    m_errorList.clear();
    m_questionCount = m_list.count();
    m_currentQuestion = 0;
}

inline bool KWQQuizModel::isOdd(int value) const { return value & 0x1; }
