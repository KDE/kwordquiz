/***************************************************************************
                          kwordquizview.cpp  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002 by Peter Hedlund
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

// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qtabwidget.h>
#include <qtable.h>
#include <qclipboard.h>
#include <qlineedit.h>

// include files for KDE
#include <kstandarddirs.h> //locate
#include <klocale.h> //i18n
#include <kmessagebox.h> // always useful
#include <krandomsequence.h>
#include <kglobalsettings.h>
#include <knotifyclient.h>

// application specific includes
#include "kwordquizview.h"
#include "kwordquizdoc.h"
#include "kwordquiz.h"
#include "dlgsort.h"
#include "configuration.h"
#include "dlgrc.h"
#include "dlgspecchar.h"

QValueList<WQUndo> *KWordQuizView::m_undoList = 0L;

KWordQuizView::KWordQuizView(QWidget *parent, const char *name) : QTable(parent, name)
{
  if(!m_undoList)
  {
    m_undoList = new QValueList<WQUndo>();
  }
  
  //setBackgroundMode(PaletteBase);
  setNumCols(2);
  setNumRows(50);
  setColumnWidth(1, 250);
  setColumnWidth(0, 250);
  setSelectionMode(QTable::Single);
  horizontalHeader()->setLabel(0, i18n("Language 1"));
  horizontalHeader()->setLabel(1, i18n("Language 2"));
  setMinimumSize(0, 0); //This seems to improve resizing of main window
  dlgSpecChar = 0;
  //connect(QTable this, SIGNAL(valueChanged(int row, int col)), this, SLOT(slotValueChanged(int row, int col)));
}

KWordQuizView::~KWordQuizView()
{
}


KWordQuizDoc *KWordQuizView::getDocument() const
{
  KWordQuizApp *theApp=(KWordQuizApp *) parentWidget();

  return theApp->getDocument();
}



void KWordQuizView::print(KPrinter *pPrinter)
{
  QPainter painter;
  
  //type 0 Vocabulary list
  //type 1 Vocabulary exam
  //type 2 Flashcards
  int type = pPrinter->option("kde-kwordquiz-type").toInt();
  
  //I think working with screen resolution is enough for our purposes
  int res = pPrinter->resolution();
  int pad = 2;
  int marg = res;
  int card_marg = res / 2;
  int card_width = 5 * res;
  int card_height = 3 * res;
  int card_text_marg = res /5;
  int card_line_top = 30;  
  
    
  if (type == 2)
    pPrinter->setOrientation(KPrinter::Landscape);  
  
  painter.begin(pPrinter);

  int pageNum = 1;
  
  int cw0 = verticalHeader()->width();
  int cw1 = columnWidth(0);
  int cw2 = columnWidth(1);
  int cw3 = 0;
    


  if (type == 1)
    cw3 = 50;
    
  int gridWidth = cw0 + cw1 + cw2 + cw3;
  int lPos = marg;
  int tPos = marg + horizontalHeader()->height();
  
  QRect w = painter.window(); 
  
  doNewPage(painter, res, type);
  
  if (type == 2)
  {
    tPos = card_marg;
    for (int rc = 0; rc < numRows(); ++rc)
    {
      
      //draw rectangle 2 cards wide
      painter.drawRect(card_marg, tPos, 2 * card_width, card_height);
      //divide into 2 cards with line
      painter.drawLine(card_marg + card_width, tPos, card_marg + card_width, tPos + card_height);
      //draw line inside card
      painter.drawLine(card_marg + card_text_marg, tPos + card_line_top, 
        card_marg + card_width - card_text_marg, tPos + card_line_top);
      painter.drawLine(card_marg + card_width + card_text_marg, tPos + card_line_top, 
        card_marg + card_width + card_width - card_text_marg, tPos + card_line_top);   
      //draw headers
      painter.setFont(KGlobalSettings::generalFont());
      painter.drawText(card_marg + card_text_marg, tPos, card_width, card_line_top, AlignAuto | AlignVCenter, horizontalHeader()->label(0));
      painter.drawText(card_marg + card_width + card_text_marg, tPos, card_width, card_line_top, AlignAuto | AlignVCenter, horizontalHeader()->label(1));
      //draw text
      painter.setFont(font());
      painter.drawText(card_marg + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg), card_height - card_line_top, AlignHCenter | AlignVCenter, text(rc, 0));
      painter.drawText(card_marg + card_width + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg), card_height - card_line_top, AlignHCenter | AlignVCenter, text(rc, 1));
      
      tPos = tPos + card_height + card_line_top;
  
      if (tPos + card_height + card_line_top > w.height() - card_marg)
      {
        doEndOfPage(painter, tPos, pageNum++, res, type);
        tPos = card_marg;
        pPrinter->newPage();
        doNewPage(painter, res, type);     
      }         
    }
   
  }
  else
  {
      
    for (int rc = 0; rc < numRows(); ++rc)
    {
      painter.drawLine(lPos, tPos, lPos + gridWidth, tPos);
      painter.setFont(KGlobalSettings::generalFont());
  
      painter.drawText(lPos, tPos, cw0 - pad, rowHeight(rc), AlignRight | AlignVCenter, QString::number(rc + 1));
      
      painter.setFont(font());
      painter.drawText(lPos + cw0 + pad, tPos, cw1, rowHeight(rc), AlignAuto | AlignVCenter, text(rc, 0));
      
      if (type == 0)
        painter.drawText(lPos + cw0 + cw1 + pad, tPos, cw2, rowHeight(rc), AlignAuto | AlignVCenter, text(rc, 1));
      
      tPos = tPos + rowHeight(rc);
  
      if (tPos + rowHeight(rc + 1) > w.height() - marg)
      {
        doEndOfPage(painter, tPos, pageNum++, res, type);
        tPos = marg + horizontalHeader()->height();
        pPrinter->newPage();
        doNewPage(painter, res, type);     
      }
    }
  }
  doEndOfPage(painter, tPos, pageNum++,  res, type);
  painter.end();
}

void KWordQuizView::doNewPage( QPainter & painter, int res, int type )
{
    int cw0 = verticalHeader()->width();
    int cw1 = columnWidth(0);
    int cw2 = columnWidth(1);
    int cw3 = 0;
    int marg = res;
    int card_marg = res / 2;
    int pad = 2;
    
    if (type == 1)
      cw3 = 50;
    
    QRect w = painter.window();
 
    painter.setFont(KGlobalSettings::generalFont());
    
    if (type == 2)
    {
      painter.drawText(card_marg, card_marg - 20, i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName()));      
      return;
    }
    painter.drawLine(marg, marg, marg + cw0 + cw1 + cw2 + cw3, marg);
    
    painter.drawText(marg, marg - 20, i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName()));

    if (type == 1)
    {
      QString score = i18n("Name:_____________________________ Date:__________");
      QRect r = painter.boundingRect(0, 0, 0, 0, AlignAuto, score);
      painter.drawText(w.width() - r.width() - marg, marg - 20, score);
    }
    
    painter.drawText(marg, marg, cw0, horizontalHeader()->height(), AlignRight | AlignVCenter, "");

    painter.drawText(marg + cw0 + pad, marg, cw1, horizontalHeader()->height(), AlignAuto | AlignVCenter, horizontalHeader()->label(0));
    painter.drawText(marg + cw0 + cw1 + pad, marg, cw2, horizontalHeader()->height(), AlignAuto | AlignVCenter, horizontalHeader()->label(1));
    
    if (type == 1)
      painter.drawText(marg + cw0 + cw1 + cw2 + pad, marg, cw3, horizontalHeader()->height(), AlignAuto | AlignVCenter, i18n("Score"));

}

void KWordQuizView::doEndOfPage( QPainter & painter, int vPos, int pageNum, int res, int type )
{
    int marg = res;
    painter.setFont(KGlobalSettings::generalFont());
    QRect w = painter.window();
    QRect r = painter.boundingRect(0, 0, 0, 0, AlignAuto, QString::number(pageNum));
    painter.drawText((w.width()/2) - (r.width()/2), w.height() - marg + 20, QString::number(pageNum));
    
    if (type == 2)
      return;
       
    int cw0 = verticalHeader()->width();
    int cw1 = columnWidth(0);
    int cw2 = columnWidth(1);
    int cw3 = 0;
    
    if (type == 1)
      cw3 = 50;
    
    //Last horizontal line
    painter.drawLine(marg, vPos, marg + cw0 + cw1 + cw2 + cw3, vPos);
   //Four vertical lines
    painter.drawLine(marg, marg, marg, vPos);
    painter.drawLine(marg + cw0, marg, marg + cw0, vPos);
    painter.drawLine(marg + cw0 + cw1, marg, marg + cw0 + cw1, vPos);
    painter.drawLine(marg + cw0 + cw1 + cw2, marg, marg + cw0 + cw1 + cw2, vPos);            
    
    if (type == 1)
      painter.drawLine(marg + cw0 + cw1 + cw2 + cw3, marg, marg + cw0 + cw1 + cw2 + cw3, vPos);  

}



/*!
    \fn KWordQuizView::gridIsEmpty()
 */
