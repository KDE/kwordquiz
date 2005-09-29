#ifndef PAUKERITEM_H
#define PAUKERITEM_H

/* This file is part of KWordQuiz
  Copyright (C) 2004 Peter Hedlund <peter.hedlund@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qvaluelist.h>

class PaukerDataItem
{
public:
  PaukerDataItem();
  PaukerDataItem(QDomElement &entry);
  virtual ~PaukerDataItem();

  QString frontSide() const;
  QString backSide() const;

protected:
  QString getText(const QString &tagName) const;

private:
  QDomElement domElement;
};

typedef QValueList<PaukerDataItem> PaukerDataItemList;

class PaukerData
{
public:
  PaukerData();
  PaukerDataItemList parse(const QString &fileName);
  int colWidth(int col);
  int numRows();
  QString language(int col);

private:
  QDomDocument* document;
};

#endif
