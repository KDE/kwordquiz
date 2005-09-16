/***************************************************************************
                                 wqlreader.cpp
                             -------------------
   copyright            : (C) 2004 by Peter Hedlund
   email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtextstream.h>
#include <qfile.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "wqlreader.h"

KWqlDataItem::KWqlDataItem()
{

}

KWqlDataItem::KWqlDataItem(const QString &front, const QString &back, int height)
{
  m_front = front;
  m_back = back;
  m_height = height;
}

KWqlDataItem::~KWqlDataItem()
{

}

/*!
    \fn WqlReader::WqlReader
 */
WqlReader::WqlReader()
{

}

KWqlDataItemList WqlReader::parse(const QString &fileName)
{
    KWqlDataItemList list;
    QFile file(fileName);
    file.open(IO_ReadOnly);
    QTextStream ts(&file);
    ts.setEncoding(QTextStream::Latin1);

    QString s = "";
    s=ts.readLine();
    if (s != "WordQuiz")
    {
      KMessageBox::error(0, i18n("This does not appear to be a (K)WordQuiz file") + s);
      file.close();
      return list;
    }
    s = ts.readLine();
    s = s.left(1);
    int iFV = s.toInt(0);
    if (iFV != 5)
    {
      KMessageBox::error(0, i18n("KWordQuiz can only open files created by WordQuiz 5.x"));
      file.close();
      return list;
    }

    while (ts.readLine() != "[Font Info]");
    s = ts.readLine();
    int p = s.find("=", 0);
    QString fam = s.right(s.length() - (p + 1));
    fam = fam.mid(1, fam.length() - 2);
    //g->font().setFamily(s);

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    int ps = s.toInt(0);

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    int b = 0;
    if (s == "1")
    {
      b = QFont::Bold;
    }

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    bool it = (s == "1");

    QFont m_font(fam, ps, b, it);

    while (ts.readLine() != "[Character Info]");  
    s = ts.readLine();    
    p = s.find("=", 0);
    m_specialCharacters = s.right(s.length() - (p + 1));

    while (ts.readLine() != "[Grid Info]");
    ts.readLine(); //skip value for width of row headers

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_colWidth1 = s.toInt(0, 10);

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_colWidth2 = s.toInt(0, 10);

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_numRows = (s.toInt(0, 10) - 1); //We need to reduce by one since the header is not included

    // Selection
    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_topLeft =s.toInt(0, 10) - 1;

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_topRight =s.toInt(0, 10) - 1;

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_bottomLeft =s.toInt(0, 10) - 1;

    s = ts.readLine();
    p = s.find("=", 0);
    s = s.right(s.length() - (p + 1));
    m_bottomRight =s.toInt(0, 10) - 1 ;

    while (ts.readLine() != "[Vocabulary]");

    s = ts.readLine();
    p = s.find("   [", 0);
    s = s.left(p);
    s = s.stripWhiteSpace();
    m_language1 = s;
    m_language2 = ts.readLine();

    while (!s.isNull())
    {
      s = ts.readLine();
      p = s.find("[", 0);
      QString r = s.mid(p + 1, 10);
      int h = r.toInt(0, 10);
      s = s.left(p);
      s = s.stripWhiteSpace();
      
      QString b;
      b = ts.readLine();
      
      KWqlDataItem item(s, b, h /15);
      list.append(item);
    }
    file.close();
    return list;
}


/*!
    \fn WqlReader::colWidth(int col)
 */
int WqlReader::colWidth(int col)
{
  if (col == 0)
    return m_colWidth1;
  else
    return m_colWidth2;
}


/*!
    \fn WqlReader::language(int col)
 */
QString WqlReader::language(int col)
{
  if (col == 0)
    return m_language1;
  else
    return m_language2;
}