bool KWordQuizView::gridIsEmpty()
{
  for(int i = 0; i < numRows(); i++){
    for(int j = 0; j < 2; j++){
      if(!text(i, j).isEmpty()){
        return false;
      }
    }
  }
  return true;
}

QWidget * KWordQuizView::beginEdit( int row, int col, bool replace )
{
  m_currentText = text(row, col);
  return QTable::beginEdit(row, col, replace);
}

void KWordQuizView::endEdit( int row, int col, bool accept, bool replace )
{
  // this code gets called after enter and arrow keys, now we
  // only process if editing really has been done

  if (cellWidget(row, col) != 0) //if edited the cellWidget still exists
  {
    if (((QLineEdit *) cellWidget(row, col))->text() != m_currentText)
      addUndo(i18n("&Undo Entry"));
    QTable::endEdit(row, col, accept, replace); //this will destroy the cellWidget
    QTableItem* itm;
    itm = item(row, col);
    itm->setWordWrap(true);
    adjustRow(row);
    getDocument() -> setModified(true);
    if (Config().m_enableBlanks)
      if (!checkForBlank(text(row, col), true))
        KNotifyClient::event("SyntaxError", i18n("There is an error with the Fill-in-the-blank brackets"));
  }
}

void KWordQuizView::adjustRow( int row )
{
  // we want to make the row high enough to display content, but
  // if the user already made it even higher we keep that height
  int rh = rowHeight(row);
  QTable::adjustRow(row);
  if (rh > rowHeight(row))
    setRowHeight(row, rh);
}

