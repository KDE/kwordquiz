/***************************************************************************
                              kwqtableview.cpp
                             -------------------
    begin          : Wed Jul 24 20:12:30 PDT 2002
    copyright      : (C) 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>

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

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QDBusInterface>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QImageReader>
#include <QLineEdit>
#include <QList>
#include <QPointer>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextCursor>
#include <QTextLayout>
#include <QTextTable>

#include <KLocalizedString>
#include <KNotification>
#include <KConfigGroup>

#include "kwqtablemodel.h"
#include "keduvocdocument.h"
#include "kwordquiz.h"
#include "prefs.h"
#include "kwqcommands.h"
#include "kwqcleardialog.h"
#include "kwordquiz_version.h"

//krazy:excludeall=qclasses

KWQTableView::KWQTableView(QUndoStack *undoStack, QWidget *parent)
    : QTableView(parent)
    , m_undoStack(undoStack)
{
  m_model = 0;

  setSelectionMode(QAbstractItemView::ContiguousSelection);
  setSelectionBehavior(QAbstractItemView::SelectItems);
  setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
  setTabKeyNavigation(true);
  connect(horizontalHeader(), &QHeaderView::sectionResized, this, &KWQTableView::horizontalHeaderResized);
  connect(horizontalHeader(), &QHeaderView::sectionClicked, this, &KWQTableView::slotHeaderClicked);
  m_delegate = new KWQTableDelegate(this);
  setItemDelegate(m_delegate);

  setSortingEnabled(true);
  horizontalHeader()->setSortIndicatorShown(false);
  setWordWrap(true);

}

void KWQTableView::doPrint()
{
  QPrinter printer;

  WQPrintDialogPage * p = new WQPrintDialogPage(this);
  p->setPrintStyle(Prefs::printStyle());
  QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer, this);
  printDialog->setOptionTabs(QList<QWidget*>() << p);
  if (printDialog->exec() == QDialog::Accepted) {
    Prefs::setPrintStyle(p->printStyle());
    QTextDocument td;
    createPages(&printer, &td, true);
  }
  delete printDialog;
}

void KWQTableView::doPrintPreview()
{
  QPrinter printer;
  QPrintPreviewDialog preview(&printer);
  
  connect(&preview, &QPrintPreviewDialog::paintRequested, this, &KWQTableView::slotPrintPreviewRequested);
  
  preview.exec();
}

void KWQTableView::slotPrintPreviewRequested(QPrinter* printer)
{
  QTextDocument td;
  KWQTableView::createPages(printer, &td, true);
}

bool KWQTableView::doHtmlExport(const QUrl &url)
{
  bool success = false;

  QUrl tmp(url);
  QFile data(tmp.path());
  if (data.open(QFile::WriteOnly)) {
    QPrinter printer;
    QTextDocument td;
    td.setMetaInformation(QTextDocument::DocumentTitle, model()->sourceModel()->document()->title());
    QTextStream out(&data);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("utf-8");
#endif
    createPages(&printer, &td, false);
    out << td.toHtml("utf-8");
    data.close();
    success = true;
  }
  return success;
}

void KWQTableView::createPages(QPrinter *printer, QTextDocument *textDoc, bool sendToPrinter)
{
  printer->setFullPage(true);
  int myDpi = printer->logicalDpiY();

  if (Prefs::printStyle() == Prefs::EnumPrintStyle::Flashcard) {
    printer->setOrientation(QPrinter::Landscape);

    int cardWidth = qRound(5 * qreal(myDpi));
    int cardHeight = qRound(3 * qreal(myDpi));

    QTextTable *table = textDoc->rootFrame()->lastCursorPosition().insertTable(model()->rowCount(), 2);

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
    headerCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

    QTextBlockFormat cellFormat;
    cellFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat cellCharFormat;
    cellCharFormat.setFont(Prefs::editorFont());

    QTextFrameFormat cardFormat;
    cardFormat.setBorder(1);
    cardFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    cardFormat.setBorderBrush(QBrush(Qt::black));
    cardFormat.setWidth(QTextLength(QTextLength::FixedLength, cardWidth));
    cardFormat.setHeight(QTextLength(QTextLength::FixedLength, cardHeight));
    cardFormat.setPadding(qRound(0.25 * myDpi));

    QTextFrameFormat lineFormat;
    lineFormat.setBorder(1);
    lineFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    lineFormat.setBorderBrush(QBrush(Qt::black));
    lineFormat.setWidth(QTextLength(QTextLength::FixedLength, qRound(4.5 * myDpi)));
    lineFormat.setHeight(1.1); //1 is drawn as a box whereas this is drawn as a line. Strange...
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
    textDoc->rootFrame()->lastCursorPosition().insertText(QStringLiteral("kwordquiz %1").arg(KWORDQUIZ_VERSION_STRING));

    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam)
      textDoc->rootFrame()->lastCursorPosition().insertText(' ' + i18n("Name:_____________________________ Date:__________"));

    QTextTable* table;
    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam)
      table = textDoc->rootFrame()->lastCursorPosition().insertTable(model()->rowCount() + 1, model()->columnCount() + 2);
    else
      table = textDoc->rootFrame()->lastCursorPosition().insertTable(model()->rowCount() + 1, model()->columnCount() + 1);

    QTextTableFormat tableFormat = table->format();
    tableFormat.setHeaderRowCount(1);
    tableFormat.setBorder(1);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorderBrush(QBrush(Qt::black));
    tableFormat.setCellPadding(2);

    QVector<QTextLength> constraints;
    constraints.append(QTextLength(QTextLength::FixedLength, verticalHeader()->width()));
    constraints.append(QTextLength(QTextLength::FixedLength, columnWidth(0)));
    constraints.append(QTextLength(QTextLength::FixedLength, columnWidth(1)));
    if (Prefs::printStyle() == Prefs::EnumPrintStyle::Exam)
        constraints.append(QTextLength(QTextLength::FixedLength, 50));
    tableFormat.setColumnWidthConstraints(constraints);

    table->setFormat(tableFormat);

    QTextBlockFormat headerFormat;
    headerFormat.setAlignment(Qt::AlignHCenter);

    QTextCharFormat headerCharFormat;
    headerCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

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

  if (sendToPrinter)
    textDoc->print(printer);
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
    if (selectionHasMoreThanText()) {
      QPointer<KWQClearDialog> clearDialog = new KWQClearDialog(this);
      if (clearDialog->exec() == QDialog::Rejected) {
        delete clearDialog;
        return;
      }
      delete clearDialog;
    }
    else
    {
      Prefs::setClearAll(false);
      Prefs::setClearText(true);
      Prefs::setClearImageLink(false);
      Prefs::setClearSoundLink(false);
    }
    KWQCommandClear *kwqc = new KWQCommandClear(this);
    m_undoStack->push(kwqc);
  }
}

void KWQTableView::doEditInsert()
{
  if (selectionModel()->currentIndex().isValid()) {
    KWQCommandInsert *kwqc = new KWQCommandInsert(this);
    m_undoStack->push(kwqc);
  }
}

void KWQTableView::doEditDelete()
{
  if (selectionModel()->currentIndex().isValid()) {
    KWQCommandDelete *kwqc = new KWQCommandDelete(this);
    m_undoStack->push(kwqc);
  }
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
            s = s.replace(fr, 1, QLatin1String(""));
            cs--;
          }
          int ff = s.indexOf(delim_end, cs);
          if (ff > 0)
            s = s.replace(ff, 1, QLatin1String(""));

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

void KWQTableView::setFilterModel(KWQSortFilterModel *model)
{
  QTableView::setModel(model);
  m_model = model;
  setCurrentIndex(model->index(0, 0));
  scrollTo(currentIndex());
  connect(verticalHeader(), &QHeaderView::sectionResized, this, &KWQTableView::verticalHeaderResized);
  connect(horizontalHeader(), &QHeaderView::sectionResized, this, &KWQTableView::horizontalHeaderResized);
  connect(m_model, &QAbstractItemModel::headerDataChanged, this, &KWQTableView::horizontalHeaderDataChanged);
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
        KNotification::event(QStringLiteral("SyntaxError"), i18n("There is an error with the Fill-in-the-blank brackets"));
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
  //qDebug() << "Row resized\n";
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
  QTableView::sortByColumn(column, Qt::AscendingOrder);
}


void KWQTableView::slotHeaderClicked(int column)
{
  KWQCommandSort *kwqc = new KWQCommandSort(this, column);
  m_undoStack->push(kwqc);
}


void KWQTableView::doVocabImage()
{
  QUrl currentUrl(model()->data(currentIndex(), KWQTableModel::ImageRole).toString());
  QStringList imageFormats;
  for(const QByteArray &b: QImageReader::supportedMimeTypes()) {
      if (! b.isEmpty()) imageFormats.append(QString(b));
  }

  QPointer<QFileDialog> imageOpenDialog = new QFileDialog(this);
  imageOpenDialog->setWindowTitle(i18nc("@title:window", "Select Image"));
  imageOpenDialog->setDirectory(currentUrl.path());
  imageOpenDialog->setMimeTypeFilters(imageFormats);
  imageOpenDialog->setAcceptMode(QFileDialog::AcceptOpen);
  imageOpenDialog->setFileMode(QFileDialog::ExistingFile);
  if (imageOpenDialog->exec() == QDialog::Accepted) {
      QUrl imageUrl = imageOpenDialog->selectedUrls().first();
      if (!imageUrl.isEmpty()) {
          KWQCommandImage *kwqc = new KWQCommandImage(this, imageUrl);
          m_undoStack->push(kwqc);
      }
  }
  delete imageOpenDialog;
}


void KWQTableView::doVocabSound()
{
  QUrl currentUrl(model()->data(currentIndex(), KWQTableModel::SoundRole).toString());

  QUrl soundUrl = QFileDialog::getOpenFileUrl(this, i18nc("@title:window", "Select Sound"), currentUrl, i18n("All Files (*)"));
  if (!soundUrl.isEmpty()) {
    KWQCommandSound *kwqc = new KWQCommandSound(this, soundUrl);
    m_undoStack->push(kwqc);
  }
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
        QDBusInterface kxkb(QStringLiteral("org.kde.keyboard"), QStringLiteral("/Layouts"), QStringLiteral("org.kde.KeyboardLayouts"));
        if (kxkb.isValid())
            kxkb.call(QStringLiteral("setLayout"), layout);
    }
}

bool KWQTableView::selectionHasMoreThanText()
{
  bool result = false;
  QModelIndexList indexes = selectionModel()->selectedIndexes();
  foreach (const QModelIndex &idx, indexes) {
     result = (!model()->data(idx, KWQTableModel::SoundRole).isNull() ||
               !model()->data(idx, KWQTableModel::ImageRole).isNull());
     if (result)
       break;
  }

  return result;
}
