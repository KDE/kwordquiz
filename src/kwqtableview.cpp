/***************************************************************************
                          kwqtableview.cpp  -  description
                             -------------------
    begin          : Wed Jul 24 20:12:30 PDT 2002
    copyright      : (C) 2002-2009 Peter Hedlund <peter.hedlund@kdemail.net>

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
#include <QtGui/QPrintDialog>
#include <QtGui/QTextDocument>
#include <QtGui/QTextCursor>
#include <QtGui/QTextTable>
#include <QtGui/QTextLayout>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QHeaderView>
#include <QtDBus/QDBusInterface>

#include <KLocale>
#include <KGlobalSettings>
#include <KNotification>
#include <KDebug>
#include <kdeprintdialog.h>

#include "kwqtablemodel.h"
#include "keduvocdocument.h"
#include "kwordquiz.h"
#include "prefs.h"
#include "kwqcommands.h"

//krazy:excludeall=qclasses

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

void KWQTableView::print()
{
  WQPrintDialogPage * p = new WQPrintDialogPage(this);
  p->setPrintStyle(Prefs::printStyle());
  QPrinter printer(QPrinter::ScreenResolution);
  QPrintDialog *printDialog = KdePrint::createPrintDialog(&printer, QList<QWidget*>() << p, this);
  printer.setFullPage(true);
  if (printDialog->exec() != QDialog::Accepted)
  {
    delete printDialog;
    return;
  }
  Prefs::setPrintStyle(p->printStyle());

  QTextDocument td;

  if (Prefs::printStyle() == Prefs::EnumPrintStyle::Flashcard) {
    printer.setOrientation(QPrinter::Landscape);

    int cardWidth = qRound(5 * printer.logicalDpiY());
    int cardHeight = qRound(3 * printer.logicalDpiY());

    QTextTable *table = td.rootFrame()->lastCursorPosition().insertTable(model()->rowCount(), 2);

    QTextTableFormat tableFormat = table->format();
    tableFormat.setHeaderRowCount(0);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(0);

    QVector<QTextLength> constraints;
    constraints.append(QTextLength(QTextLength::FixedLength, cardWidth));
    constraints.append(QTextLength(QTextLength::FixedLength, cardWidth));

    tableFormat.setColumnWidthConstraints(constraints);
    table->setFormat(tableFormat);

    QTextBlockFormat headerFormat;
    headerFormat.setAlignment(Qt::AlignLeft);

    QTextCharFormat headerCharFormat;
    headerCharFormat.setFont(KGlobalSettings::generalFont());

    QTextBlockFormat cellFormat;
    cellFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat cellCharFormat;
    cellCharFormat.setFont(Prefs::editorFont());

    QTextFrameFormat cardFormat;
    cardFormat.setBorder(logicalDpiY()/72);
    cardFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    cardFormat.setBorderBrush(QBrush(Qt::black));
    cardFormat.setWidth(QTextLength(QTextLength::FixedLength, cardWidth));
    cardFormat.setHeight(QTextLength(QTextLength::FixedLength, cardHeight));
    cardFormat.setPadding(qRound(0.25 * printer.logicalDpiY()));

    QTextFrameFormat lineFormat;
    lineFormat.setBorder(logicalDpiY()/72);
    lineFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    lineFormat.setBorderBrush(QBrush(Qt::black));
    lineFormat.setWidth(QTextLength(QTextLength::FixedLength, qRound(4.5 * printer.logicalDpiY())));
    lineFormat.setHeight(logicalDpiY()/38);
    lineFormat.setPadding(0);

    QTextFrame *card;
    for (int i = 0; i < model()->rowCount(); i++) {
      for (int j = 0; j < model()->columnCount(); j++) {
        cardFormat.setPosition(QTextFrameFormat::FloatLeft);
        card = table->cellAt(i, j).firstCursorPosition().insertFrame(cardFormat);
        card->lastCursorPosition().insertText(model()->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString(), headerCharFormat);
        card->lastCursorPosition().insertFrame(lineFormat);
        card->lastCursorPosition().insertBlock();
        card->lastCursorPosition().insertBlock();
        card->lastCursorPosition().insertBlock(cellFormat, cellCharFormat);
        card->lastCursorPosition().insertText(model()->data(model()->index(i, j)).toString(), cellCharFormat);
      }
    }
  }
  else
  {
    td.rootFrame()->lastCursorPosition().insertText(KGlobal::caption());

    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam)
      td.rootFrame()->lastCursorPosition().insertText(' ' + i18n("Name:_____________________________ Date:__________"));

    QTextTable* table;
    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam)
      table = td.rootFrame()->lastCursorPosition().insertTable(model()->rowCount() + 1, model()->columnCount() + 2);
    else
      table = td.rootFrame()->lastCursorPosition().insertTable(model()->rowCount() + 1, model()->columnCount() + 1);

    QTextTableFormat tableFormat = table->format();
    tableFormat.setHeaderRowCount(1);
    tableFormat.setBorder(logicalDpiY()/72 /*0.5*/);
    kDebug() << logicalDpiY();
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorderBrush(QBrush(Qt::black));
    tableFormat.setCellPadding(2);

    QVector<QTextLength> constraints;
    constraints.append(QTextLength(QTextLength::FixedLength, verticalHeader()->width() * printer.logicalDpiY()/logicalDpiY()));
    constraints.append(QTextLength(QTextLength::FixedLength, columnWidth(0) * printer.logicalDpiY()/logicalDpiY()));
    constraints.append(QTextLength(QTextLength::FixedLength, columnWidth(1) * printer.logicalDpiY()/logicalDpiY()));
    constraints.append(QTextLength(QTextLength::FixedLength, 50 * printer.logicalDpiY()/logicalDpiY()));
    tableFormat.setColumnWidthConstraints(constraints);

    table->setFormat(tableFormat);

    QTextBlockFormat headerFormat;
    headerFormat.setAlignment(Qt::AlignHCenter);

    QTextCharFormat headerCharFormat;
    headerCharFormat.setFont(KGlobalSettings::generalFont());

    QTextCursor cellCursor;
    cellCursor = table->cellAt(0, 1).firstCursorPosition();
    cellCursor.mergeBlockFormat(headerFormat);
    cellCursor.mergeCharFormat(headerCharFormat);
    cellCursor.insertText(model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());

    cellCursor = table->cellAt(0, 2).firstCursorPosition();
    cellCursor.mergeBlockFormat(headerFormat);
    cellCursor.mergeCharFormat(headerCharFormat);
    cellCursor.insertText(model()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam) {
      cellCursor = table->cellAt(0, 3).firstCursorPosition();
      cellCursor.mergeBlockFormat(headerFormat);
      cellCursor.mergeCharFormat(headerCharFormat);
      cellCursor.insertText(i18n("Score"));
    }

    headerCharFormat = cellCursor.charFormat();
    QTextCharFormat cellCharFormat = cellCursor.charFormat();
    cellCharFormat.setFont(Prefs::editorFont());

    for (int i = 0; i < model()->rowCount(); i++) {
      table->cellAt(i + 1, 0).firstCursorPosition().insertText(model()->headerData(i, Qt::Vertical, Qt::DisplayRole).toString(), headerCharFormat);
      table->cellAt(i + 1, 1).firstCursorPosition().insertText(model()->data(model()->index(i, 0)).toString(), cellCharFormat);
      if (Prefs::printStyle() == Prefs::EnumPrintStyle::List)
        table->cellAt(i + 1, 2).firstCursorPosition().insertText(model()->data(model()->index(i, 1)).toString(), cellCharFormat);
    }
  }
  td.print(&printer);
  delete printDialog;
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

  if (newColumn != currentColumn)
    updateKeyboardLayout();
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
  connect(m_model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this, SLOT(horizontalHeaderDataChanged(Qt::Orientation, int, int)));
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

void KWQTableView::horizontalHeaderDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast)
{
    Q_UNUSED(orientation);
    Q_UNUSED(logicalFirst);
    Q_UNUSED(logicalLast);
    slotModelReset();
    updateKeyboardLayout();
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
  setFocus();
  KWQCommandShuffle *kwqc = new KWQCommandShuffle(this, 0);
  m_undoStack->push(kwqc);
}

void KWQTableView::updateKeyboardLayout()
{
    QString layout;
    layout.clear();
    layout = model()->headerData(currentIndex().column(), Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();

    if (!layout.isEmpty()) {
        QDBusInterface kxkb("org.kde.kxkb", "/kxkb", "org.kde.KXKB");
        if (kxkb.isValid())
            kxkb.call("setLayout", layout);
    }
}

#include "kwqtableview.moc"
