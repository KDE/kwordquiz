/***************************************************************************
                          kwqtableview.cpp  -  description
                             -------------------
    begin          : Wed Jul 24 20:12:30 PDT 2002
    copyright      : (C) 2002-2008 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kwqtableview.h"

#include <QPainter>
#include <QClipboard>
#include <QLineEdit>
#include <QKeyEvent>
#include <QList>
#include <QApplication>
#include <QtGui/QPrinter>
#include <QHeaderView>

#include <KLocale>
#include <KGlobalSettings>
#include <KNotification>
#include <KDebug>

#include "kwqtablemodel.h"
#include "keduvocdocument.h"
#include "kwordquiz.h"
#include "prefs.h"
#include "dlgrc.h"
#include "kwqcommands.h"

KWQTableView::KWQTableView(KUndoStack *undoStack, QWidget *parent) : QTableView(parent), m_undoStack(undoStack)
{
  m_model = 0;

  setSelectionMode(QAbstractItemView::ContiguousSelection);
  setSelectionBehavior(QAbstractItemView::SelectItems);
  setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
  setTabKeyNavigation(true);
  connect(horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(horizontalHeaderResized(int, int, int)));
  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderClicked(int)));
  m_delegate = new KWQTableDelegate(this);
  setItemDelegate(m_delegate);

  setSortingEnabled(true);
  horizontalHeader()->setSortIndicatorShown(false);
  setWordWrap(true);

}

void KWQTableView::print(QPrinter *pPrinter, WQPrintDialogPage::PrintStyle type)
{
  QPainter painter;

  //type 0 Vocabulary list
  //type 1 Vocabulary exam
  //type 2 Flashcards

  //I think working with screen resolution is enough for our purposes
  int res = pPrinter->resolution();
  int pad = 2;
  int marg = res;
  int card_marg = res / 2;
  int card_width = 5 * res;
  int card_height = 3 * res;
  int card_text_marg = res /5;
  int card_line_top = 30;

  if (type == WQPrintDialogPage::Flashcard)
    pPrinter->setOrientation(QPrinter::Landscape);

  painter.begin(pPrinter);

  int pageNum = 1;

  int cw0 = verticalHeader()->width();
  //int cw0 = 50;
  int cw1 = columnWidth(0);
  int cw2 = columnWidth(1);
  int cw3 = 0;



  if (type == WQPrintDialogPage::Exam)
    cw3 = 50;

  int gridWidth = cw0 + cw1 + cw2 + cw3;
  int lPos = marg;
  int tPos = marg + horizontalHeader()->height();
  //int tPos = marg + 20;

  QRect w = painter.window();

  doNewPage(painter, res, type);

  if (type == WQPrintDialogPage::Flashcard)
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
      painter.drawText(card_marg + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter,
          model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
      painter.drawText(card_marg + card_width + card_text_marg, tPos, card_width, card_line_top, Qt::AlignLeft | Qt::AlignVCenter,
          model()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());
      //draw text
      painter.setFont(font());
      painter.drawText(card_marg + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
          card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, model()->data(model()->index(rc, 0)).toString());

      painter.drawText(card_marg + card_width + card_text_marg, tPos + card_line_top, card_width - (2 * card_text_marg),
          card_height - card_line_top, Qt::AlignHCenter | Qt::AlignVCenter, model()->data(model()->index(rc, 1)).toString());

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
      painter.drawText(lPos + cw0 + pad, tPos, cw1, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter, model()->data(model()->index(rc, 0)).toString());

      if (type == WQPrintDialogPage::List)
        painter.drawText(lPos + cw0 + cw1 + pad, tPos, cw2, rowHeight(rc), Qt::AlignLeft | Qt::AlignVCenter,
          model()->data(model()->index(rc, 1)).toString());

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

void KWQTableView::doNewPage(QPainter & painter, int res, WQPrintDialogPage::PrintStyle type)
{
    int cw0 = verticalHeader()->width();
    int cw1 = columnWidth(0);
    int cw2 = columnWidth(1);
    int cw3 = 0;
    int marg = res;
    int card_marg = res / 2;
    int pad = 2;

    if (type == WQPrintDialogPage::Exam)
      cw3 = 50;

    QRect w = painter.window();

    painter.setFont(KGlobalSettings::generalFont());

    if (type == WQPrintDialogPage::Flashcard)
    {
      /// @todo find a more elegant way to retrieve the caption
      painter.drawText(card_marg, card_marg - 20, KGlobal::caption() /*i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName())*/);
      return;
    }
    painter.drawLine(marg, marg, marg + cw0 + cw1 + cw2 + cw3, marg);

    painter.drawText(marg, marg - 20, KGlobal::caption() /*i18n("KWordQuiz - %1").arg(getDocument()->URL().fileName())*/);

    if (type == WQPrintDialogPage::Exam)
    {
      QString score = i18n("Name:_____________________________ Date:__________");
      QRect r = painter.boundingRect(0, 0, 0, 0, Qt::AlignLeft, score);
      painter.drawText(w.width() - r.width() - marg, marg - 20, score);
    }

    painter.drawText(marg, marg, cw0, horizontalHeader()->height(), Qt::AlignRight | Qt::AlignVCenter, "");

    painter.drawText(marg + cw0 + pad, marg, cw1, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
    painter.drawText(marg + cw0 + cw1 + pad, marg, cw2, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, model()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

    if (type == WQPrintDialogPage::Exam)
      painter.drawText(marg + cw0 + cw1 + cw2 + pad, marg, cw3, horizontalHeader()->height(), Qt::AlignLeft | Qt::AlignVCenter, i18n("Score"));

}

void KWQTableView::doEndOfPage(QPainter & painter, int vPos, int pageNum, int res, WQPrintDialogPage::PrintStyle type)
{
    int marg = res;
    painter.setFont(KGlobalSettings::generalFont());
    QRect w = painter.window();
    QRect r = painter.boundingRect(0, 0, 0, 0, Qt::AlignLeft, QString::number(pageNum));
    painter.drawText((w.width()/2) - (r.width()/2), w.height() - marg + 20, QString::number(pageNum));

    if (type == WQPrintDialogPage::Flashcard)
      return;

    int cw0 = verticalHeader()->width();
    int cw1 = columnWidth(0);
    int cw2 = columnWidth(1);
    int cw3 = 0;

    if (type == WQPrintDialogPage::Exam)
      cw3 = 50;

    //Last horizontal line
    painter.drawLine(marg, vPos, marg + cw0 + cw1 + cw2 + cw3, vPos);
   //Four vertical lines
    painter.drawLine(marg, marg, marg, vPos);
    painter.drawLine(marg + cw0, marg, marg + cw0, vPos);
    painter.drawLine(marg + cw0 + cw1, marg, marg + cw0 + cw1, vPos);
    painter.drawLine(marg + cw0 + cw1 + cw2, marg, marg + cw0 + cw1 + cw2, vPos);

    if (type == WQPrintDialogPage::Exam)
      painter.drawLine(marg + cw0 + cw1 + cw2 + cw3, marg, marg + cw0 + cw1 + cw2 + cw3, vPos);

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
    KWQCommandCut *kwqc = new KWQCommandCut(this);
    m_undoStack->push(kwqc);
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
    copyToClipboard(this);
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
    KWQCommandPaste *kwqc = new KWQCommandPaste(this);
    m_undoStack->push(kwqc);
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
    KWQCommandClear *kwqc = new KWQCommandClear(this);
    m_undoStack->push(kwqc);
  }
}

