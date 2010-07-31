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

#include <tqfile.h>
#include <tqtextstream.h>

/**
@author Peter Hedlund
*/
class KVTMLWriter{
public:
  KVTMLWriter(TQFile *file);

  ~KVTMLWriter();
  void addFirstItem(const TQString &ll, int lwidth, const TQString &left, const TQString &rl, int rwidth, const TQString &right);
  void addHeader(const TQString &generator, int cols, int rows, const TQString &title);
  void addItem(const TQString &left, const TQString &right);
private:
  TQString escape(const TQString &);
  TQFile *outputFile;
  TQTextStream outputStream;
};

#endif
