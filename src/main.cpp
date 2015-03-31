/***************************************************************************
                                 main.cpp
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2010 by Peter Hedlund
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


#include <KAboutData>
#include <KActionCollection>
#include <QApplication>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QCommandLineOption>


#include "kwqtutor.h"
#include "kwordquiz.h"
#include "version.h"

static const char *description = I18N_NOOP("A powerful flashcard and vocabulary learning program");
int main(int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("kwordquiz");
    QApplication::setApplicationName("kwordquiz");
    QApplication::setApplicationVersion(KWQ_VERSION);
    QApplication::setOrganizationDomain("kde.org");
    QApplication::setApplicationDisplayName(i18n("kwordquiz"));

    QApplication app(argc, argv);

    KAboutData aboutData("kwordquiz",
                         i18n("KWordQuiz"),
                         KWQ_VERSION,
                         i18n(description),
                         KAboutLicense::GPL_V2,
                         i18n("(c) 2003-2010, Peter Hedlund"),
                         QString(),
                         "http://edu.kde.org/kwordquiz",
                         "submit@bugs.kde.org");
    aboutData.addAuthor(i18n("Peter Hedlund"), QString(), "peter.hedlund@kdemail.net");
    aboutData.addCredit(i18n("Anne-Marie Mahfouf"), i18n("KDE Edutainment Maintainer"), "annma@kde.org");
    aboutData.addCredit(i18n("Martin Pfeiffer"), i18n("Leitner System and several code contributions"), "hubipete@gmx.net");

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);

    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("m") << QLatin1String("mode"), i18n("A number 1-5 corresponding to the \nentries in the Mode menu"), QLatin1String("number")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("g") << QLatin1String("goto"), i18n("Type of session to start with: \n'flash' for flashcard, \n'mc' for multiple choice, \n'qa' for question and answer, \n'tutor' for tutor"), QLatin1String("session")));
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("+[File]"), i18n("File to open")));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QStringList args = parser.positionalArguments();

    if (parser.isSet("g") && parser.value("g") == "tutor") {
        QApplication tutorapp(argc, argv);
        tutorapp.setQuitOnLastWindowClosed(false);
        //KWQTutor* m_tutor = new KWQTutor(args.count() ?
        //                                 QUrl::fromLocalFile(args.at(args.count() - 1)) : QUrl()); //last arg - file to open
        return tutorapp.exec();
    }

    if (app.isSessionRestored()) {
        RESTORE(KWordQuizApp);
    } else {
        KWordQuizApp *kwordquiz = new KWordQuizApp();
        kwordquiz->show();

        if (args.count()) {
            kwordquiz->openDocumentFile(QUrl::fromLocalFile(args.at(args.count() - 1)));

            QString mode = parser.value("mode");

            if (!mode.isEmpty()) {
                QAction *a = kwordquiz->actionCollection()->action(QString("mode_%1").arg(QString(mode)));
                kwordquiz->slotModeActionGroupTriggered(a);
            }

            QString go_to = parser.value("goto");
            if (!go_to.isEmpty()) {
                if (go_to == "flash")
                    kwordquiz->slotQuizFlash();
                if (go_to == "mc")
                    kwordquiz->slotQuizMultiple();
                if (go_to == "qa")
                    kwordquiz->slotQuizQA();
            }
        } else {
            kwordquiz->openDocumentFile();
        }
    }
    return app.exec();
}