void KWQTableView::doEditInsert()
{
  KWQCommandInsert *kwqc = new KWQCommandInsert(this);
  m_undoStack->push(kwqc);
}

void KWQTableView::doEditDelete()
{
  KWQCommandDelete *kwqc = new KWQCommandDelete(this);
  m_undoStack->push(kwqc);
}


void KWQTableView::doEditMarkBlank()
{
  if (state() == QAbstractItemView::EditingState)
  {
    if (QApplication::focusWidget())
    {
      QLineEdit *l = static_cast<QLineEdit*>(QApplication::focusWidget());
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
    KWQCommandUnmarkBlank *kwqc = new KWQCommandUnmarkBlank(this);
    m_undoStack->push(kwqc);
  }
}


void KWQTableView::doVocabRC()
{
  DlgRC* dlg;
  dlg = new DlgRC(this);
  int rowCount = model()->rowCount();
  dlg->setNumRows(rowCount);
  dlg->setRowHeight(rowHeight(currentIndex().row()));
  dlg->setColWidth(columnWidth(currentIndex().column()));

  if (dlg->exec() == KDialog::Accepted) {
    KWQCommandFormat *kwqc = new KWQCommandFormat(this, dlg->numRows(), dlg->rowHeight(), dlg->colWidth());
    m_undoStack->push(kwqc);
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

void KWQTableView::nextCell()
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
    selModel->clear();
    selModel->setCurrentIndex(newIndex, QItemSelectionModel::SelectCurrent);
  }
  else if (indexes.count() > 1) //a larger selection, move within it
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

void KWQTableView::keyPressEvent(QKeyEvent * e)
{
  if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
  {
    e->accept();
    if (state() != QAbstractItemView::EditingState)
      nextCell();
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

void KWQTableView::setModel(KWQSortFilterModel * model)
{
  QTableView::setModel(model);
  m_model = model;
  setCurrentIndex(model->index(0, 0));
  scrollTo(currentIndex());
  connect(verticalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(verticalHeaderResized(int, int, int)));
  connect(horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(horizontalHeaderResized(int, int, int)));
}

void KWQTableView::closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
  QTableView::closeEditor(editor, hint);
  if (hint == QAbstractItemDelegate::SubmitModelCache) {
    adjustRow(currentIndex().row());
    nextCell();
  }
}

void KWQTableView::commitData(QWidget * editor)
{
  QString currentText = model()->data(currentIndex(), Qt::DisplayRole).toString();
  QLineEdit *l = static_cast<QLineEdit*>(editor);
  QString newText = l->text();
  if (newText != currentText) {
    KWQCommandEntry *kwqc = new KWQCommandEntry(this, currentText, newText);
    m_undoStack->push(kwqc);
  }
  if (!newText.isEmpty()) {
    if (Prefs::enableBlanks())
      if (!m_model->sourceModel()->checkBlanksSyntax(newText) /*checkForBlank(newText, true)*/)
        KNotification::event("SyntaxError", i18n("There is an error with the Fill-in-the-blank brackets"));
  }
  QTableView::commitData(editor);
}

void KWQTableView::adjustRow(int row)
{
  // we want to make the row high enough to display content, but
  // if the user already made it even higher we keep that height
  //int r = currentIndex().row();
  int rh = rowHeight(row);
  resizeRowToContents(row);
  if (rh > rowHeight(row))
    setRowHeight(row, rh);
}


void KWQTableView::verticalHeaderResized(int , int , int)
{
  //kDebug() << "Row resized\n";
}

void KWQTableView::horizontalHeaderResized(int logicalIndex, int oldSize, int newSize)
{
  Q_UNUSED(oldSize);
  model()->setHeaderData(logicalIndex, Qt::Horizontal, QSize(newSize, 25), Qt::SizeHintRole);
}

void KWQTableView::slotModelReset()
{
  setColumnWidth(0, qvariant_cast<QSize>(model()->headerData(0, Qt::Horizontal, Qt::SizeHintRole)).width());
  setColumnWidth(1, qvariant_cast<QSize>(model()->headerData(1, Qt::Horizontal, Qt::SizeHintRole)).width());
}


void KWQTableView::slotSortByColumn(int column)
{
  QTableView::sortByColumn(column);
}


void KWQTableView::slotHeaderClicked(int column)
{
  KWQCommandSort *kwqc = new KWQCommandSort(this, column);
  m_undoStack->push(kwqc);
}


void KWQTableView::doVocabShuffle()
{
  KWQCommandShuffle *kwqc = new KWQCommandShuffle(this, 0);
  m_undoStack->push(kwqc);
}

#include "kwqtableview.moc"
