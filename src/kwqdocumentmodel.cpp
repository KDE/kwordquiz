// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqdocumentmodel.h"
#include "prefs.h"
#include <keduvocdocument.h>

KWQDocumentModel::KWQDocumentModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

KWQDocumentModel::~KWQDocumentModel()
{
    save();
}

void KWQDocumentModel::load()
{
    const auto urls = Prefs::documents();
    for (const auto &url : urls) {
        auto doc = std::make_unique<KEduVocDocument>(nullptr);
        doc->open(QUrl(url), KEduVocDocument::FileIgnoreLock);
        m_documents.push_back(std::move(doc));
    }
}

void KWQDocumentModel::save() const
{
    QStringList urls;
    for (const auto &document : std::as_const(m_documents)) {
        urls << document->url().url();
    }
    Prefs::setDocuments(urls);
    Prefs::self()->save();
}

int KWQDocumentModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_documents.size();
}

QVariant KWQDocumentModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &document = m_documents[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return document->title();

    case UrlRole:
        return document->url().url();

    case DocumentRole:
        return QVariant::fromValue(document.get());

    default:
        return {};
    }
}

QHash<int, QByteArray> KWQDocumentModel::roleNames() const
{
    return {
        { TitleRole, "title" },
        { UrlRole, "url" },
        { DocumentRole, "document" },
    };
}


void KWQDocumentModel::add(KEduVocDocument *document)
{
    document->setParent(nullptr);
    beginInsertRows({}, rowCount(), rowCount());
    m_documents.push_back(std::unique_ptr<KEduVocDocument>(document));
    endInsertRows();

    save();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void KWQDocumentModel::entryChanged(KNSCore::EntryWrapper *wrapper)
{
    const auto entry = wrapper->entry();
    if (entry.status() == KNS3::Entry::Deleted) {
#else
void KWQDocumentModel::entryChanged(const KNSCore::Entry &entry)
{
    if (entry.status() == KNSCore::Entry::Deleted) {
#endif
        const auto uninstalledFiles = entry.uninstalledFiles();
        for (const auto &url : uninstalledFiles) {
            auto it = std::find_if(cbegin(m_documents), cend(m_documents), [url](const auto &document) {
                return document->url() == QUrl(url);
            });

            if (it == cend(m_documents)) {
                continue;
            }

            beginRemoveRows({}, cend(m_documents) - it, cend(m_documents) - it);
            m_documents.erase(it);
            endInsertRows();
        }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    } else if (entry.status() == KNS3::Entry::Installed) {
#else
    } else if (entry.status() == KNSCore::Entry::Installed) {
#endif
        qDebug() << entry.installedFiles();
        const auto installedFiles = entry.installedFiles();
        beginInsertRows({}, rowCount(), rowCount() + entry.installedFiles().count() - 1);
        for (const auto &url : installedFiles) {
            auto doc = std::make_unique<KEduVocDocument>(nullptr);
            doc->open(QUrl(QStringLiteral("file:") + url), KEduVocDocument::FileIgnoreLock);
            m_documents.push_back(std::move(doc));
        }
        endInsertRows();
    }
}
