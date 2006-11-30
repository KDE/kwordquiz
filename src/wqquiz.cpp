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

#include <krandomsequence.h>

#include <qregexp.h>

#include "wqquiz.h"
#include "prefs.h"

WQQuiz::WQQuiz(KWordQuizView * parent, const char *name) : QObject(parent, name)
{
  m_table = parent;

  m_list.clear();
  m_errorList.clear();
  m_quizList.clear();
}

void WQQuiz::activateErrorList()
{
  m_list.clear();

  WQListItem l;

  QuizList::iterator it;
  for ( it = m_errorList.begin(); it != m_errorList.end(); ++it )
    m_list.append(*it);

  m_errorList.clear();
  m_questionCount = m_list.count();

}

void WQQuiz::activateBaseList()
{
  m_list.clear();

  if (m_quizMode > 2)
  {
    listRandom();
  };

  QuizList::iterator it;
  for (it = m_quizList.begin(); it != m_quizList.end(); ++it)
    m_list.append(*it);

  m_questionCount = m_list.count();

}

void WQQuiz::addToList(int aCol, int bCol)
{
  //build a list of row numbers containing text in both columns

  typedef QValueList<int> IntList;
  IntList tempList;
  for (int current = 0; current < m_table ->numRows(); ++current)
  {
    if (!m_table->text(current, 0).isEmpty() && !m_table->text(current, 1).isEmpty())
    {
      tempList.append(current);
    }
  }

  KRandomSequence *rs = new KRandomSequence(0);

  int count = tempList.count();

  IntList::ConstIterator it;
  for ( it = tempList.begin(); it != tempList.end(); ++it )
  {
    WQListItem *li;
    li = new WQListItem();
    li->setQuestion(aCol);
    li->setCorrect(1);
    li->setOneOp(*it);

    if (count > 2)
    {

      int a, b;
      do
        a = rs->getLong(count); //rand() % count;
      while(a==*it);

      li->setTwoOp(a);

      do
        b = rs->getLong(count); //rand() % count;
      while(b == *it || b == a /*|| b < 0*/);

      li->setThreeOp(b);

    }
    m_quizList.append(*li);

  }

}

bool WQQuiz::init()
{

  bool result = false;
  if (Prefs::enableBlanks())
  {
    result = m_table->checkSyntax(true, true);
  }
  else
  {
    result = true;
  }

  if (!result)
  {
    return false;
  }

  int aCol;
  int bCol;

  switch (m_quizMode)
  {
  case 1:
    aCol = 0;
    bCol = 1;
    break;
  case 2:
    aCol = 1;
    bCol = 0;
    break;
  case 3:
    aCol = 0;
    bCol = 1;
    break;
  case 4:
    aCol = 1;
    bCol = 0;
    break;
  case 5:
  default:
    aCol = 0;
    bCol = 1;
    break;
  }

  addToList(aCol, bCol);

  //check if enough in list
  switch (m_quizType)
  {
  case qtEditor:
    //
    break;  
  case qtFlash:
    result = (m_quizList.count() > 0);
    break;
  case qtQA:
    result = (m_quizList.count() > 0);
    break;
  case qtMultiple:
    result = (m_quizList.count() > 2);
    break;
  }

  if (!result)
  {
    return false;
  }

  if (m_quizMode == 5)
  {
    aCol = 1;
    bCol = 0;
    addToList(aCol, bCol);
  }

  //Prepare final lists
  activateBaseList();
  return true;
}

void WQQuiz::listRandom()
{
  QPtrList<void> list;

  for(int i = 0; i < m_quizList.count(); i++)
    list.append((void*) i);

  KRandomSequence seq;
  seq.randomize(&list);

  QuizList items;
  for(int i = 0; i < m_quizList.count(); i++)
    items.append(m_quizList[(long) list.take()]);

  m_quizList.clear();

  for(int i = 0; i < items.count(); i++)
    m_quizList.append(items[i]);
}

