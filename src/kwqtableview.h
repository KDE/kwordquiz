/***************************************************************************
                          kwordquizview.h  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2006 by Peter Hedlund
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

#ifndef KWORDQUIZVIEW_H
#define KWORDQUIZVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QTableView>
#include <QList>
#include <QKeyEvent>
#include <QEvent>

#include <kprinter.h>

#include "kwqtabledelegate.h"
#include "wqundo.h"

class KEduVocDocument;
class DlgSpecChar;

const char delim_start = '[';
const char delim_end = ']';

/**
@author Peter Hedlund
*/

class KWQTableView : public QTableView
{
Q_OBJECT
public:
  /** Constructor for the main view */
  KWQTableView(QWidget *parent = 0, const char *name=0);
  /** Destructor for the main view */
  ~KWQTableView();

  /** returns a pointer to the document connected to the view instance. Mind that this method requires a KWordQuizApp instance as a parent
    * widget to get to the window document pointer by calling the KWordQuizApp::getDocument() method.
    *
    * @see KWordQuizApp#getDocument
    */
  KEduVocDocument *getDocument() const;


  //Reimplemented from QTable
  void paintCell ( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg );
  void setFont( const QFont &);

  void displayDoc();
  void initSelection();

  /** contains the implementation for printing functionality */
  void print(KPrinter *pPrinter);
  void addUndo(const QString & caption);
  void doEditUndo();
  void doEditCut();
  void doEditCopy();
  void doEditPaste();
  void doEditClear();
  void doEditInsert();
  void doEditDelete();
  void doEditMarkBlank();
  void doEditUnmarkBlank();
  void doVocabSort();
  void doVocabShuffle();
  void doVocabRC();
  void doVocabSpecChar();
  bool checkForBlank(const QString & s, bool blank);
protected:
  const QRect selection();
  void selectCells(const QRect & selection);
  QWidget * beginEdit(int row, int col, bool replace);
  void endEdit ( int row, int col, bool accept, bool replace );
  void activateNextCell();
  //void keyPressEvent( QKeyEvent* );
  //bool eventFilter( QObject*, QEvent* );
public slots:
  void adjustRow(int row);
  void slotSpecChar(const QChar &);
  void slotCheckedAnswer(int );
protected slots:
  void closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint);
  void commitData (QWidget * editor);

private slots:
  void slotDlgSpecCharClosed();


signals:
  void undoChange(const QString & text, bool enabled);

private:
  QString m_currentText;
  DlgSpecChar* dlgSpecChar;
  QWidget * cellEditor;
  KWQTableDelegate * m_delegate;

  /** the list of the undo objects */
  static QList<WQUndo> *m_undoList;

  void doNewPage(QPainter & painter, int res, int type);
  void doEndOfPage(QPainter & painter, int vPos, int pageNum, int res, int type);
};

#endif // KWORDQUIZVIEW_H
