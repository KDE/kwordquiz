/***************************************************************************
                          kvtmlwriter.cpp  -  description
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
 
#include "kvtmlwriter.h"

KVTMLWriter::KVTMLWriter(TQFile *file)
{
  outputFile = file;
  if(outputFile->open(IO_WriteOnly))
    {
      outputStream.setDevice(outputFile);
      outputStream.setEncoding(TQTextStream::UnicodeUTF8);

      outputStream << "<?xml version=\"1.0\"?>" << endl;
      outputStream << "<!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">" << endl;
    }
}

/*!
    \fn KVTMLWriter::addHeader(const TQString &generator, int cols, int rows, const TQString &title)
 */
void KVTMLWriter::addHeader(const TQString &generator, int cols, int rows, const TQString &title)
{
  TQString s = TQString("<kvtml\n generator=\"%1\"\n cols=\"%2\"\n lines=\"%3\"\n title=\"%4\">")
    .arg(generator)
    .arg(cols)
    .arg(rows)
    .arg(title);

  outputStream << s << endl << endl;
}

/*!
    \fn KVTMLWriter::addFirstItem(const TQString &ll, int lwidth, const TQString &left, const TQString &rl, int rwidth, const TQString &right)
 */
void KVTMLWriter::addFirstItem(const TQString &ll, int lwidth, const TQString &left, const TQString &rl, int rwidth, const TQString &right)
{
  outputStream << " <e>" << endl;
  TQString s = TQString("  <o width=\"%1\" l=\"%2\">")
    .arg(lwidth)
    .arg(ll);
  outputStream << s << escape(left) << "</o>" << endl;
  s = TQString("  <t width=\"%1\" l=\"%2\">")
    .arg(rwidth)
    .arg(rl);
  outputStream << s << escape(right) << "</t>" << endl;
  outputStream << " </e>" << endl;
}

/*!
    \fn KVTMLWriter::addItem(const TQString &left, const TQString &right)
 */
void KVTMLWriter::addItem(const TQString &left, const TQString &right)
{
  outputStream << " <e>" << endl;
  outputStream << "  <o>" << escape(left) << "</o>" << endl;
  outputStream << "  <t>" << escape(right) << "</t>" << endl;
  outputStream << " </e>" << endl;
}


KVTMLWriter::~KVTMLWriter()
{
  outputStream << "</kvtml>" << endl;
  outputFile->close();
}

TQString KVTMLWriter::escape( const TQString & s)
{
  TQString result = s;
  result.replace(TQChar('&'), "&amp;"); //must be done first 
  result.replace(TQChar('<'), "&lt;");
  result.replace(TQChar('>'), "&gt;");

  return result;  
}




