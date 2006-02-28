/***************************************************************************
                          kwordquizview.cpp  -  description
                             -------------------
    begin          : Wed Jul 24 20:12:30 PDT 2002
    copyright      : (C) 2002-2005 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>
#include <Q3Table>
#include <QClipboard>
#include <QLineEdit>
#include <Q3MemArray>
#include <QKeyEvent>
#include <QEvent>
#include <QList>

#include <klocale.h>
#include <kmessagebox.h>
#include <krandomsequence.h>
#include <kglobalsettings.h>
#include <knotification.h>

#include "kwordquizview.h"
#include "keduvocdocument.h"
#include "kwordquiz.h"
#include "dlgsort.h"
#include "prefs.h"
#include "dlgrc.h"
#include "dlgspecchar.h"

QList<WQUndo> *KWQTableView::m_undoList = 0L;

KWQTableItem::KWQTableItem(Q3Table* table, EditType et, const QString & text) : Q3TableItem(table, et, text)
{}

int KWQTableItem::alignment() const
{
  bool num;
  bool ok1 = false;
  bool ok2 = false;
  (void)text().toInt(&ok1);
  if (!ok1)
    (void)text().toDouble(&ok2);
  num = ok1 || ok2;

  return (num ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter;
}

KWQTableView::KWQTableView(QWidget *parent, const char *name) : QTableView(parent)
{
  if(!m_undoList)
    m_undoList = new QList<WQUndo>();

  //setNumCols(2);
  setSelectionMode(QAbstractItemView::SingleSelection);

  setMinimumSize(0, 0); //This seems to improve resizing of main window
  dlgSpecChar = 0;
}

KWQTableView::~KWQTableView()
{
}

void KWQTableView::displayDoc()
{
  setUpdatesEnabled(false);
  saveCurrentSelection(false);
//  setNumRows(getDocument()->numEntries());
//  horizontalHeader()->setLabel(0, getDocument()->originalIdentifier());
//  horizontalHeader()->setLabel(1, getDocument()->identifier(1));
  setColumnWidth(0, getDocument()->sizeHint(0));
  setColumnWidth(1, getDocument()->sizeHint(1));
  if (getDocument()->font() != NULL)
    setFont(*(getDocument()->font()));

  for (int i=0; i<getDocument()->numEntries(); i++)
  {
    setText(i, 0, getDocument()->entry(i)->original());
    setText(i, 1, getDocument()->entry(i)->translation(1));
  }
  setUpdatesEnabled(true);
//  addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));

//  setCurrentCell(m_currentRow, m_currentCol);
//  repaintContents();
}

KEduVocDocument *KWQTableView::getDocument() const
{
  KWordQuizApp *theApp=(KWordQuizApp *) parentWidget();
  return theApp->getDocument();
}

void KWQTableView::print(KPrinter *pPrinter)
{
  ///@todo retrieve text to print from doc instead of table
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

//  int cw0 = verticalHeader()->width();
  int cw0 = 50;
  int cw1 = columnWidth(0);
  int cw2 = columnWidth(1);
  int cw3 = 0;



  if (type == 1)
    cw3 = 50;

  int gridWidth = cw0 + cw1 + cw2 + cw3;
  int lPos = marg;
//  int tPos = marg + horizontalHeader()->height();
  int tPos = marg + 20;

  QRect w = painter.window();

  doNewPage(painter, res, type);

  if (type == 2)
  {
    tPos = card_marg;
    for (int rc = 0; rc < getDocument()->numEntries(); ++rc)
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
      painter.drawText(card_marg + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter, getDocument()->originalIdentifier());
      painter.drawText(card_marg + card_width + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter, getDocument()->identifier(1));
      //draw text
      painter.setFont(font());
      painter.drawText(card_marg + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
        card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, getDocument()->entry(rc)->original());

      painter.drawText(card_marg + card_width + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
        card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, getDocument()->entry(rc)->translation(1));

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

    for (int rc = 0; rc < getDocument()->numEntries(); ++rc)
    {
      painter.drawLine(lPos, tPos, lPos + gridWidth, tPos);
      painter.setFont(KGlobalSettings::generalFont());

      painter.drawText(lPos, tPos, cw0 - pad, rowHeight(rc), Qt::AlignRight | Qt::AlignVCenter, QString::number(rc + 1));

      painter.setFont(font());
      painter.drawText(lPos + cw0 + pad, tPos, cw1, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter, getDocument()->entry(rc)->original());

      if (type == 0)
        painter.drawText(lPos + cw0 + cw1 + pad, tPos, cw2, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter,
          getDocument()->entry(rc)->translation(1));

      tPos = tPos + rowHeight(rc);

      if (tPos + rowHeight(rc + 1) > w.height() - marg)
      {
        doEndOfPage(painter, tPos, pageNum++, res, type);
        tPos = marg + 20 /*horizontalHeader()->height()*/;
        pPrinter->newPage();
        doNewPage(painter, res, type);
      }
    }
  }
  doEndOfPage(painter, tPos, pageNum++,  res, type);
  painter.end();
}

