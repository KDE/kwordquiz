//
// C++ Implementation: kvtmlwriter
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "kvtmlwriter.h"

KVTMLWriter::KVTMLWriter(QFile *file)
{
  outputFile = file;
  if(outputFile->open(IO_WriteOnly))
    {
      outputStream.setDevice(outputFile);
      outputStream.setEncoding(QTextStream::UnicodeUTF8);

      outputStream << "<?xml version=\"1.0\"?>" << endl;
      outputStream << "<!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">" << endl;
    }
}

/*!
    \fn KVTMLWriter::addHeader(const QString &generator, int cols, int rows, const QString &title)
 */
void KVTMLWriter::addHeader(const QString &generator, int cols, int rows, const QString &title)
{
  QString s = QString("<kvtml\n generator=\"%1\"\n cols=\"%2\"\n lines=\"%3\"\n title=\"%4\">")
    .arg(generator)
    .arg(cols)
    .arg(rows)
    .arg(title);

  outputStream << s << endl << endl;
}

/*!
    \fn KVTMLWriter::addFirstItem(const QString &ll, int lwidth, const QString &left, const QString &rl, int rwidth, const QString &right)
 */
void KVTMLWriter::addFirstItem(const QString &ll, int lwidth, const QString &left, const QString &rl, int rwidth, const QString &right)
{
  outputStream << " <e>" << endl;
  QString s = QString("  <o width=\"%1\" l=\"%2\">")
    .arg(lwidth)
    .arg(ll);
  outputStream << s << left << "</o>" << endl;
  s = QString("  <t width=\"%1\" l=\"%2\">")
    .arg(rwidth)
    .arg(rl);
  outputStream << s << right << "</t>" << endl;
  outputStream << " </e>" << endl;
}

/*!
    \fn KVTMLWriter::addItem(const QString &left, const QString &right)
 */
void KVTMLWriter::addItem(const QString &left, const QString &right)
{
  outputStream << " <e>" << endl;
  outputStream << "  <o>" << left << "</o>" << endl;
  outputStream << "  <t>" << right << "</t>" << endl;
  outputStream << " </e>" << endl;
}


KVTMLWriter::~KVTMLWriter()
{
  outputStream << "</kvtml>" << endl;
  outputFile->close();
}




