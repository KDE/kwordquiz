/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2003 by Peter Hedlund
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

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kwordquiz.h"

static const char *description =
	I18N_NOOP("KWordQuiz");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

	KAboutData aboutData( "kwordquiz", I18N_NOOP("KWordQuiz"),
		VERSION, description, KAboutData::License_GPL,
		"(c) 2003, Peter Hedlund", 0, 0, "peter@peterandlinda.com");
	aboutData.addAuthor("Peter Hedlund",0, "peter@peterandlinda.com");
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
        kwordquiz->openDocumentFile(args->arg(0));
		}
		else
		{
		  kwordquiz->openDocumentFile();
		}
		args->clear();
  }

  return app.exec();
}  
