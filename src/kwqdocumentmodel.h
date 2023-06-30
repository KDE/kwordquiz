// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <keduvocdocument.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <KNSCore/EntryWrapper>
#else
#include <KNSCore/Entry>
#endif

/// @author Carl Schwan <carl@carlschwan.eu>
class KWQDocumentModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ExtraRoles {
        TitleRole = Qt::UserRole + 1,
        UrlRole,
        DocumentRole,
    };

    explicit KWQDocumentModel(QObject *parent = nullptr);
    ~KWQDocumentModel();

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void load();
    void save() const;

    Q_INVOKABLE void add(const QUrl &url);
    Q_INVOKABLE void add(KEduVocDocument *document);
    Q_INVOKABLE void remove(const int row);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_INVOKABLE void entryChanged(KNSCore::EntryWrapper *wrapper);
#else
    Q_INVOKABLE void entryChanged(const KNSCore::Entry &wrapper);
#endif

private:
    std::vector<std::unique_ptr<KEduVocDocument>> m_documents;
};
