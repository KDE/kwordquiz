/***************************************************************************
                          kvtmlwriter.h  -  description
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
 
#ifndef KVTMLWRITER_H
#define KVTMLWRITER_H

#include <qfile.h>
#include <qtextstream.h>

/**
@author Peter Hedlund
*/
class KVTMLWriter{
public:
  KVTMLWriter(QFile *file);

  ~KVTMLWriter();
  void addFirstItem(const QString &ll, int lwidth, const QString &left, const QString &rl, int rwidth, const QString &right);
  void addHeader(const QString &generator, int cols, int rows, const QString &title);
  void addItem(const QString &left, const QString &right);
private:
  QString escape(const QString &);
  QFile *outputFile;
  QTextStream outputStream;
};

#endif
