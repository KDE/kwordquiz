/***************************************************************************
                               kwqcommands.h
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

#ifndef KWQCOMMANDS_H
#define KWQCOMMANDS_H

#include <QUndoCommand>

#include <KLocalizedString>

#include "kwqtableview.h"
#include "kwqsortfiltermodel.h"
#include "prefs.h"

void copyToClipboard(QTableView * view);

struct IndexAndData
{
  QModelIndex index;
  QVariant data;
  QVariant image;
  QVariant sound;
  int height;
};

typedef QList<IndexAndData> IndexAndDataList;

struct ColumnData
{
    QString identifier;
    QString layout;
    int width;
};

typedef QList<ColumnData> ColumnDataList;


class KWQUndoCommand : public QUndoCommand
{
public:
  explicit KWQUndoCommand(KWQTableView *view);

  void undo() override;
  void redo() override {};

  KWQTableView * view() {return m_view;};
  QModelIndex oldCurrentIndex() const {return m_currentIndex;};
  QModelIndexList oldSelectedIndexes() const {return m_selectedIndexes;};
  IndexAndDataList oldData() {return m_indexAndData;};

private:
  KWQTableView *m_view;
  IndexAndDataList m_indexAndData;
  QModelIndexList m_selectedIndexes;
  QModelIndex m_currentIndex;
};


class KWQCommandClear : public KWQUndoCommand
{
public:
   explicit KWQCommandClear(KWQTableView *view);
   void redo() override;
};


class KWQCommandCut : public KWQCommandClear
{
public:
   explicit KWQCommandCut(KWQTableView *view);
   void redo() override;
};


class KWQCommandPaste : public KWQUndoCommand
{
public:
  explicit KWQCommandPaste(KWQTableView *view);
  void undo() override;
  void redo() override;
private:
  int m_rowCount;
  IndexAndDataList m_pasteIndexAndData;
};


class KWQCommandFont : public KWQUndoCommand
{
public:
  KWQCommandFont(KWQTableView *view, const QFont &oldFont, const QFont &newFont) : KWQUndoCommand(view), m_oldFont(oldFont), m_newFont(newFont)
    { setText(i18n("Font")); }
  void undo() override
    { Prefs::setEditorFont(m_oldFont);
      view()->reset(); }
  void redo() override
    { Prefs::setEditorFont(m_newFont);
      view()->reset(); }
private:
  QFont m_oldFont;
  QFont m_newFont;
};


class KWQCommandEntry : public KWQUndoCommand
{
public:
  KWQCommandEntry(KWQTableView *view, const QString &oldText, const QString &newText)
    : KWQUndoCommand(view), m_oldText(oldText), m_newText(newText)
      { setText(i18n("Entry")); }

  void redo() override
      { view()->model()->setData(oldCurrentIndex(), m_newText, Qt::EditRole);
        view()->setCurrentIndex(oldCurrentIndex()); }
private:
  QString m_oldText;
  QString m_newText;
};


class KWQCommandSort : public QUndoCommand
{
public:
  KWQCommandSort(QTableView *view, int column);
  void undo() override;
  void redo() override;
private:
  QTableView *m_view;
  int m_column;
};


class KWQCommandShuffle : public KWQCommandSort
{
public:
  KWQCommandShuffle(QTableView *view, int column);
  void undo() override;
  void redo() override;
private:
  QTableView *m_view;
};


class KWQCommandInsert : public KWQUndoCommand
{
public:
  explicit KWQCommandInsert(KWQTableView *view);
  void undo() override;
  void redo() override;
};


class KWQCommandDelete : public KWQUndoCommand
{
public:
  explicit KWQCommandDelete(KWQTableView *view);
  void undo() override;
  void redo() override;
private:
  IndexAndDataList m_deleteIndexAndData;
};


class KWQCommandUnmarkBlank : public KWQUndoCommand
{
public:
  explicit KWQCommandUnmarkBlank(KWQTableView *view);
  //virtual void undo();
  void redo() override;
};


class KWQCommandIdentifiers : public KWQUndoCommand
{
public:
  KWQCommandIdentifiers(KWQTableView *view, const ColumnDataList &newColumnData);
  void undo() override;
  void redo() override;
private:
  ColumnDataList m_oldColumnData;
  ColumnDataList m_newColumnData;
};


class KWQCommandImage : public KWQUndoCommand
{
public:
  KWQCommandImage(KWQTableView *view, const QUrl &);
  void undo() override;
  void redo() override;

private:
  QUrl m_oldUrl;
  QUrl m_newUrl;
};


class KWQCommandSound : public KWQUndoCommand
{
public:
  KWQCommandSound(KWQTableView *view, const QUrl &);
  void undo() override;
  void redo() override;

private:
  QUrl m_oldUrl;
  QUrl m_newUrl;
};

#endif // KWQCOMMANDS_H
