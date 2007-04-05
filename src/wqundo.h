/***************************************************************************
                          wqundo.h  -  description
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
#ifndef WQUNDO_H
#define WQUNDO_H

#include <qtable.h>

#include "wqlreader.h"

/**
@author Peter Hedlund
*/
class WQUndo{
public:
  QFont font() const {return m_font;}
  void setFont(const QFont & font) {m_font = font;}
  
  int colWidth0() {return m_colWidth0;}
  void setColWidth0(int cw) {m_colWidth0 = cw;}
  int colWidth1() {return m_colWidth1;}
  void setColWidth1(int cw) {m_colWidth1 = cw;}
  int colWidth2() {return m_colWidth2;}
  void setColWidth2(int cw) {m_colWidth2 = cw;}
  int numRows() {return m_numRows;}
  void setNumRows(int n) {m_numRows = n;}      
  
  int currentRow() {return m_currentRow;}
  void setCurrentRow(int r) {m_currentRow = r;}
  int currentCol() {return m_currentCol;}
  void setCurrentCol(int c) {m_currentCol = c;}
  QTableSelection selection() const {return m_selection;}
  void setSelection(const QTableSelection & sel) {m_selection = sel;}
  
  KWqlDataItemList list() const {return m_list;}
  void setList(const KWqlDataItemList & list) {m_list = list;}
  
  QString text() const {return m_text;}
  void setText(const QString & s) {m_text = s;}

private:
  QFont m_font;
  int m_colWidth0;
  int m_colWidth1;
  int m_colWidth2;
  int m_numRows;
  int m_currentRow;
  int m_currentCol;
  QTableSelection m_selection;
  KWqlDataItemList m_list;
  
  QString m_text;
};

#endif
