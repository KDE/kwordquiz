/***************************************************************************
                          wqlwriter.cpp  -  description
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
#include "wqlwriter.h"

#define winendl "\r\n"

WqlWriter::WqlWriter(QFile *file)
{
  outputFile = file;
  if(outputFile->open(IO_WriteOnly))
  {
    outputStream.setDevice(outputFile);
    outputStream.setEncoding(QTextStream::Latin1);

    outputStream << "WordQuiz" << winendl;
    outputStream << "5.9.0" << winendl << winendl;      
  }
}

void WqlWriter::writeFont( const QFont & font )
{
  outputStream << "[Font Info]" << winendl;
  outputStream << "FontName1=\"" << font.family() << "\"" << winendl;
  outputStream << "FontSize1=" << QString::number(font.pointSize()) << winendl;
  outputStream << QString("FontBold1=%1").arg(font.bold() ? "1" : "0") <<winendl;
  outputStream << QString("FontItalic1=%1").arg(font.italic() ? "1" : "0") <<winendl;
  outputStream << "FontColor1=0" << winendl;
  outputStream << "CharSet1=0" << winendl;
  outputStream << "Layout1=0" << winendl;
  
  outputStream << "FontName2=\"" << font.family() << "\"" << winendl;
  outputStream << "FontSize2=" << QString::number(font.pointSize()) << winendl;
  outputStream << QString("FontBold2=%1").arg(font.bold() ? "1" : "0") <<winendl;
  outputStream << QString("FontItalic2=%1").arg(font.italic() ? "1" : "0") <<winendl;
  outputStream << "FontColor2=0" << winendl;
  outputStream << "CharSet2=0" << winendl;
  outputStream << "Layout2=0" << winendl << winendl;
}

void WqlWriter::writeCharacters( const QString & s )
{
  outputStream << "[Character Info]" << winendl;
  outputStream << "Characters1=" << s << winendl;
  outputStream << "Characters2=" << s << winendl << winendl;
}

void WqlWriter::writeGridInfo( int col0, int col1, int col2, int numRows )
{
  outputStream << "[Grid Info]" << winendl;
  outputStream << "ColWidth0=" << QString::number(col0) << winendl;
  outputStream << "ColWidth1=" << QString::number(col1) << winendl;
  outputStream << "ColWidth2=" << QString::number(col2) << winendl;
  outputStream << "RowCount=" << QString::number(numRows + 1) << winendl; //Add one for the header
}

void WqlWriter::writeSelection( int lc, int tr, int rc, int br ) //part of [Grid Info]
{
  outputStream << "SelLeft=" << QString::number(lc + 1) << winendl;
  outputStream << "SelTop=" << QString::number(tr + 1) << winendl;
  outputStream << "SelRight=" << QString::number(rc + 1) << winendl;
  outputStream << "SelBottom=" << QString::number(br + 1) << winendl << winendl;
}

void WqlWriter::writeFirstItem( const QString & ll, const QString & rl )
{
  outputStream << "[Vocabulary]" << winendl;
  outputStream << ll  << "   [0000000300]" << winendl;
  outputStream << rl  << winendl;
}

void WqlWriter::writeItem( const QString & left, const QString & right, int rh )
{
  outputStream << left  << QString( "   [%1]").arg(rh * 15, 10, 10 ) << winendl ;
  outputStream << right << winendl;
}

WqlWriter::~WqlWriter()
{
  outputFile->close();
}