/*!
    \fn KWordQuizView::colFont(int i)
 */
QFont KWordQuizView::colFont(int i)
{
  return font();
}

void KWordQuizView::wqCurrentSelection(bool clear = true)
{

  m_currentRow = currentRow();
  m_currentCol = currentColumn();

  if (numSelections() > 0)
  {
    m_currentSel = selection(0);
    if (clear)
      clearSelection();
  }
  else
  {
    QTableSelection* ts;
    ts = new QTableSelection(m_currentRow, m_currentCol, m_currentRow, m_currentCol);
    m_currentSel = *ts;
  }
}

void KWordQuizView::doEditUndo( )
{
  if (isEditing())
  {
    if (((QLineEdit *) cellWidget(currentRow(), currentColumn()))->isUndoAvailable())
      ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->undo();
  }
  else
  {
    WQUndo undo;
    if (m_undoList->count() > 0)
    {
      undo = m_undoList->first();
      QTextStream * ts = new QTextStream(undo.data(), IO_ReadOnly);
      fromStream(ts);
      m_undoList->remove(m_undoList->begin());
    }

    if (m_undoList->count() > 0)
    {
      undo = m_undoList->first();
      emit undoChange(undo.text(), true);
    }
    else
      emit undoChange(i18n("Can't &Undo"), false);
  }
}

void KWordQuizView::doEditCut( )
{
  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->cut();
  }
  else
  {
    addUndo(i18n("&Undo Cut"));
    doEditCopy();
    wqCurrentSelection();
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
        clearCell(r, c);
  }
  getDocument()->setModified(true);
}

void KWordQuizView::doEditCopy( )
{
  bool mod = getDocument()->isModified();
  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->copy();
  }
  else
  {
    wqCurrentSelection(false);
    QString s;
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
    {
      for (int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
        s = s + text(r, c) + "\t";
      s = s + "\n";
    }
    kapp->clipboard()->setText(s);
  }
  getDocument()->setModified(mod);
}

