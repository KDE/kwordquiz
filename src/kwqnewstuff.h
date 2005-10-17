/***************************************************************************
                          Implementation of KNewStuff
                             -------------------
    begin                : Thu Jun 02 13:12:30 PDT 2002
    copyright            : (C) 2005 by Peter Hedlund
    email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWQNEWSTUFF_H
#define KWQNEWSTUFF_H

#include <knewstuff/knewstuff.h>

class KWordQuizApp;

/**
@author Peter Hedlund
*/
class KWQNewStuff : public QObject, public KNewStuff
{
Q_OBJECT
public:
  KWQNewStuff(QWidget *parent = 0, const char *name = 0);

  /**
    Installs a downloaded file according to the application's configuration.

    @param fileName filename of the donwloaded file
    @return @c true in case of installation success, @c false otherwise
  */
  bool install(const QString &fileName);

  /**
    Creates a file suitable for upload.
    Note that this method always fails, since using KNewStuffGeneric
    means that the provided file must already be in a usable format.

    @param fileName the name of the file to upload after its creation
    @return @c true in case of creation success, @c false otherwise
  */
  bool createUploadFile(const QString &fileName);

  /**
    Queries the preferred destination file for a download.

    @param entry a Hotstuff data entry
    @return destination filename, or 0 to return directory only
  */
  QString downloadDestination(KNS::Entry *entry);

private:
  QString destinationPath(KNS::Entry *entry);

  KWordQuizApp * m_app;
};

#endif
