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

#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kwordquiz.h"
#include "version.h"

static const char *description = I18N_NOOP("A powerful flashcard and vocabulary learning program");

static KCmdLineOptions options[] =
{
  { "m", 0, 0 },
  { "mode <number>", I18N_NOOP("A number 1-5 corresponding to the \nentries in the Mode menu"), 0 },
  { "g", 0, 0 },
  { "goto <session>", I18N_NOOP("Type of session to start with: \n'flash' for flashcard, \n'mc' for multiple choice, \n'qa' for question and answer"), 0 },
  { "+[File]", I18N_NOOP("File to open"), 0 },
  { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
  KAboutData aboutData("kwordquiz",
                       I18N_NOOP("KWordQuiz"),
                       KWQ_VERSION,
                       description,
                       KAboutData::License_GPL,
                       "(c) 2003-2005, Peter Hedlund",
                       0,
                       "http://edu.kde.org/kwordquiz",
                       "submit@bugs.kde.org");

  aboutData.addAuthor("Peter Hedlund", 0, "peter.hedlund@kdemail.net");
  aboutData.addCredit("Anne-Marie Mahfouf", I18N_NOOP("KDE Edutainment Maintainer"), "annma@kde.org", 0);

  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;

  if (app.isRestored())
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
      kwordquiz->openDocumentFile(args->arg(args->count() - 1));
      QCString mode = args->getOption("mode");
      if (!mode.isEmpty())
      {
        if (mode == "1")
          kwordquiz->slotMode1();
        if (mode == "2")
          kwordquiz->slotMode2();
        if (mode == "3")
          kwordquiz->slotMode3();
        if (mode == "4")
          kwordquiz->slotMode4();
        if (mode == "5")
          kwordquiz->slotMode5();
      }
      QCString go_to = args->getOption("goto");
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