void KWordQuizView::doEditPaste( )
{
  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->paste();
  }
  else
  {
    addUndo(i18n("&Undo Paste"));
    wqCurrentSelection();
    int tr = m_currentSel.topRow();
    int br = m_currentSel.bottomRow();
    int lc = m_currentSel.leftCol();
    int rc = m_currentSel.rightCol();

    if (lc == rc && tr == br) //one cell selected
    {
      QString s = kapp->clipboard()->text();
      QStringList sl;
      sl = QStringList::split("\n", s);

      uint r = numRows() - tr;

      if (sl.count() > r) //do we need to add rows?
      {
        setNumRows(numRows() + (sl.count() - r));
        br = numRows();
      }
      else
        br= br + sl.count() - 1;

      if (lc == 0) //left col?
        if (sl[0].find("\t") < (sl[0].length() - 1))
          rc = 1; //expand to second column;

      uint i = 0;
      int ar = tr;
      QStringList sr;
      while(i < sl.count() && br <= numRows() )
      {
        int ac = lc;

        sr = QStringList::split("\t", sl[i]);
        int c = 0;
        while (ac <= rc)
        {
          setText(ar, ac, sr[c]);
          ac++;
          c++;
        }
        ar++;
        i++;
      }


    }
    else
    {
      QString s = kapp->clipboard()->text();
      QStringList sl;
      sl = QStringList::split("\n", s);
      uint i = 0;
      int ar = tr;
      QStringList sr;
      while(i < sl.count() && ar <= br )
      {
        int ac = lc;

        sr = QStringList::split("\t", sl[i]);
        int c = 0;
        while (ac <= rc)
        {
          setText(ar, ac, sr[c]);
          ac++;
          c++;
        }
        ar++;
        i++;
      }
    }

    //restore selection
    addSelection(QTableSelection(tr, lc, br, rc));
    setCurrentCell(m_currentRow, m_currentCol);
  }
  getDocument()->setModified(true);
}

void KWordQuizView::doEditClear( )
{
  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->clear();
  }
  else
  {
    addUndo(i18n("&Undo Clear"));
    wqCurrentSelection();
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
        clearCell(r, c);
  }
  getDocument()->setModified(true);
}

void KWordQuizView::doEditInsert( )
{
  addUndo(i18n("&Undo Insert"));
  setUpdatesEnabled(false);
  wqCurrentSelection();
  insertRows(m_currentSel.topRow(), m_currentSel.bottomRow() - m_currentSel.topRow() + 1);

  addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
  setCurrentCell(m_currentRow, m_currentCol);
  setUpdatesEnabled(true);
  repaintContents();
  getDocument()->setModified(true);
}

void KWordQuizView::doEditDelete( )
{
  addUndo(i18n("&Undo Delete"));
  //retrieve current selection
  wqCurrentSelection();

  int tr = m_currentSel.topRow();
  int br = m_currentSel.bottomRow();

  if (tr == 0 && br == numRows() - 1)
    br--; //leave one row if all rows are selected

  for (int r = br; r >= tr; --r)
    removeRow(r);

  if (br > numRows())
    br = numRows(); //adjust for new numRows

  //restore selection as much as possible
  addSelection(QTableSelection(tr, m_currentSel.leftCol(), br, m_currentSel.rightCol()));
  setCurrentCell(m_currentRow, m_currentCol);
  getDocument()->setModified(true);
}

const char delim_start = '[';
const char delim_end = ']';


bool KWordQuizView::checkForBlank( QString s, bool blank )
{
  if (!blank)
    return true;
  
  bool result = false;
  int openCount = 0;
  int closeCount = 0;
  QMemArray<int> openPos(0);
  QMemArray<int> closePos(0);

  for (int i = 0; i<= s.length(); ++i)
  {
    if (s[i] == delim_start)
    {
      openCount++;
      openPos.resize(openCount);
      openPos[openCount] = i;
    }

    if (s[i] == delim_end)
    {
      closeCount++;
      closePos.resize(closeCount);
      closePos[closeCount] = i;
    }
  }

  if (openCount == 0 && closeCount == 0)
    return true;
    
  if (openCount > 0 && closeCount > 0)
    if (openPos.size() == closePos.size())
      for (int i = 0; i <= openPos.size(); ++i)
        result = (openPos[i] < closePos[i]);

  return result;
}

