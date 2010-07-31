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

#include <tqdom.h>
#include <tqvaluelist.h>

class PaukerDataItem
{
public:
  PaukerDataItem();
  PaukerDataItem(TQDomElement &entry);
  virtual ~PaukerDataItem();

  TQString frontSide() const;
  TQString backSide() const;

protected:
  TQString getText(const TQString &tagName) const;

private:
  TQDomElement domElement;
};

typedef TQValueList<PaukerDataItem> PaukerDataItemList;

class PaukerData
{
public:
  PaukerData();
  PaukerDataItemList parse(const TQString &fileName);
  int colWidth(int col);
  int numRows();
  TQString language(int col);

private:
  TQDomDocument* document;
};

#endif
