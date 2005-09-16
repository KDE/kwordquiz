/***************************************************************************
                          wqlwriter.h  -  description
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
#ifndef WQLWRITER_H
#define WQLWRITER_H

#include <qfile.h>
#include <qtextstream.h>
#include <qfont.h>

/**
@author Peter Hedlund
*/
class WqlWriter{
public:
  WqlWriter(QFile *file);

  ~WqlWriter();
  void writeFont(const QFont & font);
  void writeCharacters(const QString & s);
  void writeGridInfo(int col0, int col1, int col2, int numRows);
  void writeSelection(int lc, int tr, int rc, int br);
  void writeFirstItem(const QString &ll, const QString &rl);
  void writeItem(const QString &left, const QString &right, int rh);
private:
  QFile *outputFile;
  QTextStream outputStream;
};

#endif
