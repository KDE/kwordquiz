/* This file is part of the KDE Edu Library
   Copyright (C) 2002 Scott Wheeler <wheeler@kde.org>

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

#include "keduvocdata.h"

#include <qfile.h>

#include <kdebug.h>

////////////////////////////////////////////////////////////////////////////////
// class KEduVocDataItem
////////////////////////////////////////////////////////////////////////////////

// public methods

KEduVocDataItem::KEduVocDataItem()
{

}

KEduVocDataItem::KEduVocDataItem(QDomElement &entry)
{
    domElement = entry;
}

KEduVocDataItem::~KEduVocDataItem()
{

}

QString KEduVocDataItem::originalText() const
{
    return getText("o");
}

QString KEduVocDataItem::translatedText() const
{
    return getText("t");
}

// protected methods

QString KEduVocDataItem::getText(const QString &tagName) const
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

////////////////////////////////////////////////////////////////////////////////
// class KEduVocData
////////////////////////////////////////////////////////////////////////////////


/*!
    \fn KEduVocData::KEduVocData
 */
KEduVocData::KEduVocData()
{
  document = new QDomDocument();
}

KEduVocDataItemList KEduVocData::parse(const QString &fileName)
{
    KEduVocDataItemList list;

    //QDomDocument document;
    QFile file(fileName);
    document->setContent(&file);

    QDomNodeList entries = document->elementsByTagName("e");

    // loop through the "e" (entry) tags looking for data
    for(uint i = 0 ; i < entries.count() ; i++) {

        // get an entry to operate on
        QDomElement entry = entries.item(i).toElement();

        // if the "node" is in fact an element -- i.e. not null
        if(!entry.isNull()) {
            KEduVocDataItem item(entry);
            list.append(item);
        }
    }

    return list;
}


/*!
    \fn KEduVocData::colWidth(int col)
 */
int KEduVocData::colWidth(int col)
{
  if (!document->isNull())
    if (col == 0)
      return document -> documentElement().namedItem("e").namedItem("o").toElement().attribute("width", "250").toInt();
    else
      return document -> documentElement().namedItem("e").namedItem("t").toElement().attribute("width", "250").toInt();
  else
    return 250;
}


/*!
    \fn KEduVocData::numRows()
 */
int KEduVocData::numRows()
{
  if (!document->isNull())
    return document->documentElement().attribute("lines", "0").toInt();
  else
    return 0;
}


/*!
    \fn KEduVocData::language(int col)
 */
QString KEduVocData::language(int col)
{
  if (!document->isNull())
    if (col == 0)
      return document -> documentElement().namedItem("e").namedItem("o").toElement().attribute("l");
    else
      return document -> documentElement().namedItem("e").namedItem("t").toElement().attribute("l");
  else
    return QString::null;

}


