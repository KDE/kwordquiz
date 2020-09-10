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
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <KAboutData>
#include <KActionCollection>
#include <KCrash>
#include <KLocalizedString>

#include "kwqtutor.h"
#include "kwordquiz.h"
#include "kwordquiz_version.h"

static const char *description = I18N_NOOP("A powerful flashcard and vocabulary learning program");
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QApplication app(argc, argv);

    KCrash::initialize();
    KLocalizedString::setApplicationDomain("kwordquiz");
    QApplication::setApplicationName(QStringLiteral("kwordquiz"));
    QApplication::setApplicationVersion(KWORDQUIZ_VERSION_STRING);
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setApplicationDisplayName(i18n("kwordquiz"));

    KAboutData aboutData(QStringLiteral("kwordquiz"),
                         i18n("KWordQuiz"),
                         KWORDQUIZ_VERSION_STRING,
                         i18n(description),
                         KAboutLicense::GPL_V2,
                         i18n("(c) 2003-2010, Peter Hedlund"),
                         QString(),
                         QStringLiteral("https://kde.org/applications/education/kwordquiz"),
                         QStringLiteral("submit@bugs.kde.org"));
    aboutData.addAuthor(i18n("Peter Hedlund"), QString(), QStringLiteral("peter.hedlund@kdemail.net"));
    aboutData.addCredit(i18n("Anne-Marie Mahfouf"), i18n("KDE Edutainment Maintainer"), QStringLiteral("annma@kde.org"));
    aboutData.addCredit(i18n("Martin Pfeiffer"), i18n("Leitner System and several code contributions"), QStringLiteral("hubipete@gmx.net"));

    KAboutData::setApplicationData(aboutData);

    KCrash::initialize();

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("m") << QStringLiteral("mode"), i18n("A number 1-5 corresponding to the \nentries in the Mode menu"), QStringLiteral("number")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("g") << QStringLiteral("goto"), i18n("Type of session to start with: \n'flash' for flashcard, \n'mc' for multiple choice, \n'qa' for question and answer, \n'tutor' for tutor"), QStringLiteral("session")));
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("+[File]"), i18n("File to open")));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QStringList args = parser.positionalArguments();

    if (parser.isSet(QStringLiteral("g")) && parser.value(QStringLiteral("g")) == QLatin1String("tutor")) {
        QApplication tutorapp(argc, argv);
        tutorapp.setQuitOnLastWindowClosed(false);
        //KWQTutor* m_tutor = new KWQTutor(args.count() ?
        //                                 QUrl::fromLocalFile(args.at(args.count() - 1)) : QUrl()); //last arg - file to open
        return tutorapp.exec();
    }

    if (app.isSessionRestored()) {
        kRestoreMainWindows<KWordQuizApp>();
    } else {
        KWordQuizApp *kwordquiz = new KWordQuizApp();
        kwordquiz->show();

        if (args.count()) {
            kwordquiz->openDocumentFile(QUrl::fromLocalFile(args.at(args.count() - 1)));

            QString mode = parser.value(QStringLiteral("mode"));

            if (!mode.isEmpty()) {
                QAction *a = kwordquiz->actionCollection()->action(QStringLiteral("mode_%1").arg(QString(mode)));
                kwordquiz->slotModeActionGroupTriggered(a);
            }

            QString go_to = parser.value(QStringLiteral("goto"));
            if (!go_to.isEmpty()) {
                if (go_to == QLatin1String("flash"))
                    kwordquiz->slotQuizFlash();
                if (go_to == QLatin1String("mc"))
                    kwordquiz->slotQuizMultiple();
                if (go_to == QLatin1String("qa"))
                    kwordquiz->slotQuizQA();
            }
        } else {
            kwordquiz->openDocumentFile();
        }
    }
    return app.exec();
}
