/***************************************************************************
                          kwordquizview.cpp  -  description
                             -------------------
    begin          : Wed Jul 24 20:12:30 PDT 2002
    copyright      : (C) 2002-2006 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QHeaderView>
#include <QPainter>
#include <QClipboard>
#include <QLineEdit>
#include <QKeyEvent>
#include <QEvent>
#include <QList>

#include <klocale.h>
#include <kmessagebox.h>
#include <krandomsequence.h>
#include <kglobalsettings.h>
#include <knotification.h>
#include <kdebug.h>

#include "kwqtableview.h"
#include "keduvocdocument.h"
#include "kwordquiz.h"
#include "dlgsort.h"
#include "prefs.h"
#include "dlgrc.h"

QList<WQUndo> *KWQTableView::m_undoList = 0L;

KWQTableView::KWQTableView(QWidget *parent) : QTableView(parent)
{
  if(!m_undoList)
    m_undoList = new QList<WQUndo>();

  setSelectionMode(QAbstractItemView::ContiguousSelection);
  setSelectionBehavior(QAbstractItemView::SelectItems);
  setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
  setTabKeyNavigation(true);
  m_delegate = new KWQTableDelegate(this);
  setItemDelegate(m_delegate);
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
    for (int rc = 0; rc < model()->rowCount(); ++rc)
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
      //painter.drawText(card_marg + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter, getDocument()->originalIdentifier());
      //painter.drawText(card_marg + card_width + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter, getDocument()->identifier(1));
      //draw text
      painter.setFont(font());
      //painter.drawText(card_marg + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
      //  card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, getDocument()->entry(rc)->original());

     // painter.drawText(card_marg + card_width + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
       // card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, getDocument()->entry(rc)->translation(1));

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

    for (int rc = 0; rc < model()->rowCount(); ++rc)
    {
      painter.drawLine(lPos, tPos, lPos + gridWidth, tPos);
      painter.setFont(KGlobalSettings::generalFont());

      painter.drawText(lPos, tPos, cw0 - pad, rowHeight(rc), Qt::AlignRight | Qt::AlignVCenter, QString::number(rc + 1));

      painter.setFont(font());
      //painter.drawText(lPos + cw0 + pad, tPos, cw1, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter, getDocument()->entry(rc)->original());

      if (type == 0)
        //painter.drawText(lPos + cw0 + cw1 + pad, tPos, cw2, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter,
          //getDocument()->entry(rc)->translation(1));

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

void KWQTableView::doNewPage(QPainter & painter, int res, int type)
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
      //painter.drawText(card_marg, card_marg - 20, i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName()));
      return;
    }
    painter.drawLine(marg, marg, marg + cw0 + cw1 + cw2 + cw3, marg);

    //painter.drawText(marg, marg - 20, i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName()));

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

void KWQTableView::doEndOfPage(QPainter & painter, int vPos, int pageNum, int res, int type)
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

void KWQTableView::endEdit(int row, int col, bool accept, bool replace)
{

}

void KWQTableView::doEditUndo()
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

void KWQTableView::doEditCut()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *lineEdit = static_cast<QLineEdit*>(QApplication::focusWidget());
      lineEdit->cut();
    }
  }
  else
  {
    addUndo(i18n("&Undo Cut"));

    doEditCopy();

    QModelIndex index;
    QModelIndexList items = selectionModel()->selectedIndexes();

    foreach (index, items)
      model()->setData(index, QVariant());
  }
}

void KWQTableView::doEditCopy()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *lineEdit = static_cast<QLineEdit*>(QApplication::focusWidget());
      lineEdit->copy();
    }
  }
  else
  {
    QRect sel = selection();
    QString s;
    QString rs;
    for (int r = sel.top(); r <= sel.bottom(); ++r)
    {
      for (int c = sel.left(); c <= sel.right(); ++c)
      {
        QVariant v =  model()->data(model()->index(r, c, QModelIndex()), Qt::DisplayRole);
        rs = v.toString();
        s = s + rs + "\t";
      }
      s = s + "\n";
    }
    kapp->clipboard()->setText(s);
  }
}

void KWQTableView::doEditPaste()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *lineEdit = static_cast<QLineEdit*>(QApplication::focusWidget());
      lineEdit->paste();
    }
  }
  else
  {
    addUndo(i18n("&Undo Paste"));
    QRect sel = selection();
    int tr = sel.top();
    int br = sel.bottom();
    int lc = sel.left();
    int rc = sel.right();

    QString s = kapp->clipboard()->text();
    QStringList sl;
    sl = s.split("\n", QString::SkipEmptyParts);

    int i = 0;
    int ar = tr;
    int ac;
    QStringList sr;

    if (lc == rc && tr == br) //one cell selected
    {
      //Do we need to add entries (rows)?
      while (sl.count() > model()->rowCount() - tr)
        model()->insertRows(model()->rowCount(), 1, QModelIndex());

      br = br + sl.count() - 1;

      if (lc == 0) //left col?
        if (sl[0].indexOf("\t") < ((int) sl[0].length() - 1))
          rc = 1; //expand to second column;


      while (i < sl.count() && br <= model()->rowCount())
      {
        ac = lc;

        sr = sl.at(i).split("\t", QString::SkipEmptyParts);
        int c = 0;
        while (ac <= rc)
        {
          model()->setData(model()->index(ar, ac), QVariant(sr[c]), Qt::EditRole);
          ac++;
          c++;
        }
        ar++;
        i++;
      }
    }
    else
    {
      while(i < sl.count() && ar <= br )
      {
        ac = lc;

        sr = sl.at(i).split("\t", QString::SkipEmptyParts);
        int c = 0;
        while (ac <= rc)
        {
          model()->setData(model()->index(ar, ac), QVariant(sr[c]), Qt::EditRole);
          ac++;
          c++;
        }
        ar++;
        i++;
      }
    }
    selectCells(QRect(lc, tr, ac - lc, ar - tr));
  }
}

void KWQTableView::doEditClear()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *lineEdit = static_cast<QLineEdit*>(QApplication::focusWidget());
      lineEdit->clear();
    }
  }
  else
  {
    addUndo(i18n("&Undo Clear"));

    QModelIndex index;
    QModelIndexList items = selectionModel()->selectedIndexes();

    foreach (index, items)
      model()->setData(index, QVariant());
  }
}

void KWQTableView::doEditInsert()
{
  addUndo(i18n("&Undo Insert"));
  QRect sel = selection();
  int currentRow = currentIndex().row();
  int currentColumn = currentIndex().column();
  model()->insertRows(sel.top(), sel.height(), QModelIndex());
  setCurrentIndex(model()->index(currentRow, currentColumn));
  selectCells(sel);
}

void KWQTableView::doEditDelete()
{
  addUndo(i18n("&Undo Delete"));
  QRect sel = selection();
  int currentRow = currentIndex().row();
  int currentColumn = currentIndex().column();
  model()->removeRows(sel.top(), sel.height(), QModelIndex());

  while (!model()->hasIndex(currentRow, currentColumn))
    currentRow--;

  setCurrentIndex(model()->index(currentRow, currentColumn));
  selectCells(sel);
}

bool KWQTableView::checkForBlank(const QString  & s, bool blank)
{
  if (!blank)
    return true;

  bool result = false;
  int openCount = 0;
  int closeCount = 0;
  QVector<int> openPos(0);
  QVector<int> closePos(0);

  for (int i = 0; i< s.length(); ++i)
  {
    if (s[i] == delim_start)
    {
      openCount++;
      openPos.resize(openCount);
      openPos[openCount - 1] = i;
    }

    if (s[i] == delim_end)
    {
      closeCount++;
      closePos.resize(closeCount);
      closePos[closeCount - 1] = i;
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

void KWQTableView::doEditMarkBlank()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *l = static_cast<QLineEdit*>(QApplication::focusWidget());
      addUndo(i18n("&Undo Mark Blank"));

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
    }
  }
}

void KWQTableView::doEditUnmarkBlank()
{
  addUndo(i18n("&Undo Unmark Blank"));
  QString s;

  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *l = static_cast<QLineEdit*>(QApplication::focusWidget());
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

          int fr = s.lastIndexOf(delim_start, cs);
          if (fr > 0)
          {
            s = s.replace(fr, 1, "");
            cs--;
          }
          int ff = s.indexOf(delim_end, cs);
          if (ff > 0)
            s = s.replace(ff, 1, "");

          l->setText(s);
          l->setCursorPosition(cs);
        }
      }
    }
  }
  else
  {
    QModelIndex index;
    QModelIndexList items = selectionModel()->selectedIndexes();

    foreach (index, items)
    {
      s = model()->data(index, Qt::DisplayRole).toString();
      s = s.remove(delim_start);
      s = s.remove(delim_end);
      model()->setData(index, QVariant(s));
    }
  }
}

void KWQTableView::doVocabSort()
{
  DlgSort* dlg;
  dlg = new DlgSort(this);
  dlg->setLanguage(1, model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
  dlg->setLanguage(2, model()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

  if (dlg->exec() == KDialog::Accepted)
  {
    addUndo(i18n("&Undo Sort"));
    model()->sort(dlg->base() ? 0 : 1, dlg->ascending() ? Qt::AscendingOrder : Qt::DescendingOrder);
  }
}

void KWQTableView::doVocabShuffle()
{
  addUndo(i18n("&Undo Shuffle"));
  QRect sel = selection();
  int currentRow = currentIndex().row();
  int currentColumn = currentIndex().column();  
  qobject_cast<KWQTableModel *> (model())->shuffle();
  setCurrentIndex(model()->index(currentRow, currentColumn));
  selectCells(sel);
}

void KWQTableView::doVocabRC()
{
  DlgRC* dlg;
  dlg = new DlgRC(this);
  int rowCount = model()->rowCount();
  dlg->setNumRows(rowCount);
  dlg->setRowHeight(rowHeight(currentIndex().row()));
  dlg->setColWidth(columnWidth(currentIndex().column()));

  if (dlg->exec() == KDialog::Accepted)
  {
    int newRowCount = dlg->numRows();
    if (newRowCount < rowCount)
    {
      addUndo(i18n("&Undo Delete"));
      model()->removeRows(newRowCount, rowCount - newRowCount, QModelIndex());
    }

    if (newRowCount > rowCount)
    {
      addUndo(i18n("&Undo Insert"));
      model()->insertRows(rowCount, newRowCount - rowCount, QModelIndex());
    }

    QModelIndex index;
    QModelIndexList items = selectionModel()->selectedIndexes();

    foreach (index, items)
    {
      setRowHeight(index.row(), dlg->rowHeight());
      setColumnWidth(index.column(), dlg->colWidth());
    }
  }
}

void KWQTableView::slotSpecChar(const QChar & c)
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *l = static_cast<QLineEdit*>(QApplication::focusWidget());
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
  }
  else
  {
    edit(currentIndex());
    if (QApplication::focusWidget())
    {
      QLineEdit *l = static_cast<QLineEdit*>(QApplication::focusWidget());
      l->setText(QString(c));
      l->setCursorPosition(1);
    }
  }
}

void KWQTableView::activateNextCell()
{
  QItemSelectionModel * selModel = selectionModel();
  QModelIndexList indexes = selModel->selectedIndexes();
  QModelIndex currentIndex = selModel->currentIndex();

  int currentRow = currentIndex.row();
  int currentColumn = currentIndex.column();
  int newRow = currentRow;
  int newColumn = currentColumn;

  if (indexes.count() == 1) //one cell selected
  {
    switch(Prefs::enterMove())
    {
     case 0: //down
        if (currentRow == (model()->rowCount() - 1))
        {
          model()->insertRows(model()->rowCount(), 1, QModelIndex());
          newRow++;
        } else
          newRow++;
        break;

     case 1: //right
        if (currentRow == (model()->rowCount() - 1) && currentColumn == 1)
        {
          model()->insertRows(model()->rowCount(), 1, QModelIndex());
          newRow++;
          newColumn = 0;
        } else {
          if (currentColumn == 0)
            newColumn++;
          else {
            newRow++;
            newColumn--;
          }
        }
        break;

      case 2: //no move
        //do nothing
        break;

    }

    QModelIndex newIndex = model()->index(newRow, newColumn);
    setCurrentIndex(newIndex/*, QItemSelectionModel::SelectCurrent*/);
  }
  else //a larger selection, move within it
  {
    QModelIndex topLeft = indexes.first();
    QModelIndex bottomRight = indexes.last();

    int tr = topLeft.row();
    int lc = topLeft.column();
    int br = bottomRight.row();
    int rc = bottomRight.column();

    switch(Prefs::enterMove())
    {
      case 0:
        if (currentRow == br)
        {
          newRow = tr;
          if (currentColumn < rc)
            newColumn = rc;
          else
            newColumn = lc;
        }
        else
          if (currentRow < br)
            newRow++;
        break;
      case 1:
        if (currentColumn == rc)
        {
          newColumn = lc;
          if (currentRow < br)
            newRow++;
          else
            newRow = tr;
        }
        else
          newColumn++;
        break;
      case 2:
        //do nothing
        break;
    }
    QModelIndex newIndex = model()->index(newRow, newColumn);
    selModel->setCurrentIndex(newIndex, QItemSelectionModel::Current);
  }
}

