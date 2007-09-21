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

#include "kwqquiz.h"

#include <QRegExp>

#include <KRandomSequence>

#include "keduvocexpression.h"
#include "leitnersystem.h"
#include "prefs.h"

KWQQuiz::KWQQuiz(QObject * parent) : QObject(parent)
{
  m_model = 0;

  m_list.clear();
  m_errorList.clear();
  m_quizList.clear();
}


void KWQQuiz::activateErrorList()
{
  m_list.clear();
  foreach(const KWQListItem &l, m_errorList)
    m_list.append(l);

  m_errorList.clear();
  m_questionCount = m_list.count();
}


void KWQQuiz::activateBaseList()
{
  m_list.clear();

  if (m_quizMode > 2)
  {
    KRandomSequence rs;
    rs.randomize(m_quizList);
  };

  foreach(const KWQListItem &l, m_quizList)
    m_list.append(l);

  m_questionCount = m_list.count();
}


void KWQQuiz::buildList(int column)
{
  //build a list of row numbers containing text in both columns
  QList<int> tempList;
  for (int current = 0; current < m_model->rowCount(QModelIndex()); current++)
  {
    QString front = m_model->data(m_model->index(current, 0, QModelIndex()), Qt::DisplayRole).toString();
    QString back  = m_model->data(m_model->index(current, 1, QModelIndex()), Qt::DisplayRole).toString();
    if (!front.isEmpty() && !back.isEmpty())
      tempList.append(current);
  }

  KRandomSequence rs(0);

  int count = tempList.count();

  foreach(int i, tempList)
  {
    int a = -1;
    int b = -1;
    if (count > 2)
    {
      do
        a = rs.getLong(count);
      while(a == i);

      do
        b = rs.getLong(count);
      while(b == i || b == a);
    }
    m_quizList.append(KWQListItem(column, i, a, b));
  }
}


bool KWQQuiz::init()
{
  bool result = false;
  int aCol = 0;

  switch (m_quizMode)
  {
    case 1: aCol = 0; break;
    case 2: aCol = 1; break;
    case 3: aCol = 0; break;
    case 4: aCol = 1; break;
    case 5: aCol = 0; break;
  }

  buildList(aCol);

  //check if enough in list
  switch (m_quizType)
  {
  case QuizEditor:
    //
    break;
  case QuizFlashCard:
    result = (m_quizList.count() > 0);
    break;
  case QuizQuestionAnswer:
    result = (m_quizList.count() > 0);
    break;
  case QuizMultipleChoice:
    result = (m_quizList.count() > 2);
    break;
  }

  if (!result)
    return false;

  if (m_quizMode == 5)
  {
    aCol = 1;
    buildList(aCol);
  }

  //Prepare final lists
  activateBaseList();
  return true;
}


