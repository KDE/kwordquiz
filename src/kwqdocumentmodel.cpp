// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqdocumentmodel.h"
#include "stateprefs.h"
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
    const auto urls = StatePrefs::documents();
    if (urls.empty()) {
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/example.kvtml"))));
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/french_verbs.kvtml"))));
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/fill_in_the_blank.kvtml"))));
        add(QUrl::fromLocalFile(
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/us_states_and_capitals.kvtml"))));

        return;
    }

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
    StatePrefs::setDocuments(urls);
    StatePrefs::self()->save();
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
        {TitleRole, "title"},
        {UrlRole, "url"},
        {DocumentRole, "document"},
    };
}

void KWQDocumentModel::addDocument(KEduVocDocument *document)
{
    document->setParent(nullptr);
    const int row = rowCount();
    beginInsertRows({}, row, row);
    m_documents.push_back(std::unique_ptr<KEduVocDocument>(document));
    endInsertRows();

    save();
}

int KWQDocumentModel::add(const QUrl &url)
{
    int row = 0;
    for (const auto &doc : std::as_const(m_documents)) {
        if (doc->url() == url) {
            qDebug() << row;
            return row;
        }
        row++;
    }

    auto doc = std::make_unique<KEduVocDocument>(nullptr);
    doc->open(url, KEduVocDocument::FileIgnoreLock);

    row = rowCount();
    beginInsertRows({}, row, row);
    m_documents.push_back(std::move(doc));
    endInsertRows();

    save();

    return row;
}

void KWQDocumentModel::remove(const int row)
{
    beginRemoveRows({}, row, row);
    m_documents.erase(std::begin(m_documents) + row);
    endRemoveRows();
}

void KWQDocumentModel::entryChanged(const KNSCore::Entry &entry)
{
    if (entry.status() == KNSCore::Entry::Deleted) {
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
    } else if (entry.status() == KNSCore::Entry::Installed) {
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

#include "moc_kwqdocumentmodel.cpp"
