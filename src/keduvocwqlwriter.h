/***************************************************************************
                   export a KEduVocDocument to a WQL file
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

#ifndef KEDUVOCWQLWRITER_H
#define KEDUVOCWQLWRITER_H

#include <qfile.h>
#include <qfont.h>

class KEduVocDocument;

class KEduVocWqlWriter
{
public:
  KEduVocWqlWriter(QFile *file);
  ~KEduVocWqlWriter();

  bool writeDoc(KEduVocDocument *doc);

  void writeFont(const QFont & font);
  void writeCharacters(const QString & s);
  void writeGridInfo(int col0, int col1, int col2, int numRows);
  void writeSelection(int lc, int tr, int rc, int br);
  void writeFirstItem(const QString &ll, const QString &rl);
  void writeItem(const QString &left, const QString &right, int rh);

private:
  QFile *m_outputFile;
  QTextStream m_outputStream;
  KEduVocDocument *m_doc;
};

#endif
