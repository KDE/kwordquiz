/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2003 by Peter Hedlund
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

#include <QAction>

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kapplication.h>

#include "kwordquiz.h"
#include "version.h"

static const char *description = I18N_NOOP("A powerful flashcard and vocabulary learning program");

int main(int argc, char *argv[])
{
  KAboutData aboutData("kwordquiz", 0,
                       ki18n("KWordQuiz"),
                       KWQ_VERSION,
                       ki18n(description),
                       KAboutData::License_GPL,
                       ki18n("(c) 2003-2007, Peter Hedlund"),
                       KLocalizedString(),
                       "http://edu.kde.org/kwordquiz",
                       "submit@bugs.kde.org");

  aboutData.addAuthor(ki18n("Peter Hedlund"), KLocalizedString(), "peter.hedlund@kdemail.net");
  aboutData.addCredit(ki18n("Anne-Marie Mahfouf"), ki18n("KDE Edutainment Maintainer"), "annma@kde.org");
  aboutData.addCredit(ki18n("Martin Pfeiffer"), ki18n("Leitner System and several code contributions"), "hubipete@gmx.net");

  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("m");
  options.add("mode <number>", ki18n("A number 1-5 corresponding to the \nentries in the Mode menu"));
  options.add("g");
  options.add("goto <session>", ki18n("Type of session to start with: \n'flash' for flashcard, \n'mc' for multiple choice, \n'qa' for question and answer"));
  options.add("+[File]", ki18n("File to open"));
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;

  if (app.isSessionRestored())
  {
    RESTORE(KWordQuizApp);
  }
  else 
  {
    KWordQuizApp *kwordquiz = new KWordQuizApp();
    kwordquiz->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count())
    {
      kwordquiz->openDocumentFile(KUrl(args->url(args->count() - 1)));

      QString mode = args->getOption("mode");

      if (!mode.isEmpty())
      {
        QAction *a = kwordquiz->actionCollection()->action(QString("mode_%1").arg(QString(mode)));
        kwordquiz->slotModeActionGroupTriggered(a);
      }

      QString go_to = args->getOption("goto");
      if (!go_to.isEmpty())
      {
        if (go_to == "flash")
          kwordquiz->slotQuizFlash();
        if (go_to == "mc")
          kwordquiz->slotQuizMultiple();
        if (go_to == "qa")
          kwordquiz->slotQuizQA();
      }
    }
    else
    {
      kwordquiz->openDocumentFile();
    }
    args->clear();
  }
  return app.exec();
}