bool KWQQuiz::checkAnswer(int i, const QString & a)
{
  bool result = false;
  KWQListItem li = m_list.at(i);
  QString ans = a;
  QString tTemp = m_model->data(m_model->index(li.firstChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString();

  tTemp = tTemp.simplified();
  ans = ans.simplified();

  if (m_quizType == QuizQuestionAnswer)
  {
    if (QString(m_correctBlank).length() > 0)
    {
      QStringList la, ls;
      if (ans.indexOf(";") > 0)
        ls = ans.split(";", QString::SkipEmptyParts);
      else
        ls.append(ans);

      if (m_correctBlank.indexOf(";") > 0)
        la = m_correctBlank.split(";", QString::SkipEmptyParts);
      else
        la.append(m_correctBlank);

      result = (ls.count() == la.count());
      if (result)
      {
        for (int counter = 0; counter < la.count(); counter++)
        {
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
    if (m_quizType == QuizMultipleChoice)
    {
      if (Prefs::enableBlanks())
      {
        tTemp.remove("[");
        tTemp.remove("]");
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

  if (!result)
  {
    //m_doc->entry(li.oneOp())->setLeitnerBox(m_doc->leitnerSystem()->wrongBox( tmpLeitner ));
    m_errorList.append(li);
  }
  //else
    //m_doc->entry(li.oneOp())->setLeitnerBox(m_doc->leitnerSystem()->correctBox( tmpLeitner ));

  return result;
}


QStringList KWQQuiz::multiOptions(int i)
{
  QStringList ls;

  KWQListItem li = m_list.at(i);

  ls.append(m_model->data(m_model->index(li.firstChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString());
  ls.append(m_model->data(m_model->index(li.secondChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString());
  ls.append(m_model->data(m_model->index(li.thirdChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString());
  if (Prefs::enableBlanks()) {
    for (int i = 0; i < ls.count(); i++) {
      ls[i].remove("[");
      ls[i].remove("]");
      }
  }

  KRandomSequence rs(0);
  rs.randomize(ls);

  return ls;
}


QString KWQQuiz::quizIcon(int i, QuizIcon ico)
{
  QString s;
  KWQListItem li = m_list.at(i);
  if (ico == IconLeftCol)
  {
    if (li.question() == 0)
      s = "question";
    else
      s = "answer";
  }

  if (ico == IconRightCol)
  {
    if (li.question() == 0)
      s = "answer";
    else
      s = "question";
  }
  return s;
}


QString KWQQuiz::yourAnswer(const QString & s)
{
  QString result ="";

  if (QString(m_answerBlank).length() > 0)
  {
    QStringList ls;

    if (s.indexOf(";") > 0)
      ls = s.split(";");
    else
      ls.append(s);

    result = m_answerBlank.replace("..........", "<u></u>");

    int offset = 0, counter = 0;
    while (offset >= 0)
    {
      offset = result.indexOf("<u>", offset);
      if (offset >= 0)
      {
        result.insert(offset + 3, ls[counter]);
        offset++;
        counter++;
      }
    }
    result.append("</qt>");
    result.prepend("<qt>");
  }
  else
  {
    result = s;
  }
  return result;
}


QString KWQQuiz::hint(int i)
{
  if (QString(m_correctBlank).length() > 0)
  {
    return m_correctBlank;
  }
  else
  {
    return answer(i);
  }
}


void KWQQuiz::setQuizType(QuizType qt)
{
  m_quizType = qt;
}


void KWQQuiz::setQuizMode(int qm)
{
  m_quizMode = qm;
}


QString KWQQuiz::question(int i)
{
  KWQListItem li = m_list.at(i);
  QString s = m_model->data(m_model->index(li.firstChoice(), li.question() ? 1 : 0, QModelIndex()), Qt::DisplayRole).toString();

  if (Prefs::enableBlanks())
  {
    s.remove("[");
    s.remove("]");
  }
  if (m_quizType != QuizFlashCard && i > 0)
  {
    KWQListItem li2 = m_list.at(i - 1);
    emit checkingAnswer(li2.firstChoice());
  }
  else
    emit checkingAnswer(li.firstChoice());

  return s;
}


QString KWQQuiz::blankAnswer(int i)
{

  QString r = "";
  m_correctBlank = "";
  m_answerBlank = "";
  QString tTemp;

  if (m_quizType == QuizQuestionAnswer && Prefs::enableBlanks())
  {
    KWQListItem li = m_list.at(i);
    tTemp = m_model->data(m_model->index(li.firstChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString();
    r = tTemp;
    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern("\\[.*\\]");

    r.replace(rx, "..........");

    if (r != tTemp)
    {
      m_answerBlank = r;
      int offset = 0;
      while (offset >= 0)
      {
        offset = rx.indexIn(tTemp, offset);
        if (offset >= 0)
        {
          if (m_correctBlank.length() > 0)
            m_correctBlank = m_correctBlank + "; " + tTemp.mid(offset + 1, tTemp.indexOf(']', offset) - offset - 1);
          else
            m_correctBlank = tTemp.mid(offset + 1, tTemp.indexOf(']', offset) - offset - 1);
          offset++;
        }
      }
    }
  }
  return m_answerBlank;
}


QString KWQQuiz::answer(int i)
{
  QString s;
  KWQListItem li = m_list.at(i);

  if (m_quizType == QuizQuestionAnswer)
  {
    s = m_model->data(m_model->index(li.firstChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString();
    if (Prefs::enableBlanks())
    {
      s.replace("[", "<u>");
      s.replace("]", "</u>");
      s.prepend("<qt>");
      s.append("</qt>");
    }
  }
  else
  {
    s = m_model->data(m_model->index(li.firstChoice(), li.question() ? 0 : 1, QModelIndex()), Qt::DisplayRole).toString();
    if (Prefs::enableBlanks())
    {
      s.remove("[");
      s.remove("]");
    }
  }
  return s;
}


QString KWQQuiz::langQuestion(int i)
{
  KWQListItem li = m_list.at(i);
  if (li.question() == 0)
    return m_model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
  else
    return m_model->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();
}


QString KWQQuiz::langAnswer(int i)
{
  KWQListItem li = m_list.at(i);
  if (li.question() == 1)
    return m_model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
  else
    return m_model->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();;
}


int KWQQuiz::kbAnswer(int /*i*/)
{
/*  KWQListItem *li = m_list->at(i);
  if (li->question() == 0)
  {
    //@todo return m_table ->layoutLeft();
  }
  else
  {
    //@todo return m_table -> layoutRight();
  }*/
  return 0;
}


int KWQQuiz::questionCount()
{
  return m_questionCount;
}


void KWQQuiz::finish()
{
  emit checkingAnswer(-1);
}


void KWQQuiz::setModel(QAbstractItemModel * model)
{
  m_model = model;
}

#include "kwqquiz.moc"
