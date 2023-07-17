// SPDX-FileCopyrightText: 1999-2003 Hans Petter Bieker <bieker@kde.org>
// SPDX-FileCopyrightText: 2007 David Jarvie <djarvie@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "languagelistmodel.h"
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStandardPaths>
#include <qstringliteral.h>

bool operator<(const LanguageListModel::Language &a, const LanguageListModel::Language &b)
{
    return a.name < b.name;
}

static QString nameFromEntryFile(const QString &entryFile)
{
    const KConfig entry(entryFile, KConfig::SimpleConfig);
    const KConfigGroup group(&entry, "KCM Locale");
    return group.readEntry("Name", QString());
}

LanguageListModel::LanguageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    const QStringList localeDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("locale"), QStandardPaths::LocateDirectory);
    for (const QString &localeDir : localeDirs) {
        const QStringList entries = QDir(localeDir).entryList(QDir::Dirs, QDir::Name);
        for (const QString &languageCode : entries) {
            const QString entryFile = localeDir + QLatin1Char('/') + languageCode + QStringLiteral("/kf5_entry.desktop");
            if (QFile::exists(entryFile)) {
                QString text;
                const QString entryFile =
                    QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + languageCode + QLatin1String("/kf5_entry.desktop"));
                if (QFile::exists(entryFile)) {
                    text = nameFromEntryFile(entryFile);
                }

                if (text.isEmpty()) {
                    text = languageCode;
                    QLocale locale(languageCode);
                    if (locale != QLocale::c()) {
                        text = locale.nativeLanguageName();
                        // For some languages the native name might be empty.
                        // In this case use the non native language name as fallback.
                        // See: QTBUG-51323
                        text = text.isEmpty() ? QLocale::languageToString(locale.language()) : text;
                    }
                }

                m_availableLanguages.append(LanguageListModel::Language{languageCode, text});
            }
        }
    }
}

int LanguageListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_availableLanguages.size();
}

QVariant LanguageListModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &language = m_availableLanguages[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case NativeNameRole:
        return language.name;
    case LanguageCodeRole:
        return language.locale;
    case FlagRole:
        return flagFromName(language.locale);
    }
    return {};
}

QString LanguageListModel::languageName(const QString &language) const
{
    const QStringList localeDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("locale"), QStandardPaths::LocateDirectory);
    for (const QString &localeDir : localeDirs) {
        const QString entryFile = localeDir + QLatin1Char('/') + language + QStringLiteral("/kf5_entry.desktop");
        if (QFile::exists(entryFile)) {
            QString text;
            const QString entryFile =
                QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + language + QLatin1String("/kf5_entry.desktop"));
            if (QFile::exists(entryFile)) {
                text = nameFromEntryFile(entryFile);
            }

            if (!text.isEmpty()) {
                return text;
            }
        }
    }

    QLocale locale(language);
    return locale.nativeLanguageName();
}

QString LanguageListModel::flagFromName(const QString &language) const
{
    QString flagCode;
    const QStringList split = QLocale(language).name().split(QLatin1Char('_'));
    if (split.size() > 1) {
        flagCode = split[1].toLower();
    }

    static constexpr auto surrogatePairCodePoint = 0xD83C;
    static constexpr auto flagCodePointStart = 0xDDE6; // https://en.wikipedia.org/wiki/Regional_indicator_symbol
    static constexpr auto offsetCodePointA = QChar('A').unicode(); // offset from 0, the flag code points have the same offsets
    static constexpr auto basePoint = flagCodePointStart - offsetCodePointA;

    QString emoji;
    for (const auto &c : flagCode) {
        emoji.append(surrogatePairCodePoint);
        emoji.append(QChar(basePoint + c.toUpper().unicode()));
    }

    if (emoji.isEmpty()) {
        return QString();
    }

    return emoji;
}

QHash<int, QByteArray> LanguageListModel::roleNames() const
{
    return {
        {NativeNameRole, QByteArrayLiteral("nativeName")},
        {LanguageCodeRole, QByteArrayLiteral("languageCode")},
        {FlagRole, QByteArrayLiteral("flag")},
    };
}
