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

#include <qdir.h>

#include <kprocess.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <knewstuff/entry.h>

#include "kwordquiz.h"
#include "kwqnewstuff.h"
#include "prefs.h"

KWQNewStuff::KWQNewStuff(QWidget *parent, const char *name) : QObject(), KNewStuff("kdeedu/vocabulary", parent)
{
  m_app = (KWordQuizApp *) parent;
}


bool KWQNewStuff::install(const QString & fileName)
{
  m_app->slotFileOpenRecent(KURL(fileName));
  return true;
}


bool KWQNewStuff::createUploadFile(const QString & fileName)
{
  return true;
}


QString KWQNewStuff::destinationPath(KNS::Entry * entry)
{
  if (entry)
  {
    KURL url = entry->payload();
    QString fileName = url.fileName();

    QString path = Prefs::installPath(); //default is Vocabularies which will be created in the user's home directory
    QString file;

    if (path.isEmpty())
      file = KNewStuff::downloadDestination(entry); //fall back on a temp file, should never happen
    else
    {
      file = QDir::home().path() + "/" + path + "/";
      KStandardDirs::makeDir(file); //ensure the directory exists
      file += fileName;
    }
    return file;
  }
  else
    return QString::null;
}


QString KWQNewStuff::downloadDestination(KNS::Entry * entry)
{
  QString file = destinationPath(entry);

  if (KStandardDirs::exists(file))
  {
    int result = KMessageBox::questionYesNo(parentWidget(),
        i18n("The file '%1' already exists. Do you want to overwrite it?")
        .arg(file),
        QString::null,
        i18n("Overwrite"),i18n("Do Not Overwrite"));
    if (result == KMessageBox::No)
      return QString::null;
  }
  KMessageBox::information(parentWidget(),
    i18n("<qt>The selected file will now be downloaded and saved as\n<b>'%1'</b>.</qt>")
    .arg(file),
    QString::null,
    "NewStuffDownloadLocation");
  return file;
}

#include "kwqnewstuff.moc"
