/***************************************************************************
                                 wqlreader.h
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
#ifndef WQLREADER_H
#define WQLREADER_H

/**
@author Peter Hedlund
*/

#include <qstring.h>
#include <qvaluelist.h>
#include <qfont.h>

class KWqlDataItem
{
public:
  KWqlDataItem();
  KWqlDataItem(const QString &front, const QString &back, int height);
  virtual ~KWqlDataItem();

  QString frontText() const {return m_front;}
  QString backText() const {return m_back;}
  int rowHeight() const {return m_height;}

private:
  QString m_front;
  QString m_back;
  int m_height;
};

typedef QValueList<KWqlDataItem> KWqlDataItemList;

class WqlReader{
public:
  WqlReader();
  KWqlDataItemList parse(const QString &fileName);
  int colWidth(int col);
  int numRows() {return m_numRows;}
  int startCol() {return m_topLeft;}
  int startRow() {return m_topRight;}
  int endCol() {return m_bottomLeft;}
  int endRow() {return m_bottomRight;}
  QString language(int col);
  QFont font() {return m_font;}
  QString specialCharacters() {return m_specialCharacters;}
  
private:
  QFont m_font;
  int m_colWidth1;
  int m_colWidth2;
  int m_numRows;
  int m_topLeft;
  int m_topRight;
  int m_bottomLeft;
  int m_bottomRight;
  QString m_language1;
  QString m_language2;
  QString m_specialCharacters;

};

#endif
