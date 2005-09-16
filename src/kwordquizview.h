/***************************************************************************
                          kwordquizview.h  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002 by Peter Hedlund
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

// include files for Qt
#include <qtable.h>

#include <kprinter.h>

#include "wqlreader.h"
#include "wqundo.h"

#include <kprinter.h>
#include "wqundo.h"
//include <wqviewbase.h>

class KWordQuizDoc;
class DlgSpecChar;

/**
@author Peter Hedlund
*/

class KWQTableItem :public QTableItem
{
public:
  KWQTableItem( QTable* table, EditType et, const QString & text );
  virtual int alignment() const;
};

class KWordQuizView : public QTable
{
  Q_OBJECT
  public:
    /** Constructor for the main view */
    KWordQuizView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~KWordQuizView();

    /** returns a pointer to the document connected to the view instance. Mind that this method requires a KWordQuizApp instance as a parent
     * widget to get to the window document pointer by calling the KWordQuizApp::getDocument() method.
     *
     * @see KWordQuizApp#getDocument
     */
    KWordQuizDoc *getDocument() const;

    //Reimplemented from QTable
    void paintCell ( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg );
    void setFont( const QFont &);
    void setText( int row, int col, const QString &text );

    /** contains the implementation for printing functionality */
    void print(KPrinter *pPrinter);
    bool gridIsEmpty();
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
    bool checkSyntax(bool all, bool blanks);
    void saveCurrentSelection(bool clear);
  protected:
    QWidget * beginEdit(int row, int col, bool replace);
    void endEdit ( int row, int col, bool accept, bool replace );
    void activateNextCell();
    void keyPressEvent( QKeyEvent* );
    bool eventFilter( QObject*, QEvent* );
  public slots:
    void adjustRow(int row);
    void slotSpecChar(const QChar &);
    void slotCheckedAnswer(int );

  private slots:
    void slotDlgSpecCharClosed();

  signals:
    void undoChange(const QString & text, bool enabled);

  private:
    int m_currentRow;
    int m_currentCol;
    QString m_currentText;
    QTableSelection m_currentSel;
    DlgSpecChar* dlgSpecChar;
    QWidget * cellEditor;
    /** the list of the undo objects */
    static QValueList<WQUndo> *m_undoList;

    void doNewPage(QPainter & painter, int res, int type);
    void doEndOfPage(QPainter & painter, int vPos, int pageNum, int res, int type);
    bool checkForBlank(const QString & s, bool blank);
};

#endif // KWORDQUIZVIEW_H