void KWQTableView::addUndo(const QString & caption)
{
  while (m_undoList->count() > 10)
  {
    m_undoList->erase(m_undoList->begin());
  }

  WQUndo* undo = new WQUndo();
  undo->setText(caption);
//  if (getDocument()->font() != NULL)
//    undo->setFont(*(getDocument()->font()));
//  undo->setColWidth0(verticalHeader()->width());
  undo->setColWidth1(columnWidth(0));
  undo->setColWidth2(columnWidth(1));
//  undo->setCurrentRow(currentRow());
//  undo->setCurrentCol(currentColumn());
//  undo->setSelection(selection(0));

  QList<KEduVocExpression> list;
  for(int i = 0; i < model()->rowCount(); i++)
  {
//    KWqlDataItem item(text(i, 0), text(i, 1), rowHeight(i));
//    list.append(*getDocument()->entry(i));
  }

  undo->setList(list);

  m_undoList->prepend(*undo);

  //getDocument()->setModified(true);

  emit undoChange(caption, true);
}

void KWQTableView::keyPressEvent(QKeyEvent * e)
{
  if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
  {
    e->accept();
    activateNextCell();
    return;
  }
  QTableView::keyPressEvent(e);
}

void KWQTableView::slotCheckedAnswer(int i)
{
  if (i == -1)
  {
    QModelIndex current = currentIndex();
    selectionModel()->clear();
    setCurrentIndex(current);
  }
  else
  {
    selectionModel()->clear();
    setCurrentIndex(model()->index(i, 0));
    selectRow(i);
  }
}

