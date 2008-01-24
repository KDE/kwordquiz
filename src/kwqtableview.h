/***************************************************************************
                          kwqtableview.h  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2008 by Peter Hedlund
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

#ifndef KWQTABLEVIEW_H
#define KWQTABLEVIEW_H

#include <QTableView>
#include <QList>
#include <QKeyEvent>

class QPrinter;

#include <KUndoStack>

#include "kwqsortfiltermodel.h"
#include "kwqtabledelegate.h"
#include "wqprintdialogpage.h"

/**
@author Peter Hedlund
*/

class KWQTableView : public QTableView
{
Q_OBJECT
public:
  /** Constructor for the main view */
  KWQTableView(KUndoStack *undoStack, QWidget *parent = 0);

  void setModel(KWQSortFilterModel * model);
  KWQSortFilterModel * model() {return m_model;};

  /** contains the implementation for printing functionality */
  void print(QPrinter *pPrinter, WQPrintDialogPage::PrintStyle type);

  void doEditCut();
  void doEditCopy();
  void doEditPaste();
  void doEditClear();
  void doEditInsert();
  void doEditDelete();
  void doEditMarkBlank();
  void doEditUnmarkBlank();
  void doVocabShuffle();
  void doVocabRC();

protected:
  QRect selection() const;
  QPoint currentCell() const;
  void selectCells(const QRect & selection);
  void setCurrentCell(const QPoint & currentCell);
  void nextCell();
  void keyPressEvent(QKeyEvent*);

public slots:
  void adjustRow(int );
  void slotSpecChar(const QChar &);
  void slotCheckedAnswer(int );
  void slotModelReset();

  void slotHeaderClicked(int);
  void slotSortByColumn(int);

protected slots:
  void closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint);
  void commitData (QWidget * editor);
  void verticalHeaderResized(int, int, int);
  void horizontalHeaderResized(int, int, int);

private:
  QString m_currentText;

  QWidget * cellEditor;
  KWQTableDelegate * m_delegate;
  KWQSortFilterModel *m_model;
  KUndoStack *m_undoStack;

  void doNewPage(QPainter & painter, int res, WQPrintDialogPage::PrintStyle type);
  void doEndOfPage(QPainter & painter, int vPos, int pageNum, int res, WQPrintDialogPage::PrintStyle type);
};

#endif // KWORDQUIZVIEW_H