void KWordQuizView::doEditMarkBlank( )
{
  if (isEditing())
  {
    addUndo(i18n("&Undo Mark Blank"));
    QLineEdit * l = (QLineEdit *) cellWidget(currentRow(), currentColumn());
    if (l->text().length() > 0)
    {
      QString s = l->text();
      int cp = l->cursorPosition();      
      if (!l->hasSelectedText())
      {      
        if (!s[cp].isSpace() && !s[cp - 1].isSpace())
        {
          l->cursorWordBackward(false);
          int cp1 = l->cursorPosition();
          l->cursorWordForward(false);
          if (l->cursorPosition() != s.length())
          {  while(l->text()[l->cursorPosition()].isSpace())
              l->cursorBackward(false, 1);
          }
          int cp2 = l->cursorPosition();
          if (cp2 == s.length())
            l->setSelection(cp1, cp2 - cp1);
          else
            l->setSelection(cp1, cp2 - cp1 - 1);
         
        }
        else
          return;     
      }

      
      if (l->hasSelectedText())
      {
        QString st = l->selectedText();
        int len = st.length();
        st = st.prepend(delim_start);
        st = st.append(delim_end);
        int ss = l->selectionStart();
        s = s.replace(ss, len, st);
        l->setText(s);
        l->setSelection(ss, st.length());        
      }
    }
  }
}

void KWordQuizView::doEditUnmarkBlank( )
{
  addUndo(i18n("&Undo Unmark Blank"));
  QString s;
  
  if (isEditing())
  {
    QLineEdit * l = (QLineEdit *) cellWidget(currentRow(), currentColumn());
    
    if (l->hasSelectedText())
    {
      QString ls = l->text();
      s = l->selectedText();
      int len = s.length();
      s.remove(delim_start);
      s.remove(delim_end);
      int ss = l->selectionStart();
      ls = ls.replace(ss, len, s);
      l->setText(ls);
      l->setSelection(ss, s.length());
    }
    else
    {
      if (l->text().length() > 0)
      {
        s = l->text();
        int cs = l->cursorPosition();
        
        int fr = s.findRev(delim_start, cs);
        if (fr > 0)
        {
          s = s.replace(fr, 1, "");
          cs--;
        }
        int ff = s.find(delim_end, cs);
        if (ff > 0)
          s = s.replace(ff, 1, "");
        
        l->setText(s);
        l->setCursorPosition(cs);
      }
    }
  }
  else
  {
    wqCurrentSelection(false);
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
      {
        s = text(r, c);
        s = s.remove(delim_start);
        s = s.remove(delim_end);
        setText(r, c, s);    
      }     
  }  
}

bool KWordQuizView::checkSyntax(bool all, bool blanks)
{
  int errorCount = 0;
  int r1, r2, c1 ,c2;
  
  if (all)
  {
    r1 = 0;
    r2 = numRows() - 1;
    c1 = 0;
    c2 = 1;
  }
  else
  {
    wqCurrentSelection(false);
    r1 = m_currentSel.topRow();
    r2 = m_currentSel.bottomRow();
    c1 = m_currentSel.leftCol();
    c2 = m_currentSel.rightCol();
  }  
  
  for (int r = r1; r <= r2; ++r)
    for(int c = c1 ; c <= c2 ; ++c )
    {
      QString s = text(r, c);
      if (s.length() > 0)
        for (int i = 0; i <= s.length(); ++i)
          if (s[i] == delim_start || s[i] == delim_end)
            if (!checkForBlank(s, blanks))
              errorCount++;
    }
  return (errorCount == 0);
}


void KWordQuizView::doVocabSort( )
{
  wqCurrentSelection();
  DlgSort* dlg;
  dlg = new DlgSort(this, "dlg_sort", true);
  dlg->setInitialSize(QSize(245, 240), true);
  dlg->setLanguage(1, horizontalHeader()->label(0));
  dlg->setLanguage(2, horizontalHeader()->label(1));
  if (dlg->exec() == KDialogBase::Accepted)
  {
    addUndo(i18n("&Undo Sort"));
    if (dlg->base())
      sortColumn(0, dlg->ascending(), true);
    else
      sortColumn(1, dlg->ascending(), true);
      getDocument()->setModified(true);
  }
  //restore selection
  addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
  setCurrentCell(m_currentRow, m_currentCol);
}

