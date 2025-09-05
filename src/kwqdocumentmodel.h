// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KEduVocDocument>
#include <KNSCore/Entry>
#include <QAbstractListModel>
#include <qqmlintegration.h>

/// @author Carl Schwan <carl@carlschwan.eu>
class KWQDocumentModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(DocumentModel)

public:
    enum ExtraRoles {
        TitleRole = Qt::UserRole + 1,
        UrlRole,
        DocumentRole,
        DocumentStatusRole,
    };

    enum class DocumentStatus {
        NoError,
        CannotLoadFile,
        CannotSaveFile,
    };
    Q_ENUM(DocumentStatus)

    explicit KWQDocumentModel(QObject *parent = nullptr);
    ~KWQDocumentModel();

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void load();
    void save() const;

    Q_INVOKABLE int add(const QUrl &url);
    Q_INVOKABLE void addDocument(KEduVocDocument *document);
    Q_INVOKABLE void remove(const int row);

    Q_INVOKABLE void entryChanged(const KNSCore::Entry &wrapper);

private:
    std::vector<std::pair<std::unique_ptr<KEduVocDocument>, DocumentStatus>> m_documents;
};
