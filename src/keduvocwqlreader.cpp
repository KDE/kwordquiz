/***************************************************************************
                     read a KEduVocDocument from a WQL file
    -----------------------------------------------------------------------
    copyright            : (C) 2004 Peter Hedlung
                           (C) 2005 The KDE-EDU team
    email                : peter@peterandlinda.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qdom.h>
#include <qtextstream.h>

#include "keduvocwqlreader.h"
#include "keduvocdocument.h"

KEduVocWqlReader::KEduVocWqlReader(QFile *file)
{
  // the file must be already open
  m_inputFile = file;
}

KEduVocWqlReader::~KEduVocWqlReader()
{
}

bool KEduVocWqlReader::readDoc(KEduVocDocument *doc)
{
  m_doc = doc;

  QTextStream inputStream(m_inputFile);
  inputStream.setEncoding(QTextStream::Latin1);

  QString s = "";
  s=inputStream.readLine();
  if (s != "WordQuiz")
  {
    KMessageBox::error(0, i18n("This does not appear to be a (K)WordQuiz file") + s);
    return false;
  }
  s = inputStream.readLine();
  s = s.left(1);
  int iFV = s.toInt(0);
  if (iFV != 5)
  {
    KMessageBox::error(0, i18n("KWordQuiz can only open files created by WordQuiz 5.x"));
    return false;
  }
  // TODO these loops cause crashes if the input file is invalid !
  while (inputStream.readLine() != "[Font Info]");
  s = inputStream.readLine();
  int p = s.find("=", 0);
  QString fam = s.right(s.length() - (p + 1));
  fam = fam.mid(1, fam.length() - 2);
  //g->font().setFamily(s);

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  int ps = s.toInt(0);

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  int b = 0;
  if (s == "1")
  {
    b = QFont::Bold;
  }

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  bool it = (s == "1");

  m_doc->setFont(new QFont(fam, ps, b, it));

/* TODO
  while (inputStream.readLine() != "[Character Info]");
  s = inputStream.readLine();
  p = s.find("=", 0);
  m_specialCharacters = s.right(s.length() - (p + 1));
*/
  while (inputStream.readLine() != "[Grid Info]");
  inputStream.readLine(); //skip value for width of row headers

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_doc->setSizeHint(0, s.toInt());

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_doc->setSizeHint(1, s.toInt());

/* TODO
  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_numRows = (s.toInt() - 1); //We need to reduce by one since the header is not included
  // Selection
  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_topLeft =s.toInt(0, 10) - 1;

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_topRight =s.toInt(0, 10) - 1;

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_bottomLeft =s.toInt(0, 10) - 1;

  s = inputStream.readLine();
  p = s.find("=", 0);
  s = s.right(s.length() - (p + 1));
  m_bottomRight =s.toInt(0, 10) - 1 ;
*/
  while ((inputStream.readLine() != "[Vocabulary]"));

  s = inputStream.readLine();
  p = s.find("   [", 0);
  s = s.left(p);
  s = s.stripWhiteSpace();
  m_doc->m_identifiers.push_back(s);
  m_doc->m_identifiers.push_back(inputStream.readLine());

  while (!s.isNull())
  {
    s = inputStream.readLine();
    p = s.find("[", 0);
    QString r = s.mid(p + 1, 10);
    int h = r.toInt();
    s = s.left(p);
    s = s.stripWhiteSpace();

    QString b;
    b = inputStream.readLine();

    KEduVocExpression expr = KEduVocExpression(s);
    expr.setTranslation(1, b);
    m_doc->appendEntry(&expr);
  }
  return true;
}
