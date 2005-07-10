/***************************************************************************
                     read a KEduVocDocument from a WQL file
    -----------------------------------------------------------------------
    copyright            : (C) 2004 Peter Hedlund
                         : (c) 2005 The KDE-EDU team
    email                : peter@peterandlinda.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEDUVOCWQLREADER_H
#define KEDUVOCWQLREADER_H

#include <qfile.h>

#include "keduvocdocument.h"

class KEduVocDocument;

class KEduVocWqlReader : public QObject
{
public:
	KEduVocWqlReader(QFile *file);
	~KEduVocWqlReader();

	bool readDoc(KEduVocDocument *doc);

private:
  QFile *m_inputFile;
  KEduVocDocument *m_doc;
};

#endif
