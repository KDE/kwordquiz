/***************************************************************************
                   export a KEduVocDocument to a WQL file
    -----------------------------------------------------------------------
    copyright            : (C) 2004 Peter Hedlund
                           (C) 2005 Eric Pignet
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

#include <kdebug.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qfont.h>

#include "keduvocwqlwriter.h"
#include "keduvocdocument.h"

#define winendl "\r\n"

KEduVocWqlWriter::KEduVocWqlWriter(QFile *file)
{
  // the file must be already open
  m_outputFile = file;
}

KEduVocWqlWriter::~KEduVocWqlWriter()
{
}

bool KEduVocWqlWriter::writeDoc(KEduVocDocument *doc)
{
  m_doc = doc;

  m_outputStream.setDevice(m_outputFile);
  m_outputStream.setEncoding(QTextStream::Latin1);

  m_outputStream << "WordQuiz" << winendl;
  m_outputStream << "5.9.0" << winendl << winendl;

  if (doc->font() != NULL)
    writeFont(*(doc->font()));
  // TODO writeCharacters(Prefs::specialCharacters());
  // TODO Find the vertical header width, handle the case where there are not exactly two languages
  writeGridInfo(10, m_doc->sizeHint(0), m_doc->sizeHint(1), m_doc->numEntries());
  /* TODO Find the selected cells
  if (g->numSelections() > 0)
  {
    QTableSelection qts = g->selection(0);
    writeSelection(qts.leftCol(), qts.topRow(), qts.rightCol(), qts.bottomRow());
  }
  else
  {
    writeSelection(g->currentColumn(), g->currentRow(), g->currentColumn(), g->currentRow());
  }*/
  writeFirstItem(m_doc->originalIdentifier(), m_doc->identifier(1));
  int r = m_doc->numEntries();
  for (int w=0; w < r; w++)
  {
    // TODO Find the row height (g->rowHeight(w))
    writeItem(m_doc->entry(w)->original(), m_doc->entry(w)->translation(1), 30);
  }
}

void KEduVocWqlWriter::writeFont( const QFont & font )
{
  m_outputStream << "[Font Info]" << winendl;
  m_outputStream << "FontName1=\"" << font.family() << "\"" << winendl;
  m_outputStream << "FontSize1=" << QString::number(font.pointSize()) << winendl;
  m_outputStream << QString("FontBold1=%1").arg(font.bold() ? "1" : "0") <<winendl;
  m_outputStream << QString("FontItalic1=%1").arg(font.italic() ? "1" : "0") <<winendl;
  m_outputStream << "FontColor1=0" << winendl;
  m_outputStream << "CharSet1=0" << winendl;
  m_outputStream << "Layout1=0" << winendl;

  m_outputStream << "FontName2=\"" << font.family() << "\"" << winendl;
  m_outputStream << "FontSize2=" << QString::number(font.pointSize()) << winendl;
  m_outputStream << QString("FontBold2=%1").arg(font.bold() ? "1" : "0") <<winendl;
  m_outputStream << QString("FontItalic2=%1").arg(font.italic() ? "1" : "0") <<winendl;
  m_outputStream << "FontColor2=0" << winendl;
  m_outputStream << "CharSet2=0" << winendl;
  m_outputStream << "Layout2=0" << winendl << winendl;
}

void KEduVocWqlWriter::writeCharacters( const QString & s )
{
  m_outputStream << "[Character Info]" << winendl;
  m_outputStream << "Characters1=" << s << winendl;
  m_outputStream << "Characters2=" << s << winendl << winendl;
}

void KEduVocWqlWriter::writeGridInfo( int col0, int col1, int col2, int numRows )
{
  m_outputStream << "[Grid Info]" << winendl;
  m_outputStream << "ColWidth0=" << QString::number(col0) << winendl;
  m_outputStream << "ColWidth1=" << QString::number(col1) << winendl;
  m_outputStream << "ColWidth2=" << QString::number(col2) << winendl;
  m_outputStream << "RowCount=" << QString::number(numRows + 1) << winendl; //Add one for the header
}

void KEduVocWqlWriter::writeSelection( int lc, int tr, int rc, int br ) //part of [Grid Info]
{
  m_outputStream << "SelLeft=" << QString::number(lc + 1) << winendl;
  m_outputStream << "SelTop=" << QString::number(tr + 1) << winendl;
  m_outputStream << "SelRight=" << QString::number(rc + 1) << winendl;
  m_outputStream << "SelBottom=" << QString::number(br + 1) << winendl << winendl;
}

void KEduVocWqlWriter::writeFirstItem( const QString & ll, const QString & rl )
{
  m_outputStream << "[Vocabulary]" << winendl;
  m_outputStream << ll  << "   [0000000300]" << winendl;
  m_outputStream << rl  << winendl;
}

void KEduVocWqlWriter::writeItem( const QString & left, const QString & right, int rh )
{
  m_outputStream << left  << QString( "   [%1]").arg(rh * 15, 10, 10 ) << winendl ;
  m_outputStream << right << winendl;
}