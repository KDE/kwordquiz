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

#include <tqiodevice.h>
#include <tqfile.h>

#include <kdebug.h>
#include <kfilterdev.h>

PaukerDataItem::PaukerDataItem()
{
}

PaukerDataItem::PaukerDataItem(TQDomElement &entry)
{
  domElement = entry;
}

PaukerDataItem::~PaukerDataItem()
{
}

TQString PaukerDataItem::frontSide() const
{
  return getText("FrontSide");
}

TQString PaukerDataItem::backSide() const
{
  return getText("BackSide");
}

TQString PaukerDataItem::getText(const TQString &tagName) const
{
  if(!domElement.isNull()) {

    TQDomNodeList list = domElement.elementsByTagName(tagName);

    if(list.count() > 0) {

      TQDomElement element = list.item(0).toElement();

      if(!element.isNull()) {
        return element.text();
      }
      else
        return TQString::null;
    }
    else
      return TQString::null;
  }
  else
    return TQString::null;
}

/*!
    \fn PaukerData::PaukerData
 */
PaukerData::PaukerData()
{
  document = new TQDomDocument();
}

PaukerDataItemList PaukerData::parse(const TQString &fileName)
{
  PaukerDataItemList list;

  TQIODevice * file = KFilterDev::deviceForFile(fileName);
  document->setContent(file);

  TQDomNodeList entries = document->elementsByTagName("Card");

  // loop through the "Card" tags looking for data
  for(uint i = 0 ; i < entries.count() ; i++) {

    // get an entry to operate on
    TQDomElement entry = entries.item(i).toElement();

    // if the "node" is in fact an element -- i.e. not null
    if(!entry.isNull()) {
      PaukerDataItem item(entry);
      list.append(item);
    }
  }
  delete file;
  return list;
}


