//
// C++ Implementation: wqquiz
//
// Description:
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#include <krandomsequence.h>

#include <qvaluelist.h>
#include <qregexp.h>

#include "wqquiz.h"
#include "wqlistitem.h"

QPtrList<WQListItem> *WQQuiz::m_list = 0L;
QPtrList<WQListItem> *WQQuiz::m_errorList = 0L;
QPtrList<WQListItem> *WQQuiz::m_quizList = 0L;

WQQuiz::WQQuiz(KWordQuizView *t)
{
  m_table = t;

  m_list = new QPtrList<WQListItem>();
  //m_list -> setAutoDelete(true);
  m_errorList = new QPtrList<WQListItem>();
  //m_errorList -> setAutoDelete(true);
  m_quizList = new QPtrList<WQListItem>();
  //m_quizList -> setAutoDelete(true);
}

WQQuiz::~WQQuiz()
{
}

void WQQuiz::activateErrorList()
{
  m_list->clear();

  WQListItem *l;
  for (l = m_errorList->first(); l; l = m_errorList->next())
  {
    m_list->append(l);
  }

  m_errorList->clear();
  m_questionCount = m_list->count();

}

void WQQuiz::activateBaseList()
{
  m_list->clear();

  if (m_quizMode > 2)
  {
    listRandom();
  };

  WQListItem *l;
  for (l = m_quizList->first(); l; l = m_quizList->next())
    m_list->append(l);

  m_questionCount = m_list->count();

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
    m_quizList->append(li);

  }

}

bool WQQuiz::init()
{

  bool result = false;
  if (m_enableBlanks)
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
    result = (m_quizList -> count() > 0);
    break;
  case qtQA:
    result = (m_quizList -> count() > 0);
    break;
  case qtMultiple:
    result = (m_quizList -> count() > 2);
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
  KRandomSequence *rs = new KRandomSequence(0);
  rs->randomize(m_quizList);
}

bool WQQuiz::checkAnswer(int i, QString ans)
{
  bool result = false;
  WQListItem *li = m_list->at(i);
  int j;
  if (li->question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }

  QString tTemp = m_table -> text(li->oneOp(), j);
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
      if (m_enableBlanks)
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
    m_errorList -> append(li);
  }

  return result;
}

QStringList WQQuiz::multiOptions(int i)
{
  QString *s;
  QStringList Result;
  WQListItem *li = m_list->at(i);

  typedef QPtrList<QString> LS;
  LS *ls;
  ls = new QPtrList<QString>();

  int j;
  if (li->question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }

  s= new QString(m_table->text(li->oneOp(), j)); 
  if (m_enableBlanks)
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  s = new QString(m_table->text(li->twoOp(), j));
  if (m_enableBlanks)
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  s = new QString(m_table->text(li->threeOp(), j));
  if (m_enableBlanks)
  {
    s->remove("[");
    s->remove("]");
  }
  ls->append(s);

  KRandomSequence *rs = new KRandomSequence(0);
  rs->randomize(ls);

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
  WQListItem *li = m_list->at(i);
  if (ico == qiLeftCol)
  {
    if (li->question() == 0)
      s = "lang1";
    else
      s = "lang2";
  }
  
  if (ico == qiRightCol)
  {
    if (li->question() == 0)
      s = "lang2";
    else
      s = "lang1";
  }  
  return s;     
}

QString WQQuiz::yourAnswer(int i, QString s)
{
  QString result ="";

  if (QString(m_answerBlank).length() > 0)
  {
    QStringList ls;

    if (s.find(";") > 0)
      ls = QStringList::split(";", s, true);
    else
      ls.append(s);

    result = m_answerBlank.replace("..........", "[]");

    int offset = 0, counter = 0;
    while (offset >= 0)
    {
      offset = result.find("[", offset);
      if (offset >= 0)
      {
        result.insert(offset + 1, ls[counter]);
        offset++;
        counter++; 
      }
    }    
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
  WQListItem *li = m_list->at(i);
  QString s = m_table->text(li->oneOp(), li->question());
  if (m_enableBlanks)
  {
    s.remove("[");
    s.remove("]");
  }
  return s;
}

QString WQQuiz::blankAnswer(int i)
{

  QString r = "";
  m_correctBlank = "";
  m_answerBlank = "";
  QString tTemp;

  if (m_quizType == qtQA && m_enableBlanks)
  {
    WQListItem *li = m_list->at(i);
    int j;
    if (li->question() == 0)
    {
      j = 1;
    }
    else
    {
      j= 0;
    }
    tTemp = m_table->text(li->oneOp(), j);
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
  WQListItem *li = m_list->at(i);
  int j;
  if (li->question() == 0)
  {
    j = 1;
  }
  else
  {
    j= 0;
  }


  if (m_quizType == qtQA)
  {
    return m_table->text(li->oneOp(), j);
  }
  else
  {
    QString s = m_table->text(li->oneOp(), j);
    if (m_enableBlanks)
    {
      s.remove("[");
      s.remove("]");
    }
    return s;
  }
}

QString WQQuiz::langQuestion(int i)
{
  WQListItem *li = m_list->at(i);
  return m_table->horizontalHeader()->label(li->question());
}

QString WQQuiz::langAnswer(int i)
{

  WQListItem *li = m_list->at(i);

  int j;
  if (li->question() == 0)
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

void WQQuiz::setEnableBlanks(bool b)
{
  m_enableBlanks = b;
}

int WQQuiz::questionCount()
{
  return m_questionCount;
}
