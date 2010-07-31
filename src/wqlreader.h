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

#include <tqstring.h>
#include <tqvaluelist.h>
#include <tqfont.h>

class KWqlDataItem
{
public:
  KWqlDataItem();
  KWqlDataItem(const TQString &front, const TQString &back, int height);
  virtual ~KWqlDataItem();

  TQString frontText() const {return m_front;}
  TQString backText() const {return m_back;}
  int rowHeight() const {return m_height;}

private:
  TQString m_front;
  TQString m_back;
  int m_height;
};

typedef TQValueList<KWqlDataItem> KWqlDataItemList;

class WqlReader{
public:
  WqlReader();
  KWqlDataItemList parse(const TQString &fileName);
  int colWidth(int col);
  int numRows() {return m_numRows;}
  int startCol() {return m_topLeft;}
  int startRow() {return m_topRight;}
  int endCol() {return m_bottomLeft;}
  int endRow() {return m_bottomRight;}
  TQString language(int col);
  TQFont font() {return m_font;}
  TQString specialCharacters() {return m_specialCharacters;}
  
private:
  TQFont m_font;
  int m_colWidth1;
  int m_colWidth2;
  int m_numRows;
  int m_topLeft;
  int m_topRight;
  int m_bottomLeft;
  int m_bottomRight;
  TQString m_language1;
  TQString m_language2;
  TQString m_specialCharacters;

};

#endif
