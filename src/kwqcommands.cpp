/***************************************************************************
                          kwqcommands.cpp  -  description
                             -------------------
    begin          : Fri Jan 18 10:37:00 PST 2008
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

#include "kwqcommands.h"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QUndoCommand>

#include "kwqtablemodel.h"
#include "prefs.h"

QRect selection(QTableView * view)
{
  const QList<QItemSelectionRange> ranges = view->selectionModel()->selection();
  QRect result;
  if (ranges.count() > 0)
  {
    result.setTop(ranges.at(0).top());
    result.setLeft(ranges.at(0).left());
    result.setBottom(ranges.at(0).bottom());
    result.setRight(ranges.at(0).right());
  }
  else
  {
    result.setTop(0);
    result.setLeft(0);
    result.setBottom(0);
    result.setRight(0);
  }
  return result;
}


void copyToClipboard(QTableView * view)
{
    QRect sel = selection(view);
    QString s;
    QString rs;
    for (int r = sel.top(); r <= sel.bottom(); ++r)
    {
      for (int c = sel.left(); c <= sel.right(); ++c)
      {
        QVariant v =  view->model()->data(view->model()->index(r, c, QModelIndex()), Qt::DisplayRole);
        s.append(v.toString());
        s.append('\t');
      }
      s.append('\n');
    }
    QApplication::clipboard()->setText(s.trimmed());
}


KWQUndoCommand::KWQUndoCommand(KWQTableView * view) : QUndoCommand()
{
  m_view = view;
  m_currentIndex = m_view->selectionModel()->currentIndex();  
  m_selectedIndexes = m_view->selectionModel()->selectedIndexes();
  foreach (const QModelIndex &index, m_selectedIndexes)
  {
    IndexAndData id;
    id.index = index;
    id.data = m_view->model()->data(index, Qt::DisplayRole);
    m_indexAndData.append(id);
  }
}


void KWQUndoCommand::undo()
{
  m_view->selectionModel()->clear();
  foreach (const IndexAndData &id, m_indexAndData)
  {
    m_view->model()->setData(id.index, id.data, Qt::EditRole);
    m_view->selectionModel()->select(id.index, QItemSelectionModel::Select);
  }
  m_view->selectionModel()->setCurrentIndex(m_currentIndex, QItemSelectionModel::Current);
}


KWQCommandEdit::KWQCommandEdit(KWQTableView * view, EditCommand cmd) : KWQUndoCommand(view)
{
  m_command = cmd;
  switch (m_command) {
    case EditCut:
      setText(i18n("Cut"));
      break;
    case EditClear:
      setText(i18n("Clear"));
      break;
  }
}


void KWQCommandEdit::redo()
{
  if (m_command == EditCut)
    copyToClipboard(view());

  view()->selectionModel()->clear();
  foreach (const QModelIndex &index, oldSelectedIndexes())
  {
    view()->model()->setData(index, QVariant());
    view()->selectionModel()->select(index, QItemSelectionModel::Select);

  }
  view()->selectionModel()->setCurrentIndex(oldCurrentIndex(), QItemSelectionModel::Current);
};

KWQCommandPaste::KWQCommandPaste(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18n("Paste"));
  m_rowCount = view->model()->rowCount();
}

void KWQCommandPaste::undo()
{
  foreach (const IndexAndData &id, m_pasteIndexAndData)
    view()->model()->setData(id.index, id.data, Qt::EditRole);

  while (view()->model()->rowCount(QModelIndex()) > m_rowCount) {
      view()->model()->removeRows(view()->model()->rowCount() - 1, 1, QModelIndex());
    }

  KWQUndoCommand::undo();
}

void KWQCommandPaste::redo()
{
  int tr;
  int br;
  int lc;
  int rc;
  int i = 0;
  int ar = 0;
  int ac = 0;

  QString s = QApplication::clipboard()->text();
  QStringList sl;
  sl = s.split("\n", QString::SkipEmptyParts);

  QStringList sr;

  if (view()->selectionModel()->selectedIndexes().count() == 1) //one cell selected
  {
    //Do we need to add entries (rows)?
    ar = tr = view()->selectionModel()->currentIndex().row();
    lc = view()->selectionModel()->currentIndex().column();

    while (sl.count() + tr > view()->model()->rowCount(QModelIndex())) {
      view()->model()->insertRows(view()->model()->rowCount(), 1, QModelIndex());
    }

    br = tr + sl.count() - 1;

    if (lc == 0) //left col?
      if (sl[0].indexOf("\t") > -1)
        rc = 1; //expand to second column;

    while (i < sl.count() && br <= view()->model()->rowCount(QModelIndex()))
    {
      ac = lc;

      sr = sl.at(i).split("\t", QString::SkipEmptyParts);
      int c = 0;
      while (ac <= rc)
      {
        IndexAndData id;
        id.index = view()->model()->index(ar, ac);
        id.data = view()->model()->data(id.index, Qt::DisplayRole);
        m_pasteIndexAndData.append(id);

        view()->model()->setData(id.index, QVariant(sr[c]), Qt::EditRole);
        view()->selectionModel()->select(id.index, QItemSelectionModel::Select);
        ac++;
        c++;
      }
      ar++;
      i++;
    }
  }
  else
  {
    if (view()->selectionModel()->columnIntersectsSelection(0, QModelIndex())) {
      lc = 0;
      if (view()->selectionModel()->columnIntersectsSelection(1, QModelIndex()))
        rc = 1;
      else
        rc =0;
    }
    else
      lc = rc = 1;

    while (i < sl.count() && ar <= br )
    {
      ac = lc;

      sr = sl.at(i).split("\t", QString::SkipEmptyParts);
      int c = 0;
      while (ac <= rc)
      {
        if (view()->selectionModel()->isSelected(view()->model()->index(ar, ac)))
          view()->model()->setData(view()->model()->index(ar, ac), QVariant(sr[c]), Qt::EditRole);
        ac++;
        c++;
      }
      ar++;
      i++;
    }
  }
}


KWQCommandSort::KWQCommandSort(QTableView * view, int column)
{
  m_view = view;
  m_column = column;
  setText(i18n("Sort"));
}


void KWQCommandSort::undo()
{
  m_view->setUpdatesEnabled(false);
  static_cast<KWQSortFilterModel*>(m_view->model())->restoreNativeOrder();
  m_view->horizontalHeader()->setSortIndicatorShown(false);
  m_view->setUpdatesEnabled(true);
}


void KWQCommandSort::redo()
{
  int currentRow = m_view->currentIndex().row();
  int currentColumn = m_view->currentIndex().column();
  m_view->sortByColumn(m_column);
  m_view->horizontalHeader()->setSortIndicatorShown(true);
  m_view->horizontalHeader()->setSortIndicator(m_column, m_view->horizontalHeader()->sortIndicatorOrder());
  m_view->setCurrentIndex(m_view->model()->index(currentRow, currentColumn, QModelIndex()));
}


KWQCommandShuffle::KWQCommandShuffle(QTableView * view, int column) : KWQCommandSort(view, column)
{
  m_view = view;
  setText(i18n("Shuffle"));
}

void KWQCommandShuffle::undo()
{
  KWQCommandSort::undo();
}

void KWQCommandShuffle::redo()
{
  int currentRow = m_view->currentIndex().row();
  int currentColumn = m_view->currentIndex().column();
  static_cast<KWQSortFilterModel*>(m_view->model())->shuffle();
  m_view->horizontalHeader()->setSortIndicatorShown(false);
  m_view->setCurrentIndex(m_view->model()->index(currentRow, currentColumn, QModelIndex()));
}