void KWordQuizView::doVocabShuffle( )
{
  //@todo handle empty rows
  addUndo(i18n("&Undo Shuffle"));
  wqCurrentSelection();
  KRandomSequence* rs;
  rs = new KRandomSequence();
  int count = numRows();
  setUpdatesEnabled(false);
  while (count > 0)
  {
    swapRows(count-1, rs->getLong(count));
    count--;
  }
  //restore selection
  addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
  setCurrentCell(m_currentRow, m_currentCol);
  setUpdatesEnabled(true);
  repaintContents();
  getDocument()->setModified(true);
}

void KWordQuizView::doVocabRC( )
{
  wqCurrentSelection();
  DlgRC* dlg;
  dlg = new DlgRC(this, "dlg_rc", true);
  dlg->setInitialSize(QSize(225, 230), true);
  dlg->setNumRows(numRows());
  dlg->setRowHeight(rowHeight(m_currentRow));
  dlg->setColWidth(columnWidth(m_currentCol));
  if (dlg->exec() == KDialogBase::Accepted)
  {
    if (dlg->numRows() < 1)
      setNumRows(1);
    else
      setNumRows(dlg->numRows());
    for (int i = m_currentSel.topRow(); i <= m_currentSel.bottomRow(); ++i)
      setRowHeight(i, dlg->rowHeight());
    for (int i = m_currentSel.leftCol(); i <= m_currentSel.rightCol(); ++i)
      setColumnWidth(i, dlg->colWidth());
    getDocument()->setModified(true);
  }
  addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
  setCurrentCell(m_currentRow, m_currentCol);
}

