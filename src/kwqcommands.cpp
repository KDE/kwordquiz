/***************************************************************************
                               kwqcommands.cpp
                             -------------------
    begin          : Fri Jan 18 10:37:00 PST 2008
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

#include "kwqcommands.h"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>

#include "kwqtablemodel.h"

void copyToClipboard(QTableView * view)
{
  QModelIndexList selIndexes = view->selectionModel()->selectedIndexes();
  if( selIndexes.isEmpty())
     return;
  int tr = selIndexes.first().row();
  int lc = selIndexes.first().column();
  int br = selIndexes.last().row();
  int rc = selIndexes.last().column();

  QString s;
  for (int r = tr; r <= br; ++r)
  {
    for (int c = lc; c <= rc; ++c)
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
  if (m_selectedIndexes.isEmpty())
      m_selectedIndexes.append(m_currentIndex);
      
  foreach (const QModelIndex &index, m_selectedIndexes) {
    IndexAndData id;
    id.index = index;
    id.data = m_view->model()->data(index, Qt::DisplayRole);
    id.image = m_view->model()->data(index, KWQTableModel::ImageRole);
    id.sound = m_view->model()->data(index, KWQTableModel::SoundRole);
    id.height = m_view->rowHeight(index.row());
    m_indexAndData.append(id);
  }
}


void KWQUndoCommand::undo()
{
  m_view->selectionModel()->clear();
  foreach (const IndexAndData &id, m_indexAndData) {
    m_view->model()->setData(id.index, id.data, Qt::EditRole);
    m_view->model()->setData(id.index, id.image, KWQTableModel::ImageRole);
    m_view->model()->setData(id.index, id.sound, KWQTableModel::SoundRole);
    m_view->selectionModel()->select(id.index, QItemSelectionModel::Select);
  }
  m_view->selectionModel()->setCurrentIndex(m_currentIndex, QItemSelectionModel::Current);
}


KWQCommandClear::KWQCommandClear(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo clear", "Clear"));
}


void KWQCommandClear::redo()
{
  view()->selectionModel()->clear();
  foreach (const QModelIndex &index, oldSelectedIndexes()) {
    if (Prefs::clearText() || Prefs::clearAll())
      view()->model()->setData(index, QVariant(), Qt::EditRole);
    if (Prefs::clearImageLink() || Prefs::clearAll())
      view()->model()->setData(index, QVariant(), KWQTableModel::ImageRole);
    if (Prefs::clearSoundLink() || Prefs::clearAll())
      view()->model()->setData(index, QVariant(), KWQTableModel::SoundRole);
    view()->selectionModel()->select(index, QItemSelectionModel::Select);
  }
  view()->selectionModel()->setCurrentIndex(oldCurrentIndex(), QItemSelectionModel::Current);
}


KWQCommandCut::KWQCommandCut(KWQTableView * view) : KWQCommandClear(view)
{
  setText(i18nc("@item:inmenu undo cut", "Cut"));
}


void KWQCommandCut::redo()
{
  copyToClipboard(view());
  KWQCommandClear::redo();
}


KWQCommandPaste::KWQCommandPaste(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo paste", "Paste"));
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
  QModelIndexList selIndexes = oldSelectedIndexes();
  if(selIndexes.isEmpty())
     return;
  QModelIndex topLeft = view()->model()->index(selIndexes.first().row(), selIndexes.first().column(), QModelIndex());
  QModelIndex bottomRight = view()->model()->index(selIndexes.last().row(), selIndexes.last().column(), QModelIndex());

  int tr;
  int lc;
  int ar = tr = topLeft.row();
  int ac = lc = topLeft.column();
  int rc = bottomRight.column();
  int br = bottomRight.row();

  QString s = QApplication::clipboard()->text();
  QStringList sl;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  sl = s.split('\n', QString::SkipEmptyParts);
#else
  sl = s.split('\n', Qt::SkipEmptyParts);
#endif
  if (sl.isEmpty())
    return;
  QStringList sr;
  int i = 0;

  if (selIndexes.count() == 1) //one cell selected
  {
    //Do we need to add entries (rows)?
    while (sl.count() + tr > view()->model()->rowCount(QModelIndex())) {
      view()->model()->insertRows(view()->model()->rowCount(), 1, QModelIndex());
    }

    br = tr + sl.count() - 1;

    if (lc == 0) //left col?
      if (sl[0].indexOf('\t') > -1)
        rc = 1; //expand to second column;

    while (i < sl.count() && br <= view()->model()->rowCount(QModelIndex())) {
      ac = lc;

      sr = sl.at(i).split('\t');
      int c = 0;
      while (ac <= rc) {
        IndexAndData id;
        id.index = view()->model()->index(ar, ac);
        id.data = view()->model()->data(id.index, Qt::DisplayRole);
        m_pasteIndexAndData.append(id);
        if (c < sr.count())
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
    while (i < sl.count() && ar <= br) {
      ac = lc;

      sr = sl.at(i).split('\t');
      int c = 0;
      while (ac <= rc) {
        if (view()->selectionModel()->isSelected(view()->model()->index(ar, ac)))
            if (c < sr.count())
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
  setText(i18nc("@item:inmenu undo sort", "Sort"));
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
  m_view->sortByColumn(m_column, Qt::AscendingOrder);
  m_view->horizontalHeader()->setSortIndicatorShown(true);
  m_view->horizontalHeader()->setSortIndicator(m_column, m_view->horizontalHeader()->sortIndicatorOrder());
  m_view->setCurrentIndex(m_view->model()->index(currentRow, currentColumn, QModelIndex()));
}


KWQCommandShuffle::KWQCommandShuffle(QTableView * view, int column) : KWQCommandSort(view, column)
{
  m_view = view;
  setText(i18nc("@item:inmenu undo shuffle", "Shuffle"));
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


KWQCommandInsert::KWQCommandInsert(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo insert", "Insert"));
}

void KWQCommandInsert::undo()
{
  QModelIndexList selIndexes = oldSelectedIndexes();
  QModelIndex topLeft = view()->model()->index(selIndexes.first().row(), selIndexes.first().column(), QModelIndex());
  QModelIndex bottomRight = view()->model()->index(selIndexes.last().row(), selIndexes.last().column(), QModelIndex());

  view()->model()->removeRows(topLeft.row(), bottomRight.row() - topLeft.row() + 1, QModelIndex());

  view()->selectionModel()->clear();
  view()->setCurrentIndex(view()->model()->index(oldCurrentIndex().row(), oldCurrentIndex().column(), QModelIndex()));
  view()->selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::Select);
}

void KWQCommandInsert::redo()
{
  QModelIndexList selIndexes = oldSelectedIndexes();
  QModelIndex topLeft = view()->model()->index(selIndexes.first().row(), selIndexes.first().column(), QModelIndex());
  QModelIndex bottomRight = view()->model()->index(selIndexes.last().row(), selIndexes.last().column(), QModelIndex());

  view()->model()->insertRows(topLeft.row(), bottomRight.row() - topLeft.row() + 1, QModelIndex());

  view()->selectionModel()->clear();
  view()->setCurrentIndex(view()->model()->index(oldCurrentIndex().row(), oldCurrentIndex().column(), QModelIndex()));
  view()->selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::Select);
}


KWQCommandDelete::KWQCommandDelete(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo delete", "Delete"));
}


void KWQCommandDelete::undo()
{
  QModelIndexList selIndexes = oldSelectedIndexes();
  QModelIndex topLeft = view()->model()->index(selIndexes.first().row(), selIndexes.first().column(), QModelIndex());
  QModelIndex bottomRight = view()->model()->index(selIndexes.last().row(), selIndexes.last().column(), QModelIndex());

  view()->model()->insertRows(topLeft.row(), bottomRight.row() - topLeft.row() + 1, QModelIndex());

  foreach (const IndexAndData &id, m_deleteIndexAndData)
    view()->model()->setData(id.index, id.data, Qt::EditRole);

  view()->selectionModel()->clear();
  view()->setCurrentIndex(view()->model()->index(oldCurrentIndex().row(), oldCurrentIndex().column(), QModelIndex()));
  view()->selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::Select);
}


void KWQCommandDelete::redo()
{
  QModelIndexList selIndexes = oldSelectedIndexes();
  QModelIndex topLeft = view()->model()->index(selIndexes.first().row(), selIndexes.first().column(), QModelIndex());
  QModelIndex bottomRight = view()->model()->index(selIndexes.last().row(), selIndexes.last().column(), QModelIndex());

  QModelIndex topLeftDeletion = view()->model()->index(selIndexes.first().row(), 0, QModelIndex());
  QModelIndex bottomRightDeletion = view()->model()->index(selIndexes.last().row(), 1, QModelIndex());

  QItemSelection deletion(topLeftDeletion, bottomRightDeletion);
  m_deleteIndexAndData.clear();
  foreach (const QModelIndex &idx, deletion.indexes()) {
    IndexAndData id;
    id.index = idx;
    id.data = view()->model()->data(id.index, Qt::DisplayRole);
    m_deleteIndexAndData.append(id);
  }

  view()->model()->removeRows(topLeft.row(), bottomRight.row() - topLeft.row() + 1, QModelIndex());

  view()->selectionModel()->clear();
  view()->setCurrentIndex(view()->model()->index(oldCurrentIndex().row(), oldCurrentIndex().column(), QModelIndex()));
  view()->selectionModel()->select(QItemSelection(topLeft, bottomRight), QItemSelectionModel::Select);
}


KWQCommandUnmarkBlank::KWQCommandUnmarkBlank(KWQTableView * view) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo unmark blank", "Unmark Blank"));
}


void KWQCommandUnmarkBlank::redo()
{
  QString s;
  view()->selectionModel()->clear();
  foreach (const QModelIndex &index, oldSelectedIndexes())
  {
    s = view()->model()->data(index, Qt::DisplayRole).toString();
    s = s.remove(delim_start);
    s = s.remove(delim_end);
    view()->model()->setData(index, QVariant(s), Qt::EditRole);
    view()->selectionModel()->select(index, QItemSelectionModel::Select);
  }
  view()->selectionModel()->setCurrentIndex(oldCurrentIndex(), QItemSelectionModel::Current);
}


KWQCommandIdentifiers::KWQCommandIdentifiers(KWQTableView * view, const ColumnDataList &newColumnData) : KWQUndoCommand(view)
{
  setText(i18nc("@item:inmenu undo column titles", "Column Settings"));
  m_oldColumnData.clear();
  ColumnData columnData;
  columnData.identifier = view->model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
  columnData.layout = view->model()->headerData(0, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
  columnData.width = view->model()->headerData(0, Qt::Horizontal, Qt::SizeHintRole).toSize().width();
  m_oldColumnData.append(columnData);

  columnData.identifier = view->model()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();
  columnData.layout = view->model()->headerData(1, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
  columnData.width = view->model()->headerData(1, Qt::Horizontal, Qt::SizeHintRole).toSize().width();
  m_oldColumnData.append(columnData);

  m_newColumnData = newColumnData;
}


void KWQCommandIdentifiers::undo()
{
  view()->model()->setHeaderData(0, Qt::Horizontal, m_oldColumnData[0].identifier, Qt::EditRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, m_oldColumnData[1].identifier, Qt::EditRole);

  view()->model()->setHeaderData(0, Qt::Horizontal, m_oldColumnData[0].layout, KWQTableModel::KeyboardLayoutRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, m_oldColumnData[1].layout, KWQTableModel::KeyboardLayoutRole);

  view()->model()->setHeaderData(0, Qt::Horizontal, QSize(m_oldColumnData[0].width, 25), Qt::SizeHintRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, QSize(m_oldColumnData[1].width, 25), Qt::SizeHintRole);
}


void KWQCommandIdentifiers::redo()
{
  view()->model()->setHeaderData(0, Qt::Horizontal, m_newColumnData[0].identifier, Qt::EditRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, m_newColumnData[1].identifier, Qt::EditRole);

  view()->model()->setHeaderData(0, Qt::Horizontal, m_newColumnData[0].layout, KWQTableModel::KeyboardLayoutRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, m_newColumnData[1].layout, KWQTableModel::KeyboardLayoutRole);

  view()->model()->setHeaderData(0, Qt::Horizontal, QSize(m_newColumnData[0].width, 25), Qt::SizeHintRole);
  view()->model()->setHeaderData(1, Qt::Horizontal, QSize(m_newColumnData[1].width, 25), Qt::SizeHintRole);
}


KWQCommandImage::KWQCommandImage(KWQTableView *view, const QUrl &newUrl) : KWQUndoCommand(view), m_newUrl(newUrl)
{
  setText(i18nc("@item:inmenu undo link image", "Link Image"));
  m_oldUrl = QUrl(view->model()->data(oldCurrentIndex(), KWQTableModel::ImageRole).toString());
}


void KWQCommandImage::undo()
{
  view()->model()->setData(oldCurrentIndex(), QVariant(m_oldUrl), KWQTableModel::ImageRole);
}


void KWQCommandImage::redo()
{
  view()->model()->setData(oldCurrentIndex(), QVariant(m_newUrl), KWQTableModel::ImageRole);
}


KWQCommandSound::KWQCommandSound(KWQTableView *view, const QUrl &newUrl) : KWQUndoCommand(view), m_newUrl(newUrl)
{
  setText(i18nc("@item:inmenu undo link sound", "Link Sound"));
  m_oldUrl = QUrl(view->model()->data(oldCurrentIndex(), KWQTableModel::SoundRole).toString());
}


void KWQCommandSound::undo()
{
  view()->model()->setData(oldCurrentIndex(), QVariant(m_oldUrl), KWQTableModel::SoundRole);
}


void KWQCommandSound::redo()
{
  view()->model()->setData(oldCurrentIndex(), QVariant(m_newUrl), KWQTableModel::SoundRole);
}
