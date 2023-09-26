/*
    SPDX-FileCopyrightText: 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QAction>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <qstringliteral.h>

#include "audioprober.h"
#include "blankanswer.h"
#include "exporter.h"
#include "fileopener.h"
#include "kwordquiz_version.h"
#include "kwqcardmodel.h"
#include "kwqdocumentmodel.h"
#include "kwqrandomsortmodel.h"
#include "languagelistmodel.h"
#include "prefs.h"
#include "stateprefs.h"

#ifndef Q_OS_ANDROID
#include <KDBusService>
#endif

using namespace Qt::Literals::StringLiterals;

void parseArgs(QCommandLineParser &parser, QQuickWindow *view, const QString &workingDirectory)
{
    const QStringList args = parser.positionalArguments();
    if (args.count() == 0) {
        return;
    }

    auto fileOpener = view->findChild<FileOpener *>(u"FileOpener"_s);

    QString goTo = parser.value(u"goto"_s);
    Prefs::EnumStartSession mode = Prefs::Flashcard;

    if (!goTo.isEmpty()) {
        if (goTo == u"flash"_s) {
            mode = Prefs::Flashcard;
        } else if (goTo == u"mc"_s) {
            mode = Prefs::MultipleChoice;
        } else if (goTo == u"qa"_s) {
            mode = Prefs::QA;
        } else {
            qWarning() << i18n("Invalid goto given: %1. Allowed: flash, mc and qa", goTo);
        }
    }

    auto file = QUrl::fromUserInput(args.at(args.count() - 1), workingDirectory);
    fileOpener->openFile(file, mode);
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(u"org.kde.breeze"_s);
    QIcon::setThemeName(u"tokodon"_s);
#else
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(u"org.kde.desktop"_s);
    }
#endif

    KCrash::initialize();
    KLocalizedString::setApplicationDomain("kwordquiz");
    QApplication::setApplicationName(u"kwordquiz"_s);
    QApplication::setApplicationVersion(QStringLiteral(KWORDQUIZ_VERSION_STRING));
    QApplication::setOrganizationDomain(u"kde.org"_s);
    QIcon::setFallbackThemeName(u"breeze"_s);

    KAboutData aboutData(u"kwordquiz"_s,
                         i18n("KWordQuiz"),
                         QStringLiteral(KWORDQUIZ_VERSION_STRING),
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

#ifndef Q_OS_ANDROID
    KDBusService service(KDBusService::Unique);
#endif

    KCrash::initialize();

#ifdef Q_OS_WIN
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addOption(
        QCommandLineOption(QStringList() << QStringLiteral("g") << QStringLiteral("goto"),
                           i18n("Type of session to start with: \n'flash' for flashcard, \n'mc' for multiple choice, \n'qa' for question and answer"),
                           QStringLiteral("session")));
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("+[File]"), i18n("File to open")));
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QQmlApplicationEngine engine;

    QObject::connect(&service, &KDBusService::activateRequested, &engine, [&engine, &parser](const QStringList &arguments, const QString &workingDirectory) {
        parser.parse(arguments);
        const auto rootObjects = engine.rootObjects();

        for (auto obj : rootObjects) {
            auto view = qobject_cast<QQuickWindow *>(obj);
            parseArgs(parser, view, workingDirectory);
        }
    });

    auto prefs = Prefs::self();
    auto statePrefs = StatePrefs::self();
    qmlRegisterSingletonInstance("org.kde.kwordquiz", 1, 0, "Prefs", prefs);
    qmlRegisterSingletonInstance("org.kde.kwordquiz", 1, 0, "StatePrefs", statePrefs);
    qmlRegisterType<KWQCardModel>("org.kde.kwordquiz", 1, 0, "CardModel");
    qmlRegisterType<FileOpener>("org.kde.kwordquiz", 1, 0, "FileOpener");
    qmlRegisterType<Exporter>("org.kde.kwordquiz", 1, 0, "Exporter");
    qmlRegisterType<AudioProber>("org.kde.kwordquiz", 1, 0, "AudioProber");
    qmlRegisterType<BlankAnswer>("org.kde.kwordquiz", 1, 0, "BlankAnswer");
    qmlRegisterType<KWQDocumentModel>("org.kde.kwordquiz", 1, 0, "DocumentModel");
    qmlRegisterType<KWQRandomSortModel>("org.kde.kwordquiz", 1, 0, "RandomSortModel");
    qmlRegisterType<KWQRandomSortModel>("org.kde.kwordquiz", 1, 0, "RandomSortModel");
    qmlRegisterSingletonType("org.kde.kwordquiz", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });
    qmlRegisterSingletonType<LanguageListModel>("org.kde.kwordquiz", 1, 0, "LanguageListModel", [](QQmlEngine *engine, QJSEngine *) {
        return new LanguageListModel;
    });

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    const auto rootObjects = engine.rootObjects();
    if (rootObjects.isEmpty()) {
        return -1;
    }

    for (auto obj : rootObjects) {
        auto view = qobject_cast<QQuickWindow *>(obj);
        parseArgs(parser, view, QDir::currentPath());
    }

    return app.exec();
}