void KWQTableView::doNewPage( QPainter & painter, int res, int type )
{
    int cw0 = 200 /*verticalHeader()->width()*/;
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
      QRect r = painter.boundingRect(0, 0, 0, 0, Qt::AlignLeft, score);
      painter.drawText(w.width() - r.width() - marg, marg - 20, score);
    }
/*
    painter.drawText(marg, marg, cw0, horizontalHeader()->height(), Qt::AlignRight | Qt::AlignVCenter, "");

    painter.drawText(marg + cw0 + pad, marg, cw1, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, getDocument()->originalIdentifier());
    painter.drawText(marg + cw0 + cw1 + pad, marg, cw2, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, getDocument()->identifier(1));

    if (type == 1)
      painter.drawText(marg + cw0 + cw1 + cw2 + pad, marg, cw3, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, i18n("Score"));
*/
}

void KWQTableView::doEndOfPage( QPainter & painter, int vPos, int pageNum, int res, int type )
{
    int marg = res;
    painter.setFont(KGlobalSettings::generalFont());
    QRect w = painter.window();
    QRect r = painter.boundingRect(0, 0, 0, 0, Qt::AlignLeft, QString::number(pageNum));
    painter.drawText((w.width()/2) - (r.width()/2), w.height() - marg + 20, QString::number(pageNum));

    if (type == 2)
      return;

    int cw0 = 200 /*verticalHeader()->width()*/;
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

QWidget * KWQTableView::beginEdit( int row, int col, bool replace )
{
  if (col == 0)
    m_currentText = getDocument()->entry(row)->original();
  else
    m_currentText = getDocument()->entry(row)->translation(1);

//  cellEditor = Q3Table::beginEdit(row, col, replace);
  if (cellEditor)
    cellEditor->installEventFilter(this);
  return cellEditor;
}

void KWQTableView::endEdit( int row, int col, bool accept, bool replace )
{
  // this code gets called after enter and arrow keys, now we
  // only process if editing really has been done
/*
  if ((cellWidget(row, col) != 0) && accept) //if edited the cellWidget still exists
  {
    if (((QLineEdit *) cellWidget(row, col))->text() != m_currentText)
    {
      addUndo(i18n("&Undo Entry"));

      if (col == 0)
        getDocument()->entry(row)->setOriginal(((QLineEdit *) cellWidget(row, col))->text());
      else
        getDocument()->entry(row)->setTranslation(1, ((QLineEdit *) cellWidget(row, col))->text());
    }

    Q3Table::endEdit(row, col, accept, replace); //this will destroy the cellWidget

    if (!text(row, col).isEmpty())
    {
      Q3TableItem* itm;
      itm = item(row, col);
      itm->setWordWrap(true);
      adjustRow(row);
      getDocument() -> setModified(true);
      if (Prefs::enableBlanks())
        if (!checkForBlank(text(row, col), true))
          KNotification::event("SyntaxError", i18n("There is an error with the Fill-in-the-blank brackets"));
    }
  }*/
}

void KWQTableView::adjustRow( int row )
{
  // we want to make the row high enough to display content, but
  // if the user already made it even higher we keep that height
  int rh = rowHeight(row);
//  Q3Table::adjustRow(row);
  if (rh > rowHeight(row))
    setRowHeight(row, rh);
}

void KWQTableView::saveCurrentSelection(bool clear = true)
{
/*
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
    Q3TableSelection* ts;
    ts = new Q3TableSelection(m_currentRow, m_currentCol, m_currentRow, m_currentCol);
    m_currentSel = *ts;
  }*/
}

void KWQTableView::doEditUndo( )
{
/*  if (isEditing())
  {
    if (((QLineEdit *) cellWidget(currentRow(), currentColumn()))->isUndoAvailable())
      ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->undo();
  }
  else
  {
    WQUndo undo;
    if (m_undoList->count() > 0)
    {
      setUpdatesEnabled(false);
      undo = m_undoList->first();
      getDocument()->setFont(new QFont(undo.font()));
      verticalHeader()->setMinimumWidth(undo.colWidth0());
      getDocument()->setSizeHint(0, undo.colWidth1());
      getDocument()->setSizeHint(1, undo.colWidth2());

      getDocument()->removeAllEntries();

      QList<KEduVocExpression> dataList = undo.list();
      QList<KEduVocExpression>::Iterator end(dataList.end());
      for(QList<KEduVocExpression>::Iterator dataIt = dataList.begin(); dataIt != end; ++dataIt)
      {
        getDocument()->appendEntry(&(*dataIt));
        // TODO EPT setRowHeight(i, (*dataIt).rowHeight());
      }
      displayDoc();

      setCurrentCell(undo.currentRow(), undo.currentCol());
      addSelection(undo.selection());

      m_undoList->remove(m_undoList->begin());
      setUpdatesEnabled(true);
    }

    if (m_undoList->count() > 0)
    {
      undo = m_undoList->first();
      emit undoChange(undo.text(), true);
    }
    else
      emit undoChange(i18n("Cannot &Undo"), false);
  }*/
}

void KWQTableView::doEditCut( )
{
/*  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->cut();
  }
  else
  {
    addUndo(i18n("&Undo Cut"));
    doEditCopy();
    saveCurrentSelection();
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
        if (c == 0)
          getDocument()->entry(r)->setOriginal("");
        else
          getDocument()->entry(r)->setTranslation(1, "");
  }
  getDocument()->setModified(true);*/
}

void KWQTableView::doEditCopy( )
{
/*  bool mod = getDocument()->isModified();
  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->copy();
  }
  else
  {
    saveCurrentSelection(false);
    QString s;
    QString rs;
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
    {
      for (int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
      {
        if (c == 0)
          rs = getDocument()->entry(r)->original();
        else
          rs = getDocument()->entry(r)->translation(1);
        s = s + rs + "\t";
      }
      s = s + "\n";
    }
    kapp->clipboard()->setText(s);
  }
  getDocument()->setModified(mod);*/
}

void KWQTableView::doEditPaste( )
{
/*  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->paste();
  }
  else
  {
    addUndo(i18n("&Undo Paste"));
    saveCurrentSelection();
    int tr = m_currentSel.topRow();
    int br = m_currentSel.bottomRow();
    int lc = m_currentSel.leftCol();
    int rc = m_currentSel.rightCol();

    if (lc == rc && tr == br) //one cell selected
    {
      QString s = kapp->clipboard()->text();
      QStringList sl;
      sl = QStringList::split("\n", s);

      //Do we need to add entries (rows)?
      while (sl.count() > getDocument()->numEntries() - tr)
        getDocument()->appendEntry(new KEduVocExpression);

      br= br + sl.count() - 1;

      if (lc == 0) //left col?
        if (sl[0].find("\t") < ((int) sl[0].length() - 1))
          rc = 1; //expand to second column;

      int i = 0;
      int ar = tr;
      QStringList sr;
      while (i < sl.count() && br <= getDocument()->numEntries())
      {
        int ac = lc;

        sr = QStringList::split("\t", sl[i]);
        int c = 0;
        while (ac <= rc)
        {
          if (ac == 0)
            getDocument()->entry(ar)->setOriginal(sr[c]);
          else
            getDocument()->entry(ar)->setTranslation(1, sr[c]);
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
      int i = 0;
      int ar = tr;
      QStringList sr;
      while(i < sl.count() && ar <= br )
      {
        int ac = lc;

        sr = QStringList::split("\t", sl[i]);
        int c = 0;
        while (ac <= rc)
        {
          if (ac == 0)
            getDocument()->entry(ar)->setOriginal(sr[c]);
          else
            getDocument()->entry(ar)->setTranslation(1, sr[c]);
          ac++;
          c++;
        }
        ar++;
        i++;
      }
    }

    displayDoc();

    //restore selection
    addSelection(Q3TableSelection(tr, lc, br, rc));
    setCurrentCell(m_currentRow, m_currentCol);
  }
  getDocument()->setModified(true);*/
}

void KWQTableView::doEditClear( )
{
/*  if (isEditing())
  {
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->clear();
  }
  else
  {
    addUndo(i18n("&Undo Clear"));
    saveCurrentSelection(false);
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
        if (c == 0)
          getDocument()->entry(r)->setOriginal("");
        else
          getDocument()->entry(r)->setTranslation(1, "");
  }
  displayDoc();
  getDocument()->setModified(true);*/
}

void KWQTableView::doEditInsert( )
{
  addUndo(i18n("&Undo Insert"));
  saveCurrentSelection(false);

  for (int i = m_currentSel.topRow(); i <= m_currentSel.bottomRow(); i++)
    getDocument()->insertEntry(new KEduVocExpression, i);

//  addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
  displayDoc();
  getDocument()->setModified(true);
}

void KWQTableView::doEditDelete( )
{
  addUndo(i18n("&Undo Delete"));
  //retrieve current selection
  saveCurrentSelection();

  int tr = m_currentSel.topRow();
  int br = m_currentSel.bottomRow();

  if (tr == 0 && br == getDocument()->numEntries() - 1)
    br--; //leave one row if all rows are selected

  for (int r = br; r >= tr; --r)
    getDocument()->removeEntry(r);

  displayDoc();

  if (br > getDocument()->numEntries())
    br = getDocument()->numEntries(); //adjust for new numRows

  //restore selection as much as possible
//  addSelection(Q3TableSelection(tr, m_currentSel.leftCol(), br, m_currentSel.rightCol()));
//  setCurrentCell(m_currentRow, m_currentCol);
  getDocument()->setModified(true);
}

bool KWQTableView::checkForBlank( const QString  & s, bool blank )
{
  if (!blank)
    return true;

  bool result = false;
  int openCount = 0;
  int closeCount = 0;
  QVector<int> openPos(0);
  QVector<int> closePos(0);

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

void KWQTableView::doEditMarkBlank( )
{
/*  if (isEditing())
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
          if (l->cursorPosition() != (int) s.length())
          {  while(l->text()[l->cursorPosition()].isSpace())
              l->cursorBackward(false, 1);
          }
          int cp2 = l->cursorPosition();
          if (cp2 == (int) s.length())
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
  }*/
}

void KWQTableView::doEditUnmarkBlank( )
{
/*  addUndo(i18n("&Undo Unmark Blank"));
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
    saveCurrentSelection(false);
    for (int r = m_currentSel.topRow(); r <= m_currentSel.bottomRow(); ++r)
      for(int c = m_currentSel.leftCol(); c <= m_currentSel.rightCol(); ++c)
      {
        s = text(r, c);
        s = s.remove(delim_start);
        s = s.remove(delim_end);
        if (c == 0)
          getDocument()->entry(r)->setOriginal(s);
        else
          getDocument()->entry(r)->setTranslation(1, s);
        setText(r, c, s);
      }
  }*/
}

void KWQTableView::doVocabSort( )
{
  saveCurrentSelection();
  DlgSort* dlg;
  dlg = new DlgSort(this, "dlg_sort", true);
  dlg->setLanguage(1, getDocument()->originalIdentifier());
  dlg->setLanguage(2, getDocument()->identifier(1));

  if (dlg->exec() == KDialogBase::Accepted)
  {
    addUndo(i18n("&Undo Sort"));
    if (dlg->base())
      getDocument()->sort(0);
      // TODO EPT sortColumn(0, dlg->ascending(), true);
    else
      getDocument()->sort(1);
      // TODO EPT sortColumn(1, dlg->ascending(), true);
    getDocument()->setModified(true);
    displayDoc();
  }
  //restore selection
//  addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
//  setCurrentCell(m_currentRow, m_currentCol);
}

void KWQTableView::doVocabShuffle( )
{
  /**@todo handle empty rows, port to document class */
  addUndo(i18n("&Undo Shuffle"));
  saveCurrentSelection();
  KRandomSequence* rs;
  rs = new KRandomSequence();
  int count = getDocument()->numEntries();
  setUpdatesEnabled(false);
  while (count > 0)
  {
//    swapRows(count-1, rs->getLong(count));
    count--;
  }
  //restore selection
//  addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
//  setCurrentCell(m_currentRow, m_currentCol);
  setUpdatesEnabled(true);
//  repaintContents();
  getDocument()->setModified(true);
}

void KWQTableView::doVocabRC( )
{
  saveCurrentSelection();
  DlgRC* dlg;
  dlg = new DlgRC(this, "dlg_rc", true);
  dlg->setNumRows(getDocument()->numEntries());
  dlg->setRowHeight(rowHeight(m_currentRow));
  dlg->setColWidth(columnWidth(m_currentCol));

  if (dlg->exec() == KDialogBase::Accepted)
  {
    int newNumRows;
    if (dlg->numRows() < 1)
      newNumRows = 1;
    else
      newNumRows = dlg->numRows();

    while (newNumRows > getDocument()->numEntries())
      getDocument()->appendEntry(new KEduVocExpression);

    while (newNumRows < getDocument()->numEntries())
      getDocument()->removeEntry(getDocument()->numEntries()-1);

    for (int i = m_currentSel.topRow(); i <= m_currentSel.bottomRow(); ++i)
      setRowHeight(i, dlg->rowHeight());
    for (int i = m_currentSel.leftCol(); i <= m_currentSel.rightCol(); ++i)
      getDocument()->setSizeHint(i, dlg->colWidth());

    displayDoc();

    getDocument()->setModified(true);
  }
//  addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
//  setCurrentCell(m_currentRow, m_currentCol);
}

void KWQTableView::doVocabSpecChar( )
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

void KWQTableView::slotDlgSpecCharClosed( )
{
  if ( dlgSpecChar )
  {
    disconnect( dlgSpecChar, SIGNAL(insertChar(QChar)), this, SLOT(slotSpecChar(QChar)));
    disconnect( dlgSpecChar, SIGNAL( finished() ), this, SLOT( slotDlgSpecCharClosed() ) );
    dlgSpecChar->deleteLater();
    dlgSpecChar = 0L;
  }
}

void KWQTableView::slotSpecChar(const QChar & c)
{
/*  if (isEditing())
  {
    QLineEdit * l = (QLineEdit *) cellWidget(currentRow(), currentColumn());
    if (l->hasSelectedText())
    {
      QString ls = l->text();
      QString s = l->selectedText();
      int len = s.length();
      int ss = l->selectionStart();
      ls = ls.replace(ss, len, c);
      l->setText(ls);
      l->setSelection(ss, 1);
    }
    else
    {
      QString s = l->text();
      int i = l->cursorPosition();
      s.insert(i, c);
      l->setText(s);
      l->setCursorPosition(i + 1);
    }
  }
  else
  {
    editCell(currentRow(), currentColumn(), true);
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->setText(QString(c));
    ((QLineEdit *) cellWidget(currentRow(), currentColumn()))->setCursorPosition(1);
  }*/
}

void KWQTableView::activateNextCell( )
{
  saveCurrentSelection(false);
  int tr = m_currentSel.topRow();
  int br = m_currentSel.bottomRow();
  int lc = m_currentSel.leftCol();
  int rc = m_currentSel.rightCol();

  if (lc == rc && tr == br) //one cell selected
  {
    clearSelection();
    switch(Prefs::enterMove())
    {
      case 0:
        if (m_currentRow == (getDocument()->numEntries() - 1))
        {
          getDocument()->appendEntry(new KEduVocExpression());
//          setNumRows(getDocument()->numEntries() + 1);
        }
//        setCurrentCell(m_currentRow + 1, m_currentCol);
        break;
      case 1:
        if (m_currentCol == 0)
          /*setCurrentCell(m_currentRow, m_currentCol + 1)*/;
        else
          {
          if (m_currentRow == (getDocument()->numEntries() - 1))
          {
            getDocument()->appendEntry(new KEduVocExpression());
//            setNumRows(getDocument()->numEntries() + 1);
          }
//          setCurrentCell(m_currentRow + 1, m_currentCol - 1);
          }
        break;
      case 2:
//        setCurrentCell(m_currentRow, m_currentCol);
        break;
    }
  }
  else //a larger selection, move within it
  {
    //addSelection(QTableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
    switch(Prefs::enterMove())
    {
      case 0:
        if (m_currentRow == br)
        {
          if (m_currentCol < rc)
            /*setCurrentCell(tr, rc)*/;
          else
            /*setCurrentCell(tr, lc)*/;
        }
        else
          if (m_currentRow < br)
            /*setCurrentCell(m_currentRow + 1, m_currentCol)*/;
        break;
      case 1:
        if (m_currentCol == rc)
        {
          if (m_currentRow < br)
            /*setCurrentCell(m_currentRow + 1, lc)*/;
          else
            /*setCurrentCell(tr, lc)*/;
        }
        else
          /*setCurrentCell(m_currentRow, m_currentCol + 1)*/;
        break;
      case 2:
        /*setCurrentCell(m_currentRow, m_currentCol)*/;
        break;
    }
  }
}

void KWQTableView::addUndo( const QString & caption )
{
  while (m_undoList->count() > 10)
  {
    m_undoList->remove(m_undoList->begin());
  }

  WQUndo* undo = new WQUndo();
  undo->setText(caption);
  if (getDocument()->font() != NULL)
    undo->setFont(*(getDocument()->font()));
//  undo->setColWidth0(verticalHeader()->width());
  undo->setColWidth1(columnWidth(0));
  undo->setColWidth2(columnWidth(1));
//  undo->setCurrentRow(currentRow());
//  undo->setCurrentCol(currentColumn());
//  undo->setSelection(selection(0));

  QList<KEduVocExpression> list;
  for(int i = 0; i < getDocument()->numEntries(); i++)
  {
//    KWqlDataItem item(text(i, 0), text(i, 1), rowHeight(i));
    list.append(*getDocument()->entry(i));
  }

  undo->setList(list);

  m_undoList->prepend(*undo);

  getDocument()->setModified(true);

  emit undoChange(caption, true);
}

void KWQTableView::setFont( const QFont & font)
{
//  Q3Table::setFont(font);
//  horizontalHeader()->setFont(KGlobalSettings::generalFont());
//  verticalHeader()->setFont(KGlobalSettings::generalFont());
  for (int i = 0; i < getDocument()->numEntries(); ++i)
    // we adjust rows here because big fonts need big cells
    adjustRow(i);
}

void KWQTableView::paintCell( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg )
{
/*  QColorGroup g (cg);

  if (Prefs::enableBlanks())
    if (!checkForBlank(text(row, col), true))
      g.setColor(QColorGroup::Text, Qt::red);

  Q3Table::paintCell (p, row, col, cr, selected, g );*/
}

void KWQTableView::keyPressEvent( QKeyEvent * e)
{
  /*if (isEditing())
    if (e->key() == Key_Tab)
    {
      endEdit(currentRow(), currentColumn(), true, true);
      activateNextCell();
      return;
    }
    else
      return;
  */
  if (e->key() == Qt::Key_Tab)
  {
    activateNextCell();
    return;
  }
//  Q3Table::keyPressEvent(e);
}

void KWQTableView::slotCheckedAnswer( int i )
{
  if (i == -1)
  {
    clearSelection();
//    addSelection(Q3TableSelection(m_currentSel.topRow(), m_currentSel.leftCol(), m_currentSel.bottomRow(), m_currentSel.rightCol()));
//    setCurrentCell(m_currentRow, m_currentCol);
  }
  else
  {
    clearSelection();
//    setCurrentCell(i, 0);
    selectRow(i);
  }
}

bool KWQTableView::eventFilter( QObject * o, QEvent * e )
{
  if (o == cellEditor)
  {
    if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent *k = (QKeyEvent *)e;
      if (k->key() == Qt::Key_Tab)
      {
//        endEdit(currentRow(), currentColumn(), true, true);
        activateNextCell();
        return true;
      }
    }
  }
//  return Q3Table::eventFilter(o, e);
}

void KWQTableView::setText(int row, int col, const QString & text)
{
  Q3TableItem *itm = 0 /*item(row, col)*/;
  if (itm) {
    itm->setText(text);
    itm->updateEditor(row, col);
//    updateCell(row, col);
  } else {
//    KWQTableItem *i = new KWQTableItem(this, Q3TableItem::OnTyping, text);
//    setItem(row, col, i);
  }
}

#include "kwordquizview.moc"
