/***************************************************************************
                               kwqtableview.h
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2010 by Peter Hedlund
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

#include <QKeyEvent>
#include <QTableView>
#include <QTextDocument>
#include <QUndoStack>

#include "kwqsortfiltermodel.h"
#include "kwqtabledelegate.h"
#include "wqprintdialogpage.h"

/**
@author Peter Hedlund
*/

class QPrinter;

class KWQTableView : public QTableView
{
Q_OBJECT
public:
  /** Constructor for the main view */
  explicit KWQTableView(QUndoStack *undoStack, QWidget *parent = nullptr);

  void setFilterModel(KWQSortFilterModel * model);
  KWQSortFilterModel * model() {return m_model;};

  /** contains the implementation for printing functionality */
  void doPrint();
  void doPrintPreview();
  bool doHtmlExport(const QUrl &);

  void doEditCut();
  void doEditCopy();
  void doEditPaste();
  void doEditClear();
  void doEditInsert();
  void doEditDelete();
  void doEditMarkBlank();
  void doEditUnmarkBlank();
  void doVocabImage();
  void doVocabSound();
  void doVocabShuffle();

protected:
  void nextCell();
  void keyPressEvent(QKeyEvent*) override;
  void updateKeyboardLayout();

public slots:
  void adjustRow(int );
  void slotSpecChar(const QChar &);
  void slotCheckedAnswer(int );
  void slotModelReset();

  void slotHeaderClicked(int);
  void slotSortByColumn(int);

protected slots:
  void closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint) override;
  void commitData (QWidget * editor) override;
  void verticalHeaderResized(int, int, int);
  void horizontalHeaderResized(int, int, int);
  void horizontalHeaderDataChanged(Qt::Orientation, int, int);
  void slotPrintPreviewRequested(QPrinter* printer);

private:
  // Disable from public calling; use setFilterModel() instead.
  //void setModel(QAbstractItemModel *model);

  void createPages(QPrinter *, QTextDocument *, bool);
  bool selectionHasMoreThanText();

  QString m_currentText;

  KWQTableDelegate * m_delegate;
  KWQSortFilterModel *m_model;
  QUndoStack *m_undoStack;
};

#endif // KWORDQUIZVIEW_H