bool WQQuiz::checkAnswer(int i, const QString & a)
{
  bool result = false;
  WQListItem li = m_list[i];
  int j;
  if (li.question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }
  QString ans = a;
  QString tTemp = m_table -> text(li.oneOp(), j);
  tTemp = tTemp.stripWhiteSpace();
  ans = ans.stripWhiteSpace();

  if (m_quizType == qtQA)
  {
    if (QString(m_correctBlank).length() > 0)
    {
      QStringList la, ls;
      if (ans.find(";") > 0)
        ls = QStringList::split(";", ans);
      else
        ls.append(ans);

      if (m_correctBlank.find(";") > 0)
        la = QStringList::split(";", m_correctBlank);
      else
        la.append(m_correctBlank);

      result = (ls.count() == la.count());
      if (result)
      {
        for (uint counter = 0; counter < la.count(); counter++)
        {
          result = (ls[counter].stripWhiteSpace() == la[counter].stripWhiteSpace());
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
    if (m_quizType == qtMultiple)
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

  if (!result)
  {
    m_errorList.append(li);
  }

  return result;
}

QStringList WQQuiz::multiOptions(int i)
{
  QString *s;
  QStringList Result;
  WQListItem li = m_list[i];

  typedef QPtrList<QString> LS;
  LS *ls;
  ls = new QPtrList<QString>();

  int j;
  if (li.question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }

  s= new QString(m_table->text(li.oneOp(), j)); 
  if (Prefs::enableBlanks())
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  s = new QString(m_table->text(li.twoOp(), j));
  if (Prefs::enableBlanks())
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  s = new QString(m_table->text(li.threeOp(), j));
  if (Prefs::enableBlanks())
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  KRandomSequence rs;
  rs.randomize(ls);

  while (ls->count())
  {
    Result.append(*ls->first());
    ls->removeFirst();
  }
 return Result;
}

QString WQQuiz::quizIcon(int i, QuizIcon ico)
{
  QString s;
  WQListItem li = m_list[i];
  if (ico == qiLeftCol)
  {
    if (li.question() == 0)
      s = "question";
    else
      s = "answer";
  }

  if (ico == qiRightCol)
  {
    if (li.question() == 0)
      s = "answer";
    else
      s = "question";
  }
  return s;
}

QString WQQuiz::yourAnswer(int i, const QString & s)
{
  QString result ="";

  if (QString(m_answerBlank).length() > 0)
  {
    QStringList ls;

    if (s.find(";") > 0)
      ls = QStringList::split(";", s, true);
    else
      ls.append(s);

    result = m_answerBlank.replace("..........", "<u></u>");

    int offset = 0, counter = 0;
    while (offset >= 0)
    {
      offset = result.find("<u>", offset);
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

QString WQQuiz::hint(int i)
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

void WQQuiz::setQuizType(QuizType qt)
{
  m_quizType = qt;
}

void WQQuiz::setQuizMode(int qm)
{
  m_quizMode = qm;
}


QString WQQuiz::question(int i)
{
  WQListItem li = m_list[i];
  QString s = m_table->text(li.oneOp(), li.question());
  if (Prefs::enableBlanks())
  {
    s.remove("[");
    s.remove("]");
  }
  if (m_quizType != qtFlash && i > 0)
  {
    WQListItem li2 = m_list[i - 1];
    emit checkingAnswer(li2.oneOp());
  }
  else
    emit checkingAnswer(li.oneOp());

  return s;
}

QString WQQuiz::blankAnswer(int i)
{

  QString r = "";
  m_correctBlank = "";
  m_answerBlank = "";
  QString tTemp;

  if (m_quizType == qtQA && Prefs::enableBlanks())
  {
    WQListItem li = m_list[i];
    int j;
    if (li.question() == 0)
    {
      j = 1;
    }
    else
    {
      j= 0;
    }
    tTemp = m_table->text(li.oneOp(), j);
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
        offset = rx.search(tTemp, offset);
        if (offset >= 0)
        {
          if (m_correctBlank.length() > 0)
            m_correctBlank = m_correctBlank + ";" + " " + tTemp.mid(offset + 1, tTemp.find(']', offset) - offset - 1);
          else
            m_correctBlank = tTemp.mid(offset + 1, tTemp.find(']', offset) - offset - 1);
          offset++; 
        }
      }
    }
  }
  return m_answerBlank;
}

QString WQQuiz::answer(int i)
{
  QString s;
  WQListItem li = m_list[i];
  int j;
  if (li.question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }


  if (m_quizType == qtQA)
  {
    s = m_table->text(li.oneOp(), j);
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
    s = m_table->text(li.oneOp(), j);
    if (Prefs::enableBlanks())
    {
      s.remove("[");
      s.remove("]");
    }
  }
  return s;
}

QString WQQuiz::langQuestion(int i)
{
  WQListItem li = m_list[i];
  return m_table->horizontalHeader()->label(li.question());
}

QString WQQuiz::langAnswer(int i)
{

  WQListItem li = m_list[i];

  int j;
  if (li.question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }

  return m_table->horizontalHeader()->label(j);
}

int WQQuiz::kbAnswer(int i)
{
/*  WQListItem *li = m_list->at(i);
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

int WQQuiz::questionCount()
{
  return m_questionCount;
}

void WQQuiz::finish()
{
  emit checkingAnswer(-1);
}

#include "wqquiz.moc"
