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

#include "paukerreader.h"

#include <qiodevice.h>
#include <qfile.h>

#include <kdebug.h>
#include <kfilterdev.h>

PaukerDataItem::PaukerDataItem()
{
}

PaukerDataItem::PaukerDataItem(QDomElement &entry)
{
  domElement = entry;
}

PaukerDataItem::~PaukerDataItem()
{
}

QString PaukerDataItem::frontSide() const
{
  return getText("FrontSide");
}

QString PaukerDataItem::backSide() const
{
  return getText("BackSide");
}

QString PaukerDataItem::getText(const QString &tagName) const
{
  if(!domElement.isNull()) {

    QDomNodeList list = domElement.elementsByTagName(tagName);

    if(list.count() > 0) {

      QDomElement element = list.item(0).toElement();

      if(!element.isNull()) {
        return element.text();
      }
      else
        return QString::null;
    }
    else
      return QString::null;
  }
  else
    return QString::null;
}

/*!
    \fn PaukerData::PaukerData
 */
PaukerData::PaukerData()
{
  document = new QDomDocument();
}

PaukerDataItemList PaukerData::parse(const QString &fileName)
{
  PaukerDataItemList list;

  QIODevice * file = KFilterDev::deviceForFile(fileName);
  document->setContent(file);

  QDomNodeList entries = document->elementsByTagName("Card");

  // loop through the "Card" tags looking for data
  for(uint i = 0 ; i < entries.count() ; i++) {

    // get an entry to operate on
    QDomElement entry = entries.item(i).toElement();

    // if the "node" is in fact an element -- i.e. not null
    if(!entry.isNull()) {
      PaukerDataItem item(entry);
      list.append(item);
    }
  }
  delete file;
  return list;
}


