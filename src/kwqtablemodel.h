/*
    SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWQTABLEMODEL_H
#define KWQTABLEMODEL_H

#include <QAbstractTableModel>

#include <KImageCache>

#include <keduvocdocument.h>

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/

const char delim_start = '[';
const char delim_end = ']';

class KWQTableModel : public QAbstractTableModel
{
Q_OBJECT
public:
  enum roles {
    KeyboardLayoutRole = Qt::UserRole,
    ImageRole,
    SoundRole
  };

  explicit KWQTableModel(QObject * parent = nullptr);
  ~KWQTableModel() override;

  void setDocument(KEduVocDocument * doc);
  KEduVocDocument * document() { return m_doc;};
  bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) override;

  KEduVocLesson * currentLesson(int row);

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  Qt::ItemFlags flags (const QModelIndex & index) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  bool setHeaderData (int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole) override;

  bool isEmpty();
  bool checkSyntax() const;
  bool checkBlanksSyntax(const QString & text) const;

private:
  KEduVocDocument * m_doc;
  KImageCache *m_decorationCache;

  friend class KWordQuizApp;
};

#endif