void KWQTableView::setModel(KWQTableModel * model)
{
  QTableView::setModel(model);
  setCurrentIndex(model->index(0, 0));
  scrollTo(currentIndex());
  connect(verticalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(verticalHeaderResized(int, int, int)));
  connect(horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(horizontalHeaderResized(int, int, int)));
}

void KWQTableView::closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
  QTableView::closeEditor(editor, hint);
  adjustCurrentRow();
//   if (hint == QAbstractItemDelegate::SubmitModelCache)
//     activateNextCell();
}

void KWQTableView::commitData(QWidget * editor)
{
  QString currentText = model()->data(currentIndex(), Qt::DisplayRole).toString();
  QLineEdit *l = static_cast<QLineEdit*>(editor);
  QString newText = l->text();
  if (newText != currentText)
    addUndo(i18n("&Undo Entry"));

  if (!newText.isEmpty()) {
    if (Prefs::enableBlanks())
      if (!checkForBlank(newText, true))
        KNotification::event("SyntaxError", i18n("There is an error with the Fill-in-the-blank brackets"));
  }

  QTableView::commitData(editor);
}

void KWQTableView::adjustCurrentRow()
{
  // we want to make the row high enough to display content, but
  // if the user already made it even higher we keep that height
  int r = currentIndex().row();
  int rh = rowHeight(r);
  resizeRowToContents(r);
  if (rh > rowHeight(r))
    setRowHeight(r, rh);
}

const QRect KWQTableView::selection()
{
  const QList<QItemSelectionRange> ranges = selectionModel()->selection();
  QRect result;
  result.setTop(ranges.at(0).top());
  result.setLeft(ranges.at(0).left());
  result.setBottom(ranges.at(0).bottom());
  result.setRight(ranges.at(0).right());
  return result;
}

void KWQTableView::selectCells(const QRect & selection)
{
  if (!model()->hasIndex(selection.top(), selection.left(), rootIndex()) ||
      !model()->hasIndex(selection.bottom(), selection.right(), rootIndex()))
    return;

  QModelIndex topLeft = model()->index(selection.top(), selection.left(), rootIndex());
  QModelIndex bottomRight = model()->index(selection.bottom(), selection.right(), rootIndex());

  selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::ClearAndSelect);
}


void KWQTableView::verticalHeaderResized(int , int , int)
{
  kDebug() << "Row resized\n";
}

void KWQTableView::horizontalHeaderResized(int logicalIndex, int oldSize, int newSize)
{
  kDebug() << "Column resized\n";
  model()->setHeaderData(logicalIndex, Qt::Horizontal, QSize(newSize, 25), Qt::SizeHintRole);
}

#include "kwqtableview.moc"
