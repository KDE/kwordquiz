/*
    SPDX-FileCopyrightText: 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QAction>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

#include "blankanswer.h"
#include "exporter.h"
#include "kwordquiz_version.h"
#include "kwqcardmodel.h"
#include "kwqdocumentmodel.h"
#include "kwqrandomsortmodel.h"
#include "prefs.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    QApplication app(argc, argv);

    KCrash::initialize();
    KLocalizedString::setApplicationDomain("kwordquiz");
    QApplication::setApplicationName(QStringLiteral("kwordquiz"));
    QApplication::setApplicationVersion(KWORDQUIZ_VERSION_STRING);
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QIcon::setFallbackThemeName("breeze");

    KAboutData aboutData(QStringLiteral("kwordquiz"),
                         i18n("KWordQuiz"),
                         KWORDQUIZ_VERSION_STRING,
                         i18n("A powerful flashcard and vocabulary learning program"),
                         KAboutLicense::GPL_V2,
                         i18n("(c) 2003-2010, Peter Hedlund"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/kwordquiz"),
                         QStringLiteral("submit@bugs.kde.org"));
    aboutData.addAuthor(i18n("Peter Hedlund"), i18nc("@info:credit", "Original author"), QStringLiteral("peter.hedlund@kdemail.net"));
    aboutData.addAuthor(i18nc("@info:credit", "Carl Schwan"), i18nc("@info:credit", "Port to QML"), QStringLiteral("carl@carlschwan.eu"));
    aboutData.addCredit(i18n("Anne-Marie Mahfouf"), i18n("KDE Edutainment Maintainer"), QStringLiteral("annma@kde.org"));
    aboutData.addCredit(i18n("Martin Pfeiffer"), i18n("Leitner System and several code contributions"), QStringLiteral("hubipete@gmx.net"));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kwordquiz")));

    KAboutData::setApplicationData(aboutData);

    KCrash::initialize();

#ifdef Q_OS_WIN
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

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

    auto prefs = Prefs::self();
    qmlRegisterSingletonInstance("org.kde.kwordquiz", 1, 0, "Prefs", prefs);
    qmlRegisterType<KWQCardModel>("org.kde.kwordquiz", 1, 0, "CardModel");
    qmlRegisterType<Exporter>("org.kde.kwordquiz", 1, 0, "Exporter");
    qmlRegisterType<BlankAnswer>("org.kde.kwordquiz", 1, 0, "BlankAnswer");
    qmlRegisterType<KWQDocumentModel>("org.kde.kwordquiz", 1, 0, "DocumentModel");
    qmlRegisterType<KWQRandomSortModel>("org.kde.kwordquiz", 1, 0, "RandomSortModel");
    qmlRegisterType<KWQRandomSortModel>("org.kde.kwordquiz", 1, 0, "RandomSortModel");
    qmlRegisterSingletonType("org.kde.kwordquiz", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    //if (app.isSessionRestored()) {
    //    kRestoreMainWindows<KWordQuizApp>();
    //} else {
    //    KWordQuizApp *kwordquiz = new KWordQuizApp();
    //    kwordquiz->show();

    //    if (args.count()) {
    //        kwordquiz->openDocumentFile(QUrl::fromLocalFile(args.at(args.count() - 1)));

    //        QString mode = parser.value(QStringLiteral("mode"));

    //        if (!mode.isEmpty()) {
    //            QAction *a = kwordquiz->actionCollection()->action(QStringLiteral("mode_%1").arg(QString(mode)));
    //            kwordquiz->slotModeActionGroupTriggered(a);
    //        }

    //        QString go_to = parser.value(QStringLiteral("goto"));
    //        if (!go_to.isEmpty()) {
    //            if (go_to == QLatin1String("flash"))
    //                kwordquiz->slotQuizFlash();
    //            if (go_to == QLatin1String("mc"))
    //                kwordquiz->slotQuizMultiple();
    //            if (go_to == QLatin1String("qa"))
    //                kwordquiz->slotQuizQA();
    //        }
    //    } else {
    //        kwordquiz->openDocumentFile();
    //    }
    //}
    return app.exec();
}