void KWordQuizView::doVocabSpecChar( )
{
  QString f = font().family();
  QChar c=' ';
  if (dlgSpecChar==0)
  {
    dlgSpecChar = new DlgSpecChar( this, "insert special char", f, c, false );
    connect( dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    connect( dlgSpecChar, SIGNAL( finished() ), this, SLOT( slotDlgSpecCharClosed() ) );
  }
  dlgSpecChar->show();
}

void KWordQuizView::slotDlgSpecCharClosed( )
{
  if ( dlgSpecChar )
  {
    disconnect( dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    disconnect( dlgSpecChar, SIGNAL( finished() ), this, SLOT( slotDlgSpecCharClosed() ) );
    dlgSpecChar->deleteLater();
    dlgSpecChar = 0L;
  }
}

void KWordQuizView::slotSpecChar( QChar c)
{

  if (isEditing())
  {
    QLineEdit * l = (QLineEdit *) cellWidget(currentRow(), currentColumn());
    if (l->hasSelectedText())
    {

    }
    else
    {
      QString s = l->text();
      int i = l->cursorPosition();
      s.insert(i + 1, c);
      l->setText(s);
      l->setCursorPosition(i + 1);
    }
//
//     if Length(InPlaceEditor.SelText) > 0 then
//       InPlaceEditor.SelText := cTemp
//     else begin
// 

// 
//     end;

  }
  else
  {
    editCell(currentRow(), currentColumn(), true);
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->setText(c);
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->setCursorPosition(1);
  }
}


void KWordQuizView::activateNextCell( )
{
  wqCurrentSelection(false);
  int tr = m_currentSel.topRow();
  int br = m_currentSel.bottomRow();
  int lc = m_currentSel.leftCol();
  int rc = m_currentSel.rightCol();

  if (lc == rc && tr == br) //one cell selected
  {
    clearSelection();
    switch( Config().m_enterMove )
    {
      case 1:
        if (m_currentRow == (numRows() - 1))
          setNumRows(numRows() + 1);
        setCurrentCell(m_currentRow + 1, m_currentCol);
        break;
      case 2:
        if (m_currentCol == 0)
          setCurrentCell(m_currentRow, m_currentCol + 1);
        else
          {
          if (m_currentRow == (numRows() - 1))
            setNumRows(numRows() + 1);
          setCurrentCell(m_currentRow + 1, m_currentCol - 1);
          }
        break;
      case 3:
        setCurrentCell(m_currentRow, m_currentCol);
        break;
    }
  }
  else //a larger selection, move within it
  {
    //addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
    switch( Config().m_enterMove )
    {
      case 1:
        if (m_currentRow == br)
        {
          if (m_currentCol < rc)
            setCurrentCell(tr, rc);
          else
            setCurrentCell(tr, lc);
        }
        else
          if (m_currentRow < br)
            setCurrentCell(m_currentRow + 1, m_currentCol);
        break;
      case 2:
        if (m_currentCol == rc)
        {
          if (m_currentRow < br)
            setCurrentCell(m_currentRow + 1, lc);
          else
            setCurrentCell(tr, lc);
        }
        else
          setCurrentCell(m_currentRow, m_currentCol + 1);
        break;
      case 3:
        setCurrentCell(m_currentRow, m_currentCol);
        break;
    }
  }
}

void KWordQuizView::fromStream( QTextStream * ts )
{
      //ts->setEncoding(QTextStream::UnicodeUTF8);
      QString s = "";
      if (ts->readLine() != "WordQuiz")
      {
        KMessageBox::error(0, i18n("This does not appear to be a (K)WordQuiz file"));
        return;
      }
      s = ts->readLine();
      s = s.left(1);
      int iFV = s.toInt(0);
      if (iFV != 5)
      {
        KMessageBox::error(0, i18n("KWordQuiz can only open files created by WordQuiz 5.x"));
        return;
      }

      while (ts->readLine() != "[Font Info]");
      s = ts->readLine();
      int p = s.find("=", 0);
      QString fam = s.right(s.length() - (p + 1));
      fam = fam.mid(1, fam.length() - 2);
      //g->font().setFamily(s);

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int ps = s.toInt(0);

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int b = 0;
      if (s == "1")
      {
        b = QFont::Bold;
      }

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      bool it = (s == "1");

      QFont f(fam, ps, b, it);
      setFont(f);

      /** TODO handle font and character information
      [Font Info]
      FontName1="Arial" done
      FontSize1=12 done
      FontBold1=0 done
      FontItalic1=0 done
      FontColor1=0
      Charset1=0
      Layout1=0000041d
      FontName2="Arial"
      FontSize2=12
      FontBold2=0
      FontItalic2=0
      FontColor2=0
      Charset1=0
      Layout2=0000041d

      [Character Info]
      Characters1=ÈÉÊËÌÍÎÏÐ
      Characters2=ÜÝÞßàáâãä
      */

      while (ts->readLine() != "[Grid Info]");
      ts->readLine(); //skip value for width of row headers

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      setColumnWidth(0, s.toInt(0, 10));

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      setColumnWidth(1, s.toInt(0, 10));

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      setNumRows(s.toInt(0, 10) - 1); //We need to reduce by one since the header is not included

      // Selection
      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int sc =s.toInt(0, 10) - 1;

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int sr =s.toInt(0, 10) - 1;

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int ec =s.toInt(0, 10) - 1;

      s = ts->readLine();
      p = s.find("=", 0);
      s = s.right(s.length() - (p + 1));
      int er =s.toInt(0, 10) - 1 ;

      setCurrentCell(sr, sc);
      selectCells(sr, sc, er, ec);

      while (ts->readLine() != "[Vocabulary]");

      s = ts->readLine();
      p = s.find("   [", 0);
      s = s.left(p);
      s = s.stripWhiteSpace();
      horizontalHeader()->setLabel(0, s);
      s = ts->readLine();
      horizontalHeader()->setLabel(1, s);

      int i = 0;

      while (!s.isNull())
      {
        s = ts->readLine();
        p = s.find("[", 0);
        QString r = s.mid(p + 1, 10);
        int h = r.toInt(0, 10);
        setRowHeight(i, h / 15);
        s = s.left(p);
        s = s.stripWhiteSpace();
        if (!s.isEmpty())
          setText(i, 0, s);

        s = ts->readLine();
        if (!s.isEmpty())
          setText(i, 1, s);
        i++;
      }
}

void KWordQuizView::toStream( QTextStream* ts )
{
    //ts.setEncoding(QTextStream::UnicodeUTF8);
    // \r\n is for  compatibility with the Windows version
    QString s = "";

    *ts << "WordQuiz\r\n";
    *ts << "5.9.0\r\n\r\n";

    QFont f = font();

    *ts << "[Font Info]\r\n";
    *ts << "FontName1=\"" + f.family() + "\"\r\n";
    *ts << "FontSize1=" + s.setNum(f.pointSize()) + "\r\n";
    if (f.bold())
    {
      *ts << "FontBold1=1\r\n";
    }
    else
    {
      *ts << "FontBold1=0\r\n";
    }
    if (f.italic())
    {
      *ts << "FontItalic1=1\r\n";
    }
    else
    {
      *ts << "FontItalic1=0\r\n";
    }
    *ts << "FontColor1=0\r\n";
    *ts << "CharSet1=0\r\n";
    *ts << "Layout1=0\r\n";
    *ts << "FontName2=\"" + f.family() + "\"\r\n";
    *ts << "FontSize2=" + s.setNum(f.pointSize()) + "\r\n";
    if (f.bold())
    {
      *ts << "FontBold2=1\r\n";
    }
    else
    {
      *ts << "FontBold2=0\r\n";
    }
    if (f.italic())
    {
      *ts << "FontItalic2=1\r\n";
    }
    else
    {
      *ts << "FontItalic2=0\r\n";
    }
    *ts << "FontColor2=0\r\n";
    *ts << "CharSet2=0\r\n";
    *ts << "Layout2=0\r\n\r\n";

    *ts << "[Character Info]\r\n";
    *ts << "Characters1=abcdefghi\r\n";
    *ts << "Characters2=jklmnopqr\r\n\r\n";

    *ts << "[Grid Info]\r\n";
    *ts << "ColWidth0=" + s.setNum(verticalHeader()->sectionSize(0)) + "\r\n";
    *ts << "ColWidth1=" + s.setNum(columnWidth(0)) + "\r\n";
    *ts << "ColWidth2=" + s.setNum(columnWidth(1)) + "\r\n";
    *ts << "RowCount=" + s.setNum(numRows() + 1) + "\r\n"; //Add one for the header

    // Selection
    if (numSelections() > 0)
    {
      QTableSelection qts = selection(0);
      *ts << "SelLeft=" + s.setNum(qts.leftCol() + 1) + "\r\n";
      *ts << "SelTop=" + s.setNum(qts.topRow() + 1) + "\r\n";
      *ts << "SelRight=" + s.setNum(qts.rightCol() +1) + "\r\n";
      *ts << "SelBottom=" + s.setNum(qts.bottomRow() +1) + "\r\n\r\n";
    }
    else
    {
      *ts << "SelLeft=" + s.setNum(currentColumn() + 1) + "\r\n";
      *ts << "SelTop=" + s.setNum(currentRow() + 1) + "\r\n";
      *ts << "SelRight=" + s.setNum(currentColumn() +1) + "\r\n";
      *ts << "SelBottom=" + s.setNum(currentRow() +1) + "\r\n\r\n";
    }

    *ts << "[Vocabulary]\r\n";

    *ts << horizontalHeader()->label(0)  + "   [0000000300]\r\n"; // + s.setNum(g->horizontalHeader()->sectionSize(2) * 15) + "]\r\n";
    *ts << horizontalHeader()->label(1)  + "\r\n";
    int i = 0;
    int r = numRows();
    while (i < r)
    {
      *ts << text(i,0)  + QString( "   [%1]\r\n" ).arg(rowHeight(i) * 15, 10, 10 );
      *ts << text(i,1) + "\r\n";
      i++;
    }
}

void KWordQuizView::addUndo( const QString & caption )
{
  while (m_undoList->count() > 10)
  {
    m_undoList->remove(m_undoList->begin());
  }

  QString * s = new QString();
  QTextStream* ts = new QTextStream(s, IO_WriteOnly);
  toStream(ts);

  WQUndo* undo = new WQUndo();
  undo->setText(caption);
  undo->setData(s);

  m_undoList->prepend(*undo);

  getDocument()->setModified(true);

  emit undoChange(caption, true);
}

void KWordQuizView::setFont( const QFont & font)
{
  QTable::setFont(font);
  horizontalHeader()->setFont(KGlobalSettings::generalFont());
  verticalHeader()->setFont(KGlobalSettings::generalFont());  
  for (uint i = 0; i < numRows(); ++i)
    adjustRow(i); //setRowHeight(i, fontMetrics().lineSpacing() );
}

void KWordQuizView::paintCell( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg )
{
  QColorGroup g (cg);
  
  if (Config().m_enableBlanks)
    if (!checkForBlank(text(row, col), true))
      g.setColor(QColorGroup::Text, Qt::red);
  
  QTable::paintCell (p, row, col, cr, selected, g );
}

