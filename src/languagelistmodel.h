// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QAbstractListModel>

class LanguageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NativeNameRole = Qt::UserRole + 1,
        LanguageCodeRole,
        FlagRole,
    };
    explicit LanguageListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QString flagFromName(const QString &language) const;
    Q_INVOKABLE QString languageName(const QString &language) const;

    struct Language {
        QString locale;
        QString name;
    };

private:
    QList<Language> m_availableLanguages;
};
