//
// C++ Interface: kvtmlwriter
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  QFile *outputFile;
  QTextStream outputStream;
};

#endif
